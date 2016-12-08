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
        [DllImport("MessengerBase.dll", CallingConvention = CallingConvention.Cdecl)]
        public static extern void ExitMessenger();

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
            ExitMessenger();
            Navigate("LoginView");
            BackButtonVisibility = false;
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
                MessageBox.Show("Sorry, error #" + enterCode + " occured!");
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
