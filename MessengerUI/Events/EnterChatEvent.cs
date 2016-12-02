using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;
using Prism.Events;

namespace MessengerUI.Events
{
    public class EnterChatEvent : PubSubEvent<EnterChatEventData>
    {
    }

    public class EnterChatEventData
    {
        public string Login { get; set; }
        public string Server { get; set; }
    }
}
