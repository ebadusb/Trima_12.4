using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Linq.Expressions;
using System.ComponentModel;
using System.Xml.Linq;

namespace BCT.Common
{
    public static class Extension
    {
        /// <summary>
        /// Helper function to extract the property name given the property expression
        /// </summary>
        /// <typeparam name="T"></typeparam>
        /// <param name="property"></param>
        /// <returns></returns>
        public static string GetPropertyName<T>(Expression<Func<T>> property)
        {
            MemberExpression memberExpression = property.Body as MemberExpression;
            if (memberExpression != null)
                return memberExpression.Member.Name;
            return string.Empty;
        }
        /// <summary>
        /// This is the method that returns the method name
        /// </summary>
        /// <typeparam name="T"></typeparam>
        /// <param name="action"></param>
        /// <returns></returns>
        public static string GetMethodName<T>(Expression<Action<T>> action)
        {
            MethodCallExpression methodCall = action.Body as MethodCallExpression;
            if (methodCall != null)
                return methodCall.Method.Name;
            return string.Empty;
        }

        public static string GetPropertyName(this object obj, Expression<Func<object>> property)
        {
            return GetPropertyName<object>(property);
        }

        /// <summary>
        /// This is a helper mehtod to get the value of an xelement given the local name
        /// </summary>
        /// <param name="elementName"></param>
        /// <param name="elements"></param>
        /// <returns></returns>
        public static string GetElement(this IEnumerable<XElement> elements, string elementName)
        {
            var name = elements.Where(c => c.Name.LocalName == elementName).FirstOrDefault();
            if (name != null)
                return name.Value;
            return string.Empty;
        }
        /// <summary>
        /// This is the
        /// </summary>
        /// <param name="elements"></param>
        /// <param name="elementName"></param>
        /// <returns></returns>
        public static int? GetElementInt(this IEnumerable<XElement> elements, string elementName)
        {
            var name = elements.Where(c => c.Name.LocalName == elementName).FirstOrDefault();
            if (name == null)
                return new int?();
            return name.GetElementInt();
        }
        /// <summary>
        /// this is the method that returns the int 
        /// </summary>
        /// <param name="elements"></param>
        /// <param name="elementName"></param>
        /// <returns></returns>
        public static int? GetElementInt(this XElement element)
        {
            int? retVal = new int?();
            int converted = 0;
            if (int.TryParse(element.Value, out converted))
                retVal = converted;
            return retVal;
        }
        /// <summary>
        /// this is the method that returns the int 
        /// </summary>
        /// <param name="elements"></param>
        /// <param name="elementName"></param>
        /// <returns></returns>
        public static DateTime? GetElementEpochDate(this IEnumerable<XElement> elements, string elementName)
        {
            var name = elements.Where(c => c.Name.LocalName == elementName).FirstOrDefault();
            if (name == null)
                return null;
            return name.GetElementEpochDate();
        }
        /// <summary>
        /// this is the method that returns the int 
        /// </summary>
        /// <param name="elements"></param>
        /// <param name="elementName"></param>
        /// <returns></returns>
        public static DateTime? GetElementEpochDate(this XElement element)
        {
            DateTime? retVal = default(DateTime?);
            int converted = 0;
            if (int.TryParse(element.Value, out converted) && converted > 0)
            {
                DateTime d = new DateTime(1970, 1, 1, 0, 0, 0, DateTimeKind.Local);
                retVal = d.AddSeconds(converted);
            }
            return retVal;
        }
        /// <summary>
        /// This is the
        /// </summary>
        /// <param name="elements"></param>
        /// <param name="elementName"></param>
        /// <returns></returns>
        public static decimal? GetElementFloat(this IEnumerable<XElement> elements, string elementName)
        {
            var name = elements.Where(c => c.Name.LocalName == elementName).FirstOrDefault();
            if (name == null)
                return new decimal?();
            return name.GetElementFloat();
        }
        /// <summary>
        /// this is the method that returns the int 
        /// </summary>
        /// <param name="elements"></param>
        /// <param name="elementName"></param>
        /// <returns></returns>
        public static decimal? GetElementFloat(this XElement element)
        {
            decimal? retVal = new decimal?();
            decimal converted = 0;
            if (decimal.TryParse(element.Value, out converted))
                retVal = converted;
            return retVal;
        }
    }

}
