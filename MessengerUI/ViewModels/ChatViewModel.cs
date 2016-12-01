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
    public class ChatViewModel : BindableBase
    {
        private string _textMessage = "MyName";
        public string TextMessage
        {
            get { return _textMessage; }
            set { SetProperty(ref _textMessage, value); }
        }

        public ICommand EnterChatCommand { get; set; }

        public ChatViewModel()
        {
            EnterChatCommand = new DelegateCommand(PerformEnter, CanEnter).ObservesProperty(() => TextMessage);
        }

        private bool CanEnter()
        {
            return !String.IsNullOrWhiteSpace(TextMessage);
        }

        private void PerformEnter()
        {
            TextMessage = "Perform";
        }
    }
}
