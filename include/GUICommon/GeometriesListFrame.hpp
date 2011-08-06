#ifndef ___GC_GeometriesFrame___
#define ___GC_GeometriesFrame___

namespace GuiCommon
{
	class wxGeometryTreeItemData : public wxTreeItemData
	{
	protected:
		Castor3D::GeometryPtr m_pGeometry;

	public:
		wxGeometryTreeItemData( Castor3D::GeometryPtr p_pGeometry);
		~wxGeometryTreeItemData();

		inline Castor3D::GeometryPtr GetGeometry() { return m_pGeometry; }
	};

	class wxGeometriesListFrame : public wxFrame
	{
	public:
		typedef enum
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
		typedef enum
		{	eID_TREE_GEOMETRIES
		,	eID_COMBO_MATERIALS
		,	eID_BUTTON_DELETE
		}	eID;

		typedef enum
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

		Castor3D::ScenePtr m_scene;
		wxTreeCtrl * m_pTreeGeometries;
		wxArrayString m_items;
		unsigned int m_nbItems;
		wxButton * m_pButtonDeleteSelected;
		wxComboBox * m_pComboMaterials;
		std::set <wxTreeItemId> m_setGeometriesInTree;
		wxImageList * m_pImages;

	public:
		wxGeometriesListFrame( wxWindow * parent, const wxString & title,
							   Castor3D::ScenePtr p_scene,
							   const wxPoint & pos = wxDefaultPosition,
							   const wxSize & size = wxDefaultSize);
		~wxGeometriesListFrame();

	protected:
		DECLARE_EVENT_TABLE()
		void OnDelete( wxCommandEvent & event);
		void OnExpandItem( wxTreeEvent & event);
		void OnCollapseItem( wxTreeEvent & event);
		void OnActivateItem( wxTreeEvent & event);
	};
}

#endif
