#include "TextureTreeItemProperty.hpp"

#include <TextureUnit.hpp>
#include <FunctorEvent.hpp>

#include "AdditionalProperties.hpp"
#include <wx/propgrid/advprops.h>

using namespace Castor3D;
using namespace Castor;

namespace GuiCommon
{
	namespace
	{
		static const wxString PROPERTY_CATEGORY_TEXTURE = _( "Texture" );
		static const wxString PROPERTY_TEXTURE_IMAGE = _( "Image" );
		static const wxString PROPERTY_CHANNEL = _( "Channel" );
		static const wxString PROPERTY_CHANNEL_COLOUR = _( "Colour" );
		static const wxString PROPERTY_CHANNEL_DIFFUSE = _( "Diffuse" );
		static const wxString PROPERTY_CHANNEL_NORMAL = _( "Normal" );
		static const wxString PROPERTY_CHANNEL_OPACITY = _( "Opacity" );
		static const wxString PROPERTY_CHANNEL_SPECULAR = _( "Specular" );
		static const wxString PROPERTY_CHANNEL_HEIGHT = _( "Height" );
		static const wxString PROPERTY_CHANNEL_AMBIENT = _( "Ambient" );
		static const wxString PROPERTY_CHANNEL_GLOSS = _( "Gloss" );
	}

	TextureTreeItemProperty::TextureTreeItemProperty( bool p_editable, TextureUnitSPtr p_texture )
		: TreeItemProperty( p_editable, ePROPERTY_DATA_TYPE_TEXTURE )
		, m_texture( p_texture )
	{
	}

	TextureTreeItemProperty::~TextureTreeItemProperty()
	{
	}

	void TextureTreeItemProperty::CreateProperties( wxPGEditor * p_editor, wxPropertyGrid * p_grid )
	{
		TextureUnitSPtr l_unit = GetTexture();

		if ( l_unit )
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
			wxString l_selected;

			switch ( l_unit->GetChannel() )
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

			p_grid->Append( new wxPropertyCategory( PROPERTY_CATEGORY_TEXTURE ) );
			p_grid->Append( new wxEnumProperty( PROPERTY_CHANNEL, PROPERTY_CHANNEL, l_choices ) )->SetValue( l_selected );

			if ( l_unit->GetTexture()->GetType() == eTEXTURE_TYPE_STATIC )
			{
				p_grid->Append( new wxImageFileProperty( PROPERTY_TEXTURE_IMAGE ) )->SetValue( l_unit->GetTexturePath() );
			}
		}
	}

	void TextureTreeItemProperty::OnPropertyChange( wxPropertyGridEvent & p_event )
	{
		TextureUnitSPtr l_unit = GetTexture();
		wxPGProperty * l_property = p_event.GetProperty();

		if ( l_property && l_unit )
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

	void TextureTreeItemProperty::OnChannelChange( eTEXTURE_CHANNEL p_value )
	{
		TextureUnitSPtr l_unit = GetTexture();

		l_unit->GetEngine()->PostEvent( MakeFunctorEvent( eEVENT_TYPE_PRE_RENDER, [p_value, l_unit]()
		{
			l_unit->SetChannel( p_value );
		} ) );
	}

	void TextureTreeItemProperty::OnImageChange( Castor::String const & p_value )
	{
		TextureUnitSPtr l_unit = GetTexture();

		l_unit->GetEngine()->PostEvent( MakeFunctorEvent( eEVENT_TYPE_PRE_RENDER, [p_value, l_unit]()
		{
			if ( File::FileExists( p_value ) )
			{
				// Absolute path
				l_unit->SetAutoMipmaps( true );
				l_unit->LoadTexture( p_value );
				l_unit->Initialise();
			}
		} ) );
	}
}
