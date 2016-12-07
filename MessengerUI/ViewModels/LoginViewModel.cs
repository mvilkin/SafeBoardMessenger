using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;
using System.Windows.Input;
using MessengerUI.Events;
using Prism.Commands;
using Prism.Events;
using Prism.Mvvm;

namespace MessengerUI.ViewModels
{
    public class LoginViewModel : BindableBase
    {
        private readonly IEventAggregator _eventAggregator;

        private string _login = "MyName";
        public string Login
        {
            get { return _login; }
            set { SetProperty(ref _login, value); }
        }

        private string _password = "MyPassword";
        public string Password
        {
            get { return _password; }
            set { SetProperty(ref _password, value); }
        }

        private string _server = "MyServer";
        public string Server
        {
            get { return _server; }
            set { SetProperty(ref _server, value); }
        }

        public ICommand EnterChatCommand { get; set; }

        public LoginViewModel(IEventAggregator eventAggregator)
        {
            _eventAggregator = eventAggregator;
            EnterChatCommand = new DelegateCommand(PerformEnter, CanEnter).ObservesProperty(() => Login).ObservesProperty(() => Server);
        }

        private bool CanEnter()
        {
            return !String.IsNullOrWhiteSpace(Login) && !String.IsNullOrWhiteSpace(Server);
        }

        private void PerformEnter()
        {
            var eventData = new EnterChatEventData
            {
                Login = Login,
                Password = Password,
                Server = Server
            };
            _eventAggregator.GetEvent<EnterChatEvent>().Publish(eventData);
        }
    }
}
