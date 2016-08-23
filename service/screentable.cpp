// Definition of the screen/state transition table
// Current State --      Current Screen --        Next State --         Next Screen --        Reset all screens

#include "scrntran.hpp"
#include "defscrn.hpp"

SCREEN_STATE_TABLE screen_state_transition_table[] =
{
   {SELF_TEST, GUI_SCREEN_SERVICE,  SELF_TEST, GUI_SCREEN_SERVICE, TRUE  },
   {SERVICE_MODE,  GUI_SCREEN_SERVICE,         SERVICE_MODE,   GUI_SCREEN_TOUCHSCREEN_CAL, FALSE   },
   {SERVICE_MODE,  GUI_SCREEN_TOUCHSCREEN_CAL, SERVICE_MODE,   GUI_SCREEN_SERVICE,         FALSE   },
   {SELF_TEST, GUI_SCREEN_SERVICE,         SELF_TEST,  GUI_SCREEN_TOUCHSCREEN_CAL, FALSE   },
   {SELF_TEST, GUI_SCREEN_TOUCHSCREEN_CAL, SELF_TEST,  GUI_SCREEN_SERVICE,         FALSE   },
   {TABLE_END,           GUI_SCREEN_NULL_SCREEN,  TABLE_END, GUI_SCREEN_NULL_SCREEN, TRUE  } /// TABLE END ///
};

// Definition of the default screen table: Procedure State ==> Default Screen
DEFAULT_SCREEN_TABLE default_screen_tab[] =
{
   {DEFAULT_STATE,    GUI_SCREEN_SERVICE},
   {SELF_TEST,        GUI_SCREEN_SERVICE},
   {SERVICE_MODE,     GUI_SCREEN_SERVICE},


   // DO NOT add any states after the "table end" entry
   {TABLE_END                           }
};

/* FORMAT HASH 1567908d0c6c08bb31412c1c50401bef */
