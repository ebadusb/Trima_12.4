using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Net;
using System.Net.NetworkInformation;

using BCT.Common;
using BCT.Device.Interface;

namespace BCT.Device
{

    public class DeviceManager
    {
        private const int TIMEOUT = 2;
        static public IEnumerable<IDevice> Discover()
        {
            Ping ping = new Ping();
            IPAddress address = new IPAddress(new byte[] { 192, 168, 0, 8 });
            PingReply reply = ping.Send(address, 100);

            return null;
        }

        public static IEnumerable<PingReply> Ping(IEnumerable<IPAddress> addresses)
        {
            List<PingReply> replies = new List<PingReply>();
            if (addresses == null || addresses.Count() == 0)
                return replies;

            Ping ping = new Ping();
            foreach (IPAddress address in addresses)
                replies.Add(ping.Send(address, TIMEOUT ));

            return replies; 
        }

        public static PingReply Ping(string ipAddress)
        {
            IPAddress address = null;
            if (!IPAddress.TryParse(ipAddress, out address))
                return null;

            Ping ping = new Ping();
            return ping.Send(address);
        }

        static public IEnumerable<Node> GenerateNodes(Node parent, byte from, byte to)
        {
            if (from >= 255 || to >= 255)
                return new List<Node>();

            while( from <= to )
                parent.Children.Add(new Node() { Data = ++from });

            return parent.Children;
        }

        static public void GenerateNodes(Node parent, byte[] from, byte[] to)
        {
            if (from == null || from.Length != 4 || to == null || to.Length != 4)
                return;

            IEnumerable<Node> ret = GenerateNodes(parent, from[0], to[0]);

            foreach (Node level2 in parent.Children)
            {
                var children2 = GenerateNodes(level2, from[1], to[1]);
                foreach (Node level3 in children2)
                {
                    var children3 = GenerateNodes(level3, from[2], to[2]);
                    foreach (Node level4 in children3)
                        GenerateNodes(level4, from[3], to[3]);
                }
            }
        }

        static public IDevice Discover(IPAddress address)
        {

            return null;
        }

        public class Node
        {
            private List<Node> m_children;

            public byte Data { get; set; }

            public List<Node> Children
            {
                get
                {
                    if (m_children == null)
                        return m_children = new List<Node>();
                    return m_children;
                }
            }
        }
    }
}
