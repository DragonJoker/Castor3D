#ifndef ___GC_MaterialPanel___
#define ___GC_MaterialPanel___

namespace GuiCommon
{
	class PassPanel;

	class MaterialPanel : public wxPanel
	{
	private:
		enum eIDs
		{
			eDeletePass,
			eMaterialName,
			ePass
		};

	private:
		wxTextCtrl * m_materialName;
		wxComboBox * m_passSelector;
		wxButton * m_deletePass;
		wxPanel * m_passesPanel;

		Castor3D::MaterialPtr m_material;
		Castor3D::PassPtr m_selectedPass;
		int m_selectedPassIndex;
		PassPanel * m_selectedPassPanel;
		Castor3D::MaterialManager * m_pManager;

	public:
		MaterialPanel( Castor3D::MaterialManager * p_pManager, wxWindow * parent, const wxPoint & pos = wxPoint( 0, 0), const wxSize & size = wxSize( 370, 470));
		~MaterialPanel();

		void CreateMaterialPanel( const String & p_materialName);
		int GetPassIndex()const;

	private:
		void _createMaterialPanel( const xchar * p_materialName);
		void _createPassPanel();

		DECLARE_EVENT_TABLE()
		void _onDeletePass			( wxCommandEvent & event);
		void _onMaterialName		( wxCommandEvent & event);
		void _onPassSelect			( wxCommandEvent & event);
	};
}

#endif
