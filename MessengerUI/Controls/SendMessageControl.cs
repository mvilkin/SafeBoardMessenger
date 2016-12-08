using System;
using System.Collections.Generic;
using System.Linq;
using System.Runtime.InteropServices;
using System.Text;
using System.Threading.Tasks;
using Prism.Mvvm;

namespace MessengerUI.Controls
{
    public class SendMessageControl : BindableBase
    {
        [UnmanagedFunctionPointer(CallingConvention.StdCall)]
        delegate void OnMessageSent(string message);
        [DllImport("MessengerBase.dll", CallingConvention = CallingConvention.Cdecl)]
        static extern void SendMessage(StringBuilder to, StringBuilder text, OnMessageSent callbackPointer);

        private ChatViewControl _chatViewCtrl;
        public ChatViewControl ChatViewCtrl
        {
            get { return _chatViewCtrl; }
            set { SetProperty(ref _chatViewCtrl, value); }
        }

        private string _text;
        public string Text
        {
            get { return _text; }
            set { SetProperty(ref _text, value); }
        }

        private string _recipient;
        public string Recipient
        {
            get { return _recipient; }
            set { SetProperty(ref _recipient, value); }
        }

        public void Send()
        {
            OnMessageSent recvCallback = message => { ChatViewCtrl.Text = message; };
            SendMessage(new StringBuilder(Recipient), new StringBuilder(Text), recvCallback);
            Text = string.Empty;
        }
    }
}
