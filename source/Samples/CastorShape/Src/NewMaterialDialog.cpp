#include "NewMaterialDialog.hpp"

#include <PropertiesHolder.hpp>

#include <MaterialManager.hpp>
#include <Material.hpp>

using namespace Castor3D;
using namespace GuiCommon;

namespace CastorShape
{
	NewMaterialDialog::NewMaterialDialog( Engine * p_pEngine, wxWindow * parent, wxWindowID p_id, wxString const & p_name, wxPoint const & pos, wxSize const & size, long style )
		: wxDialog( parent, p_id, p_name, pos, size, style, p_name )
		, m_pEngine( p_pEngine )
	{
		wxSize l_size = GetClientSize();
		l_size.y -= 30;
		MaterialSPtr l_pMaterial = std::make_shared< Material >( m_pEngine, cuT( "NewMaterial" ) );
		l_pMaterial->CreatePass();
		m_material = l_pMaterial;
		m_pEngine->GetMaterialManager().insert( cuT( "NewMaterial" ), l_pMaterial );
		m_properties = new PropertiesHolder( true, this, wxDefaultPosition, l_size );
		m_okButton = new wxButton( this, wxID_OK, _( "OK" ), wxPoint( 20, l_size.y + 5 ), wxSize( 60, 25 ), wxBORDER_SIMPLE );
		m_cancelButton = new wxButton( this, wxID_CANCEL, _( "Cancel" ), wxPoint( 120, l_size.y + 5 ), wxSize( 60, 25 ), wxBORDER_SIMPLE );
	}

	NewMaterialDialog::~NewMaterialDialog()
	{
	}

	BEGIN_EVENT_TABLE( NewMaterialDialog, wxDialog )
		EVT_CLOSE( NewMaterialDialog::OnClose )
		EVT_BUTTON( wxID_OK, NewMaterialDialog::OnOk )
		EVT_BUTTON( wxID_CANCEL, NewMaterialDialog::OnCancel )
		END_EVENT_TABLE()

		void NewMaterialDialog::OnClose( wxCloseEvent & WXUNUSED( p_event ) )
	{
		if( m_material.lock() )
		{
			m_pEngine->GetMaterialManager().erase( m_material.lock()->GetName() );
		}

		m_material.reset();
		EndModal( wxID_CANCEL );
	}

	void NewMaterialDialog::OnOk( wxCommandEvent & WXUNUSED( p_event ) )
	{
		EndModal( wxID_OK );
	}

	void NewMaterialDialog::OnCancel( wxCommandEvent & WXUNUSED( p_event ) )
	{
		if( m_material.lock() )
		{
			m_pEngine->GetMaterialManager().erase( m_material.lock()->GetName() );
		}

		m_material.reset();
		EndModal( wxID_CANCEL );
	}
}
