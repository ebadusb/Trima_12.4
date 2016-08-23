using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Configuration;
using System.Reflection;

using BCT.Common.Interface;


namespace BCT.Common.Container
{
    public class Locator
    {
        private static IContainer _instance;
        public static IContainer Instance
        {
            get
            {
                if (_instance == null)
                    LocateContainer();
                return _instance;
            }
        }

        /// <summary>
        /// There is a mimimal effor on checking the
        /// values 
        /// </summary>
        private static void LocateContainer()
        {
            LocatorConfigurationSection section = LocatorConfigurationSection.GetSection();
            if (section != null)
            {
                Assembly assy = Assembly.Load(section.AssemblyElement.Value);
                if (assy == null)
                    return;

                Type type = assy.GetType(section.ContainerElement.Value);
                if (type == null)
                    return;

                ConstructorInfo constructor = type.GetConstructor(new Type[]{});
                if (constructor == null)
                    return;

                _instance = constructor.Invoke(null) as IContainer;
            }
        }
    }
}
