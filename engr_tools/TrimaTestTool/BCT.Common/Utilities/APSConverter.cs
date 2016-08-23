using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;

namespace BCT.Common.Utilities
{
    public class APSConverter
    {
        /// <summary>
        /// these are the values taken from cal.dat
        /// </summary>
        public static double aps_raw_pos { get; private set; }
        public static double aps_mmHg_pos { get; private set; }
        public static double aps_raw_ref { get; private set; }
        public static double aps_mmHg_ref { get; private set; }
        public static double aps_raw_neg { get; private set; }
        public static double aps_mmHg_neg { get; private set; }

        public static string aps_raw_pos_str = Extension.GetPropertyName(() => APSConverter.aps_raw_pos);
        public static string aps_mmHg_pos_str = Extension.GetPropertyName(() => APSConverter.aps_mmHg_pos);
        public static string aps_raw_ref_str = Extension.GetPropertyName(() => APSConverter.aps_raw_ref);
        public static string aps_mmHg_ref_str = Extension.GetPropertyName(() => APSConverter.aps_mmHg_ref);
        public static string aps_raw_neg_str = Extension.GetPropertyName(() => APSConverter.aps_raw_neg);
        public static string aps_mmHg_neg_str = Extension.GetPropertyName(() => APSConverter.aps_mmHg_neg);

        public static double posGain { get; private set; }
        public static double negGain { get; private set; }

        public static double zero { get; private set; }

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

        public static void SetZero(float refRaw, float refmmHg)
        {
            lock (monitor)
            {
                //this is taken from s_glob.cpp set_mmhg_offset
                if( refmmHg >= 0 && posGain != 0)
                    zero = aps_raw_ref + refmmHg/posGain;
                else if( refmmHg < 0 && negGain != 0)
                    zero = aps_raw_ref + refmmHg/negGain;
            }
        }


        public static void Init(Dictionary<string, string> list)
        {
            if (!list.ContainsKey(aps_raw_pos_str) ||
                !list.ContainsKey(aps_mmHg_pos_str) ||
                !list.ContainsKey(aps_raw_ref_str) ||
                !list.ContainsKey(aps_mmHg_ref_str) ||
                !list.ContainsKey(aps_raw_neg_str) ||
                !list.ContainsKey(aps_mmHg_neg_str) )

                return;
            double aps_raw_pos_input = 0;
            if (!double.TryParse(list[aps_raw_pos_str], out aps_raw_pos_input))
                return;

            double aps_mmHg_pos__input = 0;
            if (!double.TryParse(list[aps_mmHg_pos_str], out aps_mmHg_pos__input))
                return;

            double aps_raw_ref_str_input = 0;
            if (!double.TryParse(list[aps_raw_ref_str], out aps_raw_ref_str_input))
                return;

            double aps_mmHg_ref_str_input = 0;
            if (!double.TryParse(list[aps_mmHg_ref_str], out aps_mmHg_ref_str_input))
                return;

            double aps_raw_neg_input = 0;
            if (!double.TryParse(list[aps_raw_neg_str], out aps_raw_neg_input))
                return;

            double aps_mmHg_neg_input = 0;
            if (!double.TryParse(list[aps_mmHg_neg_str], out aps_mmHg_neg_input))
                return;

            Init(aps_raw_pos_input, aps_mmHg_pos__input, aps_raw_ref_str_input, aps_mmHg_ref_str_input, aps_raw_neg_input, aps_mmHg_neg_input);
        }

        static public void Init(double aps_raw_pos, double aps_mmHg_pos,
                              double aps_raw_ref, double aps_mmHg_ref,
                              double aps_raw_neg, double aps_mmHg_neg)
        {
            lock (monitor)
            {
                APSConverter.aps_raw_pos = aps_raw_pos;
                APSConverter.aps_raw_ref = aps_raw_ref;
                APSConverter.aps_raw_neg = aps_raw_neg;

                APSConverter.aps_mmHg_pos = aps_mmHg_pos;
                APSConverter.aps_mmHg_ref = aps_mmHg_ref;
                APSConverter.aps_mmHg_neg = aps_mmHg_neg;

                posGain = (aps_mmHg_pos - aps_mmHg_ref) / (aps_raw_pos - aps_raw_ref);
                negGain = (aps_mmHg_neg - aps_mmHg_ref) / (aps_raw_neg - aps_raw_ref);

                if (aps_mmHg_ref > negGain && negGain != 0)
                    zero = aps_raw_ref - aps_mmHg_ref / negGain;
                else if (aps_mmHg_ref < 0 && posGain != 0)
                    zero = aps_raw_ref - aps_mmHg_ref / posGain;
                else
                    zero = aps_raw_ref;
                IsInitialized = true;
            }
        }

        static public double? To_mmHg(uint raw)
        {
            if (!IsInitialized)
                return null;
            /*
            double zeroed = (raw - zero);
            double gain = ((raw >= zero) ? posGain : negGain);
            double temp = zeroed * gain;
            return temp;
            */
            lock(monitor)
            return (raw - zero) * ((raw >= zero) ? posGain : negGain);
        }
    }
}
