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
        private OnlineUsersControl _onlineUsersCtrl;
        public OnlineUsersControl OnlineUsersCtrl
        {
            get { return _onlineUsersCtrl; }
            set { SetProperty(ref _onlineUsersCtrl, value); }
        }

        private ChatViewControl _chatViewCtrl;
        public ChatViewControl ChatViewCtrl
        {
            get { return _chatViewCtrl; }
            set { SetProperty(ref _chatViewCtrl, value); }
        }

        private SendMessageControl _sendMessageCtrl;
        public SendMessageControl SendMessageCtrl
        {
            get { return _sendMessageCtrl; }
            set
            {
                if (_sendMessageCtrl != null)
                    _sendMessageCtrl.PropertyChanged -= OnSendMessagePropertyChanged;

                SetProperty(ref _sendMessageCtrl, value);

                if (_sendMessageCtrl != null)
                    _sendMessageCtrl.PropertyChanged += OnSendMessagePropertyChanged;
            }
        }
        private void OnSendMessagePropertyChanged(object sender, PropertyChangedEventArgs propertyChangedEventArgs)
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
        public ICommand UserChosenCommand { get; set; }

        public ChatViewModel(IEventAggregator eventAggregator)
        {
            OnlineUsersCtrl = new OnlineUsersControl();
            SendMessageCtrl = new SendMessageControl();
            RecvMessageCtrl = new RecvMessageControl();
            ChatViewCtrl = new ChatViewControl();

            SendMessageCtrl.ChatViewCtrl = ChatViewCtrl;
            RecvMessageCtrl.ChatViewCtrl = ChatViewCtrl;

            SendMessageCommand = new DelegateCommand(PerformSend, CanSend).ObservesProperty(() => SelectedUser);
            UpdateUsersCommand = new DelegateCommand(PerformUpdateUsers);
            UserChosenCommand = new DelegateCommand(PerformUserChosen).ObservesProperty(() => SelectedUser);

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
            SendMessageCtrl.Send();
        }

        private void PerformUserChosen()
        {
            if (SelectedUser >= 0)
            {
                RecvMessageCtrl.StopReceiving();
                RecvMessageCtrl.Sender = OnlineUsersCtrl.OnlineUsers[SelectedUser];
                SendMessageCtrl.Recipient = OnlineUsersCtrl.OnlineUsers[SelectedUser];
                ChatViewCtrl.Text = string.Empty;
                RecvMessageCtrl.StartReceiving();
            }
        }

        private void EnterChatEventHandler(int enterCode)
        {
            if (enterCode == 0)
            {
                OnlineUsersCtrl.Update();
                SelectedUser = -1;
                RecvMessageCtrl.StartReceiving();
            }
        }
    }
}
