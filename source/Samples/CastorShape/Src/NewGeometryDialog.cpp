#include "NewGeometryDialog.hpp"
#include "NewMaterialDialog.hpp"

#include <MaterialManager.hpp>

#include <wx/propgrid/propgrid.h>

using namespace Castor3D;
using namespace Castor;
using namespace GuiCommon;

namespace CastorShape
{
	namespace
	{
		static wxString GEOMETRY_NAME = _( "Name" );
		static wxString GEOMETRY_NAME_DEFAULT_VALUE = _( "Geometry name" );
		static wxString GEOMETRY_MATERIAL = _( "Material" );
	}

	NewGeometryDialog::NewGeometryDialog( Engine * p_pEngine, wxWindow * parent, wxWindowID p_id, wxString const & p_name, wxPoint const & pos, wxSize const & size, long style )
		: wxDialog( parent, p_id, p_name, pos, size, style, p_name )
		, m_engine( p_pEngine )
		, m_properties( NULL )
	{
		GEOMETRY_NAME = _( "Name" );
		GEOMETRY_NAME_DEFAULT_VALUE = _( "Geometry name" );
		GEOMETRY_MATERIAL = _( "Material" );

		SetBackgroundColour( PANEL_BACKGROUND_COLOUR );
		SetForegroundColour( PANEL_FOREGROUND_COLOUR );

		m_properties = new wxPropertyGrid( this, wxID_ANY, wxDefaultPosition, GetClientSize(), wxPG_SPLITTER_AUTO_CENTER | wxPG_DEFAULT_STYLE );
		m_properties->SetBackgroundColour( PANEL_BACKGROUND_COLOUR );
		m_properties->SetForegroundColour( PANEL_FOREGROUND_COLOUR );
		m_properties->SetCaptionBackgroundColour( PANEL_BACKGROUND_COLOUR );
		m_properties->SetCaptionTextColour( PANEL_FOREGROUND_COLOUR );
		m_properties->SetSelectionBackgroundColour( ACTIVE_TAB_COLOUR );
		m_properties->SetSelectionTextColour( ACTIVE_TEXT_COLOUR );
		m_properties->SetCellBackgroundColour( INACTIVE_TAB_COLOUR );
		m_properties->SetCellTextColour( INACTIVE_TEXT_COLOUR );
		m_properties->SetLineColour( BORDER_COLOUR );
		m_properties->SetMarginColour( BORDER_COLOUR );
	}

	NewGeometryDialog::~NewGeometryDialog()
	{
	}

	String NewGeometryDialog::GetGeometryName()const
	{
		return make_String( m_properties->GetPropertyValue( GEOMETRY_NAME ).GetString() );
	}

	String NewGeometryDialog::GetMaterialName()const
	{
		String l_res = make_String( m_properties->GetPropertyValue( GEOMETRY_MATERIAL ).GetString() );
		NewMaterialDialog * l_dialog;
		MaterialSPtr l_material;

		while ( l_res == _( "New..." ) )
		{
			l_dialog = new NewMaterialDialog( m_engine, NULL, wxID_ANY );

			if ( l_dialog->ShowModal() == wxID_OK )
			{
				l_material = l_dialog->GetMaterial();

				if ( l_material )
				{
					l_res = l_material->GetName();
				}
			}

			l_dialog->Destroy();
		}

		return l_res;
	}

	void NewGeometryDialog::DoInitialise()
	{
		StringArray l_choices1;
		m_engine->GetMaterialManager().GetNames( l_choices1 );
		wxPGChoices l_choices;
		l_choices.Add( wxCOMBO_NEW );

		for ( auto && l_choice : l_choices1 )
		{
			l_choices.Add( make_wxString( l_choice ) );
		}

		m_properties->Append( new wxStringProperty( GEOMETRY_NAME, wxPG_LABEL, _( "New Geometry" ) ) );
		m_properties->Append( new wxEnumProperty( GEOMETRY_MATERIAL, wxPG_LABEL, l_choices ) )->SetValue( wxCOMBO_NEW );
		DoCreateProperties();
		m_properties->SetSize( m_properties->GetBestSize() );

		wxButton * l_ok = new wxButton( this, wxID_OK, _( "OK" ), wxPoint( 5, GetClientSize().y - 35 ), wxSize( 60, 30 ), wxBORDER_SIMPLE );
		wxButton * l_cancel = new wxButton( this, wxID_CANCEL, _( "Cancel" ), wxPoint( GetClientSize().x - 65, GetClientSize().y - 35 ), wxSize( 60, 30 ), wxBORDER_SIMPLE );

		wxBoxSizer * l_btnsSizer = new wxBoxSizer( wxHORIZONTAL );
		l_btnsSizer->Add( l_ok );
		l_btnsSizer->AddStretchSpacer( 1 );
		l_btnsSizer->Add( l_cancel );

		wxBoxSizer * l_sizer = new wxBoxSizer( wxVERTICAL );
		l_sizer->Add( m_properties, wxSizerFlags( 1 ).Expand().Border( wxALL, 5 ) );
		l_sizer->Add( l_btnsSizer, wxSizerFlags( 0 ).Expand().Border( wxALL, 5 ) );
		SetSizer( l_sizer );
		l_sizer->SetSizeHints( this );
	}

	BEGIN_EVENT_TABLE( NewGeometryDialog, wxDialog )
		EVT_BUTTON(	wxID_OK, NewGeometryDialog::OnOk )
	END_EVENT_TABLE()

	void NewGeometryDialog::OnOk( wxCommandEvent & p_event )
	{
		EndModal( wxID_OK );
		p_event.Skip();
	}
}
