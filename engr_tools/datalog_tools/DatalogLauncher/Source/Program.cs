/*****************************************************************************\
**
** EventLog
**
** Implementation of an event log utility.
**
\*****************************************************************************/

namespace DatalogLauncher {

/* Usings ********************************************************************/

using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.IO;
using System.Diagnostics;
using System.Windows.Forms;
using System.Threading;
using System.Text.RegularExpressions;

/* Classes *******************************************************************/

class Program {
	[STAThread]
	static void Main(string[] args)
	{
		LauncherEngine engine = new LauncherEngine();
		List<String> arguments = new List<string>(args);

		if (arguments.Count >= 2 && arguments[0] == "-application") {
			engine.Application = args[1];
			arguments.RemoveRange(0, 2);
		}

		if (arguments.Count == 2 && arguments[0] == "-argument") {
			engine.ProcessFromArgument(arguments[1]);
		}
		else if (arguments.Count == 1 && arguments[0] == "-clipboard") {
			engine.ProcessFromClipboard();
		}
		else if (arguments.Count == 1 && arguments[0] == "-dialog") {
			Application.EnableVisualStyles();
			Application.Run(LauncherUtility.GetWindowForSettings(engine));
		}
		else {
			LauncherWindow window = LauncherUtility.GetWindowForSettings(engine);

			if (arguments.Count == 1) {
				window.Datalog = AddinUtility.DatalogPathname.ExtractDatalogName(arguments[0]);
			}
			else {
				window.Datalog = AddinUtility.DatalogPathname.ExtractDatalogName(LauncherUtility.GetTextFromClipboard());
			}

			Application.EnableVisualStyles();
			Application.Run(window);
		}
	}
}
}
