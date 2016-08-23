using System;
using System.Collections.Generic;
using System.Linq;
using System.Text.RegularExpressions;
using System.Text;
using System.IO;


namespace BCT.Device.Utility
{
    public sealed class DatFileReader : IDisposable
    {
        private static string sectionFormat = @"\[{0}\]";
        private static Regex anySectionRegex = new Regex(@"\[\w*\]");
        private StreamReader m_reader;
        /// <summary>
        /// prevent from creating instances outside 
        /// of this class
        /// </summary>
        static DatFileReader()
        {
        }

        static public DatFileReader Open(Stream stream)
        {
            if (stream == null)
                return null;
            DatFileReader datFile = new DatFileReader();
            datFile.m_reader = new StreamReader(stream);
            return datFile;
        }

        static public DatFileReader Open(string path)
        {
            DatFileReader datFile = new DatFileReader();
            datFile.m_reader = new StreamReader(File.OpenRead(path));
            return datFile;
        }

        public Dictionary<string, string> GetSection(string section, bool seekBegin = true)
        {
            if (m_reader == null)
                return null;

            Dictionary<string, string> retVal = null;

            Regex regex = new Regex(string.Format(sectionFormat, section));
            if( seekBegin)
                m_reader.BaseStream.Seek(0, SeekOrigin.Begin);
            while (true)
            {
                string line = m_reader.ReadLine();
                if (m_reader.EndOfStream)
                    break;

                if (line.StartsWith("#"))
                    continue;

                Match match = regex.Match(line);
                if (!match.Success)
                    continue;
               
               retVal = GetSection(m_reader);
               break;
            }
            return retVal;
        }

        private static bool IsSection(string line)
        {
            Match match = anySectionRegex.Match(line);
            return match.Success;
        }

        private Dictionary<string, string> GetSection(StreamReader reader)
        {
            if(reader == null || reader.EndOfStream )
                return null;
            Dictionary<string, string> list = new Dictionary<string, string>();
            while (true)
            {
                string line = reader.ReadLine();
                if (IsSection(line))
                    break;
                string[] keyvalue = line.Split(new char[] { '=' });
                if (keyvalue.Length != 2)
                    continue;
                list[keyvalue[0].Trim()] = keyvalue[1].Trim();
            }
            return list;
        }

        #region IDisposable Members

        public void Dispose()
        {
            if (m_reader != null)
            {
                m_reader.Dispose();
                m_reader = null;
            }
        }

        #endregion
    }
}
