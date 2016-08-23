using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.IO;

namespace BCT.Device.Trima.V6_3
{
    partial class VISIf
    {
        public partial class startMachineStatusMsg : VISDeviceData
        {
            /// <summary>
            /// constructor
            /// </summary>
            public startMachineStatusMsg()
            {
                hdr = new trimaVistaHeader();
                hdr.lengthInBytes = Length;
                hdr.messageType = VISIf.VIP_START_MACHINE_STATUS;
            }

            public override int Length { get { return 8; } }

            protected override trimaVistaHeader Header
            {
                get { return this.hdr; }
            }

            public override byte[] SerializeBody()
            {
                return      BitConverter.GetBytes(repeatPeriod)
                    .Concat(BitConverter.GetBytes(portNumber)).ToArray();
            }

            public override void Deserialize(BinaryReader stream)
            {
                hdr.Deserialize(stream);
                repeatPeriod = stream.ReadInt32();
                portNumber = stream.ReadInt32();
            }
        }
    }
}
