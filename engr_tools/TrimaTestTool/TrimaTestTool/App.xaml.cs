using System;
using System.Collections.Generic;
using System.Configuration;
using System.Data;
using System.Linq;
using System.Windows;
using BCT.Common;
using TrimaTestTool.ViewModel;
namespace TrimaTestTool
{
    /// <summary>
    /// Interaction logic for App.xaml
    /// </summary>
    public partial class App : Application
    {
        //private ViewModelBase viewModel;
        private MainViewModel m_mainViewModel = null;
        public App()
        {
            this.Startup += this.ApplicationStartup;
        }
      

        protected void ApplicationStartup(object sender, StartupEventArgs e)
        {
            MainWindow main = new MainWindow();
            m_mainViewModel = new MainViewModel();
            m_mainViewModel.TrimaFound = delegate(TrimaViewModelBase trimaModel)
            {
                main.TrimaUI.DataContext = trimaModel;
            };

            this.MainWindow = main;
            main.DataContext = m_mainViewModel;
            this.MainWindow.Show();
        }

        protected override void OnExit(ExitEventArgs e)
        {
            if (m_mainViewModel != null)
            {
                m_mainViewModel.Dispose();
                m_mainViewModel = null;
            }

            base.OnExit(e);
        }
    }
}
