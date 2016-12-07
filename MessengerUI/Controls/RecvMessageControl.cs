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
        static extern void RecvMessage(OnMessageReceived callbackPointer);

        private string _text;
        public string Text
        {
            get { return _text; }
            set { SetProperty(ref _text, value); }
        }

        public void StartReceiving()
        {
            OnMessageReceived recvCallback = message => { Text += message + "\n"; };
            Task mytask = Task.Run(() =>
            {
                RecvMessage(recvCallback);
            });
        }
    }
}
