#ifndef ___GC_MaterialsFrame___
#define ___GC_MaterialsFrame___

namespace GuiCommon
{
	class wxMaterialsListView;
	class wxMaterialPanel;

	class wxMaterialsFrame : public wxFrame
	{
	protected:
		typedef enum eID
		{	eID_LIST_MATERIALS
		}	eID;

	protected:
		wxMaterialsListView *	m_pMaterialsList;
		wxMaterialPanel *		m_pMaterialPanel;
		Castor3D::MaterialWPtr	m_pSelectedMaterial;
		int						m_iListWidth;
		bool					m_bCanEdit;
		Castor3D::Engine *		m_pEngine;

	public:
		wxMaterialsFrame( Castor3D::Engine * p_pEngine, bool p_bCanEdit, wxWindow * p_pParent, wxString const & p_strTitle, wxPoint const & p_ptPos = wxDefaultPosition, wxSize const & p_size = wxSize( 500, 500 ) );
		~wxMaterialsFrame();

		virtual void SetMaterialName( Castor::String const & p_strMaterialName );
		virtual void Initialise();

	protected:
		wxSizer *	DoBaseInit	(						);
		void		DoEndInit	( wxSizer * p_pSizer	);

	protected:
		DECLARE_EVENT_TABLE()
		void OnShow			( wxShowEvent & p_event );
		void OnSelected		( wxListEvent & p_event );
		void OnDeselected	( wxListEvent & p_event );
	};
}
//******************************************************************************
#endif
//******************************************************************************
