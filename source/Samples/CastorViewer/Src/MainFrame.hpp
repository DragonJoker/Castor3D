/*
See LICENSE file in root folder
*/
#ifndef ___MainFrame___
#define ___MainFrame___

#include <GuiCommonPrerequisites.hpp>

#include <wx/frame.h>
#include <wx/listctrl.h>
#include <wx/aui/framemanager.h>
#include <wx/aui/auibook.h>
#include <wx/aui/auibar.h>

#include <SceneObjectsList.hpp>
#include <Log/Logger.hpp>
#include <Data/Path.hpp>

#include <Recorder.hpp>

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
	}	eBMP;

	class MainFrame
		: public wxFrame
	{
	public:
		MainFrame( GuiCommon::SplashScreen * p_splashScreen, wxString const & title );
		~MainFrame();

		bool initialise();
		void loadScene( wxString const & p_strFileName = wxEmptyString );
		void toggleFullScreen( bool p_fullscreen );
		void select( castor3d::GeometrySPtr geometry, castor3d::SubmeshSPtr submesh );

	private:
		void doInitialiseGUI();
		bool doInitialise3D();
		bool doInitialiseImages();
		void doPopulateStatusBar();
		void doPopulateToolBar();
		void doInitialisePerspectives();
		void doLogCallback( castor::String const & p_strLog, castor::LogType p_eLogType, bool p_newLine );
		void doCleanupScene();
		void doSaveFrame();
		bool doStartRecord();
		void doRecordFrame();
		void doStopRecord();

	private:
		DECLARE_EVENT_TABLE()
		void OnRenderTimer( wxTimerEvent  & p_event );
		void OnTimer( wxTimerEvent  & p_event );
		void OnPaint( wxPaintEvent  & p_event );
		void OnSize( wxSizeEvent  & p_event );
		void OnInit( wxInitDialogEvent & p_event );
		void OnClose( wxCloseEvent  & p_event );
		void OnEnterWindow( wxMouseEvent & p_event );
		void OnLeaveWindow( wxMouseEvent & p_event );
		void OnEraseBackground( wxEraseEvent & p_event );
		void OnKeyUp( wxKeyEvent & p_event );
		void OnLoadScene( wxCommandEvent & p_event );
		void OnExportScene( wxCommandEvent & p_event );
		void OnShowLogs( wxCommandEvent & p_event );
		void OnShowLists( wxCommandEvent & p_event );
		void OnShowProperties( wxCommandEvent & p_event );
		void OnPrintScreen( wxCommandEvent & p_event );
		void OnRecord( wxCommandEvent & p_event );
		void OnStop( wxCommandEvent & p_event );

	private:
		int m_logsHeight;
		int m_propertiesWidth;
		wxAuiManager m_auiManager;
		RenderPanel * m_renderPanel;
		wxTimer * m_timer;
		wxAuiToolBar * m_toolBar;
		wxAuiNotebook * m_logTabsContainer;
		wxAuiNotebook * m_sceneTabsContainer;
		GuiCommon::PropertiesHolder * m_propertiesContainer;
		wxListBox * m_messageLog;
		wxListBox * m_errorLog;
		GuiCommon::SplashScreen * m_splashScreen;
		GuiCommon::SceneObjectsList * m_sceneObjectsList;
		GuiCommon::MaterialsList * m_materialsList;
		castor3d::SceneWPtr m_mainScene;
		castor3d::CameraWPtr m_mainCamera;
		castor3d::SceneNodeWPtr m_sceneNode;
		castor::Path m_filePath;
		wxString m_currentPerspective;
		wxString m_fullScreenPerspective;
		wxTimer * m_timerErr;
		std::vector< std::pair< wxString, bool > > m_errLogList;
		std::mutex m_errLogListMtx;
		wxTimer * m_timerMsg;
		std::vector< std::pair< wxString, bool > > m_msgLogList;
		std::mutex m_msgLogListMtx;
		GuiCommon::Recorder m_recorder;
		int m_recordFps;
	};
}

#endif
