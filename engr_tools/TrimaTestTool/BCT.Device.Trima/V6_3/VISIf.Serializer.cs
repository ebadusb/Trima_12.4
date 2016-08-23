using System;
using System.IO;
using BCT.Device.Interface;

namespace BCT.Device.Trima.V6_3
{
    public partial class Serializer
    {

        #region VIF
        const uint DEFAULT_CRC = 0xFFFFFFFF;
        const int VIF_START_PERIODIC_MSG_SIZE = 44;
        const int VIF_BODY_CRC_OFFSET = VISIf.trimaVistaHeaderSize - 8;

        public uint CalculateHeaderCRC(VISIf.trimaVistaHeader header)
        {
            uint retVal = DEFAULT_CRC;
            using (BinaryWriter writer = new BinaryWriter(new MemoryStream()))
            {
                Serializer.Serialize(writer, header);
                BinaryReader reader = new BinaryReader(writer.BaseStream);
                reader.BaseStream.Seek(0, SeekOrigin.Begin);
                byte[] buffer = new byte[VIF_HEADER_DATA_SIZE];
                reader.Read(buffer, 0, VIF_HEADER_DATA_SIZE);
                retVal = BCT.Common.Utilities.Crc32.Calculate(buffer) ^ DEFAULT_CRC;
            }
            return retVal;
        }

        public uint CalculateHeaderCRC(ITrimaData data)
        {
            uint retVal = DEFAULT_CRC;
            using (BinaryWriter writer = new BinaryWriter(new MemoryStream()))
            {
                //Serializer.Serialize(writer, data);
                BinaryReader reader = new BinaryReader(writer.BaseStream);
                reader.BaseStream.Seek(0, SeekOrigin.Begin);
                byte[] buffer = new byte[VIF_HEADER_DATA_SIZE];
                reader.Read(buffer, 0, VIF_HEADER_DATA_SIZE);
                retVal = BCT.Common.Utilities.Crc32.Calculate(buffer) ^ DEFAULT_CRC;
            }
            return retVal;
        }
        //public uint CalculateBodyCRC(


        static public byte[] Serialize(VISIf.trimaVistaHeader msg)
        {
            if (msg == null)
                return null;

            int size = 0;
            byte[] buffer = null;
            using (MemoryStream stream = new MemoryStream())
            {
                BinaryWriter writer = new BinaryWriter(stream);
                Serialize(writer, msg);
                BinaryReader reader = new BinaryReader(stream);
                buffer = new byte[VISIf.trimaVistaHeaderSize];
                stream.Seek(0, SeekOrigin.Begin);
                size = reader.Read(buffer, 0, VISIf.trimaVistaHeaderSize);
            }
            return buffer;
        }

        static public void Serialize(BinaryWriter stream, VISIf.trimaVistaHeader header, bool setHeaderCRC = true)
        {
            stream.Write(header.som);
            stream.Write(header.messageType);
            stream.Write(header.sequenceNumber);
            stream.Write(header.status);
            stream.Write(header.lengthInBytes);
            stream.Write(header.timestamp);
            stream.Write(header.bodyCrc);
            if (setHeaderCRC)
                SetHeaderCRC(stream, header);
            else
                stream.Write(header.headerCrc);
        }

        const int VIF_HEADER_DATA_SIZE = VISIf.trimaVistaHeaderSize - 4;
        /// <summary>
        /// Method to serialize a header
        /// </summary>
        /// <param name="writer"></param>
        /// <param name="header"></param>
        static void SetHeaderCRC(BinaryWriter writer, VISIf.trimaVistaHeader header)
        {
            writer.BaseStream.Seek(0, SeekOrigin.Begin);
            BinaryReader reader = new BinaryReader(writer.BaseStream);//)
            byte[] buffer = new byte[VIF_HEADER_DATA_SIZE];
            reader.Read(buffer, 0, VIF_HEADER_DATA_SIZE);
            header.headerCrc = 0;
            header.headerCrc = BCT.Common.Utilities.Crc32.Calculate(buffer);
            header.headerCrc = header.headerCrc ^ 0xFFFFFFFF;

            writer.Seek(VIF_HEADER_DATA_SIZE, SeekOrigin.Begin);
            writer.Write(header.headerCrc);
        }

        static public void Deserialize(BinaryReader stream, VISIf.trimaVistaHeader header)
        {
            header.som = stream.ReadUInt32();
            header.messageType = stream.ReadInt32();
            header.sequenceNumber = stream.ReadInt32();
            header.status = stream.ReadInt32();
            header.lengthInBytes = stream.ReadInt32();
            header.timestamp = stream.ReadInt32();
            header.bodyCrc = stream.ReadUInt32();
            header.headerCrc = stream.ReadUInt32();
        }

        static public byte[] Serialize(VISIf.startPeriodicStatusMsg msg)
        {
            if (msg == null || msg.hdr == null)
                return null;

            int size = 0;
            byte[] buffer = null;
            using (MemoryStream stream = new MemoryStream())
            {
                BinaryWriter writer = new BinaryWriter(stream);
                Serialize(writer, msg);
                BinaryReader reader = new BinaryReader(stream);
                buffer = new byte[VIF_START_PERIODIC_MSG_SIZE];
                stream.Seek(0, SeekOrigin.Begin);
                size = reader.Read(buffer, 0, VIF_START_PERIODIC_MSG_SIZE);
            }
            return buffer;
        }

        static public void Serialize(BinaryWriter stream, VISIf.startPeriodicStatusMsg msg)
        {
            Serialize(stream, msg.hdr, false);
            stream.Write(msg.repeatPeriod);
            stream.Write(msg.portNumber);
            stream.Write(msg.receiveTimeout);
            SetBodyCRCstartPeriodicStatusMsg(stream, msg);
            SetHeaderCRC(stream, msg.hdr);
        }

        static public void Deserialize(BinaryReader stream, VISIf.startPeriodicStatusMsg msg)
        {
            Deserialize(stream, msg.hdr);
            msg.repeatPeriod = stream.ReadInt32();
            msg.portNumber = stream.ReadUInt32();
            msg.receiveTimeout = stream.ReadUInt32();
        }

        static public void SetBodyCRCstartPeriodicStatusMsg(BinaryWriter writer, VISIf.startPeriodicStatusMsg req)
        {
            if (req == null || req.hdr == null)
                return;

            VISIf.trimaVistaHeader header = req.hdr;

            int msgLength = VIF_START_PERIODIC_MSG_SIZE - VISIf.trimaVistaHeaderSize;
            header.lengthInBytes = msgLength;

            writer.BaseStream.Seek(VISIf.trimaVistaHeaderSize, SeekOrigin.Begin);
            BinaryReader reader = new BinaryReader(writer.BaseStream);
            byte[] buffer = new byte[msgLength];
            reader.Read(buffer, 0, msgLength);
            header.bodyCrc = 0;
            header.bodyCrc = BCT.Common.Utilities.Crc32.Calculate(buffer);
            header.bodyCrc = header.bodyCrc ^ 0xFFFFFFFF;

            writer.Seek(VIF_BODY_CRC_OFFSET, SeekOrigin.Begin);
            writer.Write(header.bodyCrc);
        }

        static public void Serialize(BinaryWriter stream, VISIf.trimaPeriodicStatusMsg msg)
        {
            Serialize(stream, msg.hdr);
            stream.Write(msg.targetProcedureTime);
            stream.Write(msg.currentProcedureTime);
            stream.Write(msg.remainingProcedureTime);
            stream.Write(msg.targetPlateletYield);
            stream.Write(msg.currentPlateletYield);

            stream.Write(msg.targetPlasmaVolume);
            stream.Write(msg.currentPlasmaVolume);

            stream.Write(msg.targetRbcVolume);
            stream.Write(msg.currentRbcVolume);

            stream.Write(msg.targetRbcCollectHct);
            stream.Write(msg.currentRbcCollectHct);

            stream.Write(msg.targetPlateletVolume);
            stream.Write(msg.currentPlateletVolume);

            stream.Write(msg.targetPASVolume);
            stream.Write(msg.currentPASVolume);

            stream.Write(msg.targetRAS1Volume);
            stream.Write(msg.currentRAS1Volume);

            stream.Write(msg.targetRAS2Volume);
            stream.Write(msg.currentRAS2Volume);

            stream.Write(msg.currentPlateletAcVolume);
            stream.Write(msg.currentPlasmaAcVolume);
            stream.Write(msg.currentRbc1AcVolume);
            stream.Write(msg.currentRbc2AcVolume);

            stream.Write(msg.totalBloodProcessed);
            stream.Write(msg.donorTbv);
            stream.Write(msg.trimaSystemState);

            stream.Write(msg.cassetteType);
            stream.Write(msg.cassettePosition);

            stream.Write(msg.selectedProcedure);
            stream.Write(msg.catalogNumber);

            stream.Write(msg.subState, 0, VISIf.VIP_MAX_SUBSTATE_LENGTH);
            stream.Write(msg.recoveryState, 0, VISIf.VIP_MAX_RECOVERSTATE_LENGTH);
        }

        static public void Deserialize(BinaryReader stream, VISIf.trimaPeriodicStatusMsg msg)
        {
            Deserialize(stream, msg.hdr);
            msg.targetProcedureTime = stream.ReadSingle();
            msg.currentProcedureTime = stream.ReadSingle();
            msg.remainingProcedureTime = stream.ReadSingle();
            msg.targetPlateletYield = stream.ReadInt32();
            msg.currentPlateletYield = stream.ReadInt32();

            msg.targetPlasmaVolume = stream.ReadInt32();
            msg.currentPlasmaVolume = stream.ReadInt32();

            msg.targetRbcVolume = stream.ReadInt32();
            msg.currentRbcVolume = stream.ReadInt32();

            msg.targetRbcCollectHct = stream.ReadSingle();
            msg.currentRbcCollectHct = stream.ReadSingle();

            msg.targetPlateletVolume = stream.ReadInt32();
            msg.currentPlateletVolume = stream.ReadInt32();

            msg.targetPASVolume = stream.ReadInt32();
            msg.currentPASVolume = stream.ReadInt32();

            msg.targetRAS1Volume = stream.ReadInt32();
            msg.currentRAS1Volume = stream.ReadInt32();

            msg.targetRAS2Volume = stream.ReadInt32();
            msg.currentRAS2Volume = stream.ReadInt32();

            msg.currentPlateletAcVolume = stream.ReadInt32();
            msg.currentPlasmaAcVolume = stream.ReadInt32();
            msg.currentRbc1AcVolume = stream.ReadInt32();
            msg.currentRbc2AcVolume = stream.ReadInt32();

            msg.totalBloodProcessed = stream.ReadInt32();
            msg.donorTbv = stream.ReadInt32();
            msg.trimaSystemState = stream.ReadInt32();

            msg.cassetteType = stream.ReadInt32();
            msg.cassettePosition = stream.ReadInt32();

            msg.selectedProcedure = stream.ReadInt32();
            msg.catalogNumber = stream.ReadInt32();

            msg.subState = stream.ReadBytes(VISIf.VIP_MAX_SUBSTATE_LENGTH + 1);
            msg.recoveryState = stream.ReadBytes(VISIf.VIP_MAX_RECOVERSTATE_LENGTH + 1);
        }

        static public void Serialize(BinaryWriter stream, VISIf.trimaVistaMsg msg)
        {
            Serialize(stream, msg.hdr);
            stream.Write(msg.trimaVistaBuffer);
        }

        static public void Deserialize(BinaryReader stream, VISIf.trimaVistaMsg msg)
        {
            Deserialize(stream, msg.hdr);
            msg.trimaVistaBuffer = stream.ReadBytes(VISIf.trimaVistaMsgLength);
        }

        static public void Serialize(BinaryWriter stream, VISIf.pumpStatus msg)
        {
            stream.Write(msg.commandedRate);
            stream.Write(msg.actualRate);
            stream.Write(msg.accumulatedVolume);
        }

        static public void Deserialize(BinaryReader stream, VISIf.pumpStatus msg)
        {
            msg.commandedRate = stream.ReadSingle();
            msg.actualRate = stream.ReadSingle();
            msg.accumulatedVolume = stream.ReadSingle();
        }

        static public void Serialize(BinaryWriter stream, VISIf.trimaMachineStatusMsg msg)
        {
            Serialize(stream, msg.hdr);
            Serialize(stream, msg.acPump);
            Serialize(stream, msg.inletPump);
            Serialize(stream, msg.plasmaPump);
            Serialize(stream, msg.plateletPump);
            Serialize(stream, msg.returnPump);
            Serialize(stream, msg.replacementPump);
        }

        static public void Deserialize(BinaryReader stream, VISIf.trimaMachineStatusMsg msg)
        {
            Deserialize(stream, msg.hdr);
            Deserialize(stream, msg.acPump);
            Deserialize(stream, msg.inletPump);
            Deserialize(stream, msg.plasmaPump);
            Deserialize(stream, msg.plateletPump);
            Deserialize(stream, msg.returnPump);
            Deserialize(stream, msg.replacementPump);
        }

        #endregion
    }
}
