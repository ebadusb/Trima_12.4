#include "listview.hpp"
#include <vxworks.h>

/*
these are the definition of the the keyboard origin
*/
const int ORIG_X               = 20;
const int ORIG_Y               = 110;

const int SCROLL_BUTTON_HEIGHT = 29;
const int SCROLL_BUTTON_WIDTH  = 36;
const int LIST_ITEM_HEIGHT     = 31;
const int LIST_HEIGHT          = 235;
const int LIST_WIDTH           = 280;
const int TEXT_FIELD_WIDTH     = 280;
const int RIGHT_COL_MARGIN     = 10;
const int TEXT_VALUE_MARGIN    = 20;
const int LABEL_HEIGHT         = 20;
const int TEXT_VALUE_HEIGHT    = 25;
const int KB_BUTTON_OFFSET     = 90;

///////////////////////////////////////////////////
// ListView
///////////////////////////////////////////////////
ListView::ListView(int maxDisplayedItems)
   : m_upButton(NULL), m_dnButton(NULL)

{
   m_maxDisplayedItems = maxDisplayedItems;
   set_screen_id(GUI_SCREEN_LISTVIEW);
   m_topItem           = NULL;
   m_parent            = NULL;
   m_bottomItem        = NULL;
}

ListView::~ListView()
{
   list<ListItem*>::iterator iter = m_displayedItems.begin();
   for ( ; iter != m_displayedItems.end() ; iter++ )
      delete (*iter);
   delete m_upButton;
   delete m_dnButton;
}

void ListView::Initialize (int init_x, int init_y)
{
   this->set_position(init_x, init_y);

   dlt iter = m_displayedItems.begin();
   for ( ; iter != m_displayedItems.end() ; iter++)
      delete(*iter);
   if ( m_displayedItems.size() > 0)
      m_displayedItems.clear();

   int y = init_y + 9, x = init_x + SCROLL_BUTTON_WIDTH + 6;
   for ( int i = 0; i < m_maxDisplayedItems; i++)
   {
      ListItem* item = new ListItem();
      item->Initialize(x, y);
      m_displayedItems.push_back(item);
      y += LIST_ITEM_HEIGHT;
   }

   // scroll bar buttons
   // 36 x 29
   m_upButton = new Button_Bitmap(GUI_BUTTON_CONFIG_SOFT_OPTION_UP,
                                  init_x,
                                  init_y,
                                  textEmpty,
                                  BITMAP_BUTTON_CONFIG_SOFTOPT_SCROLLUP,
                                  BITMAP_BUTTON_CONFIG_SOFTOPT_SCROLLALTER,
                                  true, false);
   // 36 x 29
   m_dnButton = new Button_Bitmap(GUI_BUTTON_CONFIG_SOFT_OPTION_DN,
                                  init_x,
                                  init_y + LIST_HEIGHT + SCROLL_BUTTON_HEIGHT,
                                  textEmpty,
                                  BITMAP_BUTTON_CONFIG_SOFTOPT_SCROLLDN,
                                  BITMAP_BUTTON_CONFIG_SOFTOPT_SCROLLALTER,
                                  true, false);
}

void ListView::Initialize (const list<TEXT_STRING_ID>& items)
{
   m_topItem    = NULL;
   m_bottomItem = NULL;
   if (m_items.size() > 0)
      m_items.clear();
   m_items = items;
   list<TEXT_STRING_ID>::const_iterator sourceIter = items.begin();
   list<ListItem*>::iterator            iter       = m_displayedItems.begin();
   for ( int i = 0 ; iter != m_displayedItems.end() && sourceIter != items.end() ; iter++, sourceIter++, i++ )
   {
      (*iter)->SetText(*sourceIter);
      if ( i == 0 )
         m_topItem = (*iter);
      if ( i == m_maxDisplayedItems - 1 )
         m_bottomItem = (*iter);
   }
   // set others empty
   for ( ; iter != m_displayedItems.end() ; iter++ )
      (*iter)->SetText(textCfgSoftOptEmptyItem);

   if ( m_parent != NULL )
   {
      ShowUpButton(*m_parent);
      ShowDnButton(*m_parent);
   }
}

void ListView::allocate_resources (OSWindow& parent)
{
   AllocateChildren(parent);
}

void ListView::AllocateChildren (OSWindow& parent)
{
   m_parent = &parent;
   list<ListItem*>::iterator iter = m_displayedItems.begin();
   for ( ; iter != m_displayedItems.end() ; iter++ )
      (*iter)->allocate_resources(parent);

   ShowUpButton(parent);
   ShowDnButton(parent);
}

void ListView::ShowUpButton (OSWindow& parent)
{
   // show ub button when
   // items count > displayed items and
   // id of ist item != ist displayed item
   bool show = m_items.size() > m_displayedItems.size() && m_topItem != NULL &&
               (*(m_items.begin())) != m_topItem->GetID();
   if ( show )
   {
      m_upButton->allocate_resources(parent);
      m_upButton->set_callback(ButtonCallbackUp, (void*)this);
   }
   else
      m_upButton->deallocate_resources();
}

void ListView::ShowDnButton (OSWindow& parent)
{
   // show dn button when
   // items count > displayed items and
   // id of last item != last displayed item
   bool show = m_items.size() > m_displayedItems.size() && m_bottomItem != NULL &&
               (*(m_items.rbegin())) != m_bottomItem->GetID();
   if ( show )
   {
      m_dnButton->allocate_resources(parent);
      m_dnButton->set_callback(ButtonCallbackDn, (void*)this);
   }
   else
      m_dnButton->deallocate_resources();
}

void ListView::deallocate_resources ()
{
   dlt iter = m_displayedItems.begin();
   for ( ; iter != m_displayedItems.end() ; iter++)
      (*iter)->deallocate_resources();
   m_upButton->deallocate_resources();
   m_dnButton->deallocate_resources();

}

void ListView::ButtonCallbackUp (void* data)
{
   ListView* instance = (ListView*)data;
   instance->ScrollUp(*instance->m_parent);
}

void ListView::ButtonCallbackDn (void* data)
{
   ListView* instance = (ListView*)data;
   instance->ScrollDn(*instance->m_parent);
}

void ListView::ScrollUp (OSWindow& parent)
{
   if ( !m_upButton->allocated() || m_bottomItem == NULL )
      return;

   ltr iter = m_items.rbegin();
   for ( ; iter != m_items.rend() ; iter++)
      if ( (*iter) == m_bottomItem->GetID())
         break;
   // move item dn
   iter++;
   dltr diter = m_displayedItems.rbegin();
   for ( ; iter != m_items.rend() && diter != m_displayedItems.rend() ; iter++, diter++)
      (*diter)->SetText(*iter);

   ShowUpButton(parent);
   ShowDnButton(parent);
}

void ListView::ScrollDn (OSWindow& parent)
{
   if ( !m_dnButton->allocated() || m_topItem == NULL )
      return;

   lt iter = m_items.begin();
   for ( ; iter != m_items.end() ; iter++)
      if ( (*iter) == m_topItem->GetID())
         break;
   // move item up
   iter++;

   dlt diter = m_displayedItems.begin();
   for ( ; iter != m_items.end() && diter != m_displayedItems.end() ; iter++, diter++)
      (*diter)->SetText(*iter);
   ShowUpButton(parent);
   ShowDnButton(parent);
}

/////////////////////////////////////////////////
// -----------------------------------------------
// Class ListItem
ListItem::ListItem()
   : m_x(0), m_y(0), m_itemBg(NULL), m_text(NULL), m_stringID(TEXT_NULL)
{
   strData.initialize();
}

ListItem::~ListItem()
{
   if ( m_itemBg != NULL )
   {
      delete m_itemBg;
      m_itemBg = NULL;
   }

   if ( m_text != NULL )
   {
      delete m_text;
      m_text = NULL;
   }
}

void ListItem::Initialize (int x, int y)
{
   m_x      = x; m_y = y;
   m_itemBg = new Bitmap(x, y, BITMAP_BG_CONFIG_SOFTOPT_LIST_ITEM);
   m_text   = new Display_Text(textCfgSoftOptEmptyItem);
}

void ListItem::SetText (TEXT_STRING_ID id)
{
   m_stringID = id;
   m_text->set_string_id(m_stringID);
   OSRegion region = m_text->get_region();
   m_text->set_requested_region(OSRegion(m_x + region.x, m_y + region.y, region.width, region.height));
}

void ListItem::allocate_resources (OSWindow& parent)
{
   m_itemBg->allocate_resources(parent);
   m_text->allocate_resources(parent);
}

void ListItem::deallocate_resources ()
{
   if ( m_itemBg->allocated() )
      m_itemBg->deallocate_resources();
   if ( m_text->allocated() )
      m_text->deallocate_resources();
}

const TEXT_STRING_ID& ListItem::GetID () const
{
   return m_stringID;
}

bool ListItem::IsAllocated () const
{
   if ( m_itemBg == NULL || m_text == NULL )
      return false;
   return m_itemBg->allocated() && m_text->allocated();
}

/* FORMAT HASH 881bd91523bfdcfd748f76c1214c0f6d */
