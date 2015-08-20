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

	wxMaterialsListFrame::wxMaterialsListFrame( wxWindow * p_pParent, wxString const & p_strTitle, wxPoint const & p_ptPos, wxSize const & p_size )
		: wxPanel( p_pParent, wxID_ANY, /*p_strTitle, */p_ptPos, p_size/*, wxSYSTEM_MENU | wxCAPTION | wxCLOSE_BOX | wxCLIP_CHILDREN | wxFRAME_FLOAT_ON_PARENT*/ )
		, m_engine( NULL )
	{
		m_pMaterialsList = new wxMaterialsListView( 24, this, eID_LIST_MATERIALS, wxDefaultPosition, wxSize( GetClientSize().x, 70 ) );
		m_materialPanel = new wxMaterialPanel( true, true, this, wxDefaultPosition, wxDefaultSize );

		wxBoxSizer * l_pSizer = new wxBoxSizer( wxVERTICAL );
		l_pSizer->Add( m_pMaterialsList, wxSizerFlags( 1 ).Expand().ReserveSpaceEvenIfHidden() );
		l_pSizer->Add( m_materialPanel, wxSizerFlags( 1 ).Expand().ReserveSpaceEvenIfHidden() );
		SetSizer( l_pSizer );
		l_pSizer->SetSizeHints( this );
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
		//m_pSelectedMaterial = m_pEngine->GetMaterialManager().find( p_strMaterialName );
		m_materialPanel->SetMaterialName( m_engine, ( wxChar const * )p_event.GetText().c_str() );
	}
}
