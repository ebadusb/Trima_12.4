#ifndef MODULE_GROUP_INCLUDE
#define MODULE_GROUP_INCLUDE

class ModuleGroup
{
public:

   enum Group
   {
      CONTROL_STATUS,
      CONTROL_COMMAND,
      SAFETY_STATUS,
      SAFETY_COMMAND,
      SET_TYPE,
      STATE_CHANGE,
      LAST_GROUP
   };

   virtual ~ModuleGroup() {}

   virtual int size () const = 0;

   ModuleGroup::Group getType () const { return mtype; }

protected:

   ModuleGroup(ModuleGroup::Group type)
      : mtype(type){}

   ModuleGroup::Group mtype;

};

#endif

/* FORMAT HASH 029ff18389cda7dc3b41ef26a25b61cf */
