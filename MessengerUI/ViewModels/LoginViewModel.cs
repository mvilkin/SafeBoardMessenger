using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;
using System.Windows.Input;
using Prism.Commands;
using Prism.Mvvm;

namespace MessengerUI.ViewModels
{
    public class LoginViewModel : BindableBase
    {
        private string _login = "MyName";
        public string Login
        {
            get { return _login; }
            set { SetProperty(ref _login, value); }
        }

        private string _server;
        public string Server
        {
            get { return _server; }
            set { SetProperty(ref _server, value); }
        }

        public ICommand EnterChatCommand { get; set; }

        public LoginViewModel()
        {
            EnterChatCommand = new DelegateCommand(PerformEnter, CanEnter).ObservesProperty(() => Login).ObservesProperty((() => Server));
        }

        private bool CanEnter()
        {
            return !String.IsNullOrWhiteSpace(Login) && !String.IsNullOrWhiteSpace(Server);
        }

        private void PerformEnter()
        {
            Login = "Perform";
        }
    }
}
