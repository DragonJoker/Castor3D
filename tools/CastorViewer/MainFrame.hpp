/*
See LICENSE file in root folder
*/
#ifndef ___MainFrame___
#define ___MainFrame___

#include <GuiCommon/GuiCommonPrerequisites.hpp>

#include <CastorUtils/Config/BeginExternHeaderGuard.hpp>
#include <wx/frame.h>
#include <wx/listctrl.h>
#include <wx/aui/framemanager.h>
#include <wx/aui/auibook.h>
#include <wx/aui/auibar.h>
#include <CastorUtils/Config/EndExternHeaderGuard.hpp>

#include <GuiCommon/GuiCommonPrerequisites.hpp>

#include <GuiCommon/System/Recorder.hpp>
#include <GuiCommon/System/SceneObjectsTree.hpp>

#include <CastorUtils/Log/Logger.hpp>
#include <CastorUtils/Data/Path.hpp>

namespace CastorViewer
{
	class RenderPanel;

	struct LogContainer
	{
		c3d::Vector< c3d::Pair< wxString, bool > > queue;
		c3d::Mutex mutex;
		wxListBox * listBox{};
	};

	enum class eBMP
	{
		eScenes = uint32_t( GuiCommon::eBMP::eCount ),
		eMaterials,
		eExport,
		eLogs,
		eProperties,
		ePrintScreen
	};

	class MainFrame
		: public wxFrame
	{
	public:
		explicit MainFrame( wxString const & title );

		bool initialise( GuiCommon::SplashScreen & splashScreen );
		void loadScene( wxString const & fileName = wxEmptyString );
		void toggleFullScreen( bool fullscreen );
		void select( c3d::Geometry const * geometry, c3d::Submesh const * submesh )const;

	private:
		void doInitialiseTimers();
		void doInitialiseGUI();
		void doPopulateStatusBar();
		void doPopulateToolBar( GuiCommon::SplashScreen & splashScreen );
		void doInitialisePerspectives();
		void doLogCallback( c3d::MbString const & log, c3d::LogType type, bool newLine );
		void doCleanupScene();
		void doSaveFrame();
		bool doStartRecord();
		void doRecordFrame();
		void doStopRecord();
		void doSceneLoadEnd( c3d::RenderWindowDesc const & window );

	private:
#pragma clang diagnostic push
#pragma clang diagnostic ignored "-Wmissing-override"
		DECLARE_EVENT_TABLE()
#pragma clang diagnostic pop
		void onRenderTimer( wxTimerEvent & event );
		void onTimer( wxTimerEvent & event );
		void onFpsTimer( wxTimerEvent & event );
		void onPaint( wxPaintEvent & event );
		void onSize( wxSizeEvent & event );
		void onClose( wxCloseEvent  & event );
		void onEnterWindow( wxMouseEvent & event );
		void onLeaveWindow( wxMouseEvent & event );
		void onEraseBackground( wxEraseEvent & event );
		void onKeyUp( wxKeyEvent & event );
		void onLoadScene( wxCommandEvent & event );
		void onExportScene( wxCommandEvent & event );
		void onShowLogs( wxCommandEvent & event );
		void onShowLists( wxCommandEvent & event );
		void onPrintScreen( wxCommandEvent & event );
		void onRecord( wxCommandEvent & event );
		void onStop( wxCommandEvent & event );
		void onSceneLoadEnd( wxThreadEvent & event );

	private:
		int m_logsHeight{ 100 };
		int m_propertiesWidth{ 240 };
		wxAuiManager m_auiManager;
		RenderPanel * m_renderPanel{};
		c3d::RawUniquePtr< wxTimer > m_timer{};
		c3d::RawUniquePtr< wxTimer > m_fpsTimer{};
		wxAuiToolBar * m_toolBar{};
		wxMenu * m_fileMenu{};
		wxMenu * m_tabsMenu{};
		wxMenu * m_captureMenu{};
		wxMenuBar * m_menuBar{};
		wxAuiNotebook * m_logTabsContainer{};
		wxAuiNotebook * m_sceneTabsContainer{};
		LogContainer m_messageLog;
		LogContainer m_errorLog;
#ifndef NDEBUG
		LogContainer m_debugLog;
#endif
		GuiCommon::TreeListContainerT< GuiCommon::SceneObjectsTree > * m_sceneTree{};
		GuiCommon::TreeListContainerT< GuiCommon::SceneObjectsTree > * m_objectsTree{};
		GuiCommon::TreeListContainerT< GuiCommon::SceneObjectsTree > * m_nodesTree{};
		GuiCommon::TreeListContainerT< GuiCommon::SceneObjectsTree > * m_lightsTree{};
		GuiCommon::TreeListContainerT< GuiCommon::SceneObjectsTree > * m_materialsTree{};
		GuiCommon::TreeListContainerT< GuiCommon::SceneObjectsTree > * m_overlaysTree{};
		GuiCommon::TreeListContainerT< GuiCommon::SceneObjectsTree > * m_guiTree{};
		c3d::SceneRPtr m_mainScene{};
		c3d::CameraRPtr m_mainCamera{};
		c3d::SceneNodeRPtr m_sceneNode{};
		c3d::Path m_filePath;
		wxString m_currentPerspective;
		wxString m_fullScreenPerspective;
		wxString m_debugPerspective;
		c3d::RawUniquePtr< wxTimer > m_timerErr{};
		c3d::RawUniquePtr< wxTimer > m_timerMsg{};
		GuiCommon::Recorder m_recorder;
		int m_recordFps{};
		wxString m_title;
		uint32_t m_minCount{};
		uint32_t m_maxCount{};
		GuiCommon::SceneObjectsTree::SelectLightConnection m_selectLight;
		GuiCommon::SceneObjectsTree::SelectSubmeshConnection m_selectSubmesh;
		GuiCommon::SceneObjectsTree::SelectNodeConnection m_selectNode;
	};
}

#endif
