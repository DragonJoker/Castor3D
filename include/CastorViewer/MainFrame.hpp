#ifndef ___MainFrame___
#define ___MainFrame___

namespace CastorViewer
{
	class RenderPanel;

	class MainFrame : public wxFrame
	{
	private:
		typedef enum
		{	eID_TOOL_EXIT
		,	eID_TOOL_LOAD_SCENE
		,	eID_TOOL_GEOMETRIES
		,	eID_TOOL_MATERIALS
		}	eID;

		typedef enum
		{	eBMP_SCENE		= wxGeometriesListFrame::eBMP_COUNT
		,	eBMP_GEOMETRIES
		,	eBMP_MATERIALS
		}	eBMP;

	private:
		RenderPanel			*	m_3dFrame;
		Castor3D::ScenePtr		m_mainScene;
		Path					m_strFilePath;
		ImagesLoader		*	m_pImagesLoader;
		Castor3D::SceneNodePtr	m_pSceneNode;
		wxTimer 			* 	m_timer;
		wxSplashScreen		*	m_pSplashScreen;
		Castor3D::Root			m_castor3D;

	public:
		MainFrame( wxWindow * parent, const wxString & title);
		~MainFrame();

		void LoadScene( const wxString & p_strFileName=wxEmptyString);

	private:
		void _initialise3D();
		void _initialiseImages();
		void _populateToolbar();

	private:
		DECLARE_EVENT_TABLE()
		void _onTimer				( wxTimerEvent & event);
		void _onPaint				( wxPaintEvent & event);
		void _onSize				( wxSizeEvent & event);
		void _onMove				( wxMoveEvent & event);
		void _onClose				( wxCloseEvent & event);
		void _onEnterWindow			( wxMouseEvent & event);
		void _onLeaveWindow			( wxMouseEvent & event);
		void _onEraseBackground		( wxEraseEvent & event);
		void _onKeyUp				( wxKeyEvent & event);
		void _onLoadScene			( wxCommandEvent & event);
		void _onShowGeometriesList	( wxCommandEvent & event);
		void _onShowMaterialsList	( wxCommandEvent & event);
	};
}

#endif
