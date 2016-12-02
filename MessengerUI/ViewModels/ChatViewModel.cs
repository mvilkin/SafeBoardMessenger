using System;
using System.Collections.Generic;
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
        private string _textMessage;
        public string TextMessage
        {
            get { return _textMessage; }
            set { SetProperty(ref _textMessage, value); }
        }

        private OnlineUsersControl _onlineUsersCtrl;
        public OnlineUsersControl OnlineUsersCtrl
        {
            get { return _onlineUsersCtrl; }
            set { SetProperty(ref _onlineUsersCtrl, value); }
        }

        private int _selectedUser = -1;
        public int SelectedUser
        {
            get { return _selectedUser; }
            set { SetProperty(ref _selectedUser, value); }
        }

        public ICommand SendCommand { get; set; }
        public ICommand UpdateUsersCommand { get; set; }

        public ChatViewModel(IEventAggregator eventAggregator)
        {
            OnlineUsersCtrl = new OnlineUsersControl();

            SendCommand = new DelegateCommand(PerformSend, CanSend).ObservesProperty(() => TextMessage);
            UpdateUsersCommand = new DelegateCommand(PerformUpdateUsers);

            eventAggregator.GetEvent<EnterChatEvent>().Subscribe(EnterChatEventHandler);
        }

        private bool CanSend()
        {
            return !String.IsNullOrWhiteSpace(TextMessage);
        }

        private void PerformUpdateUsers()
        {
            OnlineUsersCtrl.Update();
            SelectedUser = -1;
        }

        private void PerformSend()
        {
            TextMessage = "Send";
        }

        private void EnterChatEventHandler(EnterChatEventData eventData)
        {
            TextMessage = "Login: " + eventData.Login + "\nServer: " + eventData.Server;
        }
    }
}
