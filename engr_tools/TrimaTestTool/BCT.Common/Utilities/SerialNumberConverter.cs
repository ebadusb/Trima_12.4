using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Net;

namespace BCT.Common.Utilities
{
    public class SerialNumberConverter
    {
        public static string GetSerialNumber(string ipAddress)
        {
            string retVal = string.Empty;
            IPAddress ipRetVal = null;
            if (IPAddress.TryParse(ipAddress, out ipRetVal))
            {
                byte[] addrBytes = ipRetVal.GetAddressBytes();
                int intAddress = (int)((addrBytes[2]) * 256) + (int)addrBytes[3];
                retVal = "1T" + intAddress.ToString("00000");
            }
            return retVal;
        }

        public static string GetIPAddress(string serialNumber)
        {
            return null;
        }
    }
}
