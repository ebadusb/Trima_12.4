/*****************************************************************************\
**
** FillWindow
**
** Implementation of the quick fill settings window.
**
\*****************************************************************************/

namespace ExcelFill.Windows {

/* Usings ********************************************************************/
	
using System;
using System.Collections.Generic;
using System.ComponentModel;
using System.Data;
using System.Drawing;
using System.Text;
using System.Windows.Forms;
using ExcelUtility.Asset;
using ExcelUtility.Fill;

/* Classes *******************************************************************/

public partial class FillWindow : Form {
	#region Constructor
	public FillWindow()
	{
		this.InitializeComponent();

		mFillInfo = new FillInfo();
	}
	#endregion

	#region Properties
	public FillInfo FillInfo
	{
		get { return mFillInfo.Clone(); }
		set { mFillInfo.Copy(value); }
	}
	#endregion

	#region Handlers
	protected override void OnShown(EventArgs e)
	{
		base.OnShown(e);

		mHighlightCheckbox.Checked = mFillInfo.Highlight;
		mHyperlinkCheckbox.Checked = mFillInfo.Hyperlink;
		mColorButton.BackColor = mFillInfo.Color;
	}

	protected override void OnClosed(EventArgs e)
	{
		base.OnClosed(e);

		if (this.DialogResult == DialogResult.OK) {
			mFillInfo.Highlight = mHighlightCheckbox.Checked;
			mFillInfo.Hyperlink = mHyperlinkCheckbox.Checked;
			mFillInfo.Color = mColorButton.BackColor;
		}
	}
	
	private void ColorButtonHandler(object sender, EventArgs e)
	{
		if (mColorDialog.ShowDialog() == DialogResult.OK) {
			mColorButton.BackColor = mColorDialog.Color;
		}
	}
	#endregion

	#region Members
	private FillInfo mFillInfo;
	#endregion
}
}
