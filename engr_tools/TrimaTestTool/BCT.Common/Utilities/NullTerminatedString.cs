using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;

namespace BCT.Common.Utilities
{
    public class NullTerminatedString
    {
        static public string GetASCIIString(byte[] data)
        {
            string str = Encoding.ASCII.GetString(data);
            string[] split = str.Split(new char[] { '\0' });
            if (split.Length > 0)
                return split[0];
            return str;
        }
    }
}
