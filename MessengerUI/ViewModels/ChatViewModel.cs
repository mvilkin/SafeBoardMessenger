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
        private string _textMessage = "MyName";
        public string TextMessage
        {
            get { return _textMessage; }
            set { SetProperty(ref _textMessage, value); }
        }

        public ICommand PerformSendCommand { get; set; }

        public ChatViewModel(IEventAggregator eventAggregator)
        {
            PerformSendCommand = new DelegateCommand(PerformSend, CanSend).ObservesProperty(() => TextMessage);
            eventAggregator.GetEvent<EnterChatEvent>().Subscribe(UpdateTextMessage);
        }

        private bool CanSend()
        {
            return !String.IsNullOrWhiteSpace(TextMessage);
        }

        private void PerformSend()
        {
            TextMessage = "Perform";
        }

        private void UpdateTextMessage(bool obj)
        {
            TextMessage = "123dw";
        }
    }
}
