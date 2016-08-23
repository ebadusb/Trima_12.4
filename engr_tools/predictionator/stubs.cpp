#include "stappl.h"
#include "datalog.h"
#include "datastore.h"
#include "messagesystemconstant.h"
#include "timermessage.h"

CStdAppl::CStdAppl() {}
CStdAppl::~CStdAppl() {}
void CStdAppl::Cleanup( void ) {}
int CStdAppl::InitAlarm( ALARM_VALUES alarm ) { return 0; }
void CStdAppl::Terminate( char *file, int lineNumber, int logCode ) {}

DataLog_Critical::DataLog_Critical() {}
DataLog_Critical::~DataLog_Critical() {}
unsigned int const  MessageSystemConstant::DEFAULT_Q_SIZE = 30;
//
//
//   Kludge implementation (to be kind) of a global symbol table.  Works since
//   I know everything is in one task.
//
class symbollookupkludge
{
public:
   char key[200];
   char *dataptr;
};

typedef std::list<symbollookupkludge *> symbolList_t;
static symbolList_t symbolList;

int symFindByName(char *nameKey, char **valptr)
{
   for (symbolList_t::iterator iter = symbolList.begin(); iter != symbolList.end(); ++iter) {
      if (strcmp(nameKey,(*iter)->key) == 0) {
         *valptr = (*iter)->dataptr;
         return 1;
      }
   }
   return 0;
}

int symAdd(char *nameKey, char *data, BIND_ITEM_TYPE item)
{
   symbollookupkludge *element = new symbollookupkludge;
   strcpy(element->key,nameKey);
   element->dataptr = data;
   symbolList.push_back(element);
   return 1;
}

DataLog_Result datalog_CreatePeriodicSet(const char * setName, DataLog_SetHandle * handle) { return DataLog_OK; }
DataLog_Result datalog_ForcePeriodicOutput(DataLog_SetHandle handle){ return DataLog_OK; }
DataLog_PeriodicItemBase::DataLog_PeriodicItemBase(DataLog_SetHandle set, size_t size, const char * key, const char * description, const char * format) {}
DataLog_PeriodicItemBase::~DataLog_PeriodicItemBase() {}
bool DataLog_PeriodicItemBase::itemChanged(void) { return false;}
void DataLog_PeriodicItemBase::setItemData(size_t size, const void * data) {}
TimerMessage::TimerMessage() {}
TimerMessage::~TimerMessage() {}
const char *TimerMessage::genMsgName() { return '\0'; }
