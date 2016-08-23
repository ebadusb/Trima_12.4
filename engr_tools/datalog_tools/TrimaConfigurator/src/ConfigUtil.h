/*****************************************************************************\
**
** ConfigUtil.h
**
** Provides utility functions for the configurations pages.
**
\*****************************************************************************/

#pragma once

/* Includes ******************************************************************/

#include "TrimaConfigCDS.h"
#include "TrimaConfigCDSUtil.h"
#include "ConfigPage.h"

/* Classes *******************************************************************/

namespace CONFIGURATOR {

using namespace System;
using namespace System::Windows::Forms;

public interface class ConfigUtil {
public:

	static void SetTextfieldInt(TextBox ^control, int value)
	{
		control->Text = Int32(value).ToString();
	}

	static int GetTextfieldInt(TextBox ^control)
	{
		int value = 0;

		try {
			value = Int32::Parse(control->Text);
		}
		catch (System::FormatException ^e) {
			// Just return 0;
		}	

		return value;
	}

	static void SetTextfieldFloat(TextBox ^control, float value)
	{
		control->Text = String::Format("{0:N}", value);
	}

	static float GetTextfieldFloat(TextBox ^control)
	{
		float value = 0;

		try {
			value = (float)Double::Parse(control->Text);
		}
		catch (System::FormatException ^e) {
			// Just return 0;
		}	

		return value;
	}

	static void SetCheckBoxState(CheckBox ^control, int value)
	{
		control->Checked = value != 0;
	}

	static int GetCheckBoxState(CheckBox ^control)
	{
		return control->Checked;
	}

	static void SetListSelection(ComboBox ^control, int value)
	{
		control->SelectedIndex = value;
	}

	static int GetListSelection(ComboBox ^control)
	{
		return control->SelectedIndex;
	}

	static void ValidateTextfieldInt(TextBox ^control, CONFIG_DATA_KEYS key, ValidationInfo ^info)
	{
		if (info->result) {
			try {
				int value = Int32::Parse(control->Text);
				int min = 0;
				int max = 0;

				if (!CONFIG::TrimaConfigCDSUtil::IsValueValid(key,value, &min, &max)) {
					info->result = false;
					info->message = "Value out of range. \n" 
							+ "The value must be between " 
							+ String::Format("{0:G}", min)
							+ " and "
							+ String::Format("{0:G}", max);
					info->control = control;
				}
			}
			catch (System::FormatException ^e) {
				info->result = false;
				info->message = "The value is not properly formated.";
				info->control = control;
			}	
		}
	}

	static void ValidateTextfieldFloat(TextBox ^control, CONFIG_DATA_KEYS key, ValidationInfo ^info)
	{
		if (info->result) {
			try {
				float value = (float)Double::Parse(control->Text);
				float min = 0;
				float max = 0;

				if (!CONFIG::TrimaConfigCDSUtil::IsValueValid(key,value, &min, &max)) {
					info->result = false;
					info->message = "Value out of range. \n" 
							+ "The value must be between " 
							+ String::Format("{0:N}", min)
							+ " and "
							+ String::Format("{0:N}", max);
					info->control = control;
				}
			}
			catch (System::FormatException ^e) {
				info->result = false;
				info->message = "The value is not properly formated.";
				info->control = control;
			}	
		}
	}

	static void KeyPressIntOnly(Object ^o, System::Windows::Forms::KeyPressEventArgs ^e)
	{
		if (!Char::IsNumber(e->KeyChar) 
				&& (e->KeyChar != 127)		// DEL
				&& (e->KeyChar != 010)		// BS
				&& (e->KeyChar != '-')) {	
			e->Handled = true;
		}

		if ((e->KeyChar == '-')) {
			TextBox ^control = dynamic_cast<TextBox^>(o);

			if (control->Text->IndexOf('-') != -1) {
				e->Handled = true;
				return;
			}

			if (control->SelectionStart != 0) {
				e->Handled = true;
				return;
			}
		}
	}

	static void KeyPressFloatOnly(Object ^o, System::Windows::Forms::KeyPressEventArgs ^e)
	{
		if (!Char::IsNumber(e->KeyChar) 
				&& (e->KeyChar != 127)		// DEL
				&& (e->KeyChar != 010)		// BS
				&& (e->KeyChar != '.')
				&& (e->KeyChar != '-')) {
			e->Handled = true;
			return;
		}

		if ((e->KeyChar == '-')) {
			TextBox ^control = dynamic_cast<TextBox^>(o);

			if (control->Text->IndexOf('-') != -1) {
				e->Handled = true;
				return;
			}

			if (control->SelectionStart != 0) {
				e->Handled = true;
				return;
			}
		}

		if ((e->KeyChar == '.')) {
			TextBox ^control = dynamic_cast<TextBox^>(o);

			if (control->Text->IndexOf('.') != -1) {
				e->Handled = true;
				return;
			}
		}
	}
};

} // namespace CONFIGURATOR
