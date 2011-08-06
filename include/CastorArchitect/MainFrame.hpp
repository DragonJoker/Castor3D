#ifndef ___CA_MainFrame___
#define ___CA_MainFrame___

namespace CastorArchitect
{
	class RenderPanel;

	class MainFrame : public wxFrame
	{
	private:
		enum eIDs
		{
			eExit,
			eLoadScene,
			eGeometries,
			eMaterials,
		};

		enum eBMPs
		{
			eBmpScene		= wxGeometriesListFrame::eNbBmps,
			eBmpGeometries,
			eBmpMaterials,
		};

	private:
		RenderPanel			*	m_p3dFrame;
		RenderPanel			*	m_p2dFrame;
		wxPanel				*	m_pPanelTools;
		Castor3D::ScenePtr		m_pMainScene;
		String					m_strFilePath;
		ImagesLoader		*	m_pImagesLoader;
		Castor3D::Root			m_castor3D;

	public:
		MainFrame( wxWindow * p_pParent, const wxString & p_strTitle);
		~MainFrame();

		void LoadScene( const String & p_strFileName=cuEmptyString);

	private:
		void _initialise3D();
		void _populateToolbar();

	private:
		DECLARE_EVENT_TABLE()
		void _onPaint			( wxPaintEvent & p_event);
		void _onSize			( wxSizeEvent & p_event);
		void _onMove			( wxMoveEvent & p_event);
		void _onClose			( wxCloseEvent & p_event);
		void _onEnterWindow		( wxMouseEvent & p_event);
		void _onLeaveWindow		( wxMouseEvent & p_event);
		void _onEraseBackground	( wxEraseEvent & p_event);
		void _onKeyUp			( wxKeyEvent & p_event);

		void _onMenuClose				( wxCommandEvent & p_event);
		void _onLoadScene				( wxCommandEvent & p_event);
		void _onShowGeometriesList		( wxCommandEvent & p_event);
		void _onShowMaterialsList		( wxCommandEvent & p_event);
	};
}

#endif