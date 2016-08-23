using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Diagnostics;
using BCT.Trima.TelnetLibrary;
using BCT.Trima.Common.Model;

namespace BCT.Trima.SimulatorStarter
{
    class Program
    {
        static private string IP = "90.0.0.1";
        static private int PORT = 23;
        static private string START_SIMULATOR = @"d:\trima\startSimNoTelnet.bat";
        static private string START_TRIMA_APP = "<host:d:\\trima\\simguitelnet\n";
        static private int TIME_OUT = 100;
        static private int COMMAND_DELAY = 15;

        /// <summary>
        /// 
        /// </summary>
        /// <param name="args">
        /// 1.) IP
        /// 2.) port
        /// 3.) start simulator script
        /// 4.) trima app script
        /// 5.) timeout in minutes
        /// </param>
        static int Main(string[] args)
        {
            try
            {
                KillProcess();

                if (!ProcessArguments(args))
                {
                    ShowUsage();
                    return 1; 
                }
                if (!ValidateArgs())
                    return 2;
                Start();
                return 0;
            }
            catch (Exception e)
            {
                Console.WriteLine(e.Message);
            }
            KillProcess();
            return 3;
        }

        static void Start()
        {
            Process sim = Simulator.StartTrimaSimulator(START_SIMULATOR);
            Console.WriteLine("Started simulator.");

            System.Threading.Thread.Sleep(1000 * COMMAND_DELAY);

            System.Threading.AutoResetEvent connectEvent = new System.Threading.AutoResetEvent(false);

            TelnetLibrary.Telnet telnet = new Telnet();

            telnet.Connect(IP, PORT, connectEvent);
            Console.WriteLine("Connecting...");
            connectEvent.WaitOne();
            System.Threading.Thread.Sleep(1000 * COMMAND_DELAY);
            telnet.StreamDaAvailableEvent += new EventHandler(telnet_StreamDaAvailableEvent);
            Console.WriteLine("Connected.");
            telnet.Send(START_TRIMA_APP);
            Console.WriteLine("Running Trima...");
            System.Threading.Thread.Sleep(1000 * 60 * TIME_OUT);

            telnet.Close();
        }

        static void telnet_StreamDaAvailableEvent(object sender, EventArgs e)
        {
            Telnet telnet = sender as Telnet;
            if (telnet == null)
                return;
            string data = telnet.Data.Pop();
            Console.Write(string.Format("data: {0}",data));
        }

        static bool ProcessArguments(string[] args)
        {
            bool retVal = false;
            do
            {
                if (args.Length != 6)
                    break;

                IP = args[0];

                if (!Int32.TryParse(args[1], out PORT))
                    break;

                START_SIMULATOR = args[2];

                START_TRIMA_APP = args[3];

                if (!Int32.TryParse(args[4], out TIME_OUT))
                    break;

                if (!Int32.TryParse(args[5], out COMMAND_DELAY))
                    break;

                retVal = true;

            } while (false);

            return retVal;
        }

         /// <summary>
        /// 
        /// </summary>
        /// <param name="args">
        /// 1.) IP
        /// 2.) port
        /// 3.) start simulator script
        /// 4.) trima app script
        /// 5.) timeout in minutes
        /// </param>
       static void ShowUsage()
        {
           Console.WriteLine("Usage: SimulatorStarter <IP> <Port> <startScript> <trimaAppScript> <timeout>");
           Console.WriteLine("IP: default: 90.0.0.1");
           Console.WriteLine("Port: default: 23");
           Console.WriteLine("startScript: default: startSimNoTelent.bat");
           Console.WriteLine("trimaAppScript: default: simguitelnet");
           Console.WriteLine("timeout: default: 140 minutes");
        }

       static bool ValidateArgs()
       {
           bool retVal = false;
           do
           {
               if (!System.IO.File.Exists(START_SIMULATOR))
               {
                   Console.WriteLine(string.Format("Cannot find {0}", START_SIMULATOR));
                   break;
               }

               if (!System.IO.File.Exists(START_TRIMA_APP))
               {
                   Console.WriteLine(string.Format("Cannot find {0}", START_TRIMA_APP));
                   break;
               }

               START_TRIMA_APP = string.Format("<host:{0}\n", START_TRIMA_APP);
               retVal = true;
           } while (false);
           return retVal;
       }

       static void KillProcess()
       {
           var procs = Process.GetProcesses().Where(c => c.ProcessName == "vxWorks" && c.MainWindowTitle == "VxSim0");

           foreach( var proc in procs)
           {
               proc.Kill();
               System.Threading.Thread.Sleep(1000 * 2);
           }
       }
    }
}
