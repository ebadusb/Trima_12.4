using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.IO;
using System.Net;
using System.Net.Sockets;

using BCT.Common.Utilities;
using BCT.Device.Interface;
namespace BCT.Device.Trima.V6_3
{
    public partial class VISIf : IDisposable
    {

        public abstract class VISDeviceData : TrimaDeviceData
        {
            public override byte[] SerializeHeader( uint bodyCrc )
            {
                Header.bodyCrc = bodyCrc;
                byte[] headerBytes = Header.Serialize();
                Header.headerCrc = Crc32.Calculate(headerBytes, 0xFFFFFFFF) ^ 0xFFFFFFFF;
                return headerBytes.Concat(BitConverter.GetBytes(Header.headerCrc)).ToArray();
            }

            protected abstract trimaVistaHeader Header { get; }

            public override byte[] SerializeBody()
            {
                byte[] retVal = new byte[Length];
                using (BinaryWriter stream = new BinaryWriter(new MemoryStream()))
                {
                    SerializeBody(stream);
                    BinaryReader reader = new BinaryReader(stream.BaseStream);
                    reader.BaseStream.Seek(0, SeekOrigin.Begin);
                    retVal = reader.ReadBytes(Length);
                }
                return retVal;
            }

            public virtual void SerializeBody(BinaryWriter writer) { }
        }

        public partial class trimaVistaHeader : VISDeviceData
        {
            public trimaVistaHeader()
            {
                som = VISIf.VIP_SOM_ID;
            }

            public override int Length { get { return 32; } }

            protected override trimaVistaHeader Header
            {
                get { return this; }
            }

            public override byte[] Serialize()
            {
                return      BitConverter.GetBytes(som)
                    .Concat(BitConverter.GetBytes(messageType))
                    .Concat(BitConverter.GetBytes(sequenceNumber))
                    .Concat(BitConverter.GetBytes(status))
                    .Concat(BitConverter.GetBytes(lengthInBytes))
                    .Concat(BitConverter.GetBytes(timestamp))
                    .Concat(BitConverter.GetBytes(bodyCrc)).ToArray();
            }

            public override void SerializeBody(BinaryWriter writer)
            {
                writer.Write(som);
                writer.Write(messageType);
                writer.Write(sequenceNumber);
                writer.Write(status);
                writer.Write(lengthInBytes);
                writer.Write(timestamp);
                writer.Write(bodyCrc);
            }

            public override void Deserialize(BinaryReader stream)
            {
                som = stream.ReadUInt32();
                messageType = stream.ReadInt32();
                sequenceNumber = stream.ReadInt32();
                status = stream.ReadInt32();
                lengthInBytes = stream.ReadInt32();
                timestamp = stream.ReadInt32();
                bodyCrc = stream.ReadUInt32();
                headerCrc = stream.ReadUInt32();
            }
        }

        public partial class startPeriodicStatusMsg : VISDeviceData
        {
            public startPeriodicStatusMsg()
            {
                this.hdr = new trimaVistaHeader();
                this.hdr.messageType = VISIf.VIP_START_PERIODIC_STATUS;
                this.hdr.lengthInBytes = Length;
            }

            public override int Length
            {
                get{ return 12; }
            }

            protected override trimaVistaHeader Header
            {
                get { return this.hdr; }
            }

            public override byte[]  SerializeBody()
            {
                return      BitConverter.GetBytes(repeatPeriod)
                    .Concat(BitConverter.GetBytes(portNumber))
                    .Concat(BitConverter.GetBytes(receiveTimeout)).ToArray();
            }

            public override void SerializeBody(BinaryWriter writer)
            {
                writer.Write(repeatPeriod);
                writer.Write(portNumber);
                writer.Write(receiveTimeout);
            }

            public override void Deserialize(BinaryReader stream)
            {
                hdr.Deserialize(stream);
                repeatPeriod = stream.ReadInt32();
                portNumber = stream.ReadUInt32();
                receiveTimeout = stream.ReadUInt32();
            }
        }


        #region Members
        private const int VIF_SERVER_PORT = 7001;
        private const int VIF_STATUS_PORT = 999;
        const int HEADER_CRC_SIZE = 4;
        private int m_sequence = 0;
        private TcpClient m_vifCmd = null;
        private UdpClient m_vifStatusUdp = null;
        public VifPeriodicStatusUpdate VifPeriodicStatusUpdate;
        public VifMachineStatusUpdate VifMachineStatusUpdate;
        public DeviceConnectionChanged DeviceConnectionChanged;
        #endregion

        public delegate void MachineStatusUpdateEvent(startMachineStatusMsg msg);

        public void RequestStartMachineStatusUpdate( MachineStatusUpdateEvent handler, int sequence, int portNumber, int repeatPeriod )
        {
            VISIf.startMachineStatusMsg msg = new VISIf.startMachineStatusMsg();
            msg.portNumber = portNumber;
            msg.repeatPeriod = repeatPeriod;

            VISIf.trimaVistaHeader header = msg.hdr;
            header.messageType = VISIf.VIP_START_MACHINE_STATUS;
            header.sequenceNumber = sequence;
        }

        public void StartVistaConnection(IPAddress address)
        {
            m_sequence = 1;
            m_vifCmd = new TcpClient();
            m_vifStatusUdp = new UdpClient(VIF_STATUS_PORT);

            try
            {
                m_vifCmd.Connect(new IPEndPoint(address, VISIf.VIP_LISTEN_PORT));
            }
            catch (Exception e)
            {
                //TODO:
                m_vifCmd = null;
                m_vifStatusUdp = null;
                return;
            }
            if (!m_vifCmd.Connected)
            {
                //TODO: do some logging etc
                return;
            }
            if (DeviceConnectionChanged != null)
                DeviceConnectionChanged(true);

            StartPeriodicStatusMsgConnection();
            StartPeriodicMachineStatusMsgConnection();
        }

        private void StartPeriodicStatusMsgConnection()
        {
            VISIf.startPeriodicStatusMsg msg = new VISIf.startPeriodicStatusMsg();
            msg.portNumber = VIF_STATUS_PORT;
            msg.receiveTimeout = 25;
            msg.repeatPeriod = 3000;

            msg.hdr.sequenceNumber = m_sequence;
            m_vifStatusUdp.BeginReceive(new AsyncCallback(this.VistaPeriodicStatusCallback), this);

            byte[] buffer = msg.Serialize();
            int count = m_vifCmd.Client.Send(buffer, buffer.Length, SocketFlags.None);

            byte[] rec = new byte[1000];
            m_vifCmd.Client.Receive(rec);

            VISIf.trimaVistaMsg reply = ProcessVifReply(rec);
            if (reply.hdr.status == VISIf.VIP_OK)
            {

            }
        }

        private void StartPeriodicMachineStatusMsgConnection()
        {
            VISIf.startMachineStatusMsg msg = new startMachineStatusMsg();
            msg.portNumber = VIF_STATUS_PORT;
            msg.repeatPeriod = 3000;

            msg.hdr.sequenceNumber = m_sequence;
            m_vifStatusUdp.BeginReceive(new AsyncCallback(this.VistaPeriodicStatusCallback), this);

            byte[] buffer = msg.Serialize();
            int count = m_vifCmd.Client.Send(buffer, buffer.Length, SocketFlags.None);

            byte[] rec = new byte[1000];
            m_vifCmd.Client.Receive(rec);

            VISIf.trimaVistaMsg reply = ProcessVifReply(rec);
            if (reply.hdr.status == VISIf.VIP_OK)
            {
            }
        }

        private void VistaPeriodicStatusCallback(IAsyncResult result)
        {
            try
            {
                IPEndPoint endPoint = null;
                byte[] rec = this.m_vifStatusUdp.EndReceive(result, ref endPoint);

                VISIf.trimaVistaMsg reply = ProcessVifReply(rec);
                //if (reply.hdr.messageType == VISIf.VIP_PERIODIC_STATUS_MSG)
                ProcessPostVifReply(rec, reply);

                m_vifStatusUdp.BeginReceive(new AsyncCallback(VistaPeriodicStatusCallback), this);
            }
            catch (Exception e)
            {
            }
        }

        private VISIf.trimaVistaMsg ProcessVifReply(byte[] buffer)
        {
            VISIf.trimaVistaMsg reply = new VISIf.trimaVistaMsg();
            reply.hdr = new VISIf.trimaVistaHeader();
            using (BinaryReader reader = new BinaryReader(new MemoryStream(buffer)))
                Serializer.Deserialize(reader, reply);
            return reply;
        }

        private void ProcessPostVifReply(byte[] buffer, VISIf.trimaVistaMsg reply)
        {
            if (reply.hdr.messageType == VIP_PERIODIC_STATUS_MSG)
            {
                VISIf.trimaPeriodicStatusMsg msg = new VISIf.trimaPeriodicStatusMsg();
                msg.hdr = new VISIf.trimaVistaHeader();
                using (BinaryReader reader = new BinaryReader(new MemoryStream(buffer)))
                {
                    Serializer.Deserialize(reader, msg);
                }
                if (VifPeriodicStatusUpdate != null)
                    VifPeriodicStatusUpdate(msg);
            }
            if (reply.hdr.messageType == VIP_MACHINE_STATUS_MSG)
            {
                trimaMachineStatusMsg msg = new trimaMachineStatusMsg();
                using (BinaryReader reader = new BinaryReader(new MemoryStream(buffer)))
                {
                    Serializer.Deserialize(reader, msg);
                }

                if (VifMachineStatusUpdate != null)
                    VifMachineStatusUpdate(msg);
            }
        }



        #region IDisposable Members

        public void Dispose()
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
        }

        #endregion
    }
}
