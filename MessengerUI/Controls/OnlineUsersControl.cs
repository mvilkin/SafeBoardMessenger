using System;
using System.Collections.Generic;
using System.Linq;
using System.Runtime.InteropServices;
using System.Text;
using System.Threading;
using System.Threading.Tasks;
using Prism.Mvvm;

namespace MessengerUI.Controls
{
    public class OnlineUsersControl : BindableBase
    {
        [UnmanagedFunctionPointer(CallingConvention.StdCall, CharSet = CharSet.Unicode)]
        delegate void OnUserUpdate(StringBuilder users);
        [DllImport("MessengerBase.dll", CallingConvention = CallingConvention.Cdecl)]
        static extern void StartGetOnlineUsers(OnUserUpdate callback);
        [DllImport("MessengerBase.dll", CallingConvention = CallingConvention.Cdecl)]
        static extern void StopGetOnlineUsers();

        private String[] _onlineUsers;
        public String[] OnlineUsers
        {
            get { return _onlineUsers; }
            set { SetProperty(ref _onlineUsers, value); }
        }

        private Thread _updatingThread;

        public String GetUserByIndex(int index)
        {
            String user = OnlineUsers[index];
            if (OnlineUsers[index].StartsWith("** "))
                user = user.Remove(0, 3);
            return user;
        }

        public void StartUpdating()
        {
            _updatingThread = new Thread(UpdateProcess);
            _updatingThread.Start();
        }

        public void UpdateProcess()
        {
            OnUserUpdate updateCallback = users => { OnlineUsers = users.ToString().Split(';').ToArray(); };
            StartGetOnlineUsers(updateCallback);
        }

        public void StopUpdating()
        {
            StopGetOnlineUsers();
        }
    }
}
