using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.IO;

using BCT.Device.Interface;
using BCT.Common.Utilities;

namespace BCT.Device.Trima
{
    public abstract class TrimaDeviceData : IDeviceData
    {
        public TrimaDeviceData()
        {
            Error = DataError.None;
        }

        #region IDeviceData Members
        public DataError Error { get; set; }

        public virtual int Length
        {
            get { throw new NotImplementedException(); }
        }

        public virtual byte[] Serialize()
        {
            byte[] body = SerializeBody();
            uint crc = Crc32.Calculate(body) ^ 0xFFFFFFFF;
            return SerializeHeader(crc).Concat(body).ToArray();
        }

        public virtual void Deserialize(byte[] buffer)
        {
            if( buffer.Length < Length) 
            {
                Error = DataError.InsufficientLength;
                return;
            }
            using (BinaryReader stream = new BinaryReader(new MemoryStream(buffer)))
                Deserialize(stream);
        }

        #endregion

        #region Common Serialization
        public abstract byte[] SerializeBody();
        public abstract byte[] SerializeHeader( uint bodyCrc );
        public abstract void Deserialize(BinaryReader stream);
        #endregion Common Serialization

    }
}
