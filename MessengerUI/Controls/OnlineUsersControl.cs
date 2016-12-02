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
        public static extern int GetOnlineUsersStringSize();
        [DllImport("MessengerBase.dll", CallingConvention = CallingConvention.Cdecl)]
        public static extern void GetOnlineUsersString(StringBuilder users);

        private string[] _onlineUsers;
        public string[] OnlineUsers
        {
            get { return _onlineUsers; }
            set { SetProperty(ref _onlineUsers, value); }
        }

        public void Update()
        {
            var onlineUsersStringSize = GetOnlineUsersStringSize();
            var onlineUsers = new StringBuilder(onlineUsersStringSize);
            GetOnlineUsersString(onlineUsers);
            OnlineUsers = onlineUsers.ToString().Split(';').ToArray();
        }
    }
}
