using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.IO;

namespace BCT.Device.Trima.V6_3
{
    public partial class VISIf
    {
        public partial class trimaPeriodicStatusMsg : VISDeviceData
        {
            public override int Length { get { return 120; } }

            protected override trimaVistaHeader Header
            {
                get { return null; }
            }

            public override void Deserialize(BinaryReader stream)
            {
                if (stream.BaseStream.Length < Length)
                    return;

                hdr.Deserialize(stream);
                targetProcedureTime = stream.ReadSingle();
                currentProcedureTime = stream.ReadSingle();
                remainingProcedureTime = stream.ReadSingle();
                targetPlateletYield = stream.ReadInt32();
                currentPlateletYield = stream.ReadInt32();

                targetPlasmaVolume = stream.ReadInt32();
                currentPlasmaVolume = stream.ReadInt32();

                targetRbcVolume = stream.ReadInt32();
                currentRbcVolume = stream.ReadInt32();

                targetRbcCollectHct = stream.ReadSingle();
                currentRbcCollectHct = stream.ReadSingle();

                targetPlateletVolume = stream.ReadInt32();
                currentPlateletVolume = stream.ReadInt32();

                targetPASVolume = stream.ReadInt32();
                currentPASVolume = stream.ReadInt32();

                targetRAS1Volume = stream.ReadInt32();
                currentRAS1Volume = stream.ReadInt32();

                targetRAS2Volume = stream.ReadInt32();
                currentRAS2Volume = stream.ReadInt32();

                currentPlateletAcVolume = stream.ReadInt32();
                currentPlasmaAcVolume = stream.ReadInt32();
                currentRbc1AcVolume = stream.ReadInt32();
                currentRbc2AcVolume = stream.ReadInt32();

                totalBloodProcessed = stream.ReadInt32();
                donorTbv = stream.ReadInt32();
                trimaSystemState = stream.ReadInt32();

                cassetteType = stream.ReadInt32();
                cassettePosition = stream.ReadInt32();

                selectedProcedure = stream.ReadInt32();
                catalogNumber = stream.ReadInt32();

                subState = stream.ReadBytes(VISIf.VIP_MAX_SUBSTATE_LENGTH);
                recoveryState = stream.ReadBytes(VISIf.VIP_MAX_RECOVERSTATE_LENGTH + 1);
            }

            public override byte[] SerializeBody()
            {
                return BitConverter.GetBytes(targetProcedureTime)
                    .Concat(BitConverter.GetBytes(currentProcedureTime))
                    .Concat(BitConverter.GetBytes(remainingProcedureTime)).ToArray();
            }
        }
    }

}
