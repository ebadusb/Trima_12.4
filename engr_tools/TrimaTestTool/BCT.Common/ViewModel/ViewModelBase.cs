using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.ComponentModel;
using System.Linq.Expressions;

namespace BCT.Common
{
    public class ViewModelBase : INotifyPropertyChanged
    {
        #region INotifyPropertyChanged Members
        /// <summary>
        /// This si the implementation of the PropertyChanged of INotifyPropertyChanged
        /// </summary>
        public event PropertyChangedEventHandler PropertyChanged = delegate { };

        #endregion
        /// <summary>
        /// This is the helper method to raise a changed of property value
        /// </summary>
        /// <param name="propertyName"></param>
        public void RaisePropertyChanged(string propertyName)
        {
            PropertyChanged(this, new PropertyChangedEventArgs(propertyName));
        }
        /// <summary>
        /// This is the method that returns the string representation of its property
        /// </summary>
        /// <param name="property"></param>
        /// <returns></returns>
        public string GetPropertyName<K>(Expression<Func<K>> property)
        {
            return Extension.GetPropertyName(property);
        }
    }
}
