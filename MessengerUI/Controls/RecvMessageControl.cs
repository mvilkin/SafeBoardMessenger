using System;
using System.Collections.Generic;
using System.Linq;
using System.Runtime.InteropServices;
using System.Text;
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

        private string _text;
        public string Text
        {
            get { return _text; }
            set { SetProperty(ref _text, value); }
        }

        private string _sender;
        public string Sender
        {
            get { return _sender; }
            set { SetProperty(ref _sender, value); }
        }

        private Task ReceivingTask;

        public void StartReceiving()
        {
            OnMessageReceived recvCallback = message => { Text = message; };
            ReceivingTask = Task.Run(() => { ReceiveNewMessages(new StringBuilder(Sender), recvCallback); });
        }

        public void StopReceiving()
        {
            ReceivingTask.KILL!!!;
        }
    }
}
