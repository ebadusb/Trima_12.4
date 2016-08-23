using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Configuration;

namespace BCT.Common.Interface
{
    public interface IContainer : IDisposable
    {
        void Initialize();
        T Resolve<T>(string type);
        T Resolve<T>();
        /*
        IContainer RegisterInstance(Type t, string name, object instance, LifetimeManager lifetime);
        IContainer RegisterType(Type from, Type to, string name, LifetimeManager lifetimeManager, params InjectionMember[] injectionMembers);
        object Resolve(Type t, string name, params ResolverOverride[] resolverOverrides);
        IEnumerable<object> ResolveAll(Type t, params ResolverOverride[] resolverOverrides);
        */
    }
}
