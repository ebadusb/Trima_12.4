using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.IO;
using System.Net;
using System.Net.Sockets;
using System.Net.NetworkInformation;
using System.Threading;

using BCT.Device.Interface;
using BCT.Common.Utilities;
using BCT.Device.Utility;

namespace BCT.Device.Trima.V6_3
{
    /// <summary>
    /// delegate to update Control Status
    /// </summary>
    /// <param name="msg"></param>
    public delegate void TedControlStatusUpdate(TEDIf.TED_ControlHwStatusMsg msg);
    /// <summary>
    /// delegate to update Safety Status
    /// </summary>
    /// <param name="msg"></param>
    public delegate void TedSafetyStatusUpdate(TEDIf.TED_SafetyHwStatusMsg msg);

    public delegate void VifPeriodicStatusUpdate(VISIf.trimaPeriodicStatusMsg msg );

    public delegate void VifMachineStatusUpdate(VISIf.trimaMachineStatusMsg msg);

    public delegate void BroadcastUpdate(TEDIf.TED_BroadcastMsg msg);

    public class Trima : TrimaBase
    {
        private const int TED_SOM = 0xDECAF;
        private const int TED_CONNECT_PORT = 36006;
        private const int TED_STATUS_PORT = 36007;
        private const int TED_SERVER_PORT = 7000;
        private const int VIF_SERVER_PORT = 7001;
        private const int VIF_STATUS_PORT = 999;
        private const int TED_PERIODIC_STATUS_INTERVAL = 500; //ms;
        private UdpClient m_udp = null;
        private UdpClient m_tedStatusUdp = null;
        private TcpClient m_vifCmd = null;
        private UdpClient m_vifStatusUdp = null;
        private int m_sequence = 1;
        private Thread m_pingThread;

        public string HardwareRevision { get; set; }
        public string CurrentLogFile { get; set; }
        public string TED_CRC { get; set; }
        public string RealeaseNumber { get; set; }
        public string BuildNumber { get; set; }
        public string VIPIfVersion { get; set; }
        public string TEDIfVersion { get; set; }
        /// <summary>
        /// delegate to connect to control update
        /// </summary>
        public TedControlStatusUpdate ControlHwUpdate;
        /// <summary>
        /// delegate to connect to safety update
        /// </summary>
        public TedSafetyStatusUpdate SafetyHwUpdate;
        /// <summary>
        /// delegate to connecto to vif status update
        /// </summary>
        public VifPeriodicStatusUpdate VifPeriodicStatusUpdate;

        public VifMachineStatusUpdate VifMachineStatusUpdate;

        public BroadcastUpdate BroadcastUpdate;

        public override DeviceConnectionChanged DeviceConnectionChanged { get; set; }

        public Trima(IPAddress address)
        {
            m_address = address;
        }

        public override void OpenConnection()
        {
            if (!StartTEDConnection())
                return;
            StartVistaConnection();
        }

        public override void CloseConnection()
        {
            if (m_vifCmd != null)
            {
                m_vifCmd.Close();
                m_vifCmd = null;
            }

            if (m_vifStatusUdp != null)
            {
                m_vifStatusUdp.Close();
                m_vifStatusUdp = null;
            }

            if (m_udp != null)
            {
                m_udp.Close();
                m_udp = null;
            }

            if (m_tedStatusUdp != null)
            {
                m_tedStatusUdp.Close();
                m_tedStatusUdp = null;
            }

            if (m_visIf != null)
            {
                m_visIf.Dispose();
                m_visIf = null;
            }

            if (DeviceConnectionChanged != null)
                DeviceConnectionChanged(false);
        }

        #region VISTA CONNECTION
        private VISIf m_visIf;

        public override bool StartVistaConnection()
        {
            m_visIf = new VISIf();
            m_visIf.VifPeriodicStatusUpdate = this.VifPeriodicStatusUpdate;
            m_visIf.VifMachineStatusUpdate = this.VifMachineStatusUpdate;
            m_visIf.DeviceConnectionChanged = this.DeviceConnectionChanged;
            m_visIf.StartVistaConnection(m_address);
            return true;

        }
        public /*override*/ void oldStartVistaConnection()
        {
            m_sequence = 1;
            m_vifCmd = new TcpClient();
            VISIf.trimaVistaHeader header = new VISIf.trimaVistaHeader();
            header.bodyCrc = 0;
            header.timestamp = EpochTime.Now;
            header.messageType = VISIf.VIP_START_PERIODIC_STATUS;
            header.sequenceNumber = m_sequence;
            header.som = VISIf.VIP_SOM_ID;
            header.lengthInBytes = 12;

            VISIf.startPeriodicStatusMsg msg = new VISIf.startPeriodicStatusMsg();
            msg.hdr = header;
            msg.portNumber = VIF_STATUS_PORT;
            msg.receiveTimeout = 25;
            msg.repeatPeriod = 3000;

            try
            {
                m_vifCmd.Connect(new IPEndPoint(m_address, VISIf.VIP_LISTEN_PORT));

                if (m_vifCmd.Connected)
                {
                    m_vifStatusUdp = new UdpClient(VIF_STATUS_PORT);
                    m_vifStatusUdp.BeginReceive(new AsyncCallback(this.VistaPeriodicStatusCallback), this);

                    byte[] buffer = Serializer.Serialize(msg);
                    int count = m_vifCmd.Client.Send(buffer, buffer.Length, SocketFlags.None);

                    byte[] rec = new byte[1000];
                    m_vifCmd.Client.Receive(rec);

                    VISIf.trimaVistaMsg reply = ProcessVifReply(rec);

                    if (reply.hdr.status == VISIf.VIP_OK)
                    {
                        m_pingThread = new System.Threading.Thread(new System.Threading.ThreadStart(
                            delegate()
                            {
                                while (true)
                                {
                                    {
                                        header.lengthInBytes = 0;
                                        header.sequenceNumber = ++m_sequence;
                                        header.messageType = VISIf.VIP_PING;
                                        byte[] data = Serializer.Serialize(header);
                                        m_vifCmd.Client.Send(data);
                                        byte[] response = new byte[1000];
                                        m_vifCmd.Client.Receive(response);

                                        VISIf.trimaVistaMsg responseData = ProcessVifReply(response);
                                    }
                                    System.Threading.Thread.Sleep(20 * 1000);
                                }
                            }
                            ));
                        m_pingThread.Start();
                    }
                }

            }
            catch (Exception e)
            {
                if (DeviceExceptionHandler != null)
                    DeviceExceptionHandler(e);
            }
        }

        private void VistaConnectionCallback(IAsyncResult result)
        {
           
        }
        #endregion //vista connection

        #region TED Connect

        private bool TryDiscover()
        {
            m_udp = new UdpClient(TED_CONNECT_PORT);

            ///temporary, Trima is booting and have responded to 
            ///ping but does not allow ted/vif connection
            int count = 0;
            do
            {
                try
                {
                    Discover();
                    break;
                }
                catch (Exception)
                {
                    count++;
                    if (count > 5)
                        break;
                    System.Threading.Thread.Sleep(5000);
                }

            } while (true);

            if (count > 5)
            {
                m_udp.Close();
                m_udp = null;
                if (DeviceExceptionHandler != null)
                    DeviceExceptionHandler(new Exception("Trima is not responding"));
                return false;
            }
            return true;

        }
        private void Discover()
        {
            TEDIf.TED_Header header = new TEDIf.TED_Header();
            header.bodyCRC = 0;
            header.length = 0;
            header.time = EpochTime.Now;
            header.msgId = (int)TEDIf.MessageId.TED_BROADCAST_REQUEST;
            header.som = 0xDECAF;

            byte[] buffer = Serializer.Serialize(header);
            m_udp.Send(buffer, buffer.Length, new IPEndPoint(m_address, TED_SERVER_PORT));

            IPEndPoint endpoint = null;
            buffer = m_udp.Receive(ref endpoint);
            TEDIf.TED_ReplyMsg reply = TEDProcessReply(buffer);
            if (reply.hdr.msgId == (uint)TEDIf.MessageId.TED_BROADCAST)
            {
                using (BinaryReader reader = new BinaryReader(new MemoryStream(buffer)))
                {
                    TEDIf.TED_BroadcastMsg msg = new TEDIf.TED_BroadcastMsg();
                    msg.hdr = new TEDIf.TED_Header();
                    Serializer.Deserialize(reader, msg);

                    this.SerialNumber = NullTerminatedString.GetASCIIString(msg.trimaSerialNumber);

                    if (BroadcastUpdate != null)
                        BroadcastUpdate(msg);

                    MemoryStream cal = Ftp.RequestFile(m_address.ToString(), "config/cal.dat");
                    //if (cal != null)
                    //    InitializeSensorConverters(cal);
                    HandleCalibration(cal);
                }
            }
        }

        private void HandleCalibration(MemoryStream stream)
        {
            if( !Directory.Exists(SerialNumber))
                Directory.CreateDirectory(SerialNumber);
            string path = Path.Combine(SerialNumber, "cal.dat");
            if (stream != null)
            {
                FileStream writer = new FileStream(path, FileMode.Create, FileAccess.Write);
                writer.Write(stream.GetBuffer(), 0, (int)stream.Length);
                //StreamWriter writer = new StreamWriter(path);
                //writer.Write(stream.GetBuffer());
                writer.Close();
            }
            else
            {
                FileStream reader = new FileStream(path, FileMode.Open);
                byte[] buffer = new byte[reader.Length];
                reader.Read(buffer, 0, buffer.Length);
                stream = new MemoryStream(buffer);
                reader.Close();
            }
            if (stream != null)
                InitializeSensorConverters(stream);
        }

        private void InitializeSensorConverters(Stream stream)
        {
            using (DatFileReader calreader = DatFileReader.Open(stream))
            {
                var list = calreader.GetSection("APS");
                APSConverter.Init(list);
                list = calreader.GetSection("CPS");
                CPSConverter.Init(list);
            }
        }

        public override bool StartTEDConnection()
        {
            if (!TryDiscover())
                return false;

            TEDIf.TED_Header header = new TEDIf.TED_Header();
            header.bodyCRC = 0;
            header.length = 0;
            header.time = EpochTime.Now;
            header.msgId = (int)TEDIf.MessageId.TED_CONNECT_REQUEST;
            header.som = 0xDECAF;


            TEDIf.TED_ReplyMsg reply = null;

            byte[] buffer = Serializer.Serialize(header);
            m_udp.Send(buffer, buffer.Length, new IPEndPoint(m_address, TED_SERVER_PORT));

            IPEndPoint endpoint = null;
            buffer = m_udp.Receive(ref endpoint);

            reply = TEDProcessReply(buffer);
            if (reply != null && reply.status == TEDIf.TED_Status.TED_CONNECTION_ALLOWED_OPERATIONAL)
            {
                StartTEDPeriodicStatus();
                //StartTEDAPSAutoZero();
            }
            return true;
        }

        public override void GetAPSAutoZero()
        {
            TEDIf.TED_Header header = new TEDIf.TED_Header();
            header.length = 0;
            header.bodyCRC = 0;
            header.time = EpochTime.Now;
            header.msgId = (int)TEDIf.MessageId.TED_REQUEST_APS_AUTOZERO_VALUE;
            header.som = TED_SOM;

            byte[] buffer = Serializer.Serialize(header);
            try
            {
                m_udp.Send(buffer, buffer.Length, new IPEndPoint(m_address, TED_SERVER_PORT));
                IPEndPoint refipEndPoint = null; 
                byte[] rec = m_udp.Receive(ref refipEndPoint);
                TEDIf.TED_ReplyMsg reply = TEDProcessReply(rec);
                ///this is giving a wrong msg id of 11011
                if (reply.hdr.msgId == (uint)TEDIf.MessageId.TED_APS_AUTOZERO_VALUE)
                {
                    TEDIf.TED_ApsAutoZeroValueMsg apsMsg = new TEDIf.TED_ApsAutoZeroValueMsg();
                    apsMsg.hdr = new TEDIf.TED_Header();

                    BinaryReader reader = new BinaryReader(new MemoryStream(rec));
                    Serializer.Deserialize(reader, apsMsg);
                    APSConverter.SetZero(apsMsg.rawValue, apsMsg.referenceValue);
                }
            }
            catch (Exception e)
            {
                if (DeviceExceptionHandler != null)
                    DeviceExceptionHandler(e);
                //TODO log here
            }

        }

        private TEDIf.TED_ReplyMsg TEDProcessReply(byte[] buffer)
        {
            TEDIf.TED_ReplyMsg msg = null;
            if (buffer == null)
                return msg;

            using (BinaryReader reader = new BinaryReader(new MemoryStream(buffer)))
            {
                msg = new TEDIf.TED_ReplyMsg();
                msg.hdr = new TEDIf.TED_Header();
                Serializer.Deserialize(reader, msg);

            }
            return msg;
        }

        private void ProcessPostTEDReply(TEDIf.TED_ReplyMsg reply, byte[] buffer)
        {
            if (reply.hdr.msgId ==  (uint)TEDIf.MessageId.TED_CONTROL_HARDWARE_STATUS )
            {
                TEDIf.TED_ControlHwStatusMsg msg = new TEDIf.TED_ControlHwStatusMsg();
                msg.hdr = new TEDIf.TED_Header();
                msg.acPump = new TEDIf.CHWPumpStatus();
                msg.inletPump = new TEDIf.CHWPumpStatus();
                msg.plasmaPump = new TEDIf.CHWPumpStatus();
                msg.plateletPump = new TEDIf.CHWPumpStatus();
                msg.returnPump = new TEDIf.CHWPumpStatus();
                
                
                BinaryReader reader = new BinaryReader( new MemoryStream(buffer));
                Serializer.Deserialize(reader, msg);

                if (ControlHwUpdate != null)
                    ControlHwUpdate(msg);
            }
            else if (reply.hdr.msgId == (uint)TEDIf.MessageId.TED_SAFETY_HARDWARE_STATUS)
            {
                TEDIf.TED_SafetyHwStatusMsg msg = new TEDIf.TED_SafetyHwStatusMsg();
                msg.hdr = new TEDIf.TED_Header();
                BinaryReader reader = new BinaryReader(new MemoryStream(buffer));
                Serializer.Deserialize(reader, msg);

                if (SafetyHwUpdate != null)
                    SafetyHwUpdate(msg);
            }
        }

        private void StartTEDPeriodicStatus()
        {
            TEDIf.TED_Header header = new TEDIf.TED_Header();
            header.length = 12;
            header.time = EpochTime.Now;
            header.msgId = (int)TEDIf.MessageId.TED_HW_PERIODIC_STATUS_REQUEST;
            header.som = TED_SOM;

            TEDIf.TED_HwPeriodicStatusRequestMsg req = new TEDIf.TED_HwPeriodicStatusRequestMsg();
            req.board = TEDIf.TED_BoardType.CONTROL;
            req.period = TED_PERIODIC_STATUS_INTERVAL;
            req.port = TED_STATUS_PORT;
            req.hdr = header;

            byte[] buffer = Serializer.Serialize(req);
            try
            {
                m_tedStatusUdp = new UdpClient(TED_STATUS_PORT);
                m_tedStatusUdp.BeginReceive(new AsyncCallback(TedPeriodicStatusCallback), this);
                m_udp.Send(buffer, buffer.Length, new IPEndPoint(m_address, TED_SERVER_PORT));
            }
            catch (Exception e)
            {
                if (DeviceExceptionHandler != null)
                    DeviceExceptionHandler(e);
                //TODO log here
            }

            IPEndPoint refipEndPoint = null;
            byte[] rec = m_udp.Receive(ref refipEndPoint);

            TEDIf.TED_ReplyMsg reply = TEDProcessReply(rec);

            req.board = TEDIf.TED_BoardType.SAFETY;

            buffer = Serializer.Serialize(req);
            try
            {
                m_udp.Send(buffer, buffer.Length, new IPEndPoint(m_address, TED_SERVER_PORT));
                rec = m_udp.Receive(ref refipEndPoint);
                reply = TEDProcessReply(rec);
            }
            catch (Exception e)
            {
                if (DeviceExceptionHandler != null)
                    DeviceExceptionHandler(e);
                //TODO: log here
            }

        }

        private void TedPeriodicStatusCallback(IAsyncResult result)
        {
            try
            {
                IPEndPoint endPoint = null;
                byte[] rec = m_tedStatusUdp.EndReceive(result, ref endPoint);

                TEDIf.TED_ReplyMsg reply = TEDProcessReply(rec);

                ProcessPostTEDReply(reply, rec);

                m_tedStatusUdp.BeginReceive(new AsyncCallback(TedPeriodicStatusCallback), this);
            }
            catch (Exception e)
            {
            }

        }

        private void StartPeriodicStatusCallback(IAsyncResult result)
        {
            IPEndPoint endPoint = null;
            byte[] rec = m_udp.EndReceive(result, ref endPoint);
            TEDIf.TED_ReplyMsg reply = TEDProcessReply(rec);
        }
        #endregion //TED CONNECTION

        #region VISTA callbacks

        private void VistaPeriodicStatusCallback(IAsyncResult result)
        {
            IPEndPoint endPoint = null;
            byte[] rec = this.m_vifStatusUdp.EndReceive(result, ref endPoint);

            VISIf.trimaVistaMsg reply = ProcessVifReply(rec);
            if (reply.hdr.messageType == VISIf.VIP_PERIODIC_STATUS_MSG)
                ProcessPostVifReply(rec);
            
            m_vifStatusUdp.BeginReceive(new AsyncCallback(VistaPeriodicStatusCallback), this);
        }

        private VISIf.trimaVistaMsg ProcessVifReply(byte[] buffer)
        {
            VISIf.trimaVistaMsg reply = new VISIf.trimaVistaMsg();
            reply.hdr = new VISIf.trimaVistaHeader();
            using (BinaryReader reader = new BinaryReader(new MemoryStream(buffer)))
                Serializer.Deserialize(reader, reply);
            return reply;
        }

        private void ProcessPostVifReply(byte[] buffer)
        {
            VISIf.trimaPeriodicStatusMsg msg = new VISIf.trimaPeriodicStatusMsg();
            msg.hdr = new VISIf.trimaVistaHeader();
            using(BinaryReader reader = new BinaryReader(new MemoryStream(buffer)))
            {
                Serializer.Deserialize(reader, msg);
            }
            if (VifPeriodicStatusUpdate != null)
                VifPeriodicStatusUpdate(msg);
        }
        #endregion
    }
}
