using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Runtime.Serialization;
using System.IO;

namespace BCT.Device.Interface
{
    /// <summary>
    /// 
    /// </summary>
    public interface ITrima : IDeviceClient
    {
        /// <summary>
        /// This will initiate connection
        /// using TED interface
        /// </summary>
        bool StartTEDConnection();
        /// <summary>
        /// Method to initiate connection with
        /// trima on Vista Interface
        /// </summary>
        bool StartVistaConnection();

        void GetAPSAutoZero();
    }

    public interface ITrimaData
    {
        /// <summary>
        /// Property to indicate the length of the 
        /// data
        /// </summary>
        int Length { get; }
        /// <summary>
        /// Method to serialize the data into bytes
        /// </summary>
        /// <returns></returns>
        byte[] Serialize();
        /// <summary>
        /// Method to deserialize bytes to its
        /// data members
        /// </summary>
        /// <param name="buffer"></param>
        void Deserialize(byte[] buffer);
    }
}
