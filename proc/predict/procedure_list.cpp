/*******************************************************************************
 *
 ******************************************************************************/

#include "procedure_list.hpp"
#include <sstream>
#include <algorithm>

#ifdef __WIN32__
typedef std::basic_ostringstream<char> ostringstream;
#endif

const int        ProcedureList::MAXLEN         = 55;

procedureDetail* ProcedureList::_selected      = NULL;
procedureDetail* ProcedureList::_current       = NULL;
int              ProcedureList::_maxDesignator = 0;


ProcedureList& ProcedureList::Instance ()
{
   static ProcedureList p;
   return p;
}



ProcedureList::ProcedureList()
   : _list()
{
   _list.reserve(MAXLEN);

   // Since the singleton data members are accessed via nonstatic member
   // functions, you have to have a ref to the singleton ProcedureList to get at
   // 'em; therefore, we'll initialize them here (probably redundantly) to
   // guarantee they're initialized before first use.

   _selected      = _current = NULL;
   _maxDesignator = 0;

}

ProcedureList::~ProcedureList()
{
//   DataLog(log_level_predict_info) << "ProcedureList DTOR" << endmsg;
   for (iterator i = _list.begin(); i != _list.end(); ++i)
      delete *i;
}

void ProcedureList::insert (procedureDetail* p)
{
   int n = p->getProcedureNumber();
   if (n != _list.size() + 1)
   {
      ostringstream buf;
      buf << "ProcedureList Error: inserted procedure " << n
          << ", but expected " << _list.size() + 1;
      _FATAL_ERROR (__FILE__, __LINE__, buf.str().c_str() );
   }
   _list.push_back(p);

   DataLog(log_level_predict_info) << "ProcedureList added procedure " << n
                                   << endmsg;

   int designator = p->getDesignator();
   if (designator > _maxDesignator)
      _maxDesignator = designator;

}

procedureDetail* ProcedureList::operator [] (int i)
{
   if (i < 1)
      _FATAL_ERROR (__FILE__, __LINE__, "ProcedureList: 0 is invalid index");

   if (i > _list.size())
      _FATAL_ERROR (__FILE__, __LINE__, "ProcedureList: invalid index, out of range");

   procedureDetail* p = _list[i - 1];

   // if (p->getProcedureNumber() != i)
   //    _FATAL_ERROR (__FILE__, __LINE__, "ProcedureList: bad procedure number");

   return p;
}

procedureDetail* ProcedureList::get (int i)
{
   if (i < 1)
      return NULL;

   if (i > _list.size())
      return NULL;

   procedureDetail* p = _list[i - 1];

   return p;
}

void ProcedureList::replace (procedureDetail* p)
{
   int n = p->getProcedureNumber();
   // DataLog(log_level_predict_info) << "ProcedureList: replacing procedure " << n
   //                                 << endmsg;
   delete _list[n - 1];
   _list[n - 1] = p;

   int designator = p->getDesignator();
   if (designator > _maxDesignator)
      _maxDesignator = designator;
}



void ProcedureList::clear ()
{
   // DataLog(log_level_predict_info) << "ProcedureList clear on size " << _list.size() << endmsg;

   for (iterator i = _list.begin(); i != _list.end(); ++i)
      delete *i;

   _list.erase(_list.begin(), _list.end());
   _list.reserve(MAXLEN);

   _maxDesignator = 0;

}


struct SetUnopt
{
   void operator () (procedureDetail* p) const
   {
      if (!p) return;
      p->SetUnoptimized();
   }
} setUnopt;

void ProcedureList::setAllUnoptimized ()
{
   for_each (_list.begin(), _list.end(), setUnopt);

   // for (iter iter = _list.begin(); iter != _list.end(); iter++)
   // {
   //    (*iter)->SetUnoptimized();
   // }
}

/* FORMAT HASH 26806042a8826f8cc0d5ffa9ad1fa63c */
