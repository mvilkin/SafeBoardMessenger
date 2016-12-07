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
        [DllImport("MessengerBase.dll", CallingConvention = CallingConvention.Cdecl)]
        public static extern void RecvMessage(StringBuilder text);

        private string _text;
        public string Text
        {
            get { return _text; }
            set { SetProperty(ref _text, value); }
        }

        public void Receive()
        {
            var text = new StringBuilder(50);
            RecvMessage(text);
            Text += text + "\n";
        }
    }
}
