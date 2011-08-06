#ifndef ___GC_MaterialsFrame___
#define ___GC_MaterialsFrame___

namespace GuiCommon
{
	class wxMaterialsListView;
	class wxMaterialPanel;

	class wxMaterialsFrame : public wxFrame
	{
	protected:
		typedef enum
		{	eID_LIST_MATERIALS
		}	eID;

	protected:
		wxMaterialsListView * m_materialsList;
		int m_listWidth;
		Castor3D::MaterialPtr m_selectedMaterial;

		wxMaterialPanel * m_materialPanel;

	public:
		wxMaterialsFrame( wxWindow * parent, const wxString & title,
						  const wxPoint & pos = wxDefaultPosition,
						  const wxSize & size = wxSize( 500, 500));
		~wxMaterialsFrame();

		virtual void CreateMaterialPanel( String const & p_materialName);

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
