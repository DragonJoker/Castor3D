#include "MaterialsListFrame.hpp"

#include "MaterialsListView.hpp"
#include "MaterialPanel.hpp"

#include <Engine.hpp>
#include <Material.hpp>
#include <MaterialManager.hpp>

using namespace Castor3D;
using namespace Castor;

namespace GuiCommon
{
	namespace
	{
		typedef enum eID
		{
			eID_LIST_MATERIALS,
		}	eID;
	}
	static const int GC_IMG_SIZE = 20;

	wxMaterialsListFrame::wxMaterialsListFrame( wxWindow * p_propsParent, wxWindow * p_pParent, wxString const & p_strTitle, wxPoint const & p_ptPos, wxSize const & p_size )
		: wxPanel( p_pParent, wxID_ANY, p_ptPos, p_size )
		, m_engine( NULL )
	{
		if ( !p_propsParent || p_propsParent == p_pParent )
		{
			m_pMaterialsList = new wxMaterialsListView( 24, this, eID_LIST_MATERIALS, wxDefaultPosition, wxSize( GetClientSize().x, 70 ) );
			m_materialPanel = new wxMaterialPanel( true, true, this, wxDefaultPosition, wxDefaultSize );
			wxBoxSizer * l_pSizer = new wxBoxSizer( wxVERTICAL );
			l_pSizer->Add( m_pMaterialsList, wxSizerFlags( 1 ).Expand().ReserveSpaceEvenIfHidden() );
			l_pSizer->Add( m_materialPanel, wxSizerFlags( 2 ).Expand().ReserveSpaceEvenIfHidden() );
			SetSizer( l_pSizer );
			l_pSizer->SetSizeHints( this );
		}
		else
		{
			m_pMaterialsList = new wxMaterialsListView( 24, this, eID_LIST_MATERIALS, wxDefaultPosition, wxSize( GetClientSize().x, 70 ) );
			wxBoxSizer * l_pSizer = new wxBoxSizer( wxVERTICAL );
			l_pSizer->Add( m_pMaterialsList, wxSizerFlags( 1 ).Expand().ReserveSpaceEvenIfHidden() );
			SetSizer( l_pSizer );
			l_pSizer->SetSizeHints( this );

			m_materialPanel = new wxMaterialPanel( true, true, p_propsParent, wxDefaultPosition, wxDefaultSize );
			l_pSizer = new wxBoxSizer( wxVERTICAL );
			l_pSizer->Add( m_materialPanel, wxSizerFlags( 1 ).Expand().ReserveSpaceEvenIfHidden() );
			p_propsParent->SetSizer( l_pSizer );
			l_pSizer->SetSizeHints( p_propsParent );
			p_propsParent->Update();
		}
	}

	wxMaterialsListFrame::~wxMaterialsListFrame()
	{
	}

	void wxMaterialsListFrame::LoadMaterials( Engine * p_pEngine )
	{
		m_engine = p_pEngine;
		m_pMaterialsList->Show();
		m_pMaterialsList->CreateList( p_pEngine );
	}

	void wxMaterialsListFrame::UnloadMaterials()
	{
		m_pMaterialsList->DeleteAllItems();
	}

	BEGIN_EVENT_TABLE( wxMaterialsListFrame, wxPanel )
		EVT_CLOSE( wxMaterialsListFrame::OnClose )
		EVT_LIST_ITEM_SELECTED( eID_LIST_MATERIALS, wxMaterialsListFrame::OnSelected )
	END_EVENT_TABLE()

	void wxMaterialsListFrame::OnClose( wxCloseEvent & p_event )
	{
		p_event.Skip();
	}

	void wxMaterialsListFrame::OnSelected( wxListEvent & p_event )
	{
		m_materialPanel->SetMaterialName( m_engine, ( wxChar const * )p_event.GetText().c_str() );
	}
}
