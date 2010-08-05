#ifndef ___CV_MaterialsFrame___
#define ___CV_MaterialsFrame___

namespace CastorViewer
{
	class MaterialsListView;
	class MaterialPanel;

	class MaterialsFrame : public wxFrame
	{
	private:
		enum eIDs
		{
			eMaterialsList,
		};

	private:
		MaterialsListView * m_materialsList;
		int m_listWidth;
		Castor3D::Material * m_selectedMaterial;

		MaterialPanel * m_materialPanel;

	public:
		MaterialsFrame( wxWindow * parent, const wxString & title,
						  const wxPoint & pos = wxDefaultPosition,
						  const wxSize & size = wxSize( 500, 500));
		~MaterialsFrame();

		void CreateMaterialPanel( const String & p_materialName);

	private:
		DECLARE_EVENT_TABLE()
		void _onSelected	( wxListEvent & event);
		void _onDeselected	( wxListEvent & event);
	};
}
//******************************************************************************
#endif
//******************************************************************************
