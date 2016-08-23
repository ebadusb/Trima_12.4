using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;

namespace BCT.Device.Interface
{
    public enum DataError
    {
        None = 0,
        /// <summary>
        /// Data being parsed is insufficient
        /// </summary>
        InsufficientLength, 
    }

    public interface IDeviceData
    {
        /// <summary>
        /// Indicates presence of dataerror
        /// </summary>
        DataError Error { get; }
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
