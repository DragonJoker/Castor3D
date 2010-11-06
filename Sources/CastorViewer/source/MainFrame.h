#ifndef ___MainFrame___
#define ___MainFrame___

namespace CastorViewer
{
	class RenderPanel;
	class GeometriesListFrame;
	class MaterialsFrame;

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

	private:
		RenderPanel			*	m_3dFrame;
		Castor3D::ScenePtr		m_mainScene;
		Castor3D::Root		*	m_castor3D;
		String					m_strFilePath;
		GeometriesListFrame	*	m_geometriesList;
		MaterialsFrame		*	m_materialsList;

	public:
		MainFrame( wxWindow * parent, const wxString & title);
		~MainFrame();

		void ShowPanels();
		void LoadScene( const String & p_strFileName=C3DEmptyString);

	private:
		void _initialise3D();
		void _buildMenuBar();

	private:
		DECLARE_EVENT_TABLE()
		void _onPaint			( wxPaintEvent & event);
		void _onSize			( wxSizeEvent & event);
		void _onMove			( wxMoveEvent & event);
		void _onClose			( wxCloseEvent & event);
		void _onEnterWindow		( wxMouseEvent & event);
		void _onLeaveWindow		( wxMouseEvent & event);
		void _onEraseBackground	( wxEraseEvent & event);
		void _onKeyUp			( wxKeyEvent & event);

		void _onMenuClose				( wxCommandEvent & event);
		void _onLoadScene				( wxCommandEvent & event);
		void _onShowGeometriesList		( wxCommandEvent & event);
		void _onShowMaterialsList		( wxCommandEvent & event);
	};
}

#endif