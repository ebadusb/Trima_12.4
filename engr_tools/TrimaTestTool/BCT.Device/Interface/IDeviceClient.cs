using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Net;

namespace BCT.Device.Interface
{
    public delegate void OnConnected();

    public interface IDeviceClient : IDevice
    {
        /// <summary>
        /// Property to indicate that this device
        /// is connected to a device
        /// </summary>
        bool IsConnected { get; }

        /// <summary>
        /// This is the property for delegate when device is connected
        /// </summary>
        OnConnected OnConnected { get; }

        /// <summary>
        /// This 
        /// </summary>
        /// <param name="address"></param>
        /// <param name="port"></param>
        void Connect(IPAddress address, int port);

    }
}
