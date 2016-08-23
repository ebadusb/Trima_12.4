using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.ComponentModel;

using System.Net;
using System.Net.Sockets;
using System.IO;

using BCT.Trima.Common.Model;
using System.Threading;

namespace BCT.Trima.TelnetLibrary
{
    public class Telnet : ModelBase
    {
        private TcpClient m_tcpClient = new TcpClient();
        private string m_status;
        private AutoResetEvent m_autoReset = null;
        private NetworkStream m_netStream;
        public event EventHandler StreamDaAvailableEvent;
        private static int MAX_SIZE = 102400;
        private byte[] m_buffer = new byte[MAX_SIZE];
        private StringBuilder m_builder = new StringBuilder();
        private Stack<string> m_dataReceived = new System.Collections.Generic.Stack<string>();
        private AutoResetEvent m_autoResetSendRec = null;
        public string Status
        {
            get { return m_status; }
            set
            {
                m_status = value;
                RaisePropertyChanged(GetPropertyName(() => this.Status));
            }
        }

        public String NetData { get { return m_builder.ToString(); } }

        public bool IsConnected
        {
            get { return m_tcpClient.Connected; }
        }

        public void Connect(string ip, int port)
        {
            m_tcpClient.BeginConnect(ip, port, new AsyncCallback(Connected), this);

        }

        public void Connect(string ip, int port, AutoResetEvent autoReset)
        {
            m_autoReset = autoReset;
            Connect(ip, port);
        }

        private void Connected(IAsyncResult connectResult)
        {
            //connectResult.
            m_status = m_tcpClient.Connected ? "Connected" : "Not Connected";

            if (m_autoReset != null)
                m_autoReset.Set();

            m_builder = new StringBuilder();
            m_autoResetSendRec = new AutoResetEvent(true);
            m_netStream = m_tcpClient.GetStream();
            Send("\n");
        }

        public void Close()
        {
            m_tcpClient.Close();
            m_netStream = null;
        }

        public void Send(string message)
        {
            if (m_netStream == null)
                return;
            System.Threading.Thread.Sleep(1000 * 15);
            byte[] data = Encoding.ASCII.GetBytes(message);
            bool wait = false;
            lock (this)
            {
                if (m_waitingForResponse)
                    wait = true;
            }
            if( wait )
                m_autoResetSendRec.WaitOne();
            m_netStream.Write(data, 0, data.Length);
            lock (this)
            {
                m_waitingForResponse = true;
            }
            m_netStream.BeginRead(m_buffer, 0, MAX_SIZE, new AsyncCallback(Read), this);
        }

        private void Read(IAsyncResult readResult)
        {
            string str = Encoding.ASCII.GetString(m_buffer, 0, MAX_SIZE).Trim( new char[]{'\0'});
            Array.Clear(m_buffer, 0, MAX_SIZE);
            m_dataReceived.Push(str);
            //Console.WriteLine(str.Trim());
            //m_builder.Append(Encoding.ASCII.GetString(m_buffer, 0, MAX_SIZE));

            if (this.StreamDaAvailableEvent != null)
                this.StreamDaAvailableEvent(this, new EventArgs());
            
            m_netStream.BeginRead(m_buffer, 0, MAX_SIZE, new AsyncCallback(Read), this);
            lock (this)
            {
                m_waitingForResponse = false;
            }
            m_autoResetSendRec.Set();
        }

        private bool m_waitingForResponse = false;

        public Stack<string> Data { get { return m_dataReceived; } }
    }
}
