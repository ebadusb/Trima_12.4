using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Diagnostics;

namespace BCT.Trima.TelnetLibrary
{
    public class Simulator
    {
        /// <summary>
        /// This is the method that starts the simulator
        /// </summary>
        /// <param name="simBatch">This is the batch file that actual runs the simulator</param>
        /// <returns></returns>
        public static Process StartTrimaSimulator(string simBatch)
        {
            return Process.Start(simBatch);
        }
    }
}
