using System;
using System.Text;
using System.Collections.Generic;
using System.Linq;
using System.Net;
using Microsoft.VisualStudio.TestTools.UnitTesting;

namespace BCT.Device.UnitTest
{
    /// <summary>
    /// Summary description for UnitTest1
    /// </summary>
    [TestClass]
    public class DeviceManagerTest
    {
        public DeviceManagerTest()
        {
            //
            // TODO: Add constructor logic here
            //
        }

        private TestContext testContextInstance;

        /// <summary>
        ///Gets or sets the test context which provides
        ///information about and functionality for the current test run.
        ///</summary>
        public TestContext TestContext
        {
            get
            {
                return testContextInstance;
            }
            set
            {
                testContextInstance = value;
            }
        }

        #region Additional test attributes
        //
        // You can use the following additional attributes as you write your tests:
        //
        // Use ClassInitialize to run code before running the first test in the class
        // [ClassInitialize()]
        // public static void MyClassInitialize(TestContext testContext) { }
        //
        // Use ClassCleanup to run code after all tests in a class have run
        // [ClassCleanup()]
        // public static void MyClassCleanup() { }
        //
        // Use TestInitialize to run code before running each test 
        // [TestInitialize()]
        // public void MyTestInitialize() { }
        //
        // Use TestCleanup to run code after each test has run
        // [TestCleanup()]
        // public void MyTestCleanup() { }
        //
        #endregion

        [TestMethod]
        public void DiscoverTest()
        {
            DeviceManager.Discover();
        }
        [TestMethod]
        public void GenerateNodesTest()
        {
            DeviceManager.Node node = new DeviceManager.Node();
            DeviceManager.GenerateNodes(node, 0, 254);

            Assert.AreEqual(255, node.Children.Count, "count is not 255");


        }
        [TestMethod]
        public void GenerateNodesRangeTest()
        {
            DeviceManager.Node node = new DeviceManager.Node();
            DeviceManager.GenerateNodes(node, new byte[]{172,21,2,1}, new byte[]{172,21,3,1});

            Assert.AreEqual(1, node.Children.Count, "count is not 1");
            node = node.Children.FirstOrDefault();
            Assert.IsNotNull(node, "node is null");
            Assert.AreEqual(1, node.Children, "none in level1");
            node = node.Children.FirstOrDefault();
            Assert.IsNotNull(node, "node is null");
            Assert.AreEqual(1, node.Children, "none in level2");
            node = node.Children.FirstOrDefault();
            Assert.IsNotNull(node, "node is null");
            Assert.AreEqual(2, node.Children, "none in level3");
            node = node.Children.FirstOrDefault();
            Assert.IsNotNull(node, "node is null");
            Assert.AreEqual(255, node.Children, "none in level3");

        }
 
        [TestMethod]
        public void PingTest()
        {
            DeviceManager.Node node = new DeviceManager.Node();
            DeviceManager.GenerateNodes(node, 0, 254);

            Assert.AreEqual(255, node.Children.Count);

            List<IPAddress> addresses = new List<IPAddress>();
            foreach (DeviceManager.Node child in node.Children)
                addresses.Add(new IPAddress( new byte[]{ 192, 168, 1, child.Data }));

            var replies = DeviceManager.Ping(addresses);
            Assert.IsNotNull(replies);

            var count = replies.Where(c => c.Status == System.Net.NetworkInformation.IPStatus.Success).Count();

            Assert.IsTrue(count > 0);
        }
    }

}
