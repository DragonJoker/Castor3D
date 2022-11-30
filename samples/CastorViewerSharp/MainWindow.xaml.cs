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

using System;
using System.IO;
using System.Windows.Interop;
using Castor3D;
using System.Windows.Threading;

namespace CastorViewerSharp
{
	/// <summary>
	/// Logique d'interaction pour MainWindow.xaml
	/// </summary>
	public partial class MainWindow
		: System.Windows.Window
	{
		/// <summary>
		/// Default constructor
		/// </summary>
		public MainWindow()
		{
			InitializeComponent();
		}

		#region Private methods

		/// <summary>
		/// Loads the plug-ins alongside the executable
		/// </summary>
		private void LoadPlugins()
		{

			string path = m_engine.PluginsDirectory;
			string[] files = Directory.GetFiles( path, "*.dll" );

			foreach ( string file in files )
			{
				m_engine.LoadPlugin( file );
			}

			m_engine.LoadRenderer( "vk" );
			m_engine.Initialise( 1000, 1 );
			m_engine.StartRendering();


        }

		/// <summary>
		/// Opens a file selector dialog, to open a scene file.
		/// </summary>
		/// <returns>The scene file name</returns>
		string DoSelectSceneFile()
		{
			Microsoft.Win32.OpenFileDialog dlg = new Microsoft.Win32.OpenFileDialog();
			dlg.FileName = "";
			dlg.DefaultExt = "*.cscn;*.cbsn;*.zip";
			dlg.Filter = "All supported files|*.cscn;*.cbsn;*.zip|Castor3D Text Scene files|*.cscn|Castor3D Binary Scene files|*.cscn|Castor3D zip files|*.zip";
			dlg.Title = "Select a scene file to load";
			bool? result = dlg.ShowDialog();
			string l_return;

			if ( result == true )
			{
				l_return = dlg.FileName;
			}
			else
			{
				l_return = "";
			}

			return l_return;
		}

		/// <summary>
		/// Loads the given scene file name
		/// </summary>
		/// <param name="filename">The scene file name</param>
		private void LoadScene( string filename )
		{
			if ( filename.Length > 0 )
			{
				if ( m_scene != null )
                {
                    m_engine.PauseRendering();
                    m_scene.ClearScene();
					m_engine.RemoveScene( m_scene.Name );
					m_renderTarget = null;
					m_scene = null;
				}

                m_engine.StartRendering();
                m_renderTarget = m_engine.LoadScene( filename );

				if (m_renderTarget != null )
				{
					m_renderWindow.Initialise( m_renderTarget );
					m_scene = m_renderTarget.Scene;
				}
			}
		}

		/// <summary>
		/// Transforms given window coordinates to camera coordinates
		/// </summary>
		/// <param name="p_point">The window coordinates</param>
		/// <returns>The camera coordinates</returns>
		Position DoTransform( System.Windows.Point p_point )
		{
			Position l_return = new Position();
			var window = GetWindow( RenderPanel );
			double l_ww = window.Width;
			double l_wh = window.Height;
			int l_cw = ( int )m_renderTarget.camera.Width;
			int l_ch = ( int )m_renderTarget.camera.Height;
			l_return.Set( ( int )( ( p_point.X * l_cw ) / l_ww ), ( int )( ( p_point.Y * l_ch ) / l_wh ) );
			return l_return;
		}

		#endregion


		#region Overrides

		/// <summary>
		/// Initializes the engine, loads the plug-ins
		/// </summary>
		/// <param name="e"></param>
		protected override void OnInitialized( EventArgs e )
		{
			base.OnInitialized( e );
			base.Show();
			m_engine = new engine();
			m_engine.Create( "CastorViewerSharp", 1 );
			LoadPlugins();

			var window = GetWindow( RenderPanel );
			window.Show();
			Size size = new Size();
			size.Set((uint)window.Width, (uint)window.Height);
			var handle = new WindowInteropHelper(window).Handle;
			m_renderWindow = m_engine.CreateRenderWindow("MainWindow", size, handle);

			LoadScene(DoSelectSceneFile());
		}

		/// <summary>
		/// Cleans up the engine
		/// </summary>
		/// <param name="e"></param>
		protected override void OnClosed( EventArgs e )
		{
			m_scene = null;
			m_renderWindow.Cleanup();
			m_renderWindow = null;
			m_engine.Cleanup();
			m_engine.Destroy();
			m_engine = null;
			base.OnClosed( e );
		}

		#endregion

		#region Events

		/// <summary>
		/// Forwards the mouse position to the render window
		/// </summary>
		/// <param name="sender"></param>
		/// <param name="e"></param>
		private void OnMouseMove( object sender, System.Windows.Input.MouseEventArgs e )
		{
			if ( m_renderTarget != null )
			{
				m_oldPosition = DoTransform( e.GetPosition( RenderPanel ) );
				m_renderWindow.OnMouseMove( m_oldPosition );
			}
		}

		/// <summary>
		/// Forwards the new size to the render window
		/// </summary>
		/// <param name="sender"></param>
		/// <param name="e"></param>
		private void OnSizeChanged( object sender, System.Windows.SizeChangedEventArgs e )
		{
			if ( m_renderTarget != null )
			{
				Size size = new Size();
                size.Width = ( uint )e.NewSize.Width;
                size.Height = ( uint )e.NewSize.Height;
				m_renderWindow.Resize(size);
			}
		}

		/// <summary>
		/// Opens a files selector to select a file name, and to load it.
		/// </summary>
		/// <param name="sender"></param>
		/// <param name="e"></param>
		private void OnLoaded( object sender, System.Windows.RoutedEventArgs e )
		{
		}

		#endregion

		#region Members

		/// <summary>
		/// The Castor3D engine
		/// </summary>
		private engine m_engine;
		/// <summary>
		/// The render window
		/// </summary>
		private RenderTarget m_renderTarget;
		/// <summary>
		/// The render window
		/// </summary>
		private RenderWindow m_renderWindow;
		/// <summary>
		/// The main scene
		/// </summary>
		private Scene m_scene;
		/// <summary>
		/// The previous mouse position
		/// </summary>
		private Position m_oldPosition;

		#endregion
	}
}
