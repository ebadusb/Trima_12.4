/*****************************************************************************\
**
** SpoofBuilder.h
**
** Spoof builder implementation.
**
\*****************************************************************************/

/* Includes ******************************************************************/

#include "MainWindow.h"

using namespace WIZARD;

/* Functions *****************************************************************/

[STAThreadAttribute]
int main(array<System::String ^> ^args)
{
	Application::EnableVisualStyles();
	Application::SetCompatibleTextRenderingDefault(false); 

	Application::Run(gcnew MainWindow());

	return 0;
}
