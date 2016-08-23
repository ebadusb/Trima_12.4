// optimizer_form.cpp : implementation file
//

#include "stdafx.h"
#include "ap2 predictionater.h"
#include "optimizer_form.h"
#include "SplitPath.h"
/////////////////////////////////////////////////////////////////////////////
// optimizer_form

optimizer_form::optimizer_form(UINT nIDTemplate): CFormView(nIDTemplate),
   m_optimizer(),
   m_cobe_config(),
   m_config_dat(),
   m_initial_conditions(),
   m_simulation_active(0),
   m_start_time(GetCurrentTime()),
   m_time_elapsed(0),
   m_time_elapsed_seconds(0),
   m_time_elapsed_minutes(0),
   m_time_elapsed_hours(0),

   m_idle_function(0),
   m_derived_class(0),
   m_derived_function(0)
{
}

optimizer_form::~optimizer_form()
{
   delete m_idle_function;
   m_idle_function = 0;
}




/////////////////////////////////////////////////////////////////////////////
// optimizer_form message handlers

void optimizer_form::OnInitialConditions() 
{
   m_initial_conditions.DoModal();
   //Load up the config file automatically 
   //if it exists
   	//load up config values
   //If no config file is chosen, don't load up the 
   //default config file
	if(m_initial_conditions.m_rundata_file.IsEmpty())
		return;
	//CString configPath = m_initial_conditions.m_rundata_file + "CONFIG.dat";
	CSplitPath splitConfig(m_initial_conditions.m_rundata_file);
	CString configPath = "C:\\temp\\" + splitConfig.GetFileName() + splitConfig.GetExtension() + "CONFIG.dat";
	CString tempConfigPath = "config_temp.dat";
	std::ifstream rundataConfig(configPath,std::ios::in | std::ios::binary);
	
	if(!rundataConfig.fail()){
		//Copy this file to another location
		std::ofstream tempConfig(tempConfigPath, std::ios::out);
		while(rundataConfig.good()) {
			tempConfig << (char) rundataConfig.get();
		}
		rundataConfig.close();
		tempConfig.close();
		m_config_dat.update_config((LPCSTR) tempConfigPath);
	}
}

void optimizer_form::OnEditConfig() 
{
   m_config_dat.DoModal();
   set_title();
}

void optimizer_form::OnInitialUpdate() 
{
	CFormView::OnInitialUpdate();
	char szCurDir[256];
	::GetCurrentDirectory(256, szCurDir);
   const bool success = m_config_dat.update_config("config.dat");

   // Check status for cobe config initialization.
   if (!success)
   {
      MessageBox("config.dat failed to read", "ERROR", MB_OK);
      // Close window
   }

   set_title();

   m_initial_conditions.initialize();
   //Update Units?
   //m_initial_conditions.UpdateTBV();
}

void optimizer_form::OnStart() 
{
   //
   //
   //   Refresh configuration data
   //
   m_initial_conditions.populate_datastore();

   //
   //
   //  Capture start time
   //
   m_start_time = GetCurrentTime();

   //
   //
   //   Set up processing with main application.
   //
   if (m_derived_class && m_derived_function) {
      m_idle_function = new IdleFunction(this,(IdleFunction::idle_loop_function)&optimizer_form::idle_callback);
   }

}

void optimizer_form::OnStop() 
{
  m_simulation_active = 0;
  delete m_idle_function;
  m_idle_function = 0;
}

void optimizer_form::OnUpdate(CView* pSender, LPARAM lHint, CObject* pHint) 
{
   set_title();	
}

//
//
// ----------------------------------------------------------------------------
void optimizer_form::set_title()
{
   CFrameWnd *frame = GetParentFrame();
   // FWS_ADDTOTITLE style bit must be set
   if (frame) {
      CString title;
      title.LoadString(IDS_AP2_PREDICTIONATER);
      CString work = m_config_dat.path();
      int pos = work.ReverseFind(_T('\\'));
      if (pos >= 0) {
         title += work.Right(work.GetLength() - pos - 1);
      } else {
         title += work;
      }
      if (m_config_dat.modified()) title += "*";
      frame->SetWindowText(title);
   }
}

//
//
// ----------------------------------------------------------------------------
//
void optimizer_form::set_processing_function(optimizer_form *derived_class,
                                             IdleFunction::idle_loop_function derived_function)
{
   m_derived_class = derived_class;
   m_derived_function = derived_function;
}

void optimizer_form::idle_callback()
{
   //
   //
   //   Calculate remaining time.  There's probably a way to do this with
   //   CTimeSpan even given the *1000 probably, or use CTimeSpan and fix
   //   the resolution.  Note for some reason (probably my stinkin setup
   //   or something) time values are off by a factor of 1000.
   //
   CTime current_time = GetCurrentTime();
   m_time_elapsed = current_time - m_start_time;
   int seconds = m_time_elapsed.GetTotalSeconds()/1000;
   m_time_elapsed_hours = seconds / 3600;
   seconds -= (m_time_elapsed_hours*3600);
   m_time_elapsed_minutes = seconds/60;
   seconds -= (m_time_elapsed_minutes*60);
   m_time_elapsed_seconds = seconds;

   //
   //
   //   Invoke derived idle function
   //
   (m_derived_class->*m_derived_function)();

}
//
//
// ----------------------------------------------------------------------------
//                   Accessor functions
//
Offline_optimizer &optimizer_form::optimizer() { return m_optimizer; }
cfg_main_input &optimizer_form::config_dat() { return m_config_dat; }
Sectionfile &optimizer_form::cobe_config() { return m_cobe_config; }
Initial_conditions &optimizer_form::initial_conditions() { return m_initial_conditions; }
const CTime optimizer_form::time_start() const;
const CTimeSpan optimizer_form::time_elapsed() const { return m_time_elapsed; }
const int optimizer_form::time_elapsed_hours() const { return m_time_elapsed_hours; }
const int optimizer_form::time_elapsed_minutes() const { return m_time_elapsed_minutes; }
const int optimizer_form::time_elapsed_seconds() const { return m_time_elapsed_seconds; }
