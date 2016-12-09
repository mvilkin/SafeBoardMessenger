using System;
using System.Collections.Generic;
using System.Linq;
using System.Runtime.InteropServices;
using System.Text;
using System.Threading.Tasks;
using Prism.Mvvm;

namespace MessengerUI.Controls
{
    public class LoginControl : BindableBase
    {
        [DllImport("MessengerBase.dll", CallingConvention = CallingConvention.Cdecl)]
        public static extern int EnterMessenger(StringBuilder login, StringBuilder password, StringBuilder server);

        private string _login;
        public string Login
        {
            get { return _login; }
            set { SetProperty(ref _login, value); }
        }

        private string _password;
        public string Password
        {
            get { return _password; }
            set { SetProperty(ref _password, value); }
        }

        private string _server = "127.0.0.1";
        public string Server
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
