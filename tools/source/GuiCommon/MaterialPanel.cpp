#include "GuiCommon/PrecompiledHeader.hpp"

#include "GuiCommon/MaterialPanel.hpp"
#include "GuiCommon/PassPanel.hpp"

using namespace Castor3D;
using namespace Castor;
using namespace GuiCommon;

wxMaterialPanel :: wxMaterialPanel( Engine * p_pEngine, bool p_bCanEdit, wxWindow * p_pParent, wxPoint const & p_ptPos, wxSize const & p_size )
	:	wxPanel( p_pParent, wxID_ANY, p_ptPos, p_size, wxTAB_TRAVERSAL | wxBORDER_NONE )
	,	m_pStaticName			( NULL			)
	,	m_pEditMaterialName		( NULL			)
	,	m_pComboPass			( NULL			)
	,	m_pButtonDeletePass		( NULL			)
	,	m_pPanelPasses			( NULL			)
	,	m_pPanelSelectedPass	( NULL			)
	,	m_wpMaterial			(				)
	,	m_wpPassSelected		(				)
	,	m_iSelectedPassIndex	( -1			)
	,	m_bCanEdit				( p_bCanEdit	)
	,	m_pEngine				( p_pEngine		)
{
	m_pStaticName			= new wxStaticText(	this, wxID_ANY,					_( "Name : "	)	);
	m_pEditMaterialName		= new wxTextCtrl(	this, eID_EDIT_MATERIAL_NAME,	wxEmptyString,		wxDefaultPosition,	wxDefaultSize, wxTE_PROCESS_ENTER | wxBORDER_SIMPLE );
	m_pBoxPass				= new wxStaticBox(	this, wxID_ANY,					_( "Passes"	)	);
	m_pButtonDeletePass		= new wxButton(		this, eID_BUTTON_DELETE,		_( "Delete"	)	);
	m_pComboPass			= new wxComboBox(	this, eID_COMBO_PASS,			wxT( "0"		),	wxDefaultPosition,	wxDefaultSize, wxArrayString(), wxBORDER_SIMPLE | wxCB_READONLY );
	m_pPanelPasses			= new wxPanel(		this, wxID_ANY										);
	m_pPanelSelectedPass	= new wxPassPanel(	m_pEngine, m_bCanEdit, m_pPanelPasses				);

	wxBoxSizer *		l_pSizer		= new wxBoxSizer(		wxVERTICAL				);
	wxBoxSizer *		l_pSizerName	= new wxBoxSizer(		wxHORIZONTAL			);
	wxBoxSizer *		l_pSizerPanel	= new wxBoxSizer(		wxVERTICAL				);
	wxStaticBoxSizer *	l_pSizerBox		= new wxStaticBoxSizer( m_pBoxPass, wxVERTICAL	);

	l_pSizerName->Add(	m_pStaticName,			wxSizerFlags( 0 ).Align( wxALIGN_CENTER_VERTICAL ).ReserveSpaceEvenIfHidden()	);
	l_pSizerName->Add(	m_pEditMaterialName,	wxSizerFlags( 1 ).Align( wxALIGN_CENTER_VERTICAL ).ReserveSpaceEvenIfHidden()	);
	l_pSizerName->Add(	m_pButtonDeletePass,	wxSizerFlags( 0 ).Align( wxALIGN_CENTER_VERTICAL ).ReserveSpaceEvenIfHidden()	);

	l_pSizerPanel->Add(	m_pPanelSelectedPass,	wxSizerFlags( 1 ).Border( wxALL,	5 ).Expand()								);
	m_pPanelPasses->SetSizer( l_pSizerPanel );
	l_pSizerPanel->SetSizeHints( m_pPanelPasses );

	l_pSizerBox->Add(	m_pComboPass,			wxSizerFlags( 0 ).Border( wxLEFT,	5 )											);
	l_pSizerBox->Add(	m_pPanelPasses,			wxSizerFlags( 1 ).Expand()														);

	l_pSizer->Add(		l_pSizerName,			wxSizerFlags( 0 ).Border( wxALL,	5 ).Expand()								);
	l_pSizer->Add(		l_pSizerBox,			wxSizerFlags( 0 ).Border( wxALL,	5 ).Expand()								);

	SetSizer( l_pSizer );
	l_pSizer->SetSizeHints( this );

	DoShowMaterialFields( false );
	DoShowPassFields( false );
}

wxMaterialPanel :: ~wxMaterialPanel()
{
}

void wxMaterialPanel :: SelectPass( int p_iIndex )
{
	MaterialSPtr l_pMaterial = m_wpMaterial.lock();

	if( l_pMaterial )
	{
		m_iSelectedPassIndex = p_iIndex;

		if( m_iSelectedPassIndex == -1 )
		{
			m_wpPassSelected = l_pMaterial->CreatePass();
			wxString l_strValue;
			l_strValue << (l_pMaterial->GetPassCount() - 1);
			m_pComboPass->Insert( l_strValue, l_pMaterial->GetPassCount() - 1 );
			m_pComboPass->Update();
			m_pComboPass->SetValue( l_strValue );
		}
		else
		{
			m_wpPassSelected = l_pMaterial->GetPass( m_iSelectedPassIndex );
		}

		m_pPanelSelectedPass->SetPass( m_wpPassSelected );
		DoShowPassFields( true );
	}
}

void wxMaterialPanel :: SetMaterialName( String const & p_strMaterialName )
{
	MaterialSPtr l_pMaterial = m_wpMaterial.lock();

	if( l_pMaterial && m_bCanEdit )
	{
		l_pMaterial->Initialise();
		l_pMaterial.reset();
	}

	m_wpMaterial = m_pEngine->GetMaterialManager().find( p_strMaterialName.c_str() );
	l_pMaterial = m_wpMaterial.lock();

	if( l_pMaterial )
	{
		m_pEditMaterialName->SetValue( p_strMaterialName );
		m_pComboPass->Clear();

		for( uint32_t i = 0; i < l_pMaterial->GetPassCount(); i++ )
		{
			wxString l_strName;
			l_strName << i;
			m_pComboPass->Append( l_strName );
		}

		if( m_bCanEdit )
		{
			m_pComboPass->Append( wxCOMBO_NEW );
		}

		m_wpPassSelected.reset();
		DoShowMaterialFields( true );

		if( l_pMaterial->GetPassCount() )
		{
			m_pComboPass->SetValue( wxT( "0" ) );
			SelectPass( 0 );
		}
	}
	else
	{
		DoShowMaterialFields( false );
		DoShowPassFields( false );
	}
}

int wxMaterialPanel :: GetPassIndex()const
{
	String l_strValue = (wxChar const *)m_pComboPass->GetValue();
	int l_iReturn = -2;

	if( str_utils::is_integer( l_strValue ) )
	{
		l_iReturn = str_utils::to_int( l_strValue );
	}
	else if( l_strValue == wxCOMBO_NEW )
	{
		l_iReturn = -1;
	}

	return l_iReturn;
}

void wxMaterialPanel :: DoShowMaterialFields( bool p_bShow )
{
	m_pStaticName->Show( p_bShow );
	m_pEditMaterialName->Show( p_bShow );

	if( m_pBoxPass )
	{
		m_pBoxPass->Show( p_bShow );
	}

	m_pComboPass->Show( p_bShow );
	m_pEditMaterialName->Enable( m_bCanEdit );
}

void wxMaterialPanel :: DoShowPassFields( bool p_bShow )
{
	m_pButtonDeletePass->Show( m_bCanEdit && p_bShow );
	m_pPanelPasses->Show( p_bShow );
	m_pPanelSelectedPass->Show( p_bShow );
}

BEGIN_EVENT_TABLE( wxMaterialPanel, wxPanel )
	EVT_BUTTON(		eID_BUTTON_DELETE,			wxMaterialPanel::OnDeletePass	)
	EVT_TEXT_ENTER( eID_EDIT_MATERIAL_NAME,		wxMaterialPanel::OnMaterialName	)
	EVT_COMBOBOX(	eID_COMBO_PASS,				wxMaterialPanel::OnPassSelect	)
END_EVENT_TABLE()

void wxMaterialPanel :: OnDeletePass( wxCommandEvent & p_event )
{
	MaterialSPtr l_pMaterial = m_wpMaterial.lock();

	if( l_pMaterial )
	{
		if( l_pMaterial && l_pMaterial->GetPassCount() > 1 )
		{
			m_pComboPass->Clear();
			l_pMaterial->DestroyPass( m_iSelectedPassIndex );
			l_pMaterial->Initialise();

			for( uint32_t i = 0; i < l_pMaterial->GetPassCount(); i++ )
			{
				wxString l_strName;
				l_strName << i;
				m_pComboPass->Append( l_strName );
			}

			m_pComboPass->Append( wxCOMBO_NEW );
			m_pComboPass->SetValue( wxCOMBO_NEW );
			m_pComboPass->Update();

			m_pButtonDeletePass->Hide();
			m_pPanelPasses->Hide();
			m_pPanelSelectedPass->Hide();
		}
	}

	p_event.Skip();
}

void wxMaterialPanel :: OnMaterialName( wxCommandEvent & p_event )
{
	MaterialSPtr l_pMaterial = m_wpMaterial.lock();

	if( l_pMaterial )
	{
		l_pMaterial->ChangeName( (wxChar const *)m_pEditMaterialName->GetValue().c_str() );
		l_pMaterial->Initialise();
	}

	p_event.Skip();
}

void wxMaterialPanel :: OnPassSelect( wxCommandEvent & p_event )
{
	SelectPass( GetPassIndex() );

	p_event.Skip();
}
