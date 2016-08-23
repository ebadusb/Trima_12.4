using System;
using System.Text;
using System.Collections.Generic;
using System.Linq;
using Microsoft.VisualStudio.TestTools.UnitTesting;
using BCT.Common.Utilities;

namespace BCT.Device.UnitTest
{
    [TestClass]
    public class UtilitiesTest
    {
        [TestMethod]
        public void SerialNumberTest()
        {
            string serialNumber = SerialNumberConverter.GetSerialNumber("172.21.14.72");

            Assert.AreEqual("1T03656", serialNumber, "serialNumber not equal");
        }
    }
}
