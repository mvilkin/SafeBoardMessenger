using System;
using System.Collections.Generic;
using System.ComponentModel;
using System.Linq;
using System.Text;
using System.Threading.Tasks;
using System.Windows.Input;
using MessengerUI.Controls;
using MessengerUI.Events;
using Prism.Commands;
using Prism.Events;
using Prism.Mvvm;

namespace MessengerUI.ViewModels
{
    public class LoginViewModel : BindableBase
    {
        private readonly IEventAggregator _eventAggregator;

        private LoginControl _loginControl;
        public LoginControl LoginCtrl
        {
            get { return _loginControl; }
            set
            {
                if (_loginControl != null)
                    _loginControl.PropertyChanged -= OnLoginControlPropertyChanged;

                SetProperty(ref _loginControl, value);

                if (_loginControl != null)
                    _loginControl.PropertyChanged += OnLoginControlPropertyChanged;
            }
        }
        private void OnLoginControlPropertyChanged(object sender, PropertyChangedEventArgs propertyChangedEventArgs)
        {
            var sendMessageDelegateCommand = EnterChatCommand as DelegateCommand;
            if (sendMessageDelegateCommand != null)
                sendMessageDelegateCommand.RaiseCanExecuteChanged();
        }

        public ICommand EnterChatCommand { get; set; }

        public LoginViewModel(IEventAggregator eventAggregator)
        {
            LoginCtrl = new LoginControl();
            _eventAggregator = eventAggregator;
            EnterChatCommand = new DelegateCommand(PerformEnter, CanEnter);
        }

        private bool CanEnter()
        {
            return !String.IsNullOrWhiteSpace(LoginCtrl.Login) &&
                !String.IsNullOrWhiteSpace(LoginCtrl.Password) &&
                !String.IsNullOrWhiteSpace(LoginCtrl.Server);
        }

        private void PerformEnter()
        {
            var enterCode = LoginCtrl.EnterChat();
            _eventAggregator.GetEvent<EnterChatEvent>().Publish(enterCode);
        }
    }
}
