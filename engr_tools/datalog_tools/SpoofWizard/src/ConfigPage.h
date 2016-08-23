/*****************************************************************************\
**
** ConfigPage.h
**
** Provides the interface for spoof configurations.
**
\*****************************************************************************/

#pragma once

/* Includes ******************************************************************/

#include "SpoofGenerator.h"

/* Constants *****************************************************************/

/* Globals *******************************************************************/

/* Macros ********************************************************************/

/* Prototypes ****************************************************************/

/* Functions *****************************************************************/

/* Classes *******************************************************************/

namespace WIZARD {

using namespace System;
using namespace System::Windows::Forms;

public ref struct ValidationInfo {
	Boolean result;
    String ^message;
    Control ^control;
};

public interface class ConfigPage {
public:

	/**
	 *
	 * GetControl
	 *
	 * Gets the control associated with this configuration provider.
	 *
	 * @return The control.
	 *
	 */
	System::Windows::Forms::UserControl^ GetControl(void);

	/**
	 *
	 * ValidateConfigurations
	 *
	 * Validates the configurations.
	 *
	 * @return True if the contigurations are valid.
	 *
	 */
	ValidationInfo ^ValidateConfigurations(void);

	/**
	 *
	 * UpdateConfigurations
	 *
	 * Updates the configurations.
	 *
	 * @return True if the contigurations were updated.
	 *
	 */
	ValidationInfo ^UpdateConfigurations(void);

	/**
	 *
	 * RefreshConfigurations
	 *
	 * Refreshes the configurations.
	 *
	 * @return True if the contigurations were updated.
	 *
	 */
	ValidationInfo ^RefreshConfigurations(void);
};

} // namespace WIZARD



