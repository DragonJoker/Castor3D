#ifndef ___MainFrame___
#define ___MainFrame___

namespace CastorViewer
{
	class RenderPanel;

	class MainFrame : public wxFrame
	{
	private:
		typedef enum eID
		{	eID_TOOL_EXIT
		,	eID_TOOL_LOAD_SCENE
		,	eID_TOOL_EXPORT_SCENE
		,	eID_TOOL_GEOMETRIES
		,	eID_TOOL_MATERIALS
		}	eID;

		typedef enum eBMP
		{	eBMP_SCENE		= wxGeometriesListFrame::eBMP_COUNT
		,	eBMP_GEOMETRIES
		,	eBMP_MATERIALS
		,	eBMP_EXPORT
		}	eBMP;

	private:
		RenderPanel *				m_pRenderPanel;
		Castor3D::SceneWPtr			m_pMainScene;
		Castor3D::CameraWPtr		m_pMainCamera;
		Castor::Path				m_strFilePath;
		ImagesLoader *				m_pImagesLoader;
		Castor3D::SceneNodeWPtr		m_pSceneNode;
		wxTimer * 					m_timer;
		wxSplashScreen *			m_pSplashScreen;
		Castor3D::Engine *			m_pCastor3D;
		wxPanel *					m_pBgPanel;
		int							m_iListHeight;
		wxListView *				m_pListLog;
		Castor3D::eRENDERER_TYPE	m_eRenderer;
		wxAuiManager *				m_pAuiManager;
		wxGeometriesListFrame *		m_pGeometriesFrame;

	public:
		MainFrame( wxWindow * parent, wxString const & title, Castor3D::eRENDERER_TYPE p_eRenderer );
		~MainFrame();

		bool Initialise();
		void LoadScene( wxString const & p_strFileName=wxEmptyString );

	private:
		bool			DoInitialise3D		();
		bool			DoInitialiseImages	();
		void			DoPopulateToolbar	();
		void			DoLog				( Castor::String const & p_strLog, Castor::eLOG_TYPE p_eLogType );
		void			DoExportScene		( Castor::Path const & p_pathFile );
		void			DoObjExportScene	( Castor::Path const & p_pathFile );
		Castor::String	DoObjExport			( Castor::Path const & p_pathMtlFolder, Castor3D::MaterialSPtr p_pMaterial );
		Castor::String	DoObjExport			( Castor3D::MeshSPtr p_pMesh, uint32_t & p_uiOffset, uint32_t & p_uiCount );
		void			DoCscnExportScene	( Castor::Path const & p_pathFile );
		void			DoBinaryExportScene	( Castor::Path const & p_pathFile );

		static void DoLogCallback( MainFrame * p_pThis, Castor::String const & p_strLog, Castor::eLOG_TYPE p_eLogType );

	private:
		DECLARE_EVENT_TABLE()
		void OnTimer				( wxTimerEvent &		p_event );
		void OnPaint				( wxPaintEvent &		p_event );
		void OnSize					( wxSizeEvent &			p_event );
		void OnInit					( wxInitDialogEvent &	p_event );
		void OnClose				( wxCloseEvent &		p_event );
		void OnEnterWindow			( wxMouseEvent &		p_event );
		void OnLeaveWindow			( wxMouseEvent &		p_event );
		void OnEraseBackground		( wxEraseEvent &		p_event );
		void OnKeyUp				( wxKeyEvent &			p_event );
		void OnLoadScene			( wxCommandEvent &		p_event );
		void OnExportScene			( wxCommandEvent &		p_event );
		void OnShowGeometriesList	( wxCommandEvent &		p_event );
		void OnShowMaterialsList	( wxCommandEvent &		p_event );
	};
}

#endif
