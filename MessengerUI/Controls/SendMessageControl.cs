﻿using System;
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
        [UnmanagedFunctionPointer(CallingConvention.StdCall, CharSet = CharSet.Unicode)]
        delegate void OnMessageSent(StringBuilder message);
        [DllImport("MessengerBase.dll", CharSet = CharSet.Unicode, CallingConvention = CallingConvention.Cdecl)]
        static extern void SendNewMessage(StringBuilder recipient, StringBuilder text, OnMessageSent callbackPointer);

        private ChatViewControl _chatViewCtrl;
        public ChatViewControl ChatViewCtrl
        {
            get { return _chatViewCtrl; }
            set { SetProperty(ref _chatViewCtrl, value); }
        }

        private String _text;
        public String Text
        {
            get { return _text; }
            set { SetProperty(ref _text, value); }
        }

        private String _recipient;
        public String Recipient
        {
            get { return _recipient; }
            set { SetProperty(ref _recipient, value); }
        }

        public void Send()
        {
            OnMessageSent recvCallback = message => { ChatViewCtrl.Text = message.ToString(); };
            SendNewMessage(new StringBuilder(Recipient), new StringBuilder(Text), recvCallback);
            Text = String.Empty;
        }
    }
}
