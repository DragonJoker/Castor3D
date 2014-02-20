#ifndef ___GC_GeometriesFrame___
#define ___GC_GeometriesFrame___

namespace GuiCommon
{
	class wxGeometryTreeItemData : public wxTreeItemData
	{
	protected:
		Castor3D::GeometryWPtr m_pGeometry;

	public:
		wxGeometryTreeItemData( Castor3D::GeometrySPtr p_pGeometry );
		~wxGeometryTreeItemData();

		inline Castor3D::GeometrySPtr GetGeometry() { return m_pGeometry.lock(); }
	};
	
	class wxSubmeshTreeItemData : public wxTreeItemData
	{
	protected:
		Castor3D::SubmeshWPtr m_pSubmesh;

	public:
		wxSubmeshTreeItemData( Castor3D::SubmeshSPtr p_pSubmesh );
		~wxSubmeshTreeItemData();

		inline Castor3D::SubmeshSPtr GetSubmesh() { return m_pSubmesh.lock(); }
	};

	class wxGeometriesListFrame : public wxPanel
	{
	public:
		typedef enum eBMP
		{	eBMP_VISIBLE
		,	eBMP_VISIBLE_SEL
		,	eBMP_HIDDEN
		,	eBMP_HIDDEN_SEL
		,	eBMP_GEOMETRY
		,	eBMP_GEOMETRY_SEL
		,	eBMP_GEOMETRY_OPEN
		,	eBMP_GEOMETRY_OPEN_SEL
		,	eBMP_SUBMESH
		,	eBMP_SUBMESH_SEL
		,	eBMP_COUNT
		}	eBMP;

	private:
		typedef enum eID
		{	eID_TREE_GEOMETRIES
		,	eID_COMBO_MATERIALS
		,	eID_BUTTON_DELETE
		}	eID;

		typedef enum eTREE_ID
		{	eTREE_ID_VISIBLE
		,	eTREE_ID_VISIBLE_SEL
		,	eTREE_ID_HIDDEN
		,	eTREE_ID_HIDDEN_SEL
		,	eTREE_ID_GEOMETRY
		,	eTREE_ID_GEOMETRY_SEL
		,	eTREE_ID_GEOMETRY_OPEN
		,	eTREE_ID_GEOMETRY_OPEN_SEL
		,	eTREE_ID_SUBMESH
		,	eTREE_ID_SUBMESH_SEL
		,	eTREE_ID_COUNT
		}	eTREE_ID;

		wxTreeCtrl *				m_pTreeGeometries;
		wxButton *					m_pButtonDeleteSelected;
		wxComboBox *				m_pComboMaterials;
		wxImageList *				m_pListImages;
		wxArrayString				m_arrayItems;
		std::set< wxTreeItemId >	m_setGeometriesInTree;
		std::set< wxTreeItemId >	m_setSubmeshesInTree;
		Castor3D::SceneSPtr			m_pScene;
		uint32_t					m_uiNbItems;
		Castor3D::Engine *			m_pEngine;
		wxTreeItemId				m_selItem;

	public:
		wxGeometriesListFrame( Castor3D::Engine * p_pEngine, wxWindow * p_pParent, wxString const & p_strTitle, Castor3D::SceneSPtr p_pScene, wxPoint const & p_ptPos = wxDefaultPosition, wxSize const & p_size = wxDefaultSize );
		~wxGeometriesListFrame();

	protected:
		DECLARE_EVENT_TABLE()
		void OnClose		( wxCloseEvent &	p_event );
		void OnDeleteItem	( wxCommandEvent &	p_event );
		void OnExpandItem	( wxTreeEvent &		p_event );
		void OnCollapseItem	( wxTreeEvent &		p_event );
		void OnActivateItem	( wxTreeEvent &		p_event );
		void OnChangeItem	( wxTreeEvent &		p_event );
		void OnComboMaterials( wxCommandEvent & p_event );
	};
}

#endif
