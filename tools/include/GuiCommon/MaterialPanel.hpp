#ifndef ___GC_MaterialPanel___
#define ___GC_MaterialPanel___

namespace GuiCommon
{
	class wxPassPanel;

	class wxMaterialPanel : public wxPanel
	{
	private:
		typedef enum eID
		{	eID_BUTTON_DELETE
		,	eID_EDIT_MATERIAL_NAME
		,	eID_COMBO_PASS
		}	eID;

	private:
		wxStaticText *						m_pStaticName;
		wxTextCtrl *						m_pEditMaterialName;
		wxStaticBox *						m_pBoxPass;
		wxComboBox *						m_pComboPass;
		wxButton *							m_pButtonDeletePass;
		wxPanel *							m_pPanelPasses;
		wxPassPanel *						m_pPanelSelectedPass;
		Castor3D::MaterialWPtr				m_wpMaterial;
		Castor3D::PassWPtr					m_wpPassSelected;
		int									m_iSelectedPassIndex;
		bool								m_bCanEdit;
		Castor3D::Engine *					m_pEngine;

	public:
		wxMaterialPanel( Castor3D::Engine * p_pEngine, bool p_bCanEdit, wxWindow * p_pParent, wxPoint const & p_ptPos = wxDefaultPosition, wxSize const & p_size = wxSize( 370, 470 ) );
		~wxMaterialPanel();

		void SelectPass( int p_iIndex );
		void SetMaterialName( Castor::String const & p_strMaterialName );
		int GetPassIndex()const;

	private:
		void DoSetMaterialName( wxString const & p_strMaterialName );
		void DoCreatePassPanel();
		void DoShowMaterialFields( bool p_bShow );
		void DoShowPassFields( bool p_bShow );

		DECLARE_EVENT_TABLE()
		void OnDeletePass		( wxCommandEvent & p_event );
		void OnMaterialName		( wxCommandEvent & p_event );
		void OnPassSelect		( wxCommandEvent & p_event );
	};
}

#endif
