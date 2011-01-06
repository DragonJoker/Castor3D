#ifndef ___GC_MaterialsFrame___
#define ___GC_MaterialsFrame___

namespace GUICommon
{
	class MaterialsListView;
	class MaterialPanel;

	class MaterialsFrame : public wxFrame
	{
	protected:
		enum eIDs
		{
			eMaterialsList,
		};

	protected:
		MaterialsListView * m_materialsList;
		int m_listWidth;
		Castor3D::MaterialPtr m_selectedMaterial;

		MaterialPanel * m_materialPanel;
		Castor3D::MaterialManager * m_pManager;

	public:
		MaterialsFrame( Castor3D::MaterialManager * p_pManager, wxWindow * parent, const wxString & title,
						  const wxPoint & pos = wxDefaultPosition,
						  const wxSize & size = wxSize( 500, 500));
		~MaterialsFrame();

		virtual void CreateMaterialPanel( const String & p_materialName);

	protected:
		DECLARE_EVENT_TABLE()
		void _onShow		( wxShowEvent & event);
		void _onSelected	( wxListEvent & event);
		void _onDeselected	( wxListEvent & event);
	};
}
//******************************************************************************
#endif
//******************************************************************************
