#include "manager_base.hpp"
#include "guiglobs.hpp"
#include "statline.hpp"

namespace manager_detail
{

void update_status_line (bool waiting)
{
   if (!guiglobs::apheresis_status_line )
   {
      DataLog (log_level_gui_error) << "Unallocated apheresis status line."
                                    << endmsg;
      return;
   }

   if ( waiting )
      guiglobs::apheresis_status_line->
      set_current_status_line_text_by_type (ATTENTION_STATUS_LINE,
                                            textStatOptInProgress);
   else
      guiglobs::apheresis_status_line->
      deactivate_status_line_text_type     (textStatOptInProgress,

                                            ATTENTION_STATUS_LINE);
}


void FatalAlarm (stringstream& buf, const char* msg)
{

   DataLog(log_level_gui_info) << buf.str() << ':' << msg << endmsg;
   anAlarmMsg alarm(GUI_TRANSACTION_ERROR);
   alarm.setAlarm(msg);
}


} // namespace

/* FORMAT HASH 0cc762a92f22133547c0e47eb7eaac6f */
