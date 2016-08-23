using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;

using BCT.Device.Interface;

namespace BCT.Device.Trima
{
	public class TrimaBase : ITrima
	{
        protected System.Net.IPAddress m_address;

        public bool IsTedConnected { get; set; }

        #region IDeviceClient Members

        public bool IsConnected
        {
            get { throw new NotImplementedException(); }
        }

        public OnConnected OnConnected
        {
            get { throw new NotImplementedException(); }
        }

        public void Connect(System.Net.IPAddress address, int port)
        {
            throw new NotImplementedException();
        }

        #endregion

        #region IDevice Members

        public string Name
        {
            get { throw new NotImplementedException(); }
        }

        public string Version
        {
            get { throw new NotImplementedException(); }
        }

        public string IPAddress
        {
            get { throw new NotImplementedException(); }
        }

        public string SerialNumber
        {
            get;
            protected set;
        }

        #endregion

        #region ITrima Members

        public virtual bool StartTEDConnection()
        {
            throw new NotImplementedException();
        }

        public virtual bool StartVistaConnection()
        {
            throw new NotImplementedException();
        }

        public virtual void GetAPSAutoZero()
        {
        }

        #endregion

        #region IDevice Members

        private DeviceExceptionHandler m_deviceExceptionHandler;

        public DeviceExceptionHandler DeviceExceptionHandler
        {
            get
            {
                return m_deviceExceptionHandler;
            }
            set
            {
                m_deviceExceptionHandler = value;
            }
        }

        #endregion

        #region IDevice Members


        public virtual void OpenConnection()
        {
            throw new NotImplementedException();
        }

        public virtual void CloseConnection()
        {
            throw new NotImplementedException();
        }

        #endregion

        #region IDevice Members


        public virtual DeviceConnectionChanged DeviceConnectionChanged
        {
            get
            {
                throw new NotImplementedException();
            }
            set
            {
                throw new NotImplementedException();
            }
        }

        #endregion
    }
}
