#ifndef ___CA_MaterialPanel___
#define ___CA_MaterialPanel___

namespace CastorArchitect
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

	public:
		MaterialPanel( wxWindow * parent, const wxPoint & pos = wxPoint( 0, 0), const wxSize & size = wxSize( 370, 470));
		~MaterialPanel();

		void CreateMaterialPanel( const String & p_materialName);
		int GetPassIndex()const;

	private:
		void _onDeletePass			( wxCommandEvent & event);
		void _onMaterialName		( wxCommandEvent & event);
		void _onPassSelect			( wxCommandEvent & event);

		void _createMaterialPanel( const Char * p_materialName);
		void _createPassPanel();

	DECLARE_EVENT_TABLE()
	};
}

#endif