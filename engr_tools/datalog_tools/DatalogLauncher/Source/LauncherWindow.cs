using System;
using System.Collections.Generic;
using System.ComponentModel;
using System.Data;
using System.Drawing;
using System.Linq;
using System.Text;
using System.Windows.Forms;

namespace DatalogLauncher {
public class LauncherWindow : Form {
	#region Constructor
	public LauncherWindow()
	{
		mEngine = new LauncherEngine();
		mEngine.LoadSettings();
		mDatalog = "";
	}

	public LauncherWindow(LauncherEngine engine)
	{
		mEngine = engine;
		mDatalog = "";
	}
	#endregion

	#region Properties
	public String Datalog
	{
		get { return mDatalog; }
		set { mDatalog = value; }
	}
	#endregion

	#region Members
	protected LauncherEngine mEngine;
	protected String mDatalog;
	#endregion
}
}
