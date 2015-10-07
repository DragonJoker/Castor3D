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
	public partial class MainWindow : System.Windows.Window
	{
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
		DispatcherTimer m_timer;
		/// <summary>
		/// Default constructor
		/// </summary>
		public MainWindow()
		{
			InitializeComponent();
		}
		/// <summary>
		/// Initializes the engine, loads the plug-ins
		/// </summary>
		/// <param name="e"></param>
		protected override void OnInitialized(EventArgs e)
		{
			base.OnInitialized(e);
			m_logger = new Logger();
			m_engine = new engine();
			m_engine.Create();
			LoadPlugins();
			m_timer = new DispatcherTimer();
			m_timer.Tick += new EventHandler( OnTimer );
			m_timer.Interval = new TimeSpan(0, 0, 0, 0, 40);
		}
		/// <summary>
		/// Cleans up the engine
		/// </summary>
		/// <param name="e"></param>
		protected override void OnClosed(EventArgs e)
		{
			m_timer.Stop();
			m_scene = null;
			if (m_renderWindow != null)
			{
				m_renderWindow.Cleanup();
			}
			m_engine.Cleanup();
			m_engine.Destroy();
			base.OnClosed(e);
		}
		/// <summary>
		/// Loads the plug-ins alongside the executable
		/// </summary>
		private void LoadPlugins()
		{

			string path = m_engine.PluginsDirectory;
			string[] files = Directory.GetFiles(path, "*.dll");

			foreach ( string file in files )
			{
				m_engine.LoadPlugin(file);
			}

			m_engine.LoadRenderer(eRENDERER_TYPE.eRENDERER_TYPE_OPENGL);
		}
		/// <summary>
		/// Loads a scene from a file selector
		/// </summary>
		/// <param name="sender"></param>
		/// <param name="e"></param>
		private void OnLoadScene(object sender, System.Windows.RoutedEventArgs e)
		{
			LoadScene("J:\\Projets\\C++\\Castor3D\\data\\Scene\\Gui.zip");

			//Microsoft.Win32.OpenFileDialog dlg = new Microsoft.Win32.OpenFileDialog();
			//dlg.FileName = "Scene"; // Default file name
			//dlg.DefaultExt = ".cscn"; // Default file extension
			//dlg.Filter = "Castor3D Scene files (.cscn)|*.cscn"; // Filter files by extension
			//dlg.Title = "Select a scene file to load";

			//// Show open file dialog box
			//Nullable< bool > result = dlg.ShowDialog();

			//// Process open file dialog box results 
			//if (result == true)
			//{
			//    // Open document 
			//    string filename = dlg.FileName;
			//    LoadScene( filename );
			//}
		}
		/// <summary>
		/// Loads the given scene file name
		/// </summary>
		/// <param name="filename">The scene file name</param>
		private void LoadScene(string filename)
		{
			m_logger.LogDebug( "MainFrame::LoadScene - " + filename );

			if ( filename.Length > 0 )
			{
				m_logger.LogDebug( "MainFrame::LoadScene - file path not empty" );

				if ( m_scene != null )
				{
					m_scene.ClearScene();
					m_engine.RemoveScene( m_scene.Name );
					m_logger.LogDebug( "MainFrame::LoadScene - scene erased from manager" );
					m_scene = null;
					m_logger.LogDebug( "Scene cleared" );
				}

				m_engine.Cleanup();
				m_logger.LogDebug( "MainFrame::LoadScene - Engine cleared" );

				m_renderWindow = m_engine.LoadScene( filename );

				if ( m_renderWindow != null )
				{
					System.Windows.Window window = System.Windows.Window.GetWindow(this.RenderPanel);
					var wih = new WindowInteropHelper(window);
					IntPtr hWnd = wih.Handle;

					if ( m_renderWindow.Initialise( hWnd ) )
					{
						Size sizeScreen = new Size();
						Size sizeWnd = m_renderWindow.RenderTarget.Size;
						sizeScreen.Width = (uint)System.Windows.SystemParameters.PrimaryScreenWidth;
						sizeScreen.Height = (uint)System.Windows.SystemParameters.PrimaryScreenHeight;

						//m_listener = m_renderWindow.GetListener();
						m_scene = m_renderWindow.RenderTarget.Scene;

						if ( m_scene != null )
						{
							SceneNode camBaseNode = m_scene.CreateSceneNode( "CastorViewer_CamNode", m_scene.CameraRootNode );
							camBaseNode.Position.X = 0;
							camBaseNode.Position.Y = 0;
							camBaseNode.Position.Z = -100;
							SceneNode camYawNode = m_scene.CreateSceneNode( "CastorViewer_CamYawNode", camBaseNode );
							SceneNode camPitchNode = m_scene.CreateSceneNode( "CastorViewer_CamPitchNode", camYawNode );
							SceneNode camRollNode = m_scene.CreateSceneNode( "CastorViewer_CamRollNode", camPitchNode );
							camera cam = m_scene.CreateCamera( "CastorViewer_Camera", (int)sizeScreen.Width, (int)sizeScreen.Height, camRollNode, eVIEWPORT_TYPE.eVIEWPORT_TYPE_3D );
							m_renderWindow.RenderTarget.camera = cam;

							if ( m_renderWindow.RenderTarget.camera != null )
							{
								//m_pFpRotateCamEvent = std::make_shared< FPCameraRotateEvent		>( l_pCamRollNode,	real( 0 ), real( 0 ) );
								//m_pFpTranslateCamEvent = std::make_shared< FPCameraTranslateEvent	>( l_pCamRollNode,	real( 0 ), real( 0 ), real( 0 ) );
								//m_ptOriginalPosition = l_pCamRollNode->GetPosition();
								//m_qOriginalOrientation = l_pCamRollNode->GetOrientation();
							}

							//m_pKeyboardEvent = std::make_shared< KeyboardEvent >( p_pWindow, wxGetApp().GetMainFrame() );
						}
					}

					m_scene = m_renderWindow.RenderTarget.Scene;
					m_logger.LogInfo("Scene file read");
					m_timer.Start();
				}
			}
		}
		/// <summary>
		/// Exports the current scene
		/// </summary>
		/// <param name="sender"></param>
		/// <param name="e"></param>
		private void OnExportScene(object sender, System.Windows.RoutedEventArgs e)
		{
		}
		/// <summary>
		/// The timer function Renders one frame
		/// </summary>
		/// <param name="sender"></param>
		/// <param name="e"></param>
		private void OnTimer(object sender, EventArgs e)
		{
			if (m_engine != null)
			{
				m_engine.RenderOneFrame();
			}
		}
	}
}
