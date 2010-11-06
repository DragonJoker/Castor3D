#ifndef ___CV_GeometriesFrame___
#define ___CV_GeometriesFrame___

namespace CastorViewer
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
			eFile			= 0,
			eFileSelected	= 1,
			eFolder			= 2,
			eFolderSelected	= 3,
			eFolderOpened	= 4,
			eClassFunction	= 5,
		};

		Castor3D::ScenePtr m_scene;
		wxTreeCtrl * m_pTreeGeometries;
		wxArrayString m_items;
		unsigned int m_nbItems;
		wxButton * m_pButtonDeleteSelected;
		wxComboBox * m_pComboMaterials;
		std::set <wxTreeItemId> m_setGeometriesInTree;

	public:
		GeometriesListFrame( wxWindow * parent, const wxString & title,
							   Castor3D::ScenePtr p_scene,
							   const wxPoint & pos = wxDefaultPosition,
							   const wxSize & size = wxDefaultSize);
		~GeometriesListFrame();

	protected:
		DECLARE_EVENT_TABLE()
		void OnCheck( wxCommandEvent & event);
		void OnDelete( wxCommandEvent & event);
		void OnExpandItem( wxTreeEvent & event);
		void OnCollapseItem( wxTreeEvent & event);
		void OnActivateItem( wxTreeEvent & event);
	};
}

#endif