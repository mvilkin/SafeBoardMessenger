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
    public class ChatViewModel : BindableBase
    {
        private string _textMessage;
        public string TextMessage
        {
            get { return _textMessage; }
            set { SetProperty(ref _textMessage, value); }
        }

        private string[] _onlineUsers;
        public string[] OnlineUsers
        {
            get { return _onlineUsers; }
            set { SetProperty(ref _onlineUsers, value); }
        }

        public ICommand SendCommand { get; set; }
        public ICommand UpdateUsersCommand { get; set; }

        public ChatViewModel(IEventAggregator eventAggregator)
        {
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
            string[] users = {"User1", "User2"};
            OnlineUsers = users;
            TextMessage = "Update";
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
