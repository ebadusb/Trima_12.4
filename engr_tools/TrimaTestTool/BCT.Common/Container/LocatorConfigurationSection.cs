using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Configuration;
using System.Reflection;

using BCT.Common;
using BCT.Common.Interface;

namespace BCT.Common.Container
{
    /// <summary>
    /// This is the configuration section customized for Locator
    /// </summary>
    public class LocatorConfigurationSection : ConfigurationSection
    {
        /// <summary>
        /// This is the static string "Assembly"
        /// </summary>
        public const string Assembly = "Assembly";
        /// <summary>
        /// This is the static string "Container"
        /// </summary>
        public const string Container = "Container";
        /// <summary>
        /// This is the static string "Value"
        /// </summary>
        public const string Value = "Value";
        /// <summary>
        /// This is the static string "Locator"
        /// </summary>
        public const string Locator = "Locator";

        [ConfigurationProperty(LocatorConfigurationSection.Assembly, IsRequired = true)]
        public LocatorElement AssemblyElement
        {
            get
            {
                return (LocatorElement)this[LocatorConfigurationSection.Assembly];
            }
            set
            {
                this[LocatorConfigurationSection.Assembly] = value;
            }
        }

        [ConfigurationProperty(LocatorConfigurationSection.Container, IsRequired = true)]
        public LocatorElement ContainerElement
        {
            get
            {
                return (LocatorElement)this[LocatorConfigurationSection.Container];
            }
            set
            {
                this[LocatorConfigurationSection.Container] = value;
            }

        }

        public static LocatorConfigurationSection GetSection()
        {
            var section = ConfigurationManager.GetSection(LocatorConfigurationSection.Locator) as LocatorConfigurationSection;
            return section;
        }

        public class LocatorElement : ConfigurationElement
        {
            [ConfigurationProperty(LocatorConfigurationSection.Value, IsRequired = true)]
            public string Value
            {
                get { return (string)this[LocatorConfigurationSection.Value]; }
                set
                {
                    this[LocatorConfigurationSection.Value] = value;
                }
            }
        }

    }
}
