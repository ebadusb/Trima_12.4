using System;
using System.Text;
using System.Collections.Generic;
using System.Linq;
using Microsoft.VisualStudio.TestTools.UnitTesting;
using System.IO;

using System.Net;
using System.Net.NetworkInformation;
using System.Net.Sockets;

using BCT.Device.Trima.V6_3;
using BCT.Common.Utilities;

namespace BCT.Device.UnitTest
{
    [TestClass]
    public class Trima6_3Test
    {
        private TEDIf.TED_Header InstantiateHeader()
        {
            TEDIf.TED_Header header = new TEDIf.TED_Header();
            header.som = 0xDECAF;
            header.msgId = (uint)TEDIf.MessageId.TED_CONNECT_REQUEST;
            header.length = 100;
            header.time = 2;
            header.headerCRC = 2;
            header.bodyCRC = 3;
            return header;
        }

        [TestMethod]
        public void TedHeaderTest()
        {
            TEDIf.TED_Header header = InstantiateHeader();

            MemoryStream stream  = new MemoryStream();
            BinaryWriter writer = new BinaryWriter(stream);

            Serializer.Serialize(writer, header);

            Assert.AreEqual(24, stream.Length, "header size is not 24");

            stream.Seek(0, SeekOrigin.Begin);

            BinaryReader reader = new BinaryReader(stream);
            TEDIf.TED_Header retHeader = new TEDIf.TED_Header();
            Serializer.Deserialize(reader, retHeader);

            Assert.AreEqual(header.som, retHeader.som, "som not equal");
            Assert.AreEqual(header.msgId, retHeader.msgId, "msgId not equal");
            Assert.AreEqual(header.length, retHeader.length, "length not equal");
            Assert.AreEqual(header.time, retHeader.time, "time not equal");
            Assert.AreEqual(header.headerCRC, retHeader.headerCRC, "headerCRC not equal");
            Assert.AreEqual(header.bodyCRC, retHeader.bodyCRC, "bodyCRC not equal");

            reader.Dispose();
            writer.Dispose();
            stream.Dispose();
        }

        
        [TestMethod]
        public void TedReplyMsgTest()
        {
            TEDIf.TED_ReplyMsg msg = new TEDIf.TED_ReplyMsg();
            msg.hdr = InstantiateHeader();
            msg.status = TEDIf.TED_Status.TED_BAD_MESSAGE_RECEIVED;

            MemoryStream stream = new MemoryStream();
            BinaryWriter writer = new BinaryWriter(stream);

            Serializer.Serialize(writer, msg);
            stream.Seek(0, SeekOrigin.Begin);

            BinaryReader reader = new BinaryReader(stream);
            TEDIf.TED_ReplyMsg ret = new TEDIf.TED_ReplyMsg();
            ret.hdr = new TEDIf.TED_Header();
            Serializer.Deserialize(reader, ret);

            Assert.AreEqual(msg.hdr.som, ret.hdr.som, "som not equal");
            Assert.AreEqual(msg.hdr.msgId, ret.hdr.msgId, "msgId not equal");
            Assert.AreEqual(msg.hdr.length, ret.hdr.length, "length not equal");
            Assert.AreEqual(msg.hdr.time, ret.hdr.time, "time not equal");
            Assert.AreEqual(msg.hdr.headerCRC, ret.hdr.headerCRC, "headerCRC not equal");
            Assert.AreEqual(msg.hdr.bodyCRC, ret.hdr.bodyCRC, "bodyCRC not equal");
            Assert.AreEqual(msg.status, ret.status, "status not equal");

            reader.Dispose();
            writer.Dispose();
            stream.Dispose();

        }
        [TestMethod]
        public void TedHwPeriodicStatusRequestMsgTest()
        {
            TEDIf.TED_HwPeriodicStatusRequestMsg msg = new TEDIf.TED_HwPeriodicStatusRequestMsg();
            msg.hdr = InstantiateHeader();
            msg.board = TEDIf.TED_BoardType.CONTROL;

            MemoryStream stream = new MemoryStream();
            BinaryWriter writer = new BinaryWriter(stream);

            Serializer.Serialize(writer, msg);
            stream.Seek(0, SeekOrigin.Begin);

            BinaryReader reader = new BinaryReader(stream);
            TEDIf.TED_HwPeriodicStatusRequestMsg ret = new TEDIf.TED_HwPeriodicStatusRequestMsg();
            ret.hdr = new TEDIf.TED_Header();
            Serializer.Deserialize(reader, ret);

            Assert.AreEqual(msg.board, ret.board, "board not equal");
            Assert.AreEqual(msg.period, ret.period, "period not equal");
            Assert.AreEqual(msg.port, ret.port, "port not equal");

            reader.Dispose();
            writer.Dispose();
            stream.Dispose();

        }

        [TestMethod]
        public void TedHwPumStatusTest()
        {
            TEDIf.CHWPumpStatus pumpStatus = new TEDIf.CHWPumpStatus();
            pumpStatus.Accum = 1;
            pumpStatus.AcummIntegral = 2;
            pumpStatus.Integral = 3;
            pumpStatus.Revs = 4;
            pumpStatus.RPM = 5;
            MemoryStream stream = new MemoryStream();
            BinaryWriter writer = new BinaryWriter(stream);

            Serializer.Serialize(writer, pumpStatus);
            stream.Seek(0, SeekOrigin.Begin);

            Assert.AreEqual(21, stream.Length, "pump status are not equal");

            BinaryReader reader = new BinaryReader(stream);
            TEDIf.CHWPumpStatus ret = new TEDIf.CHWPumpStatus();
            Serializer.Deserialize(reader, ret);

            Assert.AreEqual(pumpStatus.Accum, ret.Accum, "Accum are not equal");
            Assert.AreEqual(pumpStatus.AcummIntegral, ret.AcummIntegral, "AcummIntegral are not equal");
            Assert.AreEqual(pumpStatus.errors, ret.errors, "errors are not equal");
            Assert.AreEqual(pumpStatus.Integral, ret.Integral, "Integral are not equal");
            Assert.AreEqual(pumpStatus.Revs, ret.Revs, "Revs are not equal");
            Assert.AreEqual(pumpStatus.RPM, ret.RPM, "RPM are not equal");

            reader.Dispose();
            writer.Dispose();
            stream.Dispose();

        }

        private TEDIf.TED_ControlHwStatusMsg InstantiateControlHwStatusMsg()
        {
            TEDIf.TED_ControlHwStatusMsg msg = new TEDIf.TED_ControlHwStatusMsg();
            msg.accessPressure = 1;
            msg.acDetector = TEDIf.CHW_AC_DETECTOR_STATES.CHW_AC_DETECTOR_AIR;
            msg.cassetteError = TEDIf.HW_ERROR.HW_FAULT;
            msg.cassettePosition = TEDIf.HW_CASSETTE_STATES.HW_CASSETTE_DOWN;
            msg.ccaRevision = 2;
            msg.centrifugeCurrent = 3;
            msg.centrifugeError = 4;
            msg.centrifugePressure = 5;
            msg.centrifugeRPM = 6.1f;
            msg.chwevent = TEDIf.CHW_EVENTS.CHW_AC_EVENT;
            msg.doorLocks = TEDIf.HW_DOOR_STATES.HW_DOOR_CLOSED_AND_LOCKED;
            msg.emiTemperature = 7;
            msg.fanSense0 = TEDIf.HW_ERROR.HW_NO_FAULT;
            msg.fanSense1 = TEDIf.HW_ERROR.HW_FAULT;
            msg.fanSense2 = TEDIf.HW_ERROR.HW_FAULT;
            msg.fiveVoltSupply = 8;
            msg.fpgaIdRevision = 9;
            msg.fpgaInterfaceRevision = 10;
            msg.fpgaRevision = 11;
            msg.green = 12;
            msg.highAGC = 13;
            msg.leakDetector = 14;
            msg.lowAGC = 15;
            msg.minusTwelvePS = 16;
            msg.motorFault = TEDIf.HW_ERROR.HW_FAULT;
            msg.msgCount = 17;
            msg.mxFpgaInstalled = 18;
            msg.ovpTestResult = TEDIf.CHW_PS_OVP_TEST_RESULT.CHW_OVP_TEST_RESULT_FAIL;
            msg.ovpTestStatus = TEDIf.CHW_PS_TEST_STATUS.CHW_PS_TEST_STATUS_NOT_AVAILABLE;
            msg.pauseSwitch = TEDIf.HW_SWITCH_STATES.HW_SWITCH_DISABLED;
            msg.pauseSwitchLatch = TEDIf.HW_SWITCH_STATES.HW_SWITCH_ENABLED;
            msg.plasmaValve = TEDIf.HW_VALVE_STATES.HW_VALVE_COLLECT;
            msg.plateletValve = TEDIf.HW_VALVE_STATES.HW_VALVE_OPEN;
            msg.powerSupplyOutputStatus = TEDIf.HW_PS_OUTPUT_STATUS.HW_PS_OUTPUT_STATUS_DISABLED;
            msg.psTempWarning = TEDIf.CHW_PS_OT_ERRORS.CHW_DC_NOT_OK;
            msg.rbcCommError = TEDIf.HW_ERROR.HW_NO_FAULT;
            msg.rbcValve = TEDIf.HW_VALVE_STATES.HW_VALVE_RETURN;
            msg.red = 19;
            msg.reservoir = TEDIf.CHW_RESERVOIR_STATES.CHW_RESERVOIR_EMPTY;
            msg.sealSafeOperating = TEDIf.CHW_SEAL_SAFE_OPERATION.CHW_SEAL_SAFE_OFF;
            msg.sealSafeOvertemp = TEDIf.CHW_SEAL_SAFE_OVER_TEMP_WARN.CHW_SEAL_SAFE_OTW;
            msg.sixtyFourVoltCurrent = 20;
            msg.sixtyFourVoltSupply = 21;
            msg.sixtyFourVoltSwitched = 22;
            msg.solenoidFault = TEDIf.HW_ERROR.HW_NO_FAULT;
            msg.soundLevel = TEDIf.CHW_SOUND_LEVELS.CHW_SOUND_LEVEL_FIRST;
            msg.stopSwitch = TEDIf.HW_SWITCH_STATES.HW_SWITCH_ENABLED;
            msg.stopSwitchLatch = TEDIf.HW_SWITCH_STATES.HW_SWITCH_ENABLED;
            msg.tempError = TEDIf.HW_ERROR.HW_NO_FAULT;
            msg.twelveVoltSupply = 23;
            msg.twentyFourVoltCurrent = 24;
            msg.twentyFourVoltSupply = 25;
            msg.twentyFourVoltSwitched = 26;
            msg.valveFault = TEDIf.HW_ERROR.HW_NO_FAULT;
            msg.vibration = TEDIf.HW_ERROR.HW_NO_FAULT;
            msg.watchDogStatus = TEDIf.HW_WATCHDOG_STATUS.HW_WATCHDOG_STATUS_A5_WRITE_WAITING;
            return msg;
        }

        [TestMethod]
        public void TedControlHwStatusMsgTest()
        {
            TEDIf.TED_ControlHwStatusMsg msg = InstantiateControlHwStatusMsg();
            msg.acPump = new TEDIf.CHWPumpStatus();
            msg.plasmaPump = new TEDIf.CHWPumpStatus();
            msg.plateletPump = new TEDIf.CHWPumpStatus();
            msg.inletPump = new TEDIf.CHWPumpStatus();
            msg.returnPump = new TEDIf.CHWPumpStatus();
            msg.hdr = InstantiateHeader();

            MemoryStream stream = new MemoryStream();
            BinaryWriter writer = new BinaryWriter(stream);

            Serializer.Serialize(writer, msg);
            stream.Seek(0, SeekOrigin.Begin);

            Assert.AreEqual(300, stream.Length, "length is not 300");

            BinaryReader reader = new BinaryReader(stream);
            TEDIf.TED_ControlHwStatusMsg ret = new TEDIf.TED_ControlHwStatusMsg();
            ret.acPump = new TEDIf.CHWPumpStatus();
            ret.plasmaPump = new TEDIf.CHWPumpStatus();
            ret.plateletPump = new TEDIf.CHWPumpStatus();
            ret.inletPump = new TEDIf.CHWPumpStatus();
            ret.returnPump = new TEDIf.CHWPumpStatus();
            ret.hdr = InstantiateHeader();
            Serializer.Deserialize(reader, ret);

            Assert.AreEqual(msg.watchDogStatus, ret.watchDogStatus);

            reader.Dispose();
            writer.Dispose();
            stream.Dispose();
        }

        [TestMethod]
        public void TedSafetyHwStatusMsgTest()
        {
            TEDIf.TED_SafetyHwStatusMsg msg = new TEDIf.TED_SafetyHwStatusMsg();
            msg.hdr = InstantiateHeader();
            msg.donorConnectMode = TEDIf.HW_ORDERS.HW_DISABLE;

            MemoryStream stream = new MemoryStream();
            BinaryWriter writer = new BinaryWriter(stream);

            Serializer.Serialize(writer, msg);
            stream.Seek(0, SeekOrigin.Begin);

            //Assert.AreEqual(300, stream.Length, "length is not 300");

            BinaryReader reader = new BinaryReader(stream);
            TEDIf.TED_SafetyHwStatusMsg ret = new TEDIf.TED_SafetyHwStatusMsg();
            ret.hdr = InstantiateHeader();
            Serializer.Deserialize(reader, ret);

            Assert.AreEqual(msg.donorConnectMode, ret.donorConnectMode);

            reader.Dispose();
            writer.Dispose();
            stream.Dispose();
        }

        [TestMethod]
        public void HeaderCRCTest()
        {
            TEDIf.TED_Header header = new TEDIf.TED_Header();//InstantiateHeader();
            header.bodyCRC = 0;
            header.length = 0;
            header.time = 0x512bafc7;
            header.msgId = (int)TEDIf.MessageId.TED_CONNECT_REQUEST;
            header.som = 0xDECAF;

            MemoryStream stream = new MemoryStream();
            BinaryWriter writer = new BinaryWriter(stream);

            Serializer.Serialize(writer, header);
            Assert.AreEqual(0xf80a8bf7, header.headerCRC, "header crcs are not equal");

            writer.Dispose();
            stream.Dispose();
        }

        /// <summary>
        /// this requires a device connected to run properly
        /// </summary>
        [TestMethod]
        [Ignore]
        public void ConnectTest()
        {
            TEDIf.TED_Header header = new TEDIf.TED_Header();//InstantiateHeader();
            header.bodyCRC = 0;
            header.length = 0;
            header.time = 0x512bafc7;
            header.msgId = (int)TEDIf.MessageId.TED_CONNECT_REQUEST;
            header.som = 0xDECAF;

            MemoryStream stream = new MemoryStream();
            BinaryWriter writer = new BinaryWriter(stream);

            Serializer.Serialize(writer, header);
            Assert.AreEqual(0xf80a8bf7, header.headerCRC, "header crcs are not equal");

            int n = 24;
            byte[] buffer = new byte[n];
            IPEndPoint endpoint = new IPEndPoint( new IPAddress( new byte[]{172,21,14,72}), 7000 );
            stream.Seek(0, SeekOrigin.Begin);

            BinaryReader reader = new BinaryReader(stream);
            reader.Read(buffer, 0, n);

            UdpClient client = new UdpClient();
            client.Send(buffer, n, endpoint);

            buffer = new byte[1000];

            IPEndPoint nendpoint = null;

            buffer = client.Receive(ref nendpoint);

            MemoryStream rec = new MemoryStream(buffer);
            BinaryReader recReader = new BinaryReader(rec);

            TEDIf.TED_ReplyMsg reply = new TEDIf.TED_ReplyMsg();
            reply.hdr = new TEDIf.TED_Header();
            Serializer.Deserialize(recReader, reply);


            Assert.AreEqual(TEDIf.TED_Status.TED_CONNECTION_ALLOWED_OPERATIONAL, reply.status, "STATUS NOT EQUAL");

            recReader.Dispose();
            rec.Dispose();

            reader.Dispose();
            writer.Dispose();
            stream.Dispose();
        }

        [TestMethod]
        public void SetBodyCRCHwControlRequestTest()
        {
            TEDIf.TED_HwPeriodicStatusRequestMsg msg = new TEDIf.TED_HwPeriodicStatusRequestMsg();
            msg.hdr = new TEDIf.TED_Header();
            msg.hdr.bodyCRC = 0;
            msg.hdr.som = 0XDECAF;
            msg.hdr.msgId = 0;
            msg.hdr.length = 0;
            msg.hdr.headerCRC = 0;
            msg.board = TEDIf.TED_BoardType.CONTROL;
            msg.period = 0;
            msg.port = 0;

            BinaryWriter writer = new BinaryWriter(new MemoryStream());
            Serializer.Serialize(writer, msg);

            msg = new TEDIf.TED_HwPeriodicStatusRequestMsg();
            msg.hdr = new TEDIf.TED_Header();
            writer.Seek(0, SeekOrigin.Begin);
            using (BinaryReader reader = new BinaryReader(writer.BaseStream))
            {
                Serializer.Deserialize(reader, msg);
            }

            //Assert.AreEqual(0, msg.hdr.headerCRC, "header crc is not equal");
            Assert.AreEqual(0xDECAFu, msg.hdr.som, "som is not equal");
            Assert.AreEqual(0u, msg.hdr.msgId, "msgId is not equal");
            Assert.AreEqual(0u, msg.hdr.length, "length is not equal");
            Assert.AreEqual(TEDIf.TED_BoardType.CONTROL, msg.board, "som board is not equal");
            Assert.AreEqual(0u, msg.period, "som period is not equal");
            Assert.AreEqual(0u, msg.port, "som port is not equal");
            Assert.AreNotEqual(0u, msg.hdr.bodyCRC, "bodyCRC crc is equal");

            writer.Dispose();
        }

        [TestMethod]
        public void HwRequestMsgTest()
        {
            TEDIf.TED_Header header = new TEDIf.TED_Header();
            header.bodyCRC = 0;
            header.length = 0;
            header.time = 0x512bdc81;
            header.msgId = (int)TEDIf.MessageId.TED_HW_PERIODIC_STATUS_REQUEST;
            header.som = 0xDECAF;

            TEDIf.TED_HwPeriodicStatusRequestMsg req = new TEDIf.TED_HwPeriodicStatusRequestMsg();
            req.board = TEDIf.TED_BoardType.CONTROL;
            req.period = 2000;
            req.port = 0x0000908f;
            req.hdr = header;

            BinaryWriter writer = new BinaryWriter(new MemoryStream());
            Serializer.Serialize(writer, req);

            Assert.AreEqual(0xf0469060u, req.hdr.bodyCRC, "body crc not equal");
            //??this is not calculating right?
            //Assert.AreEqual(0x3b112634u, req.hdr.headerCRC, "header crc not equal");

            writer.Dispose();
        }


        [TestMethod]
        public void VifPeriodicStatusSerializeTest()
        {
            VISIf.trimaPeriodicStatusMsg msg = new VISIf.trimaPeriodicStatusMsg();
            msg.hdr = new VISIf.trimaVistaHeader();
            BinaryWriter writer = new BinaryWriter(new MemoryStream());
            msg.catalogNumber = 20;
            Serializer.Serialize(writer, msg);

            writer.Seek(0, SeekOrigin.Begin);

            VISIf.trimaPeriodicStatusMsg rec = new VISIf.trimaPeriodicStatusMsg();
            rec.hdr = new VISIf.trimaVistaHeader();
            using (BinaryReader reader = new BinaryReader(writer.BaseStream))
            {
                Serializer.Deserialize(reader, rec);
            }
            Assert.AreEqual(msg.catalogNumber, rec.catalogNumber, "Catalog number not equal");

            writer.Dispose();
        }

        [TestMethod]
        public void TempTest()
        {
            byte[] header = new byte[1];
            byte[] body = new byte[1];

            byte[] data = body.Concat(header).ToArray();

        }


        [TestMethod]
        public void BuildHeaderTest()
        {
            VISIf.startPeriodicStatusMsg msg = new VISIf.startPeriodicStatusMsg();
            msg.portNumber = 999;
            msg.receiveTimeout = 25;
            msg.repeatPeriod = 3000;

            byte[] bytes = msg.SerializeBody();
            uint crc = Crc32.Calculate(bytes) ^ 0xFFFFFFFF;

            Assert.AreEqual(3220848833, crc, "body crc are not equal");

            msg.hdr.timestamp = 1362121546;
            msg.hdr.bodyCrc = crc;
            msg.hdr.sequenceNumber = 1;
            byte[] header = msg.SerializeHeader(crc);

            Assert.AreEqual(3348939883, msg.hdr.headerCrc, "header crc are not euqual");

            uint serializedCrc = BitConverter.ToUInt32(header, VISIf.trimaVistaHeaderSize - 4);

            Assert.AreEqual(3348939883, serializedCrc, "header crc are not euqual");
        }

        [TestMethod]
        public void StartMachineStatusUpdateConstructorTest()
        {
            VISIf.startMachineStatusMsg msg = new VISIf.startMachineStatusMsg();

            Assert.AreEqual(VISIf.VIP_SOM_ID, msg.hdr.som, "som not equal");
            Assert.IsFalse(msg.hdr.lengthInBytes == 0, "length is zero");
            Assert.AreEqual(VISIf.VIP_START_MACHINE_STATUS, msg.hdr.messageType, "message type not equal");
        }

    }
}
