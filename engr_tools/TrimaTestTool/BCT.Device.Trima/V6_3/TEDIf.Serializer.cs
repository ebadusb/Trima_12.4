using System;
using System.IO;

namespace BCT.Device.Trima.V6_3
{
    public partial class Serializer
    {
        const int HEADER_CONTENT_SIZE = 20;
        const int HEADER_SIZE = 24;
        const int PERIODIC_MSG_SIZE = 36;
        const int BODY_CRC_OFFSET = HEADER_CONTENT_SIZE - 4;
        const int REPLY_MSG_SIZE = 28;

        /// <summary>
        /// Method to serialize the header into a stream
        /// </summary>
        /// <param name="stream"></param>
        /// <param name="header"></param>
        static public void Serialize(BinaryWriter stream, TEDIf.TED_Header header, bool setHeader = true)
        {
            stream.Write(header.som);
            stream.Write(header.msgId);
            stream.Write(header.length);
            stream.Write(header.time);
            stream.Write(header.bodyCRC);
            if (setHeader)
                SetHeaderCRC(stream, header);
            else
                stream.Write(header.headerCRC);
        }

        /// <summary>
        /// Deserialie from stream to header
        /// </summary>
        /// <param name="reader"></param>
        /// <param name="header"></param>
        static public void Deserialize(BinaryReader reader, TEDIf.TED_Header header)
        {
            header.som = reader.ReadUInt32();
            header.msgId = reader.ReadUInt32();
            header.length = reader.ReadUInt32();
            header.time = reader.ReadInt32();
            header.bodyCRC = reader.ReadUInt32();
            header.headerCRC = reader.ReadUInt32();
        }

        /// <summary>
        /// Method to serialize reply msg to stream
        /// </summary>
        /// <param name="stream"></param>
        /// <param name="reply"></param>
        static public void Serialize( BinaryWriter stream, TEDIf.TED_ReplyMsg reply)
        {
            Serialize(stream, reply.hdr);
            stream.Write((int)reply.status);
        }

        /// <summary>
        /// Method to deserialize reply msg from stream
        /// </summary>
        /// <param name="reader"></param>
        /// <param name="reply"></param>
        static public void Deserialize(BinaryReader reader, TEDIf.TED_ReplyMsg reply)
        {
            Deserialize(reader, reply.hdr);
            reply.status = (TEDIf.TED_Status)reader.ReadInt32();
        }

        /// <summary>
        /// Method to serialize connect request
        /// </summary>
        /// <param name="stream"></param>
        /// <param name="req"></param>
        static public void Serialize(BinaryWriter stream, TEDIf.TED_ConnectRequestMsg req)
        {
            Serialize(stream, req.hdr);
        }

        /// <summary>
        /// Method to deserialize connection request
        /// </summary>
        /// <param name="reader"></param>
        /// <param name="req"></param>
        static public void Deserialize(BinaryReader reader, TEDIf.TED_ConnectRequestMsg req)
        {
            Deserialize(reader, req.hdr);
        }

        /// <summary>
        /// Method to serialize hw periodic status request msg
        /// </summary>
        /// <param name="stream"></param>
        /// <param name="req"></param>
        static public void Serialize(BinaryWriter stream, TEDIf.TED_HwPeriodicStatusRequestMsg req)
        {
            Serialize(stream, req.hdr, false);
            stream.Write((int)req.board);
            stream.Write(req.period);
            stream.Write(req.port);
            SetBodyCRCHwPeriodicStatusRequestMsg(stream, req);
            SetHeaderCRC(stream, req.hdr);
        }

        /// <summary>
        /// Method to deserialize hw periodic status rrqust msg
        /// </summary>
        /// <param name="reader"></param>
        /// <param name="req"></param>
        static public void Deserialize(BinaryReader reader, TEDIf.TED_HwPeriodicStatusRequestMsg req)
        {
            Deserialize(reader, req.hdr);
            req.board = (TEDIf.TED_BoardType)reader.ReadInt32();
            req.period = reader.ReadUInt32();
            req.port = reader.ReadUInt32();
        }

        /// <summary>
        /// Method to serialize pump status
        /// </summary>
        /// <param name="stream"></param>
        /// <param name="status"></param>
        static public void Serialize(BinaryWriter stream, TEDIf.CHWPumpStatus status)
        {
            stream.Write(status.errors);
            stream.Write(status.RPM);
            stream.Write(status.Revs);
            stream.Write(status.Accum);
            stream.Write(status.Integral);
            stream.Write(status.AcummIntegral);
        }

        /// <summary>
        /// Method to deserialize pump status
        /// </summary>
        /// <param name="reader"></param>
        /// <param name="status"></param>
        static public void Deserialize(BinaryReader reader, TEDIf.CHWPumpStatus status)
        {
            status.errors = reader.ReadByte();
            status.RPM = reader.ReadSingle();
            status.Revs = reader.ReadSingle();
            status.Accum = reader.ReadInt32();
            status.Integral = reader.ReadInt32();
            status.AcummIntegral = reader.ReadInt32();
        }

        /// <summary>
        /// Method to serialize control hardware status msg
        /// </summary>
        /// <param name="stream"></param>
        /// <param name="msg"></param>
        static public void Serialize(BinaryWriter stream, TEDIf.TED_ControlHwStatusMsg msg)
        {
            Serialize(stream, msg.hdr);

            stream.Write(msg.accessPressure);
            stream.Write(msg.emiTemperature);
            stream.Write(msg.centrifugeCurrent);
            stream.Write(msg.centrifugePressure);
            stream.Write(msg.fiveVoltSupply);

            stream.Write(msg.leakDetector);
            stream.Write(msg.minusTwelvePS);
            stream.Write(msg.sixtyFourVoltCurrent);
            stream.Write(msg.sixtyFourVoltSupply);
            stream.Write(msg.sixtyFourVoltSwitched);
            
            stream.Write(msg.twelveVoltSupply);
            stream.Write(msg.twentyFourVoltCurrent);
            stream.Write(msg.twentyFourVoltSupply);
            stream.Write(msg.twentyFourVoltSwitched);
            stream.Write((int)msg.cassettePosition);

            stream.Write((int)msg.cassetteError);
            stream.Write(msg.centrifugeRPM);
            stream.Write(msg.centrifugeError);
            stream.Write((int)msg.doorLocks);
            stream.Write((int)msg.pauseSwitch);

            stream.Write((int)msg.stopSwitch);
            stream.Write((int)msg.pauseSwitchLatch);
            stream.Write((int)msg.stopSwitchLatch);
            stream.Write((int)msg.vibration);
            stream.Write((int)msg.valveFault);

            stream.Write((int)msg.motorFault);
            stream.Write((int)msg.solenoidFault);
            stream.Write(msg.red);
            stream.Write(msg.green);
            stream.Write((int)msg.rbcCommError);

            Serialize(stream,msg.acPump);
            Serialize(stream, msg.inletPump);
            Serialize(stream, msg.plasmaPump);
            Serialize(stream, msg.plateletPump);
            Serialize(stream, msg.returnPump);

            stream.Write((int)msg.acDetector);
            stream.Write((int)msg.reservoir);
            stream.Write((int)msg.plasmaValve);
            stream.Write((int)msg.plateletValve);
            stream.Write((int)msg.rbcValve);


            stream.Write((int)msg.fanSense0);
            stream.Write((int)msg.fanSense1);
            stream.Write((int)msg.fanSense2);
            stream.Write((int)msg.tempError);
            stream.Write((int)msg.psTempWarning);

            stream.Write((int)msg.soundLevel);
            stream.Write((int)msg.chwevent);
            stream.Write(msg.msgCount);
            stream.Write(msg.lowAGC);
            stream.Write(msg.highAGC);

            stream.Write((int)msg.sealSafeOperating);
            stream.Write((int)msg.sealSafeOvertemp);
            stream.Write((int)msg.ovpTestResult);
            stream.Write((int)msg.ovpTestStatus);
            stream.Write(msg.mxFpgaInstalled);

            stream.Write(msg.fpgaRevision);
            stream.Write(msg.fpgaIdRevision);
            stream.Write(msg.fpgaInterfaceRevision);
            stream.Write(msg.ccaRevision);
            stream.Write((int)msg.watchDogStatus);

            stream.Write((int)msg.powerSupplyOutputStatus);
        }

        static public void Deserialize(BinaryReader reader, TEDIf.TED_ControlHwStatusMsg msg)
        {
            Deserialize(reader, msg.hdr);

            msg.accessPressure = reader.ReadInt16();
            msg.emiTemperature = reader.ReadInt16();
            msg.centrifugeCurrent = reader.ReadInt16();
            msg.centrifugePressure = reader.ReadInt16();
            msg.fiveVoltSupply = reader.ReadInt16();

            msg.leakDetector = reader.ReadInt16();
            msg.minusTwelvePS = reader.ReadInt16();
            msg.sixtyFourVoltCurrent = reader.ReadInt16();
            msg.sixtyFourVoltSupply = reader.ReadInt16();
            msg.sixtyFourVoltSwitched = reader.ReadInt16();

            msg.twelveVoltSupply = reader.ReadInt16();
            msg.twentyFourVoltCurrent = reader.ReadInt16();
            msg.twentyFourVoltSupply = reader.ReadInt16();
            msg.twentyFourVoltSwitched = reader.ReadInt16();
            msg.cassettePosition = (TEDIf.HW_CASSETTE_STATES)reader.ReadInt32();

            msg.cassetteError = (TEDIf.HW_ERROR)reader.ReadInt32();
            msg.centrifugeRPM = reader.ReadSingle();
            msg.centrifugeError = reader.ReadByte();
            msg.doorLocks = (TEDIf.HW_DOOR_STATES)reader.ReadInt32();
            msg.pauseSwitch = (TEDIf.HW_SWITCH_STATES)reader.ReadInt32();

            msg.stopSwitch = (TEDIf.HW_SWITCH_STATES)reader.ReadInt32();
            msg.pauseSwitchLatch = (TEDIf.HW_SWITCH_STATES)reader.ReadInt32();
            msg.stopSwitchLatch = (TEDIf.HW_SWITCH_STATES)reader.ReadInt32();
            msg.vibration = (TEDIf.HW_ERROR)reader.ReadInt32();
            msg.valveFault = (TEDIf.HW_ERROR)reader.ReadInt32();

            msg.motorFault = (TEDIf.HW_ERROR)reader.ReadInt32();
            msg.solenoidFault = (TEDIf.HW_ERROR)reader.ReadInt32();
            msg.red = reader.ReadInt16();
            msg.green = reader.ReadInt16();
            msg.rbcCommError = (TEDIf.HW_ERROR)reader.ReadInt32();

            Deserialize(reader, msg.acPump);
            Deserialize(reader, msg.inletPump);
            Deserialize(reader, msg.plasmaPump);
            Deserialize(reader, msg.plateletPump);
            Deserialize(reader, msg.returnPump);

            msg.acDetector = (TEDIf.CHW_AC_DETECTOR_STATES)reader.ReadInt32();
            msg.reservoir = (TEDIf.CHW_RESERVOIR_STATES)reader.ReadInt32();
            msg.plasmaValve = (TEDIf.HW_VALVE_STATES)reader.ReadInt32();
            msg.plateletValve = (TEDIf.HW_VALVE_STATES)reader.ReadInt32();
            msg.rbcValve = (TEDIf.HW_VALVE_STATES)reader.ReadInt32();

            msg.fanSense0 = (TEDIf.HW_ERROR)reader.ReadInt32();
            msg.fanSense1 = (TEDIf.HW_ERROR)reader.ReadInt32();
            msg.fanSense2 = (TEDIf.HW_ERROR)reader.ReadInt32();
            msg.tempError = (TEDIf.HW_ERROR)reader.ReadInt32();
            msg.psTempWarning = (TEDIf.CHW_PS_OT_ERRORS)reader.ReadInt32();

            msg.soundLevel = (TEDIf.CHW_SOUND_LEVELS)reader.ReadInt32();
            msg.chwevent = (TEDIf.CHW_EVENTS)reader.ReadInt32();
            msg.msgCount = reader.ReadInt32();
            msg.lowAGC = reader.ReadInt16();
            msg.highAGC = reader.ReadInt16();

            msg.sealSafeOperating = (TEDIf.CHW_SEAL_SAFE_OPERATION)reader.ReadInt32();
            msg.sealSafeOvertemp = (TEDIf.CHW_SEAL_SAFE_OVER_TEMP_WARN)reader.ReadInt32();
            msg.ovpTestResult = (TEDIf.CHW_PS_OVP_TEST_RESULT)reader.ReadInt32();
            msg.ovpTestStatus = (TEDIf.CHW_PS_TEST_STATUS)reader.ReadInt32();
            msg.mxFpgaInstalled = reader.ReadInt16();

            msg.fpgaRevision = reader.ReadByte();
            msg.fpgaIdRevision = reader.ReadByte();
            msg.fpgaInterfaceRevision = reader.ReadByte();
            msg.ccaRevision = reader.ReadByte();
            msg.watchDogStatus = (TEDIf.HW_WATCHDOG_STATUS)reader.ReadInt32();

            msg.powerSupplyOutputStatus = (TEDIf.HW_PS_OUTPUT_STATUS)reader.ReadInt32();
        }

        /// <summary>
        /// Method to deserialize safety hw status msg
        /// </summary>
        /// <param name="stream"></param>
        /// <param name="msg"></param>
        static public void Serialize(BinaryWriter stream, TEDIf.TED_SafetyHwStatusMsg msg)
        {
            Serialize(stream, msg.hdr);

            stream.Write(msg.inletRPM);
            stream.Write(msg.inletRevs);
            stream.Write(msg.inletAccumRevs);

            stream.Write(msg.plateletRPM);
            stream.Write(msg.plateletRevs);
            stream.Write(msg.plateletAccumRevs);

            stream.Write(msg.plasmaRPM);
            stream.Write(msg.plasmaRevs);
            stream.Write(msg.plasmaAccumRevs);

            stream.Write(msg.acRPM);
            stream.Write(msg.acRevs);
            stream.Write(msg.acAccumRevs);

            stream.Write(msg.returnRPM);
            stream.Write(msg.returnRevs);
            stream.Write(msg.returnAccumRevs);
            stream.Write((int)msg.returnDirection);

            stream.Write((int)msg.rbcValve);
            stream.Write((int)msg.plasmaValve);
            stream.Write((int)msg.plateletValve);

            stream.Write(msg.valveLedTest);

            stream.Write((int)msg.cassettePosition);

            stream.Write((int)msg.reservoir);

            stream.Write(msg.centrifugeRPM);
            stream.Write(msg.centrifugeError);

            stream.Write((int)msg.doorLocks);

            stream.Write(msg.pauseSwitch);
            stream.Write(msg.stopSwitch);

            stream.Write((int)msg.hwEvent);

            stream.Write(msg.msgCount);
            stream.Write(msg.returnPumpDirChgTime);

            stream.Write(msg.test1);
            stream.Write(msg.test2);

            stream.Write((int)msg.serviceEnableMode);
            stream.Write((int)msg.donorConnectMode);
        }

        /// <summary>
        /// Method to deserialize safety hw status msg
        /// </summary>
        /// <param name="stream"></param>
        /// <param name="msg"></param>
        static public void Deserialize(BinaryReader reader, TEDIf.TED_SafetyHwStatusMsg msg)
        {
            Deserialize(reader, msg.hdr);

            msg.inletRPM = reader.ReadInt32();
            msg.inletRevs = reader.ReadInt32();
            msg.inletAccumRevs = reader.ReadInt32();

            msg.plateletRPM = reader.ReadInt32();
            msg.plateletRevs = reader.ReadInt32();
            msg.plateletAccumRevs = reader.ReadInt32();

            msg.plasmaRPM = reader.ReadInt32();
            msg.plasmaRevs = reader.ReadInt32();
            msg.plasmaAccumRevs = reader.ReadInt32();

            msg.acRPM = reader.ReadInt32();
            msg.acRevs = reader.ReadInt32();
            msg.acAccumRevs = reader.ReadInt32();

            msg.returnRPM = reader.ReadInt32();
            msg.returnRevs = reader.ReadInt32();
            msg.returnAccumRevs = reader.ReadInt32();
            msg.returnDirection = (TEDIf.SHW_DIRECTION)reader.ReadInt32();

            msg.rbcValve = (TEDIf.HW_VALVE_STATES)reader.ReadInt32();
            msg.plasmaValve = (TEDIf.HW_VALVE_STATES)reader.ReadInt32();
            msg.plateletValve = (TEDIf.HW_VALVE_STATES)reader.ReadInt32();

            msg.valveLedTest = reader.ReadByte();

            msg.cassettePosition = (TEDIf.HW_CASSETTE_STATES)reader.ReadInt32();

            msg.reservoir = (TEDIf.SHW_RESERVOIR_STATES)reader.ReadInt32();

            msg.centrifugeRPM = reader.ReadSingle();
            msg.centrifugeError = reader.ReadByte();

            msg.doorLocks = (TEDIf.HW_DOOR_STATES)reader.ReadInt32();

            msg.pauseSwitch = reader.ReadBoolean();
            msg.stopSwitch = reader.ReadBoolean();

            msg.hwEvent = (TEDIf.SHW_EVENTS)reader.ReadInt32();

            msg.msgCount = reader.ReadInt32();
            msg.returnPumpDirChgTime = reader.ReadByte();

            msg.test1 = reader.ReadInt32();
            msg.test2 = reader.ReadInt16();

            msg.serviceEnableMode = (TEDIf.HW_ORDERS)reader.ReadInt32();
            msg.donorConnectMode = (TEDIf.HW_ORDERS)reader.ReadInt32();
        }

        /// <summary>
        /// Method to serialize a header
        /// </summary>
        /// <param name="writer"></param>
        /// <param name="header"></param>
        static void SetHeaderCRC(BinaryWriter writer, TEDIf.TED_Header header)
        {
            writer.BaseStream.Seek(0, SeekOrigin.Begin);
            BinaryReader reader = new BinaryReader(writer.BaseStream);//)
            byte[] buffer = new byte[HEADER_CONTENT_SIZE];
            reader.Read(buffer, 0, HEADER_CONTENT_SIZE);
            header.headerCRC = 0;
            header.headerCRC = BCT.Common.Utilities.Crc32.Calculate(buffer);
            header.headerCRC = header.headerCRC ^ 0xFFFFFFFF;

            writer.Seek(HEADER_CONTENT_SIZE, SeekOrigin.Begin);
            writer.Write(header.headerCRC);
        }

        static public void SetBodyCRCHwPeriodicStatusRequestMsg(BinaryWriter writer, TEDIf.TED_HwPeriodicStatusRequestMsg req)
        {
            if (req == null || req.hdr == null)
                return;

            TEDIf.TED_Header header = req.hdr;

            int msgLength = PERIODIC_MSG_SIZE - HEADER_SIZE;
            header.length = (uint)msgLength;

            writer.BaseStream.Seek(HEADER_SIZE, SeekOrigin.Begin);
            BinaryReader reader = new BinaryReader(writer.BaseStream);
            byte[] buffer = new byte[msgLength];
            reader.Read(buffer, 0, msgLength);
            header.bodyCRC = 0;
            header.bodyCRC = BCT.Common.Utilities.Crc32.Calculate(buffer);
            header.bodyCRC = header.bodyCRC ^ 0xFFFFFFFF;

            writer.Seek(BODY_CRC_OFFSET, SeekOrigin.Begin);
            writer.Write(header.bodyCRC);
        }

        static public byte[] Serialize(TEDIf.TED_Header header)
        {
            if (header == null)
                return null;

            byte[] buffer = null;
            using (MemoryStream stream = new MemoryStream())
            {
                BinaryWriter writer = new BinaryWriter(stream);
                Serialize(writer, header);

                BinaryReader reader = new BinaryReader(stream);
                buffer = new byte[HEADER_SIZE];
                stream.Seek(0, SeekOrigin.Begin);
                reader.Read(buffer, 0, HEADER_SIZE);
            }
            return buffer;
        }

        static public byte[] Serialize(TEDIf.TED_HwPeriodicStatusRequestMsg req)
        {
            if (req == null || req.hdr == null)
                return null;

            int size = 0;
            byte[] buffer = null;
            using (MemoryStream stream = new MemoryStream())
            {
                BinaryWriter writer = new BinaryWriter(stream);
                Serialize(writer, req);
                BinaryReader reader = new BinaryReader(stream);
                buffer = new byte[PERIODIC_MSG_SIZE];
                stream.Seek(0, SeekOrigin.Begin);
                size = reader.Read(buffer, 0, PERIODIC_MSG_SIZE);
            }
            return buffer;
        }

        static public void Deserialize(BinaryReader reader, TEDIf.TED_BroadcastMsg msg)
        {
            Deserialize(reader, msg.hdr);

            msg.broadcastCount = reader.ReadInt32();
            msg.trimaHWRev = reader.ReadBytes(TEDIf.TED_HW_REV_SIZE);
            msg.trimaIP = reader.ReadBytes(TEDIf.TED_IP_ADDRESS_SIZE);
            msg.trimaLogFile = reader.ReadBytes(TEDIf.TED_LOG_FILENAME_SIZE);
            msg.trimaSerialNumber = reader.ReadBytes(TEDIf.TED_SERIAL_NUMBER_SIZE);
            msg.trimaCRC = reader.ReadBytes(TEDIf.TED_CRC_SIZE);
            msg.trimaRelease = reader.ReadBytes(TEDIf.TED_RELEASE_SIZE);
            msg.trimaBuild = reader.ReadBytes(TEDIf.TED_BUILD_SIZE);
            msg.trimaPort = reader.ReadBytes(TEDIf.TED_PORT_NUMBER_SIZE);
            msg.sequenceNumber = reader.ReadBytes(TEDIf.TED_SEQUENCE_NUMBER_SIZE);
            msg.vipifVersionNumber = reader.ReadBytes(TEDIf.TED_VIPIF_VERSION_SIZE);
            msg.biometricDeviceList = reader.ReadUInt32();
            msg.tedifVersionNumber = reader.ReadBytes(TEDIf.TED_INTERFACE_REVISION_SIZE);
        }

        static public void Deserialize(BinaryReader reader, TEDIf.TED_ApsAutoZeroValueMsg msg)
        {
            Deserialize(reader, msg.hdr);

            msg.rawValue = reader.ReadSingle();
            msg.referenceValue = reader.ReadSingle();
        }

    }
}
