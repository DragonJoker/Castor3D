/*
See LICENSE file in root folder
*/
using System.Linq;
using System.Windows;
using Castor3DInterop;

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

			m_logger.Create(eLOG_TYPE.eLOG_TYPE_DEBUG);
			m_logger.SetFileName(path, eLOG_TYPE.eLOG_TYPE_COUNT);
			m_logger.LogInfo("CastorViewerSharp - Start");

			base.OnStartup(e);
		}
		/// <summary>
		/// Cleans the logger up
		/// </summary>
		/// <param name="e"></param>
		protected override void OnExit(ExitEventArgs e)
		{
			m_logger.LogInfo("CastorViewerSharp - Exit");
			m_logger = null;

			base.OnExit(e);
		}

		private Logger m_logger = new Logger();
	}
}
