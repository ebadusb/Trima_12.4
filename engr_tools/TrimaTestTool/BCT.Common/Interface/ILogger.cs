using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Diagnostics;
using Microsoft.Practices.EnterpriseLibrary.Logging;

namespace BCT.Common.Interface
{
    public enum LogType
    {
        /// <summary>
        /// the log is added for documentation purposes.
        /// </summary>
        Info,
        /// <summary>
        /// the log is added for debugging purposes.
        /// </summary>
        Debug,
        /// <summary>
        /// the system encountered situation that warrants to issue warning to 
        /// the user.
        /// </summary>
        Warning,
        /// <summary>
        /// the system encountered error, but can still
        /// continue to function properly
        /// </summary>
        SoftError,
        /// <summary>
        ///the system cannot recover from this error 
        /// </summary>
        HardError,
        /// <summary>
        /// this log type is use to extend this interface
        /// </summary>
        Extension,
    }
    /// <summary>
    /// This is the interface to log
    /// </summary>
    public interface ILogger
    {
        /// <summary>
        /// method to log info type
        /// </summary>
        /// <param name="message"></param>
        void Info(string message, string category = null, string title = null, IDictionary<string, object> properties = null);
        /// <summary>
        /// method to log debug type
        /// </summary>
        /// <param name="message"></param>
        void Debug(string message, string category = null, string title = null, IDictionary<string, object> properties = null);
        /// <summary>
        /// method to log warning type
        /// </summary>
        /// <param name="message"></param>
        void Warning(string message, string category = null, string title = null, IDictionary<string, object> properties = null);
        /// <summary>
        /// method to log softerror
        /// </summary>
        /// <param name="message"></param>
        void SoftError(string message, string category = null, string title = null, IDictionary<string, object> properties = null);
        /// <summary>
        /// method to log harderror
        /// </summary>
        /// <param name="message"></param>
        void HardError(string message, string category = null, string title = null, IDictionary<string, object> properties = null);
        /// <summary>
        /// method to log extended type
        /// </summary>
        /// <param name="type"></param>
        /// <param name="message"></param>
        void Extension(string type, string message, string category = null, string title = null, IDictionary<string, object> properties = null);
    }
}
