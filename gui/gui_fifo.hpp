/************************> FOCUSSED GUI DOMAIN <****************************

*     Copyright(c) 2000 Gambro BCT Inc. All rights reserved

*     Author:       Terry Wahl

*     Class name:   GUI_FIFO

*     File name:    gui_fifo.hpp

*     Contents:     See prototype header descriptions below...

*     Description:

         This contains the TEMPLATE class declaration for a FIFO object that stores any
         parameterized structure

*************************< FOCUSSED GUI DOMAIN >***************************/


// Single include define guardian
#ifndef GUI_FIFO_HPP
#define GUI_FIFO_HPP

// SYSTEM INCLUDES

#include <string.h>
#include <unistd.h>
#include "trima_ugl_mem.h"

// TRIMA HEADERS
#include "error.h"


#define  FIFO_SUCCESS 1
#define  FIFO_FAILURE 0


template <class FIFO_DATA_STRUCTURE>

class GUI_FIFO
{
private:

   // Linkage element used in linked-list FIFO
   typedef struct FE
   {
      FE*                 next_element; // really a pointer to a FIFO_ELEMENT
      FE*                 prev_element; // really a pointer to a FIFO_ELEMENT
      FIFO_DATA_STRUCTURE fifo_data;
   } FIFO_ELEMENT;


   // FIFO anchors: FIRST_IN is the incoming anchor; FIRST_OUT is the removal anchor
   FIFO_ELEMENT FIRST_IN;
   FIFO_ELEMENT FIRST_OUT;

   // FIFO anchors for the free pool: same as the FIFO so several methods could be re-used
   //     This pool is a warehouse for currently unused FIFO elements.
   FIFO_ELEMENT FREE_IN;
   FIFO_ELEMENT FREE_OUT;

   // Unimplemented methods - no copies; assignments
   GUI_FIFO(GUI_FIFO const&);
   GUI_FIFO& operator = (GUI_FIFO const&);


   //////////////////////////////////////////////////////////////////////////////////

   // INSERT_FIFO_ELEMENT
   // This method accepts pointers to two FIFO_ELEMENTs. The first is the existing
   // and linked fifo element and the second is the one that will be inserted immediately
   // after it (in either the FIFO or the Free pool).
   void insert_FIFO_element (FIFO_ELEMENT* head_of_list, FIFO_ELEMENT* insertee)
   {
      // Link up the "insertee" between the head of list and the next element in the list.
      insertee->next_element = head_of_list->next_element;
      insertee->prev_element = head_of_list;

      // Link up the head of list forward to the insertee
      head_of_list->next_element = insertee;

      // Link back from the old "first element" to the new insertee
      (insertee->next_element)->prev_element = insertee;
   }  // End of the insert_FIFO_element

   //////////////////////////////////////////////////////////////////////////////////

   // REMOVE_FIFO_ELEMENT
   // This method is passed a pointer to a fifo_element that is linked in either the
   // FIFO or the Free pool. It will unlink it from its current chain. The caller is
   // obliged to manage it movement/placement/address after removal. The passed in
   // FIFO element pointer will be returned by this method.
   FIFO_ELEMENT* remove_FIFO_element (FIFO_ELEMENT* removal_element)
   {
      // Ensure that this is NOT one of the head of chains
      if ((removal_element == &FIRST_IN)  ||
          (removal_element == &FIRST_OUT) ||
          (removal_element == &FREE_IN)   ||
          (removal_element == &FREE_OUT) )
      {
         _FATAL_ERROR (__FILE__, __LINE__, "GUI FIFO: attempted to FREE a chain anchor element");
         return(removal_element); // satisfy an over zealous compiler.
      }

      // Link its previous element's forward pointer past this (removal) element
      (removal_element->prev_element)->next_element = removal_element->next_element;

      // Link its next element back to its previous element
      (removal_element->next_element)->prev_element = removal_element->prev_element;

      // Tag it as "free"
      (removal_element->prev_element) = NULL;
      (removal_element->next_element) = NULL;

      return(removal_element);
   }  // End of the remove_FIFO_element

   //////////////////////////////////////////////////////////////////////////////////

   // GET_A_FREE_FIFO_ELEMENT
   // This method will attempt to retrieve (an unlink) a free fifo_element from the
   // Free pool. If the free pool is empty, it will allocate a new element and return
   // its pointer.
   FIFO_ELEMENT* get_a_free_FIFO_element ()
   {
      // Is the Free pool empty ?
      if (FREE_IN.next_element == &FREE_OUT ||
          FREE_OUT.prev_element == &FREE_IN)
      {
         FIFO_ELEMENT* new_fifo_element = (FIFO_ELEMENT*)GUI_POOL_MALLOC(sizeof(FIFO_ELEMENT));

         // Yes, the pool is empty so create a FIFO_ELEMENT, initialize and return it
         new_fifo_element->prev_element = NULL;
         new_fifo_element->next_element = NULL;

         // return the newly allocated
         return(new_fifo_element);
      }

      // unlink the first FREE candidate and return it
      return(remove_FIFO_element(FREE_OUT.prev_element));
   }  // End of the get_a_free_FIFO_element

   //////////////////////////////////////////////////////////////////////////////////

   // PUT_A_FREE_FIFO_ELEMENT
   // This method will receive a FIFO element pointer and link it into the Free pool.
   void put_a_free_FIFO_element (FIFO_ELEMENT* free_element)
   {
      // Ensure that this is NOT linked
      if ((free_element->prev_element) != NULL ||
          (free_element->next_element) != NULL)
      {
         _FATAL_ERROR (__FILE__, __LINE__, "GUI FIFO: attempted to FREE-POOL a linked FIFO element");
      }

      insert_FIFO_element(&FREE_IN, free_element);
   }  // End of the put_a_free_FIFO_element

   //////////////////////////////////////////////////////////////////////////////////

   // DELETE_ALL
   // This method will C++ delete all the fifo elements in the free pool
   void delete_all ()
   {
      // Run through all the FREE pool items, de-link them and C++ delete them.
      while (FREE_IN.next_element != &FREE_OUT)
      {
         FIFO_ELEMENT* elementPtr = remove_FIFO_element(FREE_OUT.prev_element);
         // De-link the first FIFO item and put it in the FREE pool
         if (elementPtr) GUI_POOL_FREE(elementPtr);
      }
   }  // End of the delete_all

//////////////////////////////////////////////////////////////////////////////////
protected:
   // COPY_DATA_STRUCTURE_DATA
   // This method will copy the data found in a data structure into
   // another data structure. Both are passed as pointer arguments to this method.
   virtual void copy_data_structure_data (const FIFO_DATA_STRUCTURE* data_source,
                                          FIFO_DATA_STRUCTURE* data_target)
   {
      memmove((void*)data_target, (const void*)data_source, sizeof (FIFO_DATA_STRUCTURE));
   }


//////////////////////////////////////////////////////////////////////////////////
public:

//////////////////////////////////////////////////////////////////////////////////
// CONSTRUCTOR
   GUI_FIFO ()
   {
      // Initialize the FIFO linkages
      FIRST_IN.prev_element  = NULL;
      FIRST_IN.next_element  = &FIRST_OUT;

      FIRST_OUT.prev_element = &FIRST_IN;
      FIRST_OUT.next_element = NULL;


      // Initialize the Free pool linkages
      FREE_IN.prev_element  = NULL;
      FREE_IN.next_element  = &FREE_OUT;

      FREE_OUT.prev_element = &FREE_IN;
      FREE_OUT.next_element = NULL;
   }  // End of the GUI_FIFO constructor


   //////////////////////////////////////////////////////////////////////////////////
   // DESTRUCTOR
   virtual ~GUI_FIFO ()
   {
      // Free all the FIFO elements
      free_all();

      // Deallocate all the Free pool elements
      delete_all();
   }  // End of the GUI_FIFO destructor

   //////////////////////////////////////////////////////////////////////////////////

   // ADD_DATA_TO_FIFO
   // This will take a pointer to a data structure and copy
   // the data into a local data structure and place it into the FIFO.
   void add_data_to_FIFO (const FIFO_DATA_STRUCTURE* current_data)
   {
      // Get a free fifo element
      FIFO_ELEMENT* new_fifo_ele = get_a_free_FIFO_element();

      // Copy the caller's data in the data structure portion of the fifo element
      copy_data_structure_data(current_data, &(new_fifo_ele->fifo_data));

      // Put it into the FIFO
      insert_FIFO_element(&FIRST_IN, new_fifo_ele);
   }  // End of the add_data_to_FIFO


   //////////////////////////////////////////////////////////////////////////////////
   // PRESET_FREEPOOL_SIZE
   // This method will preallocate and add n number of FIFO elements to the free pool
   // so that during processing there is less chance of performing a memory allocation
   // of a FIFO element.
   void preset_freepool_size (int number_of_FIFO_elements)
   {
      if (number_of_FIFO_elements > 0)
      {
         preallocate_free_pool (number_of_FIFO_elements);
      }
   }  // End of the preset_freepool_size



   //////////////////////////////////////////////////////////////////////////////////
   // RETRIEVE_DATA_FROM_FIFO
   // This will copy the data from the "first out" data structure into the structure
   // passed by the caller. It will then remove the structure from the FIFO and
   // free any associated memory. If there are no more items in the FIFO, the routine
   // will return a FIFO_FAILURE, otherwise a FIFO_SUCCESS.
   int retrieve_data_from_FIFO (FIFO_DATA_STRUCTURE* retrieve_data)
   {
      // See if the FIFO has anything in it and copy the data over to the caller's buffer.
      if (peek_next_data_from_FIFO(retrieve_data) == FIFO_SUCCESS)
      {
         // Remove the current first out fifo element and put it in the free pool
         put_a_free_FIFO_element(remove_FIFO_element(FIRST_OUT.prev_element));

         // Return triumphant
         return(FIFO_SUCCESS);
      }
      else
      {
         return(FIFO_FAILURE);
      }
   }  // End of the retrieve_data_from_FIFO



   //////////////////////////////////////////////////////////////////////////////////
   // PEEK_NEXT_DATA_FROM_FIFO
   // This will do the same function as the RETRIEVE_DATA_FROM_FIFO method above.
   // However, it will not remove the item from the FIFO -- it will remain the
   // "first out" item. If there are no more items in the FIFO, the routine
   // will return a FIFO_FAILURE, otherwise a FIFO_SUCCESS.
   int peek_next_data_from_FIFO (FIFO_DATA_STRUCTURE* peek_data)
   {
      // See if the FIFO has anything in it.
      if (FIFO_has_data() == FIFO_SUCCESS)
      {
         // Copy the fifo data into the caller's buffer
         copy_data_structure_data(&(FIRST_OUT.prev_element->fifo_data), peek_data);

         // Return triumphant
         return(FIFO_SUCCESS);
      }
      else
      {
         return(FIFO_FAILURE);
      }
   }  // End of the peek_next_data_from_FIFO



   //////////////////////////////////////////////////////////////////////////////////
   // FREE_ALL
   // This method will put all the elements in the FIFO into the Free pool.
   void free_all ()
   {
      // Run through all the FIFO items, de-link them and add them to the free pool.
      while (FIFO_has_data() == FIFO_SUCCESS)
      {
         // De-link the first FIFO item and put it in the FREE pool
         put_a_free_FIFO_element(remove_FIFO_element(FIRST_OUT.prev_element));
      }

   }  // End of the free_all



   //////////////////////////////////////////////////////////////////////////////////
   // FIFO_HAS_DATA
   // This method will return FIFO_SUCCESS if the FIFO has data in it, and FIFO_FAILURE if the
   // FIFO is empty.
   int FIFO_has_data ()
   {
      // Is the FIFO empty ?
      if (FIRST_IN.next_element != &FIRST_OUT)
      {
         // Input anchor didn't point to output anchor so there must be intervening fifo elements
         return(FIFO_SUCCESS);
      }

      return(FIFO_FAILURE);
   }  // End of the FIFO_has_data



   //////////////////////////////////////////////////////////////////////////////////
   // PREALLOCATE_FREE_POOL
   // This method will create n number of FIFO_ELEMENTS and link them into the free pool
   // in anticipation of their use as the FIFO grows.
   virtual void preallocate_free_pool (int number_of_elements)
   {
      for (int i = 1; i <= number_of_elements; i++)
      {
         FIFO_ELEMENT* new_fifo_element = (FIFO_ELEMENT*)GUI_POOL_MALLOC(sizeof(FIFO_ELEMENT));
         new_fifo_element->prev_element = NULL;
         new_fifo_element->next_element = NULL;

         put_a_free_FIFO_element(new_fifo_element);
      }
   }  // End of the preallocate_free_pool

};  // END of GUI_FIFO Template Class Declaration



// End of single include define guardian
#endif

/* FORMAT HASH 21f8d841cc0be22ca49a28cd543b8074 */
