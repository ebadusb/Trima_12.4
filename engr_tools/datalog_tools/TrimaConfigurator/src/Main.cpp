/*****************************************************************************\
**
** Main.cpp
**
** Application entry point.
**
\*****************************************************************************/

/* Includes ******************************************************************/

#include "MainWindow.h"

/* Functions *****************************************************************/

[System::STAThreadAttribute]
int main(array<System::String ^> ^args)
{
	System::Windows::Forms::Application::EnableVisualStyles();
	System::Windows::Forms::Application::SetCompatibleTextRenderingDefault(false); 
	System::Windows::Forms::Application::Run(gcnew CONFIGURATOR::MainWindow());

	return 0;
}
