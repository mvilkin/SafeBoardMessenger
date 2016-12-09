using System;
using System.Collections.Generic;
using System.Linq;
using System.Runtime.InteropServices;
using System.Text;
using System.Threading.Tasks;

namespace MessengerUI.Controls
{
    public class ExitControl
    {
        [DllImport("MessengerBase.dll", CallingConvention = CallingConvention.Cdecl)]
        public static extern void ExitMessenger();

        public void Exit()
        {
            ExitMessenger();
        }
    }
}
