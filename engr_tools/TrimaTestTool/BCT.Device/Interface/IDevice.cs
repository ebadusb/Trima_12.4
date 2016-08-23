using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Net;

namespace BCT.Device.Interface
{
    public delegate void DeviceFound(IDevice device);

    public delegate void DeviceExceptionHandler( Exception e);

    public delegate void DeviceConnectionChanged( bool connected );

    public interface IDevice
    {
        /// <summary>
        /// This is the name of the device
        /// </summary>
        string Name { get; }
        /// <summary>
        /// Thisis the version of the device
        /// </summary>
        string Version { get; }
        /// <summary>
        /// This is the ipaddress of the device
        /// </summary>
        string IPAddress { get; }
        /// <summary>
        /// This is the serial number of the device
        /// </summary>
        string SerialNumber { get; }

        /// <summary>
        /// This is the handler for the device exception
        /// </summary>
        DeviceExceptionHandler DeviceExceptionHandler { get; set; }

        DeviceConnectionChanged DeviceConnectionChanged { get; set; } 

        void OpenConnection();

        void CloseConnection();

    }
}
