using System;
using System.Collections.Generic;
using System.Linq;
using System.Runtime.InteropServices;
using System.Text;
using System.Threading;
using System.Threading.Tasks;
using Microsoft.Practices.Unity.Utility;
using Prism.Mvvm;

namespace MessengerUI.Controls
{
    public class RecvMessageControl : BindableBase
    {
        [UnmanagedFunctionPointer(CallingConvention.StdCall)]
        delegate void OnMessageReceived(string message);
        [DllImport("MessengerBase.dll", CallingConvention = CallingConvention.Cdecl)]
        static extern void ReceiveNewMessages(StringBuilder from, OnMessageReceived callbackPointer);

        private ChatViewControl _chatViewCtrl;
        public ChatViewControl ChatViewCtrl
        {
            get { return _chatViewCtrl; }
            set { SetProperty(ref _chatViewCtrl, value); }
        }

        private string _sender;
        public string Sender
        {
            get { return _sender; }
            set { SetProperty(ref _sender, value); }
        }

        private CancellationTokenSource _cancellToken;

        public void StartReceiving()
        {
            OnMessageReceived recvCallback = message => { ChatViewCtrl.Text = message; };
            _cancellToken = new CancellationTokenSource();
            Task.Factory.StartNew(() => { ReceiveNewMessages(new StringBuilder(Sender), recvCallback); }, _cancellToken.Token);
        }

        public void StopReceiving()
        {
            if (_cancellToken != null)
                _cancellToken.Cancel();
            _cancellToken = null;
        }
    }
}
