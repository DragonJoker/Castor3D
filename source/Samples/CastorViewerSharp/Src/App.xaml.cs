//This source file is part of Castor3D (http://castor3d.developpez.com/castor3d.html)
//Copyright (c) 2016 dragonjoker59@hotmail.com

//Permission is hereby granted, free of charge, to any person obtaining a copy of
//this software and associated documentation files (the "Software"), to deal in
//the Software without restriction, including without limitation the rights to
//use, copy, modify, merge, publish, distribute, sublicense, and/or sell copies
//of the Software, and to permit persons to whom the Software is furnished to do
//so, subject to the following conditions:

//The above copyright notice and this permission notice shall be included in all
//copies or substantial portions of the Software.

//THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
//IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
//FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
//AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
//LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
//OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
//SOFTWARE.

using System.Linq;
using System.Windows;
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
