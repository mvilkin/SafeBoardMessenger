using System;
using System.Collections.Generic;
using System.Linq;
using System.Runtime.InteropServices;
using System.Text;
using System.Threading.Tasks;
using System.Windows;
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
        private readonly IEventAggregator _eventAggregator;

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

        public DelegateCommand<String> NavigateCommand { get; set; }
        public ICommand EnterMessengerCommand { get; set; }
        public ICommand BackButtonClickCommand { get; set; }
        public ICommand CloseApplicationCommand { get; set; }

        public MainWindowViewModel(IRegionManager regionManager, IEventAggregator eventAggregator)
        {
            _regionManager = regionManager;
            _eventAggregator = eventAggregator;

            _eventAggregator.GetEvent<EnterChatEvent>().Subscribe(EnterChatEventHandler);

            BackButtonClickCommand = new DelegateCommand(BackButtonClickHandler);
            NavigateCommand = new DelegateCommand<String>(Navigate);
            EnterMessengerCommand = new DelegateCommand(PerformEnterMessenger);
            CloseApplicationCommand = new DelegateCommand(CloseApplicationHandler);
        }

        private void Navigate(String uri)
        {
            _regionManager.RequestNavigate("ContentRegion", uri);
        }

        private void BackButtonClickHandler()
        {
            Navigate("LoginView");
            BackButtonVisibility = false;

            _eventAggregator.GetEvent<ExitChatEvent>().Publish(true);
        }

        private static String TranslateEnterCode(int code)
        {
            if (code == 1)
                return "Auth Error";
            if (code == 2)
                return "Network Error";
            if (code == 3)
                return "Internal Error";
            return "Unknown Error";
        }

        private void CloseApplicationHandler()
        {
            _eventAggregator.GetEvent<CloseApplicationEvent>().Publish(true);
        }

        private void EnterChatEventHandler(int enterCode)
        {
            if (enterCode == 0)
            {
                BackButtonVisibility = true;
                Navigate("ChatView");
            }
            else
            {
                MessageBox.Show(TranslateEnterCode(enterCode), "Error");
            }
        }

        private void PerformEnterMessenger()
        {
            HelloViewVisibility = false;
            Navigate("ChatView");
            Navigate("LoginView");
        }
    }
}
