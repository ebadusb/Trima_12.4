#ifndef __LIST_VIEW_HPP_
#define __LIST_VIEW_HPP_

/** include files **/
#include <map>
#include "cfg_butt.hpp"
#include "button_bitmap.h"
#include "guistring.h"
#include "screen_config.hpp"

// forward declaration
class ListItem;

///////////////////////////////////////////////////////////////////
// class definition
class ListView
   : public Base_Window
{
protected:
   typedef list<TEXT_STRING_ID>::iterator lt;
   typedef list<TEXT_STRING_ID>::reverse_iterator ltr;
   typedef list<ListItem*>::iterator dlt;
   typedef list<ListItem*>::reverse_iterator dltr;

   int                  m_maxDisplayedItems;
   Button_Bitmap*       m_upButton;
   Button_Bitmap*       m_dnButton;
   list<ListItem*>      m_displayedItems;
   list<TEXT_STRING_ID> m_items;
   ListItem*            m_topItem;
   ListItem*            m_bottomItem;
   OSWindow*            m_parent;
public:
   typedef void (ListView::* CallbackMethod)(void*);

   ListView(int maxDisplayedItems);
   virtual ~ListView();

   void Initialize (int x, int y);
   void Initialize (const list<TEXT_STRING_ID>& items);

   virtual void allocate_resources (OSWindow& parent);
   virtual void deallocate_resources ();
   static void  ButtonCallbackUp (void* data);
   static void  ButtonCallbackDn (void* data);
protected:
   void AllocateChildren (OSWindow& parent);
   void ScrollUp (OSWindow& parent);
   void ScrollDn (OSWindow& parent);
   void ShowUpButton (OSWindow& parent);
   void ShowDnButton (OSWindow& parent);
};

class ListItem
{
private:
   int            m_x;
   int            m_y;
   Bitmap*        m_itemBg;
   Display_Text*  m_text;
   guistring      strData;
   TEXT_STRING_ID m_stringID;

   // noncopyable, nonassignable; with the pointer member variables,
   // copying and assigning brings up all kinds of ownership issues
   // that aren't worth dealing with.
   ListItem(const ListItem&);
   ListItem& operator = (const ListItem&);

public:
   ListItem();
   ~ListItem();
   void                  Initialize (int x, int y);
   void                  SetText (TEXT_STRING_ID id);
   const TEXT_STRING_ID& GetID () const;
   void                  allocate_resources (OSWindow& parent);
   void                  deallocate_resources ();
   bool                  IsAllocated () const;
};


#endif

/* FORMAT HASH 5dfdb423f606245f3db3d32a7a7d0878 */
