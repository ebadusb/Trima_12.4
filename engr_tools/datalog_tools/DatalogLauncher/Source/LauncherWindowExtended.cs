/*****************************************************************************\
**
** LauncherWindowEx
**
** Implementation of the extended launcher window.
**
\*****************************************************************************/

namespace DatalogLauncher {

/* Usings ********************************************************************/

using System;
using System.Collections.Generic;
using System.ComponentModel;
using System.Data;
using System.Drawing;
using System.Linq;
using System.Text;
using System.Windows.Forms;

/* Classes *******************************************************************/

public partial class LauncherWindowExtended : LauncherWindow {
	#region Constructor
	public LauncherWindowExtended() 
	{
		this.InitializeComponent();
	}

	public LauncherWindowExtended(LauncherEngine engine) : base(engine)
	{
		this.InitializeComponent();
	}
	#endregion

	#region Handlers
	protected override void OnShown(EventArgs e)
	{
		base.OnShown(e);

		mLogTextfield.Text = mDatalog;
		mLogTextfield.SelectAll();

		mApplicationList.Items.Clear();
		foreach (KeyValuePair<String, String> item in mEngine.Settings.Applications) {
			mApplicationList.Items.Add(item.Key);
		}

		if (mApplicationList.Items.Count > 0) {
			mApplicationList.SelectedIndex = 0;
		}
	}

	protected override void OnClosed(EventArgs e)
	{
		base.OnClosed(e);

		mDatalog = mLogTextfield.Text;
	}

	private void HandleLaunchEvent(object sender, EventArgs e)
	{
		if (!String.IsNullOrEmpty(mApplicationList.Text)) {
			String application = "";

			if (mEngine.Settings.Applications.TryGetValue(mApplicationList.Text, out application)) {
				mEngine.Application = application;
			}
		}

		mEngine.ProcessFromArgument(mLogTextfield.Text);
		Application.Exit();
	}

	private void HandleCancelEvent(object sender, EventArgs e)
	{
		Application.Exit();
	}
	#endregion
}
}
