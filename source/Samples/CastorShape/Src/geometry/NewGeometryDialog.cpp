#include "../geometry/NewGeometryDialog.hpp"
#include "../material/NewMaterialDialog.hpp"

#include <MaterialManager.hpp>

using namespace Castor3D;
using namespace Castor;
using namespace CastorShape;

NewGeometryDialog::NewGeometryDialog( Engine * p_pEngine, wxWindow * parent, wxWindowID p_id, wxString const & p_name, wxPoint const & pos, wxSize const & size, long style )
	:	wxDialog( parent, p_id, p_name, pos, size, style, p_name	)
	,	m_actualY( 10	)
	,	m_pEngine( p_pEngine	)
{
}

NewGeometryDialog::~NewGeometryDialog()
{
}

wxSizer * NewGeometryDialog::AddTextCtrl( wxString const & p_name, wxString const & p_defaultValue, int p_width )
{
	wxSizer * l_pSizer = NULL;

	if ( m_textCtrls.find( p_name ) == m_textCtrls.end() )
	{
		wxSize			l_size		= GetClientSize() - wxSize( 30 + p_width, 0 );
		int 			l_width		= l_size.x;
		_updateY( 30 );
		wxStaticText *	l_pStatic	= new wxStaticText( this, wxID_ANY, p_name, wxPoint( 10, m_actualY - 30 ), wxSize( l_width, 25 ) );
		wxTextCtrl *	l_pEdit		= new wxTextCtrl( this, wxID_ANY, p_defaultValue, wxPoint( 80, m_actualY - 30 ), wxSize( p_width, 25 ), wxBORDER_SIMPLE );
		m_textCtrls.insert( std::make_pair( p_name, l_pEdit ) );
		l_pSizer = new wxBoxSizer( wxHORIZONTAL );
		l_pSizer->Add( l_pStatic,	wxSizerFlags( 0 ).Expand().Align( wxALIGN_CENTER_VERTICAL )	);
		l_pSizer->Add( l_pEdit,		wxSizerFlags( 1 ).Expand().Align( wxALIGN_CENTER_VERTICAL )	);
	}

	return l_pSizer;
}

wxSizer * NewGeometryDialog::AddCubeBox( wxString const & p_name, wxArrayString const & p_values, wxString const & p_defaultValue, wxWindowID p_id, int p_width )
{
	wxSizer * l_pSizer = NULL;

	if ( m_comboBoxes.find( p_name ) == m_comboBoxes.end() )
	{
		wxSize			l_size		= GetClientSize() - wxSize( 30 + p_width, 0 );
		int				l_width		= l_size.x;
		_updateY( 30 );
		wxStaticText *	l_pStatic	= new wxStaticText( this, wxID_ANY, p_name, wxPoint( 10, m_actualY - 30 ), wxSize( l_width, 25 ) );
		wxComboBox *	l_pCombo	= new wxComboBox( this, p_id, p_defaultValue, wxPoint( 80, m_actualY - 30 ), wxSize( p_width, 25 ), p_values, wxBORDER_SIMPLE | wxCB_READONLY );
		m_comboBoxes.insert( std::make_pair( p_name, l_pCombo ) );
		l_pSizer = new wxBoxSizer( wxHORIZONTAL );
		l_pSizer->Add( l_pStatic,	wxSizerFlags( 0 ).Expand().Align( wxALIGN_CENTER_VERTICAL )	);
		l_pSizer->Add( l_pCombo,	wxSizerFlags( 1 ).Expand().Align( wxALIGN_CENTER_VERTICAL )	);
	}

	return l_pSizer;
}

wxString NewGeometryDialog::GetTextValue( wxString const & p_name )const
{
	if ( m_textCtrls.find( p_name ) != m_textCtrls.end() )
	{
		return m_textCtrls.find( p_name )->second->GetValue();
	}

	return wxEmptyString;
}

wxString NewGeometryDialog::GetCBValue( wxString const & p_name )const
{
	if ( m_comboBoxes.find( p_name ) != m_comboBoxes.end() )
	{
		return m_comboBoxes.find( p_name )->second->GetValue();
	}

	return wxEmptyString;
}

void NewGeometryDialog::RemoveTextCtrl( wxString const & p_name )
{
	TextCtrlPtrStrMap::iterator l_it = m_textCtrls.find( p_name );
	RemoveChild( l_it->second );
	delete l_it->second;
	m_textCtrls.erase( l_it );
}

void NewGeometryDialog::RemoveCubeBox( wxString const & p_name )
{
	CubeBoxPtrStrMap::iterator l_it = m_comboBoxes.find( p_name );
	RemoveChild( l_it->second );
	delete l_it->second;
	m_comboBoxes.erase( l_it );
}

String NewGeometryDialog::GetGeometryName()const
{
	return String( GetTextValue( _( "Name" ) ).c_str() );
}

String NewGeometryDialog::GetMaterialName()const
{
	wxString l_value = GetCBValue( _( "Material" ) );
	String l_res = ( const xchar * )l_value.c_str();
	NewMaterialDialog * l_dialog;
	MaterialSPtr l_material;

	while ( l_res == _( "New..." ) )
	{
		l_dialog = new NewMaterialDialog( m_pEngine, NULL, wxID_ANY );

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

void NewGeometryDialog::_updateY( int p_offset )
{
	SetSize( GetSize() + wxSize( 0, p_offset ) );
	m_actualY += p_offset;
}

void NewGeometryDialog::DoEndInit( wxSizer * p_pSizer )
{
	m_okButton = new wxButton( this, eOK, _( "OK" ), wxPoint( 70, GetClientSize().y - 35 ), wxSize( 60, 30 ), wxBORDER_SIMPLE );
	p_pSizer->Add(	m_okButton,	wxSizerFlags( 0 ).Center().Border( wxALL, 5 )	);
	SetSizer( p_pSizer );
	p_pSizer->SetSizeHints( this );
}

wxSizer * NewGeometryDialog::DoDefaultInit()
{
	StringArray l_choices1;
	m_pEngine->GetMaterialManager().GetNames( l_choices1 );
	wxArrayString l_choices;
	l_choices.push_back( _( "New..." ) );
	std::for_each( l_choices1.begin(), l_choices1.end(), [&]( String const & p_strChoice )
	{
		l_choices.push_back( wxString( p_strChoice.c_str() ) );
	} );
	wxSizer *	l_pSizerText	= AddTextCtrl( _( "Name" ), _( "Geometry Name" ), 110 );
	wxSizer *	l_pSizerCombo	= AddCubeBox( _( "Material" ), l_choices, _( "New..." ), eMaterialName, 110 );
	wxBoxSizer * l_pSizer = new wxBoxSizer( wxVERTICAL );
	l_pSizer->Add( l_pSizerText,	wxSizerFlags( 0 ).Expand().Border( wxALL, 5 ) );
	l_pSizer->Add( l_pSizerCombo,	wxSizerFlags( 0 ).Expand().Border( wxALL, 5 ) );
	return l_pSizer;
}

BEGIN_EVENT_TABLE( NewGeometryDialog, wxDialog )
	EVT_BUTTON(	eOK,			NewGeometryDialog::_onOk )
	EVT_COMBOBOX(	eMaterialName,	NewGeometryDialog::_onMaterialSelect )
END_EVENT_TABLE()

void NewGeometryDialog::_onOk( wxCommandEvent & p_event )
{
	EndModal( wxID_OK );
	p_event.Skip();
}

void NewGeometryDialog::_onMaterialSelect( wxCommandEvent & p_event )
{
	/*
		wxString l_value = GetCBValue( cuT( "Material"));
		wxString l_res;
		if (l_value == cuT( "New..."))
		{
			NewMaterialDialog * l_dialog = new NewMaterialDialog( nullptr, wxID_ANY);
			MaterialPtr l_material = nullptr;
			if (l_dialog->ShowModal() == wxID_OK)
			{
				l_material = l_dialog->GetMaterial();
			}
			l_dialog->Destroy();

			if (l_material)
			{
				C3DVector <String> l_choices1;
				Root::GetSingletonPtr()->GetMaterialManager()->GetMaterialNames( l_choices1);

				wxArrayString l_choices;
				l_choices.push_back( cuT( "New..."));

				std::for_each( l_choices1.begin(), l_choices1.end(), [&]( String const & p_strChoice )
				{
					l_choices.push_back( wxString( p_strChoice.c_str() ) );
				} );

				l_res = l_material->GetName();

				CubeBoxPtrStrMap::iterator l_it = m_comboBoxes.find( cuT( "Material"));
				RemoveChild( l_it->second);
				delete l_it->second;
				l_it->second = new wxComboBox( this, ngpMaterialName, l_res, wxPoint( 80, 40), wxSize( 110, 20), l_choices, wxBORDER_SIMPLE | wxCB_READONLY);
			}
		}
	*/
	p_event.Skip();
}
