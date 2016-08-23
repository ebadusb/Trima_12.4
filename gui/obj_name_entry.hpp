#ifndef OBJ_NAME_ENTRY_HPP
#define OBJ_NAME_ENTRY_HPP



class Object_Name_Entry
{
public:

   bool           object_registered ; // If object is registered this is TRUE, otherwise FALSE
   BC_OBJECT_NAME object_name ;       // Enumeration name of the object
   void*          object_ptr ;        // Untyped pointer to the object


   Object_Name_Entry()
      : object_registered(false),
        object_name(static_cast<BC_OBJECT_NAME>(-1)), // an impossible value
        object_ptr(NULL)
   {}

};

#endif

/* FORMAT HASH b5cbc186991f7fb75fb02c762b044a24 */
