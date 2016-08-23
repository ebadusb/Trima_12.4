using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using BCT.Common.Utilities;

namespace BCT.Common.Utilities
{
    public class CPSConverter
    {
        public static double cps_gain { get; private set; }
        public static double cps_offset { get; private set; }

        public static string cps_gain_str = Extension.GetPropertyName(() => CPSConverter.cps_gain);
        public static string cps_offset_str = Extension.GetPropertyName(() => CPSConverter.cps_offset);

        private static bool _isInitialized;
        private static object monitor = new object();

        private static bool IsInitialized
        {
            get
            {
                lock (monitor)
                    return _isInitialized;
            }
            set
            {
                lock (monitor)
                    _isInitialized = value;
            }
        }

        public static void Init(Dictionary<string, string> list)
        {
            if (!list.ContainsKey(cps_gain_str) ||
                !list.ContainsKey(cps_offset_str))
                return;
            double cps_gain_input = 0;
            if (!double.TryParse(list[cps_gain_str], out cps_gain_input))
                return;

            double cps_offset_input = 0;
            if (!double.TryParse(list[cps_offset_str], out cps_offset_input))
                return;

            Init(cps_gain_input, cps_offset_input);
        }

        public static void Init(double cps_gain, double cps_offset)
        {
            lock (monitor)
            {
                CPSConverter.cps_gain = cps_gain;
                CPSConverter.cps_offset = cps_offset;
                IsInitialized = true;
            }
        }

        public static double? To_mmHg(uint raw)
        {
            if (!IsInitialized)
                return null;
            return (raw * cps_gain) + cps_offset;
        }
    }
}
