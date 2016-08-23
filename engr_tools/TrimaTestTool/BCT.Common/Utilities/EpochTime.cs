using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;

namespace BCT.Common.Utilities
{
    /// <summary>
    /// This is the class to get the epoch time
    /// </summary>
    public class EpochTime
    {
        private static DateTime reference = new DateTime(1970, 1,1);
        public static int Now
        {
            get
            {
                var now = DateTime.Now - reference;
                return (int)now.TotalSeconds;
            }
        }
        /// <summary>
        /// Returns the date time given the number of seconds from epoch
        /// </summary>
        /// <param name="seconds"></param>
        /// <returns></returns>
        public static DateTime GetDateTime(int seconds)
        {
            return reference.AddSeconds(seconds);
        }
    }
}
