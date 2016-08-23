#if !defined(_optimizer_form_h)
#define _optimizer_form_h


#include "Offline_optimizer.h"
#include "cfg_main_input.h"
#include "initial_conditions.h"
#include "Sectionfile.hpp"

class optimizer_form : public CFormView
{
protected:
	optimizer_form(UINT nIDTemplate);   // protected constructor used by dynamic creation
	virtual ~optimizer_form();

   //
   //
   //  Overrides from form / button functionality
	virtual void OnInitialUpdate();
   virtual void OnUpdate(CView* pSender, LPARAM lHint, CObject* pHint);
   virtual void OnInitialConditions();
   virtual void OnEditConfig();
   virtual void OnStart();
   virtual void OnStop();

protected:
   virtual void set_title();
   virtual void idle_callback();
   virtual void set_processing_function(optimizer_form *derived_class,
                                        IdleFunction::idle_loop_function derived_function);
//
//
//  Accessor functions
public:
   Offline_optimizer &optimizer();
   cfg_main_input &config_dat();
   Sectionfile &cobe_config();
   Initial_conditions &initial_conditions();

   const CTime time_start() const;
   const CTimeSpan time_elapsed() const;
   const int time_elapsed_hours() const;
   const int time_elapsed_minutes() const;
   const int time_elapsed_seconds() const;

protected:
   Offline_optimizer m_optimizer;
   cfg_main_input m_config_dat;
   Sectionfile m_cobe_config;
   Initial_conditions m_initial_conditions;

   int m_simulation_active;
   CTime m_start_time;
   CTimeSpan m_time_elapsed;
   int m_time_elapsed_seconds;
   int m_time_elapsed_minutes;
   int m_time_elapsed_hours;

   //
   //
   //  Idle function callback pointers
   //
   IdleFunction *m_idle_function;
   optimizer_form *m_derived_class;
   IdleFunction::idle_loop_function m_derived_function;

};

#endif
