/*
See LICENSE file in root folder
*/
#ifndef ___MainFrame___
#define ___MainFrame___

#include <GuiCommon/GuiCommonPrerequisites.hpp>

#include <GuiCommon/Recorder.hpp>
#include <GuiCommon/SceneObjectsList.hpp>

#include <CastorUtils/Log/Logger.hpp>
#include <CastorUtils/Data/Path.hpp>

#include <wx/frame.h>
#include <wx/listctrl.h>
#include <wx/aui/framemanager.h>
#include <wx/aui/auibook.h>
#include <wx/aui/auibar.h>

#if defined( __WXOSX_COCOA__ )
#	define CV_MainFrameToolbar 0
#else
#	define CV_MainFrameToolbar 1
#endif

namespace CastorViewer
{
	class RenderPanel;

	typedef enum eBMP
	{
		eBMP_SCENES = GuiCommon::eBMP_COUNT,
		eBMP_MATERIALS,
		eBMP_EXPORT,
		eBMP_LOGS,
		eBMP_PROPERTIES,
		eBMP_PRINTSCREEN
	}	eBMP;

	class MainFrame
		: public wxFrame
	{
	public:
		MainFrame( wxString const & title );
		~MainFrame();

		bool initialise( GuiCommon::SplashScreen & splashScreen );
		void loadScene( wxString const & fileName = wxEmptyString );
		void toggleFullScreen( bool fullscreen );
		void select( castor3d::GeometrySPtr geometry, castor3d::SubmeshSPtr submesh );

	private:
		void doInitialiseGUI();
		bool doInitialise3D();
		bool doInitialiseImages();
		void doPopulateStatusBar();
		void doPopulateToolBar( GuiCommon::SplashScreen & splashScreen );
		void doInitialisePerspectives();
		void doLogCallback( castor::String const & log, castor::LogType type, bool newLine );
		void doCleanupScene();
		void doSaveFrame();
		bool doStartRecord();
		void doRecordFrame();
		void doStopRecord();

	private:
		DECLARE_EVENT_TABLE()
		void OnRenderTimer( wxTimerEvent & event );
		void OnTimer( wxTimerEvent & event );
		void OnFpsTimer( wxTimerEvent & event );
		void OnPaint( wxPaintEvent & event );
		void OnSize( wxSizeEvent & event );
		void OnInit( wxInitDialogEvent & event );
		void OnClose( wxCloseEvent  & event );
		void OnEnterWindow( wxMouseEvent & event );
		void OnLeaveWindow( wxMouseEvent & event );
		void OnEraseBackground( wxEraseEvent & event );
		void OnKeyUp( wxKeyEvent & event );
		void OnLoadScene( wxCommandEvent & event );
		void OnExportScene( wxCommandEvent & event );
		void OnShowLogs( wxCommandEvent & event );
		void OnShowLists( wxCommandEvent & event );
		void OnShowProperties( wxCommandEvent & event );
		void OnPrintScreen( wxCommandEvent & event );
		void OnRecord( wxCommandEvent & event );
		void OnStop( wxCommandEvent & event );

	private:
		int m_logsHeight{ 100 };
		int m_propertiesWidth{ 240 };
		wxAuiManager m_auiManager;
		RenderPanel * m_renderPanel{ nullptr };
		wxTimer * m_timer{ nullptr };
		wxTimer * m_fpsTimer{ nullptr };
#if CV_MainFrameToolbar
		wxAuiToolBar * m_toolBar{ nullptr };
#else
		wxMenu * m_fileMenu{ nullptr };
		wxMenu * m_tabsMenu{ nullptr };
		wxMenu * m_captureMenu{ nullptr };
		wxMenuBar * m_menuBar{ nullptr };
#endif
		wxAuiNotebook * m_logTabsContainer{ nullptr };
		wxAuiNotebook * m_sceneTabsContainer{ nullptr };
		GuiCommon::PropertiesHolder * m_propertiesHolder{ nullptr };
		GuiCommon::PropertiesContainer * m_propertiesContainer{ nullptr };
		wxListBox * m_messageLog{ nullptr };
		wxListBox * m_errorLog{ nullptr };
		GuiCommon::SceneObjectsList * m_sceneObjectsList{ nullptr };
		GuiCommon::MaterialsList * m_materialsList{ nullptr };
		castor3d::SceneWPtr m_mainScene;
		castor3d::CameraWPtr m_mainCamera;
		castor3d::SceneNodeWPtr m_sceneNode;
		castor::Path m_filePath;
		wxString m_currentPerspective;
		wxString m_fullScreenPerspective;
		wxTimer * m_timerErr{ nullptr };
		std::vector< std::pair< wxString, bool > > m_errLogList;
		std::mutex m_errLogListMtx;
		wxTimer * m_timerMsg{ nullptr };
		std::vector< std::pair< wxString, bool > > m_msgLogList;
		std::mutex m_msgLogListMtx;
		GuiCommon::Recorder m_recorder;
		int m_recordFps{ 0 };
		wxString m_title;
	};
}

#endif
