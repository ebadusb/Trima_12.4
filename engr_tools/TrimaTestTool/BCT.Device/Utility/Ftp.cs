using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Net;
using System.Threading;
using System.IO;
using BCT.Common;

namespace BCT.Device.Utility
{
    public sealed class Ftp
    {
        private static string david = Extension.GetPropertyName(() => Ftp.david);
        private static string baseUriFormat = @"ftp://{0}/{1}";

        static public MemoryStream RequestFile(string ipAddress, string filePath)
        {
            MemoryStream stream = null;
            Uri address = new Uri(string.Format(baseUriFormat, ipAddress, filePath));
            FtpWebRequest request = (FtpWebRequest)WebRequest.Create(address);
            request.Credentials = new NetworkCredential(david, david);
            request.Method = WebRequestMethods.Ftp.DownloadFile;

            try
            {
                FtpWebResponse response = (FtpWebResponse)request.GetResponse();
                byte[] buffer = new byte[1024];
                stream = new MemoryStream();
                int readBytes = 0;
                do
                {
                    readBytes = response.GetResponseStream().Read(buffer, 0, buffer.Length);
                    if (readBytes > 0)
                        stream.Write(buffer, 0, readBytes);
                } while (readBytes != 0);
            }
            catch (Exception)
            {

            }
            return stream;
        }
    }
}
