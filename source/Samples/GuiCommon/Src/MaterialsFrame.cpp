#include "MaterialsFrame.hpp"
#include "MaterialsListView.hpp"
#include "MaterialPanel.hpp"

#include <Engine.hpp>
#include <Material.hpp>
#include <MaterialManager.hpp>

using namespace Castor3D;
using namespace Castor;
using namespace GuiCommon;

wxMaterialsFrame::wxMaterialsFrame( Engine * p_pEngine, bool p_bCanEdit, wxWindow * p_pParent, wxString const & p_strTitle, wxPoint const & p_ptPos, wxSize const & p_size )
	:	wxFrame( p_pParent, wxID_ANY, p_strTitle, p_ptPos, p_size, wxSYSTEM_MENU | wxCAPTION | wxCLOSE_BOX | wxCLIP_CHILDREN | wxFRAME_FLOAT_ON_PARENT )
	,	m_iListWidth( 120	)
	,	m_bCanEdit( p_bCanEdit	)
	,	m_pEngine( p_pEngine	)
{
}

wxMaterialsFrame::~wxMaterialsFrame()
{
}

void wxMaterialsFrame::Initialise()
{
	DoEndInit( DoBaseInit() );
}

void wxMaterialsFrame::SetMaterialName( String const & p_strMaterialName )
{
	m_pSelectedMaterial = m_pEngine->GetMaterialManager().find( p_strMaterialName );
	m_pMaterialPanel->SetMaterialName( p_strMaterialName );
}

wxSizer * wxMaterialsFrame::DoBaseInit()
{
	wxSize l_size = GetClientSize();
	m_pMaterialsList = new wxMaterialsListView( m_pEngine, this, eID_LIST_MATERIALS, wxDefaultPosition, wxSize( m_iListWidth, 100 ) );//l_size.y ) );
	m_pMaterialPanel = new wxMaterialPanel( m_pEngine, m_bCanEdit, this, wxDefaultPosition, wxDefaultSize );//, wxPoint( m_iListWidth, 25 ), wxSize( l_size.x - m_iListWidth, l_size.y - 25 ) );
	m_pMaterialsList->Show();
	m_pMaterialsList->CreateList();
	wxBoxSizer * l_pSizer = new wxBoxSizer( wxHORIZONTAL );
	l_pSizer->Add(	m_pMaterialsList,	wxSizerFlags( 0 ).Expand() );
	l_pSizer->Add(	m_pMaterialPanel,	wxSizerFlags( 1 ).Expand() );
	return l_pSizer;
}

void wxMaterialsFrame::DoEndInit( wxSizer * p_pSizer )
{
	SetSizer( p_pSizer );
	p_pSizer->SetSizeHints( this );
}

BEGIN_EVENT_TABLE( wxMaterialsFrame, wxFrame )
	EVT_LIST_ITEM_SELECTED(	eID_LIST_MATERIALS, wxMaterialsFrame::OnSelected	)
	EVT_LIST_ITEM_DESELECTED(	eID_LIST_MATERIALS, wxMaterialsFrame::OnDeselected	)
END_EVENT_TABLE()

void wxMaterialsFrame::OnSelected( wxListEvent & p_event )
{
	SetMaterialName( ( wxChar const * )p_event.GetText().c_str() );
}

void wxMaterialsFrame::OnDeselected( wxListEvent & p_event )
{
// 	if( m_pSelectedMaterial )
// 	{
// 		m_pSelectedMaterial.reset();
// 	}
//
// 	SetMaterialName( cuEmptyString );
	p_event.Skip();
}
