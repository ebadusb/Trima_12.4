using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;

namespace BCT.Device.Interface
{
    /// <summary>
    /// This is the delegate that will be called when a device 
    /// is connecting
    /// </summary>
    /// <param name="device"></param>
    public delegate void DeviceConnecting(IDevice device);

    public interface IDeviceServer : IDevice
    {
        DeviceConnecting Connecting { get; }

        void StartAcceptingConnection();
    }
}
