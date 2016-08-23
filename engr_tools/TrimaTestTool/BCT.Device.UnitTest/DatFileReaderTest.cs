using System;
using System.Text;
using System.Collections.Generic;
using System.Linq;
using Microsoft.VisualStudio.TestTools.UnitTesting;
using BCT.Device.Utility;

namespace BCT.Device.UnitTest
{
    [TestClass]
    public class DatFileReaderTest
    {
        [TestMethod]
        [DeploymentItem(@"Resources\cal.dat")]
        public void GetSectionTest()
        {
            using (DatFileReader reader = DatFileReader.Open("cal.dat"))
            {
                var list = reader.GetSection("CPS");

                Assert.IsNotNull(list);
                Assert.AreEqual(3, list.Count());

                Assert.AreEqual("0.68", list["cps_gain"]);
                Assert.AreEqual("-504.07", list["cps_offset"]);
                Assert.AreEqual("30072012", list["cps_last_cal_date"]);
            }
        }
    }
}
