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
		private void DoLoadPlugins()
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
		private void DoLoadScene( string filename )
		{
			if ( filename.Length > 0 )
			{
				if ( m_scene != null )
				{
					m_engine.PauseRendering();
					DoUnloadScene();
					m_engine.ResumeRendering();
				}
				else
				{
					m_engine.StartRendering();
				}

				m_renderTarget = m_engine.LoadScene( filename );

				if (m_renderTarget != null )
				{
					m_renderWindow.Initialise( m_renderTarget );
					m_scene = m_renderTarget.Scene;
					m_sceneNode = m_renderTarget.camera.Node;
				}
			}
		}

		/// <summary>
		/// Transforms given window coordinates to camera coordinates
		/// </summary>
		/// <param name="p_point">The window coordinates</param>
		/// <returns>The camera coordinates</returns>
		private Vector2D DoTransform( System.Windows.Point point )
		{
			Vector2D result = new Vector2D();
			var window = GetWindow( RenderPanel );
			double ww = window.Width;
			double wh = window.Height;
			int cw = ( int )m_renderTarget.camera.Width;
			int ch = ( int )m_renderTarget.camera.Height;
			result.Set((float)((point.X * cw) / ww)
				, (float)((point.Y * ch) / wh));
			return result;
		}

		/// <summary>
		/// Transforms given window coordinates to camera coordinates
		/// </summary>
		/// <param name="p_point">The window coordinates</param>
		/// <returns>The camera coordinates</returns>
		private void DoUnloadScene()
		{
			m_renderWindow.Cleanup();
			var name = m_scene.Name;
			m_renderTarget = null;
			m_scene = null;
			m_engine.RemoveScene(name);
		}

		void DoRotateNode( Vector2D newPosition )
		{
			var xAxis = new Vector3D { };
			xAxis.Set(1.0f, 0.0f, 0.0f);
			m_xAngle.Degrees += newPosition.Y - m_oldPosition.Y;
			var pitch = new Quaternion { };
			pitch.FromAxisAngle( xAxis, m_xAngle );

			var yAxis = new Vector3D { };
			yAxis.Set(0.0f, 1.0f, 0.0f);
			m_yAngle.Degrees += m_oldPosition.X - newPosition.X;
			var yaw = new Quaternion { };
			yaw.FromAxisAngle( yAxis, m_yAngle );

			m_sceneNode.Orientation = yaw.Mul( pitch );
		}

		void DoTranslateNode( Vector2D newPosition )
		{
			var rot = m_sceneNode.Orientation;
			var xAxis = new Vector3D { };
			xAxis.Set(1.0f, 0.0f, 0.0f);
			xAxis = rot.Transform( xAxis );
			xAxis.Normalise();

			var yAxis = new Vector3D { };
			yAxis.Set(0.0f, 1.0f, 0.0f);
			yAxis = rot.Transform( yAxis );
			yAxis.Normalise();

			var xMod = ( m_oldPosition.X - newPosition.X ) / 10.0f;
			xAxis = xAxis.Mul(xMod);

			var yMod = ( newPosition.Y - m_oldPosition.Y ) / 10.0f;
			yAxis = yAxis.Mul(xMod);

			m_sceneNode.Translate( xAxis.CompAdd( yAxis ) );
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
			DoLoadPlugins();

			var window = GetWindow( RenderPanel );
			window.Show();
			Size size = new Size();
			size.Set((uint)window.Width, (uint)window.Height);
			var handle = new WindowInteropHelper(window).Handle;
			m_renderWindow = m_engine.CreateRenderWindow("MainWindow", size, handle);

			DoLoadScene(DoSelectSceneFile());
		}

		/// <summary>
		/// Cleans up the engine
		/// </summary>
		/// <param name="e"></param>
		protected override void OnClosed( EventArgs e )
		{
			m_engine.EndRendering();
			DoUnloadScene();
			m_engine.RemoveWindow(m_renderWindow);
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
				var newPosition = DoTransform( e.GetPosition( RenderPanel ) );
				var pos = new Position { };
				pos.Set( (int)newPosition.X, (int)newPosition.Y );

				if ( m_renderWindow.OnMouseMove( pos ) == 0
					&& m_sceneNode != null
					&& m_oldPosition != null )
				{
					if (e.LeftButton == System.Windows.Input.MouseButtonState.Pressed)
					{
						DoRotateNode( newPosition );
					}
					else if (e.RightButton == System.Windows.Input.MouseButtonState.Pressed)
					{
						DoTranslateNode( newPosition );
					}
				}

				m_oldPosition = newPosition;
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
		/// The scene node controlled by the mouse
		/// </summary>
		private SceneNode m_sceneNode;
		/// <summary>
		/// The previous mouse position
		/// </summary>
		private Vector2D m_oldPosition;
		private Angle m_xAngle = new Angle();
		private Angle m_yAngle = new Angle();

		#endregion
	}
}
