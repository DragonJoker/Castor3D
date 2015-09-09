#ifndef ___MainFrame___
#define ___MainFrame___

#include <GuiCommonPrerequisites.hpp>

#include <wx/frame.h>
#include <wx/listctrl.h>
#include <wx/aui/framemanager.h>
#include <wx/aui/auibook.h>
#include <wx/aui/auibar.h>

#include <SceneObjectsList.hpp>
#include <Logger.hpp>
#include <Path.hpp>
#include <Path.hpp>

namespace CastorViewer
{
	class RenderPanel;

	class MainFrame
		: public wxFrame
	{
	public:
		MainFrame( wxWindow * parent, wxString const & title, Castor3D::eRENDERER_TYPE p_eRenderer );
		~MainFrame();

		bool Initialise();
		void LoadScene( wxString const & p_strFileName = wxEmptyString );
		void ToggleFullScreen( bool p_fullscreen );

	private:
		bool DoLoadMeshFile();
		bool DoLoadTextSceneFile();
		bool DoLoadBinarySceneFile();
		void DoLoadSceneFile();
		void DoLoadPlugins();
		void DoInitialiseGUI();
		bool DoInitialise3D();
		bool DoInitialiseImages();
		void DoPopulateStatusBar();
		void DoPopulateToolBar();
		void DoInitialisePerspectives();
		void DoLogCallback( Castor::String const & p_strLog, Castor::ELogType p_eLogType );

	private:
		DECLARE_EVENT_TABLE()
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

	private:
		int m_iLogsHeight;
		int m_iPropertiesWidth;
		wxAuiManager m_auiManager;
		RenderPanel * m_pRenderPanel;
		wxTimer * m_timer;
		wxPanel * m_pBgPanel;
		wxAuiToolBar * m_toolBar;
		wxAuiNotebook * m_logTabsContainer;
		wxAuiNotebook * m_sceneTabsContainer;
		wxPropertiesHolder * m_propertiesContainer;
		wxListBox * m_messageLog;
		wxListBox * m_errorLog;
		GuiCommon::wxImagesLoader * m_pImagesLoader;
		GuiCommon::wxSplashScreen * m_pSplashScreen;
		GuiCommon::wxSceneObjectsList * m_sceneObjectsList;
		GuiCommon::wxMaterialsList * m_materialsList;
		Castor3D::SceneWPtr m_pMainScene;
		Castor3D::CameraWPtr m_pMainCamera;
		Castor3D::SceneNodeWPtr m_pSceneNode;
		Castor3D::Engine * m_pCastor3D;
		Castor3D::eRENDERER_TYPE m_eRenderer;
		Castor::Path m_strFilePath;
		wxString m_currentPerspective;
		wxString m_fullScreenPerspective;
		wxTimer * m_timerErr;
		wxArrayString m_errLogList;
		std::mutex m_errLogListMtx;
		wxTimer * m_timerMsg;
		wxArrayString m_msgLogList;
		std::mutex m_msgLogListMtx;
	};
}

#endif
