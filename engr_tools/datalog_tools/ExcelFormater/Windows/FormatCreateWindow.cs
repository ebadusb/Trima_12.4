/*****************************************************************************\
**
** FormatCreateWindow
**
** Implementation of the format create window.
**
\*****************************************************************************/

namespace ExcelFormater.Windows {

/* Usings ********************************************************************/

using System;
using System.Collections.Generic;
using System.Text;
using System.Windows.Forms;
using ExcelUtility.Asset;
using ExcelUtility.Format;

/* Classes *******************************************************************/

class FormatCreateWindow : FormatWindow {
	#region Constructors
	public FormatCreateWindow() 
	{
		mFirstButton.Enabled = false;
		mFirstButton.Visible = false;
		mSecondButton.Text = "Save";
		mNameList.Visible = false;
	}
	#endregion

	#region Handlers
	protected override void OnFormClosing(FormClosingEventArgs e)
	{
		if (this.DialogResult == DialogResult.OK) {
			FormatInfo info = this.FormatInfo;

			if (String.IsNullOrEmpty(info.Name)) {
				mNameTextfield.Focus();
				System.Windows.Forms.MessageBox.Show(
						"You must provide a name.",
						"Error",
						System.Windows.Forms.MessageBoxButtons.OK,
						System.Windows.Forms.MessageBoxIcon.Error);
				e.Cancel = true;
			}
			else if (String.IsNullOrEmpty(info.Destination)) {
				mDestinationTextfield.Focus();
				System.Windows.Forms.MessageBox.Show(
						"You must provide a destination.",
						"Error",
						System.Windows.Forms.MessageBoxButtons.OK,
						System.Windows.Forms.MessageBoxIcon.Error);
				e.Cancel = true;
			}
			else if (mFormaterAssets.FormatInfosManager.Exists(info)) {
				System.Windows.Forms.DialogResult results = System.Windows.Forms.MessageBox.Show(
						"A format with the same name already exists.\nYou must provide a new name.",
						"Error",
						System.Windows.Forms.MessageBoxButtons.OK,
						System.Windows.Forms.MessageBoxIcon.Error);
					e.Cancel = true;
			}
		}

		base.OnFormClosing(e);
	}
	#endregion
}
}
