/*****************************************************************************\
**
** FormatMainWindow
**
** Implementation of the format main window.
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

class FormatMainWindow : FormatWindow {
	#region Constructors
	public FormatMainWindow() 
	{
		this.mFirstButton.Click += new System.EventHandler(this.FirstButtonHandler);
		this.mSecondButton.Click += new System.EventHandler(this.SecondButtonHandler);
	}
	#endregion

	#region Delegates
	public delegate void ProcessFormatHandler(object sender, ProcessAssetArgs<FormatInfo> args);

	public event ProcessFormatHandler Format;
	#endregion

	#region Functions
	private void FirstButtonHandler(object sender, EventArgs e)
	{
		FormatInfo info = this.FormatInfo;

		if (String.IsNullOrEmpty(info.Name)) {
			mNameList.Focus();
			System.Windows.Forms.MessageBox.Show(
					"You must provide a name.",
					"Error",
					System.Windows.Forms.MessageBoxButtons.OK,
					System.Windows.Forms.MessageBoxIcon.Error);
		}
		else if (String.IsNullOrEmpty(info.Destination)) {
			mDestinationTextfield.Focus();
			System.Windows.Forms.MessageBox.Show(
					"You must provide a destination.",
					"Error",
					System.Windows.Forms.MessageBoxButtons.OK,
					System.Windows.Forms.MessageBoxIcon.Error);
		}
		else if (mFormaterAssets.FormatInfosManager.Exists(info)) {
			System.Windows.Forms.DialogResult results = System.Windows.Forms.MessageBox.Show(
					"A format with the same name already exists.\nDo you wish to replace it?",
					"Replace save format?",
					System.Windows.Forms.MessageBoxButtons.YesNo,
					System.Windows.Forms.MessageBoxIcon.Question);

			if (results == DialogResult.Yes) {
				mFormaterAssets.FormatInfosManager.Edit(info, info);
				this.ReloadInfoList();
			}
		}
		else {
			mFormaterAssets.FormatInfosManager.New(info);
			this.ReloadInfoList();
		}
	}

	private void SecondButtonHandler(object sender, EventArgs e)
	{
		FormatInfo info = this.FormatInfo;

		if (String.IsNullOrEmpty(info.Destination)) {
			mDestinationTextfield.Focus();
			System.Windows.Forms.MessageBox.Show(
					"You must provide a destination.",
					"Error",
					System.Windows.Forms.MessageBoxButtons.OK,
					System.Windows.Forms.MessageBoxIcon.Error);
		}
		else if (this.Format != null) {
			this.Format(this, new ProcessAssetArgs<FormatInfo>(info));
		}
	}
	#endregion
}
}
