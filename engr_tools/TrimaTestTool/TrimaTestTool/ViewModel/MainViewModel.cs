using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Windows;
using BCT.Common;
using BCT.Device;
using BCT.Device.Interface;
using System.Net.NetworkInformation;
using BCT.Device.Trima.V6_3;
namespace TrimaTestTool.ViewModel
{
    public delegate void TrimaFound(TrimaViewModelBase model);

    public class MainViewModel : ViewModelBase, IDisposable
    {
        IDevice m_device = null;



        public MainViewModel()
        {
            IPAddress = "172.21.";
            ConnectedVisibility = Visibility.Collapsed;
            DisconnectedVisibility = Visibility.Visible;
        }
        /// <summary>
        /// This is the id address bound to the control
        /// </summary>
        public string IPAddress { get; set; }
        public string Message { get; set; }

        public Visibility ConnectedVisibility { get; set; }
        public Visibility DisconnectedVisibility { get; set; }

        /// <summary>
        /// This is the delegate for when device is found, called
        /// </summary>
        public TrimaFound TrimaFound;



        public void DiscoverDevice(object sender, RoutedEventArgs e)
        {
            PingReply reply = DeviceManager.Ping(IPAddress);
            if (reply != null && reply.Status == IPStatus.Success && TrimaFound != null)
            {
                Message = "Connected";
                RaisePropertyChanged(GetPropertyName(() => this.Message));
                //version discovery goes here and come up with the right version of
                //trima
                Trima63ViewModel model = new Trima63ViewModel(reply.Address);
                TrimaFound(model);

                m_device = model.Trima;
                m_device.DeviceConnectionChanged = new DeviceConnectionChanged(ConnectionChangedHandler);
                m_device.DeviceExceptionHandler = new DeviceExceptionHandler(DeviceExceptionHandler);
                m_device.OpenConnection();
            }
            else
            {
                Message = "Unable to find Trima.";
                RaisePropertyChanged(GetPropertyName(() => this.Message));
            }
            
        }
        private void DeviceExceptionHandler(Exception exception)
        {
            Message = exception.Message;
            RaisePropertyChanged(GetPropertyName(() => this.Message));
        }

        private void ConnectionChangedHandler(bool connected)
        {
            ConnectedVisibility = connected ? Visibility.Visible : Visibility.Collapsed;
            DisconnectedVisibility = connected ? Visibility.Collapsed : Visibility.Visible;

            RaisePropertyChanged(GetPropertyName(() => this.DisconnectedVisibility));
            RaisePropertyChanged(GetPropertyName(() => this.ConnectedVisibility));
            Message = "Connection closed.";
            RaisePropertyChanged(GetPropertyName(() => this.Message));
        }

        #region IDisposable Members

        public void Dispose()
        {
            if (m_device != null)
                m_device.CloseConnection();
        }

        #endregion
    }
}
