using System;
using System.Collections.Generic;
using System.Linq;
using System.Runtime.InteropServices;
using System.Security.Permissions;
using System.Text;
using System.Threading;
using System.Threading.Tasks;
using Microsoft.Practices.Unity.Utility;
using Prism.Mvvm;

namespace MessengerUI.Controls
{
    public class RecvMessageControl : BindableBase
    {
        [UnmanagedFunctionPointer(CallingConvention.StdCall, CharSet = CharSet.Unicode)]
        delegate void OnMessageReceived(StringBuilder message);
        [DllImport("MessengerBase.dll", CharSet = CharSet.Unicode, CallingConvention = CallingConvention.Cdecl)]
        static extern void StartReceiveNewMessages(StringBuilder from, OnMessageReceived callback);
        [DllImport("MessengerBase.dll", CharSet = CharSet.Unicode, CallingConvention = CallingConvention.Cdecl)]
        static extern void StopReceiveNewMessages();

        private ChatViewControl _chatViewCtrl;
        public ChatViewControl ChatViewCtrl
        {
            get { return _chatViewCtrl; }
            set { SetProperty(ref _chatViewCtrl, value); }
        }

        private String _sender;
        public String Sender
        {
            get { return _sender; }
            set { SetProperty(ref _sender, value); }
        }

        private Thread _receivingThread;

        public void StartReceiving()
        {
            _receivingThread = new Thread(ReceivingProcess);
            _receivingThread.Start();
        }

        private void ReceivingProcess()
        {
            OnMessageReceived recvCallback = message => { ChatViewCtrl.Text = message.ToString(); };
            StartReceiveNewMessages( new StringBuilder(Sender), recvCallback);
        }

        public void StopReceiving()
        {
            StopReceiveNewMessages();
        }
    }
}
