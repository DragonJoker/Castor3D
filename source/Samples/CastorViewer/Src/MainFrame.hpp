/*
This source file is part of Castor3D (http://castor3d.developpez.com/castor3d.html)
Copyright (c) 2016 dragonjoker59@hotmail.com

Permission is hereby granted, free of charge, to any person obtaining a copy of
this software and associated documentation files (the "Software"), to deal in
the Software without restriction, including without limitation the rights to
use, copy, modify, merge, publish, distribute, sublicense, and/or sell copies
of the Software, and to permit persons to whom the Software is furnished to do
so, subject to the following conditions:

The above copyright notice and this permission notice shall be included in all
copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
SOFTWARE.
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

		bool Initialise();
		void LoadScene( wxString const & p_strFileName = wxEmptyString );
		void ToggleFullScreen( bool p_fullscreen );

	private:
		void DoInitialiseGUI();
		bool DoInitialise3D();
		bool DoInitialiseImages();
		void DoPopulateStatusBar();
		void DoPopulateToolBar();
		void DoInitialisePerspectives();
		void DoLogCallback( Castor::String const & p_strLog, Castor::ELogType p_eLogType );
		void DoCleanupScene();
		void DoSaveFrame();
		bool DoStartRecord();
		void DoRecordFrame();
		void DoStopRecord();

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
		int m_iLogsHeight;
		int m_iPropertiesWidth;
		wxAuiManager m_auiManager;
		RenderPanel * m_pRenderPanel;
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
		Castor3D::SceneWPtr m_pMainScene;
		Castor3D::CameraWPtr m_pMainCamera;
		Castor3D::SceneNodeWPtr m_sceneNode;
		Castor::Path m_strFilePath;
		wxString m_currentPerspective;
		wxString m_fullScreenPerspective;
		wxTimer * m_timerErr;
		wxArrayString m_errLogList;
		std::mutex m_errLogListMtx;
		wxTimer * m_timerMsg;
		wxArrayString m_msgLogList;
		std::mutex m_msgLogListMtx;
		GuiCommon::Recorder m_recorder;
		int m_recordFps;
	};
}

#endif
