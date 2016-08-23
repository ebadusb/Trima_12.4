using System;
using System.Text;
using System.Collections.Generic;
using System.Linq;
using System.IO;
using Microsoft.VisualStudio.TestTools.UnitTesting;
using BCT.Device.Utility;

namespace BCT.Device.UnitTest
{
    [TestClass]
    public class FtpTest
    {
        [TestMethod]
        public void RequestFileTest()
        {
            MemoryStream ret = Ftp.RequestFile("172.21.3.199", "config/cal.dat");

            Assert.IsNotNull(ret);
            Assert.AreEqual(2553, ret.Length);
        }
    }
}
