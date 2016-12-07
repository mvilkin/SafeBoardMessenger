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
        [DllImport("MessengerBase.dll", CallingConvention = CallingConvention.Cdecl)]
        public static extern void SendMessage(StringBuilder to, StringBuilder text);
        [DllImport("MessengerBase.dll", CallingConvention = CallingConvention.Cdecl)]
        public static extern void AddCli();

        private string _text;
        public string Text
        {
            get { return _text; }
            set { SetProperty(ref _text, value); }
        }

        public void Send()
        {
            AddCli();
            var msgTo = new StringBuilder("add");
            var msgText = new StringBuilder(Text);
            SendMessage(msgTo, msgText);
        }
    }
}
