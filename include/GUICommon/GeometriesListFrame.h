#ifndef ___GC_GeometriesFrame___
#define ___GC_GeometriesFrame___

namespace GUICommon
{
	class GeometryTreeItemData : public wxTreeItemData
	{
	protected:
		Castor3D::GeometryPtr m_pGeometry;

	public:
		GeometryTreeItemData( Castor3D::GeometryPtr p_pGeometry);
		~GeometryTreeItemData();

		inline Castor3D::GeometryPtr GetGeometry() { return m_pGeometry; }
	};

	class GeometriesListFrame : public wxFrame
	{
	private:
		enum eIDs
		{
			eGeometriesList,
			eMaterialsList,
			eDeleteSelected
		};

		enum TreeImagesIDs
		{
			eGeoVisible,
			eGeoVisibleSel,
			eGeoCachee,
			eGeoCacheeSel,
			eGeometrie,
			eGeometrieSel,
			eGeometrieOuv,
			eGeometrieOuvSel,
			eSubmesh,
			eSubmeshSel,
			eNbImages
		};

		Castor3D::ScenePtr m_scene;
		wxTreeCtrl * m_pTreeGeometries;
		wxArrayString m_items;
		unsigned int m_nbItems;
		wxButton * m_pButtonDeleteSelected;
		wxComboBox * m_pComboMaterials;
		std::set <wxTreeItemId> m_setGeometriesInTree;
		wxImageList * m_pImages;
		Castor3D::MaterialManager * m_pManager;

	public:
		GeometriesListFrame( Castor3D::MaterialManager * p_pManager, wxWindow * parent, const wxString & title,
							   Castor3D::ScenePtr p_scene,
							   const wxPoint & pos = wxDefaultPosition,
							   const wxSize & size = wxDefaultSize);
		~GeometriesListFrame();

	protected:
		DECLARE_EVENT_TABLE()
		void OnDelete( wxCommandEvent & event);
		void OnExpandItem( wxTreeEvent & event);
		void OnCollapseItem( wxTreeEvent & event);
		void OnActivateItem( wxTreeEvent & event);
	};
}

#endif