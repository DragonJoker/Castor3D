#ifndef ___MainFrame___
#define ___MainFrame___

namespace CastorViewer
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
			eBmpScene		= GeometriesListFrame::eNbBmps,
			eBmpGeometries,
			eBmpMaterials,
		};

	private:
		RenderPanel			*	m_3dFrame;
		Castor3D::ScenePtr		m_mainScene;
		Castor3D::Root		*	m_pCastor3D;
		String					m_strFilePath;
		ImagesLoader		*	m_pImagesLoader;
		Castor3D::SceneNodePtr	m_pSceneNode;

	public:
		MainFrame( wxWindow * parent, const wxString & title);
		~MainFrame();

		void LoadScene( const String & p_strFileName=C3DEmptyString);

	private:
		void _initialise3D();
		void _populateToolbar();

	private:
		DECLARE_EVENT_TABLE()
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