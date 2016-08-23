/*****************************************************************************\
**
** LauncherWindow
**
** Implementation of the launcher window.
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

public partial class LauncherWindowSimple : LauncherWindow {
	#region Constructor
	public LauncherWindowSimple()
	{
		this.InitializeComponent();
	}

	public LauncherWindowSimple(LauncherEngine engine) : base(engine)
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
	}

	protected override void OnClosed(EventArgs e)
	{
		base.OnClosed(e);

		mDatalog = mLogTextfield.Text;
	}

	private void HandleLaunchEvent(object sender, EventArgs e)
	{
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
