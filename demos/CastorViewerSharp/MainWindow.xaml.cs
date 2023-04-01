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
using System.Windows.Input;
using System.Windows;

namespace CastorViewerSharp
{
    /// <summary>
    /// Logique d'interaction pour MainWindow.xaml
    public partial class MainWindow
        : System.Windows.Window
        , Castor3D.IGuiCallbacks
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
            string[] files = Directory.GetFiles(path, "*.dll");

            foreach (string file in files)
            {
                m_engine.LoadPlugin(file);
            }

            m_engine.LoadRenderer("vk");
            m_engine.Initialise(1000, 1);
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

            if (result == true)
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
        private void DoLoadScene(string filename)
        {
            if (filename.Length > 0)
            {
                if (m_scene != null)
                {
                    m_engine.PauseRendering();
                    DoUnloadScene();
                    m_engine.ResumeRendering();
                }
                else
                {
                    m_engine.StartRendering();
                }

                m_renderTarget = m_engine.LoadScene(filename);

                if (m_renderTarget != null)
                {
                    m_renderWindow.Initialise(m_renderTarget);
                    m_scene = m_renderTarget.Scene;
                    m_nodeState = new NodeState(m_renderTarget.camera.Node);
                    Castor3D.Size size = new Castor3D.Size();
                    size.Width = (uint)Width;
                    size.Height = (uint)Height;
                    m_renderWindow.Resize(size);

                    m_motionTimer.Start();
                }
            }
        }

        /// <summary>
        /// Transforms given window coordinates to camera coordinates
        /// </summary>
        /// <param name="point">The window coordinates</param>
        /// <returns>The camera coordinates</returns>
        private Vector2D DoTransform(System.Windows.Point point)
        {
            Vector2D result = new Vector2D();
            var window = GetWindow(RenderPanel);
            double ww = window.Width;
            double wh = window.Height;
            int cw = (int)m_renderTarget.camera.Width;
            int ch = (int)m_renderTarget.camera.Height;
            result.Set((float)((point.X * cw) / ww)
                , (float)((point.Y * ch) / wh));
            return result;
        }

        /// <summary>
        /// Transforms given window coordinates to camera coordinates
        /// </summary>
        /// <param name="point">The window coordinates</param>
        /// <returns>The camera coordinates</returns>
        private void DoUnloadScene()
        {
            m_motionTimer.Stop();
            m_renderWindow.Cleanup();
            var name = m_scene.Name;
            m_renderTarget = null;
            m_scene = null;
            m_nodeState = null;
            m_engine.RemoveScene(name);
        }

        private eKEYBOARD_KEY DoConvertKey(Key key)
        {
            return (eKEYBOARD_KEY)key;

        }

        #endregion

        #region Overrides

        string IGuiCallbacks.OnGetClipBoardText()
        {
            return Clipboard.GetText();
        }

        void IGuiCallbacks.OnSetClipBoardText(string text)
        {
            Clipboard.SetText(text);
        }

        void IGuiCallbacks.OnCursorChange(eMOUSE_CURSOR cursor)
        {
            switch (cursor)
            {
                case eMOUSE_CURSOR.eMOUSE_CURSOR_ARROW:
                    Cursor = Cursors.Arrow;
                    break;
                case eMOUSE_CURSOR.eMOUSE_CURSOR_HAND:
                    Cursor = Cursors.Hand;
                    break;
                case eMOUSE_CURSOR.eMOUSE_CURSOR_TEXT:
                    Cursor = Cursors.IBeam;
                    break;
                case eMOUSE_CURSOR.eMOUSE_CURSOR_SIZE_WE:
                    Cursor = Cursors.SizeWE;
                    break;
                case eMOUSE_CURSOR.eMOUSE_CURSOR_SIZE_NS:
                    Cursor = Cursors.SizeNS;
                    break;
                case eMOUSE_CURSOR.eMOUSE_CURSOR_SIZE_NWSE:
                    Cursor = Cursors.SizeNWSE;
                    break;
                case eMOUSE_CURSOR.eMOUSE_CURSOR_SIZE_NESW:
                    Cursor = Cursors.SizeNESW;
                    break;
                default:
                    Cursor = Cursors.Arrow;
                    break;
            }
        }

        /// <summary>
        /// Initializes the engine, loads the plug-ins
        /// </summary>
        /// <param name="e"></param>
        protected override void OnInitialized(EventArgs e)
        {
            base.OnInitialized(e);
            base.Show();
            m_engine = new engine();
            m_engine.Create("CastorViewerSharp", 1);

            DoLoadPlugins();

            var window = GetWindow(RenderPanel);
            window.Show();
            Castor3D.Size size = new Castor3D.Size();
            size.Set((uint)window.Width, (uint)window.Height);
            var handle = new WindowInteropHelper(window).Handle;
            m_renderWindow = m_engine.CreateRenderWindow("MainWindow", size, handle);
            m_engine.RegisterGuiCallbacks(this);

            m_motionTimer = new DispatcherTimer();
            m_motionTimer.Tick += new EventHandler(OnMotionTimer);
            m_motionTimer.Interval = new TimeSpan(0, 0, 0, 0, 30);

            m_leftTimer = new DispatcherTimer();
            m_leftTimer.Tick += new EventHandler(OnLeftTimer);
            m_leftTimer.Interval = new TimeSpan(0, 0, 0, 0, 30);

            m_rightTimer = new DispatcherTimer();
            m_rightTimer.Tick += new EventHandler(OnRightTimer);
            m_rightTimer.Interval = new TimeSpan(0, 0, 0, 0, 30);

            m_forwardTimer = new DispatcherTimer();
            m_forwardTimer.Tick += new EventHandler(OnForwardTimer);
            m_forwardTimer.Interval = new TimeSpan(0, 0, 0, 0, 30);

            m_backTimer = new DispatcherTimer();
            m_backTimer.Tick += new EventHandler(OnBackTimer);
            m_backTimer.Interval = new TimeSpan(0, 0, 0, 0, 30);

            m_upTimer = new DispatcherTimer();
            m_upTimer.Tick += new EventHandler(OnUpTimer);
            m_upTimer.Interval = new TimeSpan(0, 0, 0, 0, 30);

            m_downTimer = new DispatcherTimer();
            m_downTimer.Tick += new EventHandler(OnDownTimer);
            m_downTimer.Interval = new TimeSpan(0, 0, 0, 0, 30);

            DoLoadScene(DoSelectSceneFile());
        }

        /// <summary>
        /// Cleans up the engine
        /// </summary>
        /// <param name="e"></param>
        protected override void OnClosed(EventArgs e)
        {
            m_engine.EndRendering();
            DoUnloadScene();

            m_motionTimer = null;
            m_engine.RemoveWindow(m_renderWindow);
            m_renderWindow = null;
            m_engine.Cleanup();
            m_engine.Destroy();
            m_engine = null;
            base.OnClosed(e);
        }

        #endregion

        #region Events

        /// <summary>
        /// </summary>
        /// <param name="sender"></param>
        /// <param name="e"></param>
        private void OnKeyUp(object sender, KeyEventArgs e)
        {
            if (m_renderTarget != null)
            {
                var key = DoConvertKey(e.Key);

                m_isShiftDown = e.KeyboardDevice.IsKeyDown(Key.LeftShift) || e.KeyboardDevice.IsKeyDown(Key.RightShift);
                m_isCtrlDown = e.KeyboardDevice.IsKeyDown(Key.LeftCtrl) || e.KeyboardDevice.IsKeyDown(Key.RightCtrl);
                m_isAltDown = e.KeyboardDevice.IsKeyDown(Key.LeftAlt) || e.KeyboardDevice.IsKeyDown(Key.RightAlt);
                if (m_renderWindow.OnKeyboardKeyUp(key
                            , (sbyte)(m_isCtrlDown ? 1 : 0)
                            , (sbyte)(m_isAltDown ? 1 : 0)
                            , (sbyte)(m_isShiftDown ? 1 : 0)) == 0
                    && m_nodeState != null)
                {
                    switch (e.Key)
                    {
                        case Key.Left:
                        case Key.Q:
                            m_leftTimer.Stop();
                            break;
                        case Key.Right:
                        case Key.D:
                            m_rightTimer.Stop();
                            break;
                        case Key.Up:
                        case Key.Z:
                            m_forwardTimer.Stop();
                            break;
                        case Key.Down:
                        case Key.S:
                            m_backTimer.Stop();
                            break;
                        case Key.PageUp:
                        case Key.E:
                            m_upTimer.Stop();
                            break;
                        case Key.PageDown:
                        case Key.A:
                            m_downTimer.Stop();
                            break;
                    }
                }
            }
        }

        /// <summary>
        /// </summary>
        /// <param name="sender"></param>
        /// <param name="e"></param>
        private void OnKeyDown(object sender, KeyEventArgs e)
        {
            if (m_renderTarget != null)
            {
                var key = DoConvertKey(e.Key);
                m_isShiftDown = e.KeyboardDevice.IsKeyDown(Key.LeftShift) || e.KeyboardDevice.IsKeyDown(Key.RightShift);
                m_isCtrlDown = e.KeyboardDevice.IsKeyDown(Key.LeftCtrl) || e.KeyboardDevice.IsKeyDown(Key.RightCtrl);
                m_isAltDown = e.KeyboardDevice.IsKeyDown(Key.LeftAlt) || e.KeyboardDevice.IsKeyDown(Key.RightAlt);

                if (m_renderWindow.OnKeyboardKeyDown(key
                            , (sbyte)(m_isCtrlDown ? 1 : 0)
                            , (sbyte)(m_isAltDown ? 1 : 0)
                            , (sbyte)(m_isShiftDown ? 1 : 0)) == 0
                    && m_nodeState != null)
                {
                    switch (e.Key)
                    {
                        case Key.Left:
                        case Key.Q:
                            m_leftTimer.Start();
                            break;
                        case Key.Right:
                        case Key.D:
                            m_rightTimer.Start();
                            break;
                        case Key.Up:
                        case Key.Z:
                            m_forwardTimer.Start();
                            break;
                        case Key.Down:
                        case Key.S:
                            m_backTimer.Start();
                            break;
                        case Key.PageUp:
                        case Key.E:
                            m_upTimer.Start();
                            break;
                        case Key.PageDown:
                        case Key.A:
                            m_downTimer.Start();
                            break;
                    }
                }
            }
        }

        private void OnMouseDown(object sender, MouseButtonEventArgs e)
        {
            if (m_renderTarget != null)
            {
                var newPosition = DoTransform(e.GetPosition(RenderPanel));
                var pos = new Position { };
                pos.Set((int)newPosition.X, (int)newPosition.Y);

                switch (e.ChangedButton)
                {
                    case MouseButton.Left:
                        m_renderWindow.OnMouseLButtonDown(pos
                            , (sbyte)(m_isCtrlDown ? 1 : 0)
                            , (sbyte)(m_isAltDown ? 1 : 0)
                            , (sbyte)(m_isShiftDown ? 1 : 0));
                        break;
                    case MouseButton.Right:
                        m_renderWindow.OnMouseRButtonDown(pos
                            , (sbyte)(m_isCtrlDown ? 1 : 0)
                            , (sbyte)(m_isAltDown ? 1 : 0)
                            , (sbyte)(m_isShiftDown ? 1 : 0));
                        break;
                    case MouseButton.Middle:
                        m_renderWindow.OnMouseMButtonDown(pos
                            , (sbyte)(m_isCtrlDown ? 1 : 0)
                            , (sbyte)(m_isAltDown ? 1 : 0)
                            , (sbyte)(m_isShiftDown ? 1 : 0));
                        break;
                }

                m_oldPosition = newPosition;
            }
        }

        private void OnMouseUp(object sender, MouseButtonEventArgs e)
        {
            if (m_renderTarget != null)
            {
                var newPosition = DoTransform(e.GetPosition(RenderPanel));
                var pos = new Position { };
                pos.Set((int)newPosition.X, (int)newPosition.Y);

                switch (e.ChangedButton)
                {
                    case MouseButton.Left:
                        m_renderWindow.OnMouseLButtonUp(pos
                            , (sbyte)(m_isCtrlDown ? 1 : 0)
                            , (sbyte)(m_isAltDown ? 1 : 0)
                            , (sbyte)(m_isShiftDown ? 1 : 0));
                        break;
                    case MouseButton.Right:
                        m_renderWindow.OnMouseRButtonUp(pos
                            , (sbyte)(m_isCtrlDown ? 1 : 0)
                            , (sbyte)(m_isAltDown ? 1 : 0)
                            , (sbyte)(m_isShiftDown ? 1 : 0));
                        break;
                    case MouseButton.Middle:
                        m_renderWindow.OnMouseMButtonUp(pos
                            , (sbyte)(m_isCtrlDown ? 1 : 0)
                            , (sbyte)(m_isAltDown ? 1 : 0)
                            , (sbyte)(m_isShiftDown ? 1 : 0));
                        break;
                }

                m_oldPosition = newPosition;
            }
        }

        /// <summary>
        /// Forwards the mouse position to the render window
        /// </summary>
        /// <param name="sender"></param>
        /// <param name="e"></param>
        private void OnMouseMove(object sender, System.Windows.Input.MouseEventArgs e)
        {
            if (m_renderTarget != null)
            {
                var newPosition = DoTransform(e.GetPosition(RenderPanel));
                var pos = new Position { };
                pos.Set((int)newPosition.X, (int)newPosition.Y);

                if (m_renderWindow.OnMouseMove(pos
                            , (sbyte)(m_isCtrlDown ? 1 : 0)
                            , (sbyte)(m_isAltDown ? 1 : 0)
                            , (sbyte)(m_isShiftDown ? 1 : 0)) == 0
                    && m_nodeState != null
                    && m_oldPosition != null)
                {
                    var mult = 8.0f;
                    var deltaX = (m_oldPosition.X - newPosition.X) / mult;
                    var deltaY = (m_oldPosition.Y - newPosition.Y) / mult;

                    if (e.LeftButton == System.Windows.Input.MouseButtonState.Pressed)
                    {
                        m_nodeState.addAngularVelocity(-deltaY, deltaX);
                    }
                    else if (e.RightButton == System.Windows.Input.MouseButtonState.Pressed)
                    {
                        m_nodeState.addScalarVelocity(deltaX, -deltaY, 0.0f);
                    }
                }

                m_oldPosition = newPosition;
            }
        }

        private void OnLeftTimer(object sender, EventArgs e)
        {
            if (m_nodeState != null)
            {
                var speed = 1.0f;
                m_nodeState.addScalarVelocity(speed, 0.0f, 0.0f);
            }
        }

        private void OnRightTimer(object sender, EventArgs e)
        {
            if (m_nodeState != null)
            {
                var speed = 1.0f;
                m_nodeState.addScalarVelocity(-speed, 0.0f, 0.0f);
            }
        }

        private void OnForwardTimer(object sender, EventArgs e)
        {
            if (m_nodeState != null)
            {
                var speed = 1.0f;
                m_nodeState.addScalarVelocity(0.0f, 0.0f, speed);
            }
        }

        private void OnBackTimer(object sender, EventArgs e)
        {
            if (m_nodeState != null)
            {
                var speed = 1.0f;
                m_nodeState.addScalarVelocity(0.0f, 0.0f, -speed);
            }
        }

        private void OnUpTimer(object sender, EventArgs e)
        {
            if (m_nodeState != null)
            {
                var speed = 1.0f;
                m_nodeState.addScalarVelocity(0.0f, speed, 0.0f);
            }
        }

        private void OnDownTimer(object sender, EventArgs e)
        {
            if (m_nodeState != null)
            {
                var speed = 1.0f;
                m_nodeState.addScalarVelocity(0.0f, -speed, 0.0f);
            }
        }

        private void OnMotionTimer(object sender, EventArgs e)
        {
            if (m_nodeState != null)
            {
                m_nodeState.update();
            }
        }

        /// <summary>
        /// Forwards the new size to the render window
        /// </summary>
        /// <param name="sender"></param>
        /// <param name="e"></param>
        private void OnSizeChanged(object sender, System.Windows.SizeChangedEventArgs e)
        {
            if (m_renderTarget != null)
            {
                Castor3D.Size size = new Castor3D.Size();
                size.Width = (uint)e.NewSize.Width;
                size.Height = (uint)e.NewSize.Height;
                m_renderWindow.Resize(size);
            }

            var window = GetWindow(RenderPanel);
            window.Height = e.NewSize.Height;
            window.Width = e.NewSize.Width;
        }

        /// <summary>
        /// Opens a files selector to select a file name, and to load it.
        /// </summary>
        /// <param name="sender"></param>
        /// <param name="e"></param>
        private void OnLoaded(object sender, System.Windows.RoutedEventArgs e)
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
        private NodeState m_nodeState;
        /// <summary>
        /// The previous mouse position
        /// </summary>
        private Vector2D m_oldPosition;
        /// <summary>
        /// The motion timer
        /// </summary>
        private DispatcherTimer m_motionTimer;
        /// <summary>
        /// The keyboard timers
        /// </summary>
        private DispatcherTimer m_leftTimer;
        private DispatcherTimer m_rightTimer;
        private DispatcherTimer m_forwardTimer;
        private DispatcherTimer m_backTimer;
        private DispatcherTimer m_upTimer;
        private DispatcherTimer m_downTimer;

        private bool m_isShiftDown;
        private bool m_isCtrlDown;
        private bool m_isAltDown;

        #endregion
    }
}
