/*****************************************************************************\
**
** ConfigPage.h
**
** Provides the interface for configurations pages.
**
\*****************************************************************************/

#pragma once

/* Includes ******************************************************************/

#include "TrimaConfigCDS.h"

/* Classes *******************************************************************/

namespace CONFIGURATOR {

using namespace System;
using namespace System::Windows::Forms;

public ref struct ValidationInfo {
	ValidationInfo() : result(true), message(nullptr), control(nullptr) {}

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
	 * @param config The configuration structure.
	 *
	 */
	void UpdateConfigurations(ConfigStruct &config);

	/**
	 *
	 * RefreshConfigurations
	 *
	 * Refreshes the configurations.
	 *
	 * @param config The configuration structure.
	 *
	 */
	void RefreshConfigurations(ConfigStruct & config);
};

} // namespace CONFIGURATOR
