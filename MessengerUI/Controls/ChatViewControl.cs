using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;
using Prism.Mvvm;

namespace MessengerUI.Controls
{
    public class ChatViewControl : BindableBase
    {
        private String _text;
        public String Text
        {
            get { return _text; }
            set { SetProperty(ref _text, value); }
        }
    }
}
