using System;
using System.Collections.Generic;
using System.Linq;
using System.Runtime.InteropServices;
using System.Text;
using System.Threading.Tasks;
using Prism.Mvvm;

namespace MessengerUI.Controls
{
    public class OnlineUsersControl : BindableBase
    {
        [DllImport("MessengerBase.dll", CallingConvention = CallingConvention.Cdecl)]
        public static extern void GetOnlineUsersString(StringBuilder users, ref int usersSize);
        [DllImport("MessengerBase.dll", CallingConvention = CallingConvention.Cdecl)]
        public static extern void AddCli();

        private string[] _onlineUsers;
        public string[] OnlineUsers
        {
            get { return _onlineUsers; }
            set { SetProperty(ref _onlineUsers, value); }
        }

        public void Update()
        {
            AddCli();
            int onlineUsersStringSize = 0;
            GetOnlineUsersString(null, ref onlineUsersStringSize);
            var onlineUsers = new StringBuilder(onlineUsersStringSize);
            GetOnlineUsersString(onlineUsers, ref onlineUsersStringSize);
            OnlineUsers = onlineUsers.ToString().Split(';').ToArray();
        }
    }
}
