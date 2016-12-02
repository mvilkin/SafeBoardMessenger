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
using Prism.Regions;

namespace MessengerUI.ViewModels
{
    public class MainWindowViewModel : BindableBase
    {
        private bool _backButtonVisibility = false;
        public bool BackButtonVisibility
        {
            get { return _backButtonVisibility; }
            set { SetProperty(ref _backButtonVisibility, value); }
        }

        private bool _helloViewVisibility = true;
        public bool HelloViewVisibility
        {
            get { return _helloViewVisibility; }
            set { SetProperty(ref _helloViewVisibility, value); }
        }

        private readonly IRegionManager _regionManager;

        public DelegateCommand<string> NavigateCommand { get; set; }
        public ICommand EnterMessengerCommand { get; set; }
        public ICommand BackButtonClickCommand { get; set; }

        public MainWindowViewModel(IRegionManager regionManager, IEventAggregator eventAggregator)
        {
            _regionManager = regionManager;

            eventAggregator.GetEvent<EnterChatEvent>().Subscribe(EnterChatEventHandler);

            BackButtonClickCommand = new DelegateCommand(BackButtonClickHandler);
            NavigateCommand = new DelegateCommand<string>(Navigate);
            EnterMessengerCommand = new DelegateCommand(PerformEnterMessenger);
        }

        private void Navigate(string uri)
        {
            _regionManager.RequestNavigate("ContentRegion", uri);
        }

        private void BackButtonClickHandler()
        {
            Navigate("LoginView");
            BackButtonVisibility = false;
        }

        private void EnterChatEventHandler(EnterChatEventData eventData)
        {
            BackButtonVisibility = true;
            Navigate("ChatView");
        }

        private void PerformEnterMessenger()
        {
            HelloViewVisibility = false;
            Navigate("ChatView");
            Navigate("LoginView");
        }
    }
}
