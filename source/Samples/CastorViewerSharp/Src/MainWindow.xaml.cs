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

			m_engine.LoadRenderer( eRENDERER_TYPE.eRENDERER_TYPE_OPENGL );
		}

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
		/// Loads a scene from a file selector
		/// </summary>
		/// <param name="sender"></param>
		/// <param name="e"></param>
		private void OnLoadScene( object sender, System.Windows.RoutedEventArgs e )
		{
			LoadScene( DoSelectSceneFile() );
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
					m_scene.ClearScene();
					m_engine.RemoveScene( m_scene.Name );
					m_scene = null;
				}

				m_renderWindow = m_engine.LoadScene( filename );

				if ( m_renderWindow != null )
				{
					var window = GetWindow( RenderPanel );

					if ( m_renderWindow.Initialise( new WindowInteropHelper( window ).Handle ) )
					{
						m_scene = m_renderWindow.RenderTarget.Scene;
						Size l_size = new Size();
						l_size.Set( ( uint )window.Width, ( uint )window.Height );
						m_renderWindow.Resize( l_size );
					}

					m_timer.Start();
				}
			}
		}

		/// <summary>
		/// Exports the current scene
		/// </summary>
		/// <param name="sender"></param>
		/// <param name="e"></param>
		private void OnExportScene( object sender, System.Windows.RoutedEventArgs e )
		{
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
			int l_cw = ( int )m_renderWindow.RenderTarget.camera.Width;
			int l_ch = ( int )m_renderWindow.RenderTarget.camera.Height;
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
			m_logger = new Logger();
			m_engine = new engine();
			m_engine.Create();
			LoadPlugins();
			m_timer = new DispatcherTimer();
			m_timer.Tick += new EventHandler( OnTimer );
			m_timer.Interval = new TimeSpan( 0, 0, 0, 0, 40 );
		}

		/// <summary>
		/// Cleans up the engine
		/// </summary>
		/// <param name="e"></param>
		protected override void OnClosed( EventArgs e )
		{
			m_timer.Stop();
			m_scene = null;
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
			if ( m_renderWindow != null )
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
			if ( m_renderWindow != null )
			{
				Size l_size = new Size();
				l_size.Width = ( uint )e.NewSize.Width;
				l_size.Height = ( uint )e.NewSize.Height;
				m_renderWindow.Resize( l_size );
			}
		}

		/// <summary>
		/// Opens a files selector to select a file name, and to load it.
		/// </summary>
		/// <param name="sender"></param>
		/// <param name="e"></param>
		private void OnLoaded( object sender, System.Windows.RoutedEventArgs e )
		{
			LoadScene( DoSelectSceneFile() );
		}

		/// <summary>
		/// The timer function Renders one frame
		/// </summary>
		/// <param name="sender"></param>
		/// <param name="e"></param>
		private void OnTimer( object sender, EventArgs e )
		{
			if ( m_engine != null )
			{
				m_engine.RenderOneFrame();
			}
		}

		#endregion

		#region Members

		/// <summary>
		/// The Castor3D engine
		/// </summary>
		private engine m_engine;
		/// <summary>
		/// The Castor logger
		/// </summary>
		private Logger m_logger;
		/// <summary>
		/// The render window
		/// </summary>
		private RenderWindow m_renderWindow;
		/// <summary>
		/// The main scene
		/// </summary>
		private Scene m_scene;
		/// <summary>
		/// The render timer
		/// </summary>
		private DispatcherTimer m_timer;
		/// <summary>
		/// The previous mouse position
		/// </summary>
		private Position m_oldPosition;

		#endregion
	}
}
