﻿using System;
using System.Collections.Generic;
using System.IO;
using System.Linq;
using System.Runtime.InteropServices;
using System.Text;
using System.Threading.Tasks;
using Prism.Mvvm;

namespace MessengerUI.Controls
{
    public class LoginControl : BindableBase
    {
        [DllImport("MessengerBase.dll", CharSet = CharSet.Unicode, CallingConvention = CallingConvention.Cdecl)]
        public static extern int EnterMessenger(StringBuilder login, StringBuilder password, StringBuilder server);

        private String _login = "1@local";
        public String Login
        {
            get { return _login; }
            set { SetProperty(ref _login, value); }
        }

        private String _password = "1";
        public String Password
        {
            get { return _password; }
            set { SetProperty(ref _password, value); }
        }

        private String _server = "127.0.0.1";
        public String Server
        {
            get { return _server; }
            set { SetProperty(ref _server, value); }
        }

        public int EnterChat()
        {
            return EnterMessenger(new StringBuilder(Login), new StringBuilder(Password), new StringBuilder(Server));
        }
    }
}
