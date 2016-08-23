using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.IO;
namespace BCT.Device.Trima.V6_3
{
    public partial class VISIf
    {
        public partial class pumpStatus : VISDeviceData
        {
            public override int Length { get { return 12; } }

            protected override trimaVistaHeader Header
            {
                get { return null; }
            }

            public override void Deserialize(BinaryReader stream)
            {
                commandedRate = stream.ReadSingle();
                actualRate = stream.ReadSingle();
                accumulatedVolume = stream.ReadSingle();
            }

            public override byte[] SerializeBody()
            {
                return BitConverter.GetBytes(commandedRate)
                    .Concat(BitConverter.GetBytes(actualRate))
                    .Concat(BitConverter.GetBytes(accumulatedVolume)).ToArray();
            }
        }
    }
}
