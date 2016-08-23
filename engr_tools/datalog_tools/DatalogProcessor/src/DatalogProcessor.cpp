/*****************************************************************************\
**
** DatalogProcessor.cpp
**
** Provides implementation of the datalog processor.
**
\*****************************************************************************/

/* Includes ******************************************************************/

#include "ProcessorEngine.h"
#include "ProcessorSettings.h"
#include "ProcessorWindow.h"

using namespace DatalogProcessor;

/* Functions *****************************************************************/

[STAThreadAttribute]
int main(array<System::String ^> ^args)
{
	if (!CONVERT::DatabaseSecurity::IsProductActivated()) {
		MessageBox::Show("The product is not activated.", 
			"Error", 
			MessageBoxButtons::OK, 
			MessageBoxIcon::Error);
	}
	else if (args->Length > 0) {
		ProcessorEngine engine;

		engine.LoadSettings();

		if (args->Length == 2 && args[0] == "-access") {
			engine.ProcessAccessFile(args[1]);
		}
		else if (args->Length == 2 && args[0] == "-excel") {
			engine.ProcessExcelFile(args[1]);
		}
		else if (args->Length == 2 && args[0] == "-hybrid") {
			engine.ProcessHybridFile(args[1]);
		}
		else if (args->Length == 2 && args[0] == "-default") {
			engine.ProcessDefaultFile(args[1]);
		}
		else if (args->Length == 3 && args[0] == "-target") {
			engine.ProcessTargetFile(args[2], args[1]);
		}
		else if (args->Length == 1) {
			engine.ProcessDefaultFile(args[0]);
		}
	}
	else {
		Application::EnableVisualStyles();
		Application::SetCompatibleTextRenderingDefault(false); 
		Application::Run(gcnew ProcessorWindow());
	}

	return 0;
}
