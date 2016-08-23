using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;

namespace BCT.Common.Utilities
{
    static public class ConvertExtension
    {
        public static string ToString(this byte[] bytes, int count )
        {
            StringBuilder builder = new StringBuilder();
            if (bytes == null || bytes.Length < count)
                throw new InvalidOperationException("Cannot convert bytes to string");

            for (int i = 0; i < count; i++)
                builder.Append(Convert.ToChar(bytes[i]));
            return builder.ToString();
        }

        public static long ToLong(this byte[] bytes, int size)
        {
            return ToLong(bytes, size, 0);
        }

        public static long ToLong(this byte[] bytes, int size, int offset)
        {
            long returnedInteger = 0;
            switch (size)
            {
                case 2:
                    returnedInteger = BitConverter.ToInt16(bytes, offset);
                    break;
                case 4:
                    returnedInteger = BitConverter.ToInt32(bytes, offset);
                    break;
                case 8:
                    returnedInteger = BitConverter.ToInt64(bytes, offset);
                    break;
            }
            return returnedInteger;
        }

        public static string ToIntHex(this byte[] bytes)
        {
            int input = ToInt(bytes, bytes.Length, 0);
            return input.ToString("x");
        }

        public static int ToInt(this byte[] bytes, int size)
        {
            return ToInt(bytes, size, 0);
        }

        public static int ToInt(this byte[] bytes, int size, int offset)
        {
            int returnedInteger = 0;
            switch (size)
            {
                case 2:
                    returnedInteger = BitConverter.ToInt16(bytes, offset);
                    break;
                case 4:
                    returnedInteger = BitConverter.ToInt32(bytes, offset);
                    break;
            }
            return returnedInteger;
        }

        public static string ToUTF8(this byte[] bytes)
        {
            if (bytes == null || bytes.Length == 0)
                return null;
            return ToUTF8(bytes, bytes.Length, 0);
        }

        public static string ToUTF8(this byte[] bytes, int size)
        {
            return ToUTF8(bytes, size, 0);
        }

        public static string ToUTF8(this byte[] bytes, int size, int offset)
        {
            return Encoding.UTF8.GetString(bytes, offset, size);
        }
    }
}
