#include "CastorShape/PrecompiledHeader.hpp"

#include "CastorShape/material/NewMaterialDialog.hpp"

using namespace Castor3D;
using namespace CastorShape;
using namespace GuiCommon;

NewMaterialDialog :: NewMaterialDialog( Engine * p_pEngine, wxWindow * parent, wxWindowID p_id, wxString const & p_name, wxPoint const & pos, wxSize const & size, long style )
	:	wxDialog( parent, p_id, p_name, pos, size, style, p_name)
	,	m_pEngine( p_pEngine )
{
	wxSize l_size = GetClientSize();
	l_size.y -= 30;
	MaterialSPtr l_pMaterial = std::make_shared< Material >( m_pEngine, cuT( "NewMaterial" ), 1 );
	m_material = l_pMaterial;
	m_pEngine->GetMaterialManager().insert( cuT( "NewMaterial"), l_pMaterial );

	m_materialPanel = new wxMaterialPanel( p_pEngine, true, this, wxPoint( 0, 0), l_size );
	m_materialPanel->SetMaterialName( cuT( "NewMaterial" ) );
	m_okButton = new wxButton( this, nmdOK, _( "OK"), wxPoint( 20, l_size.y + 5), wxSize( 60, 25 ), wxBORDER_SIMPLE);
	m_cancelButton = new wxButton( this, nmdCancel, _( "Cancel"), wxPoint( 120, l_size.y + 5), wxSize( 60, 25 ), wxBORDER_SIMPLE);

	wxBoxSizer * l_pSizerAll = new wxBoxSizer( wxVERTICAL	);
	wxBoxSizer * l_pSizerBtn = new wxBoxSizer( wxHORIZONTAL	);

	l_pSizerBtn->Add(	m_okButton,			wxSizerFlags( 0 ).Border( wxLEFT,	  5 )	);
	l_pSizerBtn->Add(	100, 0,				1											);
	l_pSizerBtn->Add(	m_cancelButton,		wxSizerFlags( 0 ).Border( wxRIGHT,	  5 )	);

	l_pSizerAll->Add( 	m_materialPanel,	wxSizerFlags( 1 ).Expand()					);
	l_pSizerAll->Add( 	l_pSizerBtn,		wxSizerFlags( 0 ).Expand()					);

	SetSizer( l_pSizerAll );
	l_pSizerAll->SetSizeHints( this );
}

NewMaterialDialog :: ~NewMaterialDialog()
{
}

BEGIN_EVENT_TABLE( NewMaterialDialog, wxDialog)
	EVT_CLOSE(					NewMaterialDialog::OnClose)
	EVT_BUTTON(	nmdOK,			NewMaterialDialog::OnOk)
	EVT_BUTTON(	nmdCancel,		NewMaterialDialog::OnCancel)
END_EVENT_TABLE()

void NewMaterialDialog :: OnClose( wxCloseEvent & WXUNUSED( p_event ))
{
	if( m_material.lock() )
	{
		m_pEngine->GetMaterialManager().erase( m_material.lock()->GetName() );
	}

	m_material.reset();
	EndModal( wxID_CANCEL);
}

void NewMaterialDialog :: OnOk( wxCommandEvent& WXUNUSED( p_event ))
{
	EndModal( wxID_OK);
}

void NewMaterialDialog :: OnCancel( wxCommandEvent& WXUNUSED( p_event ))
{
	if( m_material.lock() )
	{
		m_pEngine->GetMaterialManager().erase( m_material.lock()->GetName() );
	}

	m_material.reset();
	EndModal( wxID_CANCEL);
}
