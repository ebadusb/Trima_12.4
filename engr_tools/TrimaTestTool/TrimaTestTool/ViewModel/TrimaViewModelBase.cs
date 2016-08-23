using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using BCT.Common;
using System.Net;
using BCT.Device.Interface;

namespace TrimaTestTool.ViewModel
{
    public class TrimaViewModelBase : ViewModelBase
    {
        /// <summary>
        /// This is the ip address of trima
        /// </summary>
        protected IPAddress IPAddress { get; set; }

        /// <summary>
        /// This is the Trima device
        /// </summary>
        public ITrima Trima { get; protected set; }

        public TrimaViewModelBase(IPAddress address)
        {
            IPAddress = address;
        }
    }
}
