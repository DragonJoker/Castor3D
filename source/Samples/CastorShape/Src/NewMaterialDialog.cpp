#include "NewMaterialDialog.hpp"

#include <InitialiseEvent.hpp>
#include <MaterialManager.hpp>
#include <Material.hpp>
#include <Pass.hpp>
#include <TextureUnit.hpp>

#include <GradientButton.hpp>
#include <AdditionalProperties.hpp>

#include <wx/propgrid/advprops.h>

using namespace Castor;
using namespace Castor3D;
using namespace GuiCommon;

namespace CastorShape
{
	namespace
	{
		static wxString PROPERTY_CATEGORY_MATERIAL = _( "Material: " );
		static wxString PROPERTY_MATERIAL_NAME = _( "Name" );
		static wxString PROPERTY_MATERIAL_NAME_DEFAULT_VALUE = _( "New material" );
		static wxString PROPERTY_MATERIAL_PASSES = _( "Passes" );
		static wxString PROPERTY_CATEGORY_PASS = _( "Pass: " );
		static wxString PROPERTY_PASS_AMBIENT = _( "Ambient" );
		static wxString PROPERTY_PASS_DIFFUSE = _( "Diffuse" );
		static wxString PROPERTY_PASS_SPECULAR = _( "Specular" );
		static wxString PROPERTY_PASS_EMISSIVE = _( "Emissive" );
		static wxString PROPERTY_PASS_EXPONENT = _( "Exponent" );
		static wxString PROPERTY_PASS_TWO_SIDED = _( "Two sided" );
		static wxString PROPERTY_PASS_OPACITY = _( "Opacity" );
		static wxString PROPERTY_PASS_SHADER = _( "Shader" );
		static wxString PROPERTY_PASS_EDIT_SHADER = _( "Edit Shader..." );
		static wxString PROPERTY_PASS_TEXTURES = _( "Textures" );
		static wxString PROPERTY_CATEGORY_TEXTURE = _( "Texture: " );
		static wxString PROPERTY_TEXTURE_IMAGE = _( "Image" );
		static wxString PROPERTY_CHANNEL = _( "Channel" );
		static wxString PROPERTY_CHANNEL_COLOUR = _( "Colour" );
		static wxString PROPERTY_CHANNEL_DIFFUSE = _( "Diffuse" );
		static wxString PROPERTY_CHANNEL_NORMAL = _( "Normal" );
		static wxString PROPERTY_CHANNEL_OPACITY = _( "Opacity" );
		static wxString PROPERTY_CHANNEL_SPECULAR = _( "Specular" );
		static wxString PROPERTY_CHANNEL_HEIGHT = _( "Height" );
		static wxString PROPERTY_CHANNEL_AMBIENT = _( "Ambient" );
		static wxString PROPERTY_CHANNEL_GLOSS = _( "Gloss" );
	}

	NewMaterialDialog::NewMaterialDialog( wxPGEditor * p_editor, Engine * p_engine, wxWindow * parent, wxWindowID p_id, wxString const & p_name, wxPoint const & pos, wxSize const & size, long style )
		: wxDialog( parent, p_id, p_name, pos, size, style, p_name )
		, m_engine( p_engine )
		, m_editor( p_editor )
	{
		PROPERTY_CATEGORY_MATERIAL = _( "Material: " );
		PROPERTY_MATERIAL_NAME = _( "Name" );
		PROPERTY_MATERIAL_NAME_DEFAULT_VALUE = _( "New material" );
		PROPERTY_CATEGORY_PASS = _( "Pass: " );
		PROPERTY_PASS_AMBIENT = _( "Ambient" );
		PROPERTY_PASS_DIFFUSE = _( "Diffuse" );
		PROPERTY_PASS_SPECULAR = _( "Specular" );
		PROPERTY_PASS_EMISSIVE = _( "Emissive" );
		PROPERTY_PASS_EXPONENT = _( "Exponent" );
		PROPERTY_PASS_TWO_SIDED = _( "Two sided" );
		PROPERTY_PASS_OPACITY = _( "Opacity" );
		PROPERTY_PASS_SHADER = _( "Shader" );
		PROPERTY_PASS_EDIT_SHADER = _( "Edit Shader..." );
		PROPERTY_PASS_TEXTURES = _( "Textures" );
		PROPERTY_CATEGORY_TEXTURE = _( "Texture: " );
		PROPERTY_TEXTURE_IMAGE = _( "Image" );
		PROPERTY_CHANNEL = _( "Channel" );
		PROPERTY_CHANNEL_COLOUR = _( "Colour" );
		PROPERTY_CHANNEL_DIFFUSE = _( "Diffuse" );
		PROPERTY_CHANNEL_NORMAL = _( "Normal" );
		PROPERTY_CHANNEL_OPACITY = _( "Opacity" );
		PROPERTY_CHANNEL_SPECULAR = _( "Specular" );
		PROPERTY_CHANNEL_HEIGHT = _( "Height" );
		PROPERTY_CHANNEL_AMBIENT = _( "Ambient" );
		PROPERTY_CHANNEL_GLOSS = _( "Gloss" );

		SetBackgroundColour( PANEL_BACKGROUND_COLOUR );
		SetForegroundColour( PANEL_FOREGROUND_COLOUR );
		wxSize l_size = GetClientSize();
		l_size.y -= 30;
		m_material = std::make_shared< Material >( *m_engine, cuT( "NewMaterial" ) );

		m_properties = new wxPropertyGrid( this, wxID_ANY, wxDefaultPosition, l_size, wxPG_SPLITTER_AUTO_CENTER | wxPG_DEFAULT_STYLE );
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

		GradientButton * l_ok = new GradientButton( this, wxID_OK, _( "OK" ), wxPoint( 20, l_size.y + 5 ), wxSize( 60, 25 ), wxBORDER_NONE );
		GradientButton * l_cancel = new GradientButton( this, wxID_CANCEL, _( "Cancel" ), wxPoint( 120, l_size.y + 5 ), wxSize( 60, 25 ), wxBORDER_NONE );

		DoCreateProperties();

		wxBoxSizer * l_btnsSizer = new wxBoxSizer( wxHORIZONTAL );
		l_btnsSizer->Add( l_ok );
		l_btnsSizer->AddStretchSpacer( 1 );
		l_btnsSizer->Add( l_cancel );

		wxBoxSizer * l_sizer = new wxBoxSizer( wxVERTICAL );
		l_sizer->Add( m_properties, wxSizerFlags( 1 ).Expand().Border( wxALL, 5 ) );
		l_sizer->Add( l_btnsSizer, wxSizerFlags( 0 ).Expand().Border( wxALL, 5 ) );
		SetSizer( l_sizer );
		l_sizer->SetSizeHints( this );

		Connect( wxEVT_PG_CHANGED, wxEVENT_HANDLER_CAST( wxPropertyGridEventFunction, NewMaterialDialog::OnPropertyChange ) );
	}

	NewMaterialDialog::~NewMaterialDialog()
	{
	}

	void NewMaterialDialog::DoCreateProperties()
	{
		wxPGChoices l_choices;
		l_choices.Add( wxEmptyString );
		l_choices.Add( wxCOMBO_NEW );
		m_properties->Append( new wxPropertyCategory( PROPERTY_CATEGORY_MATERIAL ) );
		m_properties->Append( new wxStringProperty( PROPERTY_MATERIAL_NAME, wxPG_LABEL, PROPERTY_MATERIAL_NAME_DEFAULT_VALUE ) );
		m_properties->Append( new wxEnumProperty( PROPERTY_MATERIAL_PASSES, wxPG_LABEL, l_choices ) );
		DoCreatePassProperties();
	}

	void NewMaterialDialog::DoCreatePassProperties()
	{
		wxPGChoices l_choices;
		l_choices.Add( wxEmptyString );
		l_choices.Add( wxCOMBO_NEW );
		m_properties->Append( new wxPropertyCategory( PROPERTY_CATEGORY_PASS ) )->Hide( true );
		m_properties->Append( new wxColourProperty( PROPERTY_PASS_DIFFUSE ) )->Hide( true );
		m_properties->Append( new wxColourProperty( PROPERTY_PASS_AMBIENT ) )->Hide( true );
		m_properties->Append( new wxColourProperty( PROPERTY_PASS_SPECULAR ) )->Hide( true );
		m_properties->Append( new wxColourProperty( PROPERTY_PASS_EMISSIVE ) )->Hide( true );
		m_properties->Append( new wxFloatProperty( PROPERTY_PASS_EXPONENT ) )->Hide( true );
		m_properties->Append( CreateProperty( PROPERTY_PASS_TWO_SIDED, true, true ) )->Hide( true );
		m_properties->Append( new wxFloatProperty( PROPERTY_PASS_OPACITY ) )->Hide( true );
		m_properties->Append( CreateProperty( PROPERTY_PASS_SHADER, PROPERTY_PASS_EDIT_SHADER, static_cast< ButtonEventMethod >( &NewMaterialDialog::OnEditShader ), this, m_editor ) )->Hide( true );
		m_properties->Append( new wxEnumProperty( PROPERTY_PASS_TEXTURES, wxPG_LABEL, l_choices ) )->Hide( true );
		DoCreateTextureProperties();
	}

	void NewMaterialDialog::DoCreateTextureProperties()
	{
		wxPGChoices l_choices;
		l_choices.Add( PROPERTY_CHANNEL_COLOUR );
		l_choices.Add( PROPERTY_CHANNEL_DIFFUSE );
		l_choices.Add( PROPERTY_CHANNEL_NORMAL );
		l_choices.Add( PROPERTY_CHANNEL_OPACITY );
		l_choices.Add( PROPERTY_CHANNEL_SPECULAR );
		l_choices.Add( PROPERTY_CHANNEL_HEIGHT );
		l_choices.Add( PROPERTY_CHANNEL_AMBIENT );
		l_choices.Add( PROPERTY_CHANNEL_GLOSS );
		m_properties->Append( new wxPropertyCategory( PROPERTY_CATEGORY_TEXTURE ) )->Hide( true );
		m_properties->Append( new wxEnumProperty( PROPERTY_CHANNEL, PROPERTY_CHANNEL, l_choices ) )->Hide( true );
		m_properties->Append( new wxImageFileProperty( PROPERTY_TEXTURE_IMAGE ) )->Hide( true );
	}

	void NewMaterialDialog::OnMaterialNameChange( Castor::String const & p_value )
	{
		m_material->ChangeName( p_value );
	}

	void NewMaterialDialog::OnPassChange( Castor::String const & p_value )
	{
		wxString l_value = p_value;

		if ( p_value != wxCOMBO_NEW )
		{
			m_pass = m_material->GetPass( string::to_int( p_value ) );
			m_properties->SetPropertyValue( PROPERTY_PASS_DIFFUSE, wxVariant( wxColour( m_pass->GetAmbient().to_bgr() ) ) );
			m_properties->SetPropertyValue( PROPERTY_PASS_AMBIENT, wxVariant( wxColour( m_pass->GetDiffuse().to_bgr() ) ) );
			m_properties->SetPropertyValue( PROPERTY_PASS_SPECULAR, wxVariant( wxColour( m_pass->GetSpecular().to_bgr() ) ) );
			m_properties->SetPropertyValue( PROPERTY_PASS_EMISSIVE, wxVariant( wxColour( m_pass->GetEmissive().to_bgr() ) ) );
			m_properties->SetPropertyValue( PROPERTY_PASS_EXPONENT, m_pass->GetShininess() );
			m_properties->SetPropertyValue( PROPERTY_PASS_TWO_SIDED, m_pass->IsTwoSided() );
			m_properties->SetPropertyValue( PROPERTY_PASS_OPACITY, m_pass->GetAlpha() );
		}
		else
		{
			m_pass = m_material->CreatePass();
			m_properties->SetPropertyValue( PROPERTY_PASS_DIFFUSE, wxVariant( *wxBLACK ) );
			m_properties->SetPropertyValue( PROPERTY_PASS_AMBIENT, wxVariant( *wxWHITE ) );
			m_properties->SetPropertyValue( PROPERTY_PASS_SPECULAR, wxVariant( *wxWHITE ) );
			m_properties->SetPropertyValue( PROPERTY_PASS_EMISSIVE, wxVariant( *wxBLACK ) );
			m_properties->SetPropertyValue( PROPERTY_PASS_EXPONENT, 50.0 );
			m_properties->SetPropertyValue( PROPERTY_PASS_TWO_SIDED, false );
			m_properties->SetPropertyValue( PROPERTY_PASS_OPACITY, 1.0 );

			wxPGChoices l_choices;
			l_choices.Add( wxCOMBO_NEW );

			for ( size_t i = 0; i < m_material->GetPassCount(); ++i )
			{
				l_value = wxString() << i;
				l_choices.Add( l_value );
			}

			reinterpret_cast< wxEnumProperty * >( m_properties->GetProperty( PROPERTY_MATERIAL_PASSES ) )->SetChoices( l_choices );
			m_properties->SetPropertyValue( PROPERTY_MATERIAL_PASSES, l_value );
		}

		wxPGChoices l_choices;
		l_choices.Add( wxEmptyString );
		l_choices.Add( wxCOMBO_NEW );

		for ( size_t i = 0; i < m_pass->GetTextureUnitsCount(); ++i )
		{
			l_choices.Add( wxString() << i );
		}

		reinterpret_cast< wxEnumProperty * >( m_properties->GetProperty( PROPERTY_PASS_TEXTURES ) )->SetChoices( l_choices );
		m_properties->SetPropertyValue( PROPERTY_PASS_TEXTURES, wxEmptyString );
		m_properties->SetPropertyLabel( PROPERTY_CATEGORY_PASS, PROPERTY_CATEGORY_PASS + l_value );

		m_properties->HideProperty( PROPERTY_CATEGORY_TEXTURE, true );
		m_properties->HideProperty( PROPERTY_CHANNEL, true );
		m_properties->HideProperty( PROPERTY_TEXTURE_IMAGE, true );

		m_properties->HideProperty( PROPERTY_CATEGORY_PASS, false );
		m_properties->HideProperty( PROPERTY_PASS_DIFFUSE, false );
		m_properties->HideProperty( PROPERTY_PASS_AMBIENT, false );
		m_properties->HideProperty( PROPERTY_PASS_SPECULAR, false );
		m_properties->HideProperty( PROPERTY_PASS_EMISSIVE, false );
		m_properties->HideProperty( PROPERTY_PASS_EXPONENT, false );
		m_properties->HideProperty( PROPERTY_PASS_TWO_SIDED, false );
		m_properties->HideProperty( PROPERTY_PASS_OPACITY, false );
		m_properties->HideProperty( PROPERTY_PASS_TEXTURES, false );
	}

	void NewMaterialDialog::OnAmbientColourChange( Colour const & p_value )
	{
		m_pass->SetAmbient( p_value );
	}

	void NewMaterialDialog::OnDiffuseColourChange( Colour const & p_value )
	{
		m_pass->SetDiffuse( p_value );
	}

	void NewMaterialDialog::OnSpecularColourChange( Colour const & p_value )
	{
		m_pass->SetSpecular( p_value );
	}

	void NewMaterialDialog::OnEmissiveColourChange( Colour const & p_value )
	{
		m_pass->SetEmissive( p_value );
	}

	void NewMaterialDialog::OnExponentChange( double p_value )
	{
		m_pass->SetShininess( float( p_value ) );
	}

	void NewMaterialDialog::OnTwoSidedChange( bool p_value )
	{
		m_pass->SetTwoSided( p_value );
	}

	void NewMaterialDialog::OnOpacityChange( double p_value )
	{
		m_pass->SetAlpha( float( p_value ) );
	}

	bool NewMaterialDialog::OnEditShader( wxPGProperty * p_property )
	{
		ShaderDialog * l_editor = new ShaderDialog( m_pass->GetOwner(), true, NULL, m_pass );
		l_editor->Show();
		return false;
	}

	void NewMaterialDialog::OnTextureChange( String const & p_value )
	{
		wxString l_value = p_value;

		if ( p_value != wxCOMBO_NEW )
		{
			m_texture = m_pass->GetTextureUnit( string::to_int( p_value ) );
			wxString l_selected;

			switch ( m_texture->GetChannel() )
			{
			case eTEXTURE_CHANNEL_COLOUR:
				l_selected = PROPERTY_CHANNEL_COLOUR;
				break;

			case eTEXTURE_CHANNEL_DIFFUSE:
				l_selected = PROPERTY_CHANNEL_DIFFUSE;
				break;

			case eTEXTURE_CHANNEL_NORMAL:
				l_selected = PROPERTY_CHANNEL_NORMAL;
				break;

			case eTEXTURE_CHANNEL_OPACITY:
				l_selected = PROPERTY_CHANNEL_OPACITY;
				break;

			case eTEXTURE_CHANNEL_SPECULAR:
				l_selected = PROPERTY_CHANNEL_SPECULAR;
				break;

			case eTEXTURE_CHANNEL_HEIGHT:
				l_selected = PROPERTY_CHANNEL_HEIGHT;
				break;

			case eTEXTURE_CHANNEL_AMBIENT:
				l_selected = PROPERTY_CHANNEL_AMBIENT;
				break;

			case eTEXTURE_CHANNEL_GLOSS:
				l_selected = PROPERTY_CHANNEL_GLOSS;
				break;
			}

			m_properties->SetPropertyValue( PROPERTY_CHANNEL, l_selected );
			m_properties->SetPropertyValue( PROPERTY_TEXTURE_IMAGE, make_wxString( m_texture->GetTexturePath() ) );
		}
		else
		{
			m_texture = m_pass->AddTextureUnit();
			m_properties->SetPropertyValue( PROPERTY_CHANNEL, PROPERTY_CHANNEL_DIFFUSE );
			m_properties->SetPropertyValue( PROPERTY_TEXTURE_IMAGE, wxString() );

			wxPGChoices l_choices;
			l_choices.Add( wxCOMBO_NEW );

			for ( size_t i = 0; i < m_pass->GetTextureUnitsCount(); ++i )
			{
				l_value = wxString() << i;
				l_choices.Add( l_value );
			}

			reinterpret_cast< wxEnumProperty * >( m_properties->GetProperty( PROPERTY_PASS_TEXTURES ) )->SetChoices( l_choices );
			m_properties->SetPropertyValue( PROPERTY_PASS_TEXTURES, l_value );
		}
		
		m_properties->SetPropertyLabel( PROPERTY_CATEGORY_TEXTURE, PROPERTY_CATEGORY_TEXTURE + l_value );
		m_properties->HideProperty( PROPERTY_CATEGORY_TEXTURE, false );
		m_properties->HideProperty( PROPERTY_CHANNEL, false );
		m_properties->HideProperty( PROPERTY_TEXTURE_IMAGE, false );
	}

	void NewMaterialDialog::OnChannelChange( eTEXTURE_CHANNEL p_value )
	{
		m_texture->SetChannel( p_value );
	}

	void NewMaterialDialog::OnImageChange( Castor::String const & p_value )
	{
		if ( File::FileExists( p_value ) )
		{
			// Absolute path
			m_texture->SetAutoMipmaps( true );
			m_texture->LoadTexture( p_value );
			m_texture->Initialise();
		}
	}

	BEGIN_EVENT_TABLE( NewMaterialDialog, wxDialog )
	EVT_CLOSE( NewMaterialDialog::OnClose )
	EVT_BUTTON( wxID_OK, NewMaterialDialog::OnOk )
	EVT_BUTTON( wxID_CANCEL, NewMaterialDialog::OnCancel )
	END_EVENT_TABLE()

	void NewMaterialDialog::OnClose( wxCloseEvent & WXUNUSED( p_event ) )
	{
		EndModal( wxID_CANCEL );
	}

	void NewMaterialDialog::OnOk( wxCommandEvent & WXUNUSED( p_event ) )
	{
		m_engine->GetMaterialManager().insert( m_material->GetName(), m_material );
		m_engine->PostEvent( MakeInitialiseEvent( *m_material ) );
		EndModal( wxID_OK );
	}

	void NewMaterialDialog::OnCancel( wxCommandEvent & WXUNUSED( p_event ) )
	{
		EndModal( wxID_CANCEL );
	}

	void NewMaterialDialog::OnPropertyChange( wxPropertyGridEvent & p_event )
	{
		wxPGProperty * l_property = p_event.GetProperty();

		if ( l_property )
		{
			if ( l_property->GetName() == PROPERTY_MATERIAL_NAME )
			{
				OnMaterialNameChange( make_String( l_property->GetValueAsString() ) );
			}
			else if ( l_property->GetName() == PROPERTY_MATERIAL_PASSES )
			{
				OnPassChange( make_String( l_property->GetValueAsString() ) );
			}
			else if ( m_pass )
			{
				wxColour l_colour;

				if ( l_property->GetName() == PROPERTY_PASS_TEXTURES )
				{
					OnTextureChange( make_String( l_property->GetValueAsString() ) );
				}
				else if ( l_property->GetName() == PROPERTY_PASS_DIFFUSE )
				{
					l_colour << l_property->GetValue();
					OnAmbientColourChange( Colour::from_bgr( l_colour.GetRGB() ) );
				}
				else if ( l_property->GetName() == PROPERTY_PASS_AMBIENT )
				{
					l_colour << l_property->GetValue();
					OnDiffuseColourChange( Colour::from_bgr( l_colour.GetRGB() ) );
				}
				else if ( l_property->GetName() == PROPERTY_PASS_SPECULAR )
				{
					l_colour << l_property->GetValue();
					OnSpecularColourChange( Colour::from_bgr( l_colour.GetRGB() ) );
				}
				else if ( l_property->GetName() == PROPERTY_PASS_EMISSIVE )
				{
					l_colour << l_property->GetValue();
					OnEmissiveColourChange( Colour::from_bgr( l_colour.GetRGB() ) );
				}
				else if ( l_property->GetName() == PROPERTY_PASS_EXPONENT )
				{
					OnExponentChange( l_property->GetValue() );
				}
				else if ( l_property->GetName() == PROPERTY_PASS_TWO_SIDED )
				{
					OnTwoSidedChange( l_property->GetValue() );
				}
				else if ( l_property->GetName() == PROPERTY_PASS_OPACITY )
				{
					OnOpacityChange( l_property->GetValue() );
				}
				else if ( m_texture )
				{
					if ( l_property->GetName() == PROPERTY_CHANNEL )
					{
						if ( l_property->GetValueAsString() == PROPERTY_CHANNEL_COLOUR )
						{
							OnChannelChange( eTEXTURE_CHANNEL_COLOUR );
						}

						if ( l_property->GetValueAsString() == PROPERTY_CHANNEL_DIFFUSE )
						{
							OnChannelChange( eTEXTURE_CHANNEL_DIFFUSE );
						}

						if ( l_property->GetValueAsString() == PROPERTY_CHANNEL_NORMAL )
						{
							OnChannelChange( eTEXTURE_CHANNEL_NORMAL );
						}

						if ( l_property->GetValueAsString() == PROPERTY_CHANNEL_OPACITY )
						{
							OnChannelChange( eTEXTURE_CHANNEL_OPACITY );
						}

						if ( l_property->GetValueAsString() == PROPERTY_CHANNEL_SPECULAR )
						{
							OnChannelChange( eTEXTURE_CHANNEL_SPECULAR );
						}

						if ( l_property->GetValueAsString() == PROPERTY_CHANNEL_HEIGHT )
						{
							OnChannelChange( eTEXTURE_CHANNEL_HEIGHT );
						}

						if ( l_property->GetValueAsString() == PROPERTY_CHANNEL_AMBIENT )
						{
							OnChannelChange( eTEXTURE_CHANNEL_AMBIENT );
						}

						if ( l_property->GetValueAsString() == PROPERTY_CHANNEL_GLOSS )
						{
							OnChannelChange( eTEXTURE_CHANNEL_GLOSS );
						}
					}
					else if ( l_property->GetName() == PROPERTY_TEXTURE_IMAGE )
					{
						OnImageChange( String( l_property->GetValueAsString() ) );
					}
				}
			}
		}
	}
}
