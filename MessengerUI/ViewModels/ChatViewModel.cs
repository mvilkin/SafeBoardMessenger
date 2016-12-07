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
    public class ChatViewModel : BindableBase
    {
        private LoginControl _loginCtrl;
        public LoginControl LoginCtrl
        {
            get { return _loginCtrl; }
            set { SetProperty(ref _loginCtrl, value); }
        }

        private OnlineUsersControl _onlineUsersCtrl;
        public OnlineUsersControl OnlineUsersCtrl
        {
            get { return _onlineUsersCtrl; }
            set { SetProperty(ref _onlineUsersCtrl, value); }
        }

        private SendMessageControl _sendMessageCtrl;
        public SendMessageControl SendMessageCtrl
        {
            get { return _sendMessageCtrl; }
            set 
            {
                if (_sendMessageCtrl != null)
                    _sendMessageCtrl.PropertyChanged -= TextMessageOnPropertyChanged;

                SetProperty(ref _sendMessageCtrl, value); 

                if (_sendMessageCtrl != null)
                    _sendMessageCtrl.PropertyChanged += TextMessageOnPropertyChanged;
            }
        }
        private void TextMessageOnPropertyChanged(object sender, PropertyChangedEventArgs propertyChangedEventArgs)
        {
            var sendMessageDelegateCommand = SendMessageCommand as DelegateCommand;
            if (sendMessageDelegateCommand != null)
                sendMessageDelegateCommand.RaiseCanExecuteChanged();
        }

        private RecvMessageControl _recvMessageCtrl;
        public RecvMessageControl RecvMessageCtrl
        {
            get { return _recvMessageCtrl; }
            set { SetProperty(ref _recvMessageCtrl, value); }
        }

        private int _selectedUser = -1;
        public int SelectedUser
        {
            get { return _selectedUser; }
            set { SetProperty(ref _selectedUser, value); }
        }

        public ICommand SendMessageCommand { get; set; }
        public ICommand UpdateUsersCommand { get; set; }

        public ChatViewModel(IEventAggregator eventAggregator)
        {
            OnlineUsersCtrl = new OnlineUsersControl();
            SendMessageCtrl = new SendMessageControl();
            RecvMessageCtrl = new RecvMessageControl();
            LoginCtrl = new LoginControl();

            SendMessageCommand = new DelegateCommand(PerformSend, CanSend).ObservesProperty(() => SelectedUser);
            UpdateUsersCommand = new DelegateCommand(PerformUpdateUsers);

            eventAggregator.GetEvent<EnterChatEvent>().Subscribe(EnterChatEventHandler);
        }

        private bool CanSend()
        {
            return !String.IsNullOrWhiteSpace(SendMessageCtrl.Text) && SelectedUser >= 0;
        }

        private void PerformUpdateUsers()
        {
            OnlineUsersCtrl.Update();
            SelectedUser = -1;
        }

        private void PerformSend()
        {
            SendMessageCtrl.Recipient = OnlineUsersCtrl.OnlineUsers[SelectedUser];
            SendMessageCtrl.Send();
            
            System.Threading.Thread.Sleep(1000);

            RecvMessageCtrl.Receive();
        }

        private void EnterChatEventHandler(EnterChatEventData eventData)
        {
            LoginCtrl.Login = eventData.Login;
            LoginCtrl.Password = eventData.Password;
            LoginCtrl.Server = eventData.Server;
            LoginCtrl.EnterChat();
        }
    }
}
