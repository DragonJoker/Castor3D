using System;
using System.Collections.Generic;
using System.Configuration;
using System.Data;
using System.Linq;
using System.Threading.Tasks;
using System.Windows;
using System.Reflection;
using Castor3D;

namespace CastorViewerSharp
{
	/// <summary>
	/// Logique d'interaction pour App.xaml
	/// </summary>
	public partial class App : Application
	{
		/// <summary>
		/// Parses the command line and initializes the application
		/// </summary>
		/// <param name="e">Holds the command line arguments</param>
		protected override void OnStartup(StartupEventArgs e)
		{
			string fileArg = "";
			string levlArg = "";
			string rendArg = "";

			if (e.Args.Any(arg => arg.Contains("/f") || arg.Contains("/file")))
			{
				fileArg = e.Args.First(arg => arg.Contains("/f") || arg.Contains("/file"));
			}

			if (e.Args.Any(arg => arg.Contains("/l") || arg.Contains("/log")))
			{
				levlArg = e.Args.First(arg => arg.Contains("/l") || arg.Contains("/log"));
			}

			if (e.Args.Any(arg => arg.Contains("/opengl") || arg.Contains("/directx9") || arg.Contains("/directx10") || arg.Contains("/directx11")))
			{
				rendArg = e.Args.First(arg => arg.Contains("/opengl") || arg.Contains("/directx9") || arg.Contains("/directx10") || arg.Contains("/directx11"));
			}

			if (fileArg.Length > 0)
			{
			}

			string path = System.Reflection.Assembly.GetEntryAssembly().Location;
			path = path.Replace(".exe",".log");

			Logger logger = new Logger();
			logger.Initialise(eLOG_TYPE.eLOG_TYPE_DEBUG);
			logger.SetFileName(path, eLOG_TYPE.eLOG_TYPE_COUNT);
			logger.LogInfo("CastorViewerSharp - Start");

			base.OnStartup(e);
		}
		/// <summary>
		/// Cleans the logger up
		/// </summary>
		/// <param name="e"></param>
		protected override void OnExit(ExitEventArgs e)
		{
			Logger logger = new Logger();
			logger.LogInfo("CastorViewerSharp - Exit");
			logger.Cleanup();
			base.OnExit(e);
		}
	}
}
