using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.IO;

namespace BCT.Device.Trima.V6_3
{
    public partial class VISIf
    {

        public partial class trimaMachineStatusMsg : VISDeviceData
        {
            public trimaMachineStatusMsg()
            {
                this.hdr = new trimaVistaHeader();
                this.hdr.messageType = VISIf.VIP_START_PERIODIC_STATUS;
                this.hdr.lengthInBytes = Length;
                this.acPump = new pumpStatus();
                this.inletPump = new pumpStatus();
                this.plasmaPump = new pumpStatus();
                this.plateletPump = new pumpStatus();
                this.returnPump = new pumpStatus();
                this.replacementPump = new pumpStatus();
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
                return
                acPump.Serialize()
                .Concat(inletPump.Serialize())
                .Concat(plasmaPump.Serialize())
                .Concat(plateletPump.Serialize())
                .Concat(returnPump.Serialize())
                .Concat(replacementPump.Serialize())
                .ToArray();
            }

            public override void Deserialize(BinaryReader stream)
            {
                acPump.Deserialize(stream);
                inletPump.Deserialize(stream);
                plasmaPump.Deserialize(stream);
                plateletPump.Deserialize(stream);
                returnPump.Deserialize(stream);
                replacementPump.Deserialize(stream);
            }
        }
    }
}
