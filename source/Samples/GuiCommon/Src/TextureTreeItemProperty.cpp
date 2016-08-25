#include "TextureTreeItemProperty.hpp"

#include <Engine.hpp>

#include <Event/Frame/FunctorEvent.hpp>
#include <Render/RenderSystem.hpp>
#include <Texture/TextureLayout.hpp>
#include <Texture/TextureUnit.hpp>

#include "AdditionalProperties.hpp"
#include <wx/propgrid/advprops.h>

using namespace Castor3D;
using namespace Castor;

namespace GuiCommon
{
	namespace
	{
		static wxString PROPERTY_CATEGORY_TEXTURE = _( "Texture" );
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
		static wxString PROPERTY_CHANNEL_EMISSIVE = _( "Emissive" );
	}

	TextureTreeItemProperty::TextureTreeItemProperty( bool p_editable, TextureUnitSPtr p_texture )
		: TreeItemProperty( p_texture->GetEngine(), p_editable, ePROPERTY_DATA_TYPE_TEXTURE )
		, m_texture( p_texture )
	{
		PROPERTY_CATEGORY_TEXTURE = _( "Texture" );
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
		PROPERTY_CHANNEL_EMISSIVE = _( "Emissive" );

		CreateTreeItemMenu();
	}

	TextureTreeItemProperty::~TextureTreeItemProperty()
	{
	}

	void TextureTreeItemProperty::DoCreateProperties( wxPGEditor * p_editor, wxPropertyGrid * p_grid )
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
			case TextureChannel::Colour:
				l_selected = PROPERTY_CHANNEL_COLOUR;
				break;

			case TextureChannel::Diffuse:
				l_selected = PROPERTY_CHANNEL_DIFFUSE;
				break;

			case TextureChannel::Normal:
				l_selected = PROPERTY_CHANNEL_NORMAL;
				break;

			case TextureChannel::Opacity:
				l_selected = PROPERTY_CHANNEL_OPACITY;
				break;

			case TextureChannel::Specular:
				l_selected = PROPERTY_CHANNEL_SPECULAR;
				break;

			case TextureChannel::Emissive:
				l_selected = PROPERTY_CHANNEL_EMISSIVE;
				break;

			case TextureChannel::Height:
				l_selected = PROPERTY_CHANNEL_HEIGHT;
				break;

			case TextureChannel::Ambient:
				l_selected = PROPERTY_CHANNEL_AMBIENT;
				break;

			case TextureChannel::Gloss:
				l_selected = PROPERTY_CHANNEL_GLOSS;
				break;
			}

			p_grid->Append( new wxPropertyCategory( PROPERTY_CATEGORY_TEXTURE ) );
			p_grid->Append( new wxEnumProperty( PROPERTY_CHANNEL, PROPERTY_CHANNEL, l_choices ) )->SetValue( l_selected );

			if ( l_unit->GetTexture()->GetImage().IsStaticSource() )
			{
				Path l_path{ l_unit->GetTexture()->GetImage().ToString() };
				p_grid->Append( new wxImageFileProperty( PROPERTY_TEXTURE_IMAGE ) )->SetValue( l_path );
			}
		}
	}

	void TextureTreeItemProperty::DoPropertyChange( wxPropertyGridEvent & p_event )
	{
		TextureUnitSPtr l_unit = GetTexture();
		wxPGProperty * l_property = p_event.GetProperty();

		if ( l_property && l_unit )
		{
			if ( l_property->GetName() == PROPERTY_CHANNEL )
			{
				if ( l_property->GetValueAsString() == PROPERTY_CHANNEL_COLOUR )
				{
					OnChannelChange( TextureChannel::Colour );
				}

				if ( l_property->GetValueAsString() == PROPERTY_CHANNEL_DIFFUSE )
				{
					OnChannelChange( TextureChannel::Diffuse );
				}

				if ( l_property->GetValueAsString() == PROPERTY_CHANNEL_NORMAL )
				{
					OnChannelChange( TextureChannel::Normal );
				}

				if ( l_property->GetValueAsString() == PROPERTY_CHANNEL_OPACITY )
				{
					OnChannelChange( TextureChannel::Opacity );
				}

				if ( l_property->GetValueAsString() == PROPERTY_CHANNEL_SPECULAR )
				{
					OnChannelChange( TextureChannel::Specular );
				}

				if ( l_property->GetValueAsString() == PROPERTY_CHANNEL_EMISSIVE )
				{
					OnChannelChange( TextureChannel::Emissive );
				}

				if ( l_property->GetValueAsString() == PROPERTY_CHANNEL_HEIGHT )
				{
					OnChannelChange( TextureChannel::Height );
				}

				if ( l_property->GetValueAsString() == PROPERTY_CHANNEL_AMBIENT )
				{
					OnChannelChange( TextureChannel::Ambient );
				}

				if ( l_property->GetValueAsString() == PROPERTY_CHANNEL_GLOSS )
				{
					OnChannelChange( TextureChannel::Gloss );
				}
			}
			else if ( l_property->GetName() == PROPERTY_TEXTURE_IMAGE )
			{
				OnImageChange( String( l_property->GetValueAsString() ) );
			}
		}
	}

	void TextureTreeItemProperty::OnChannelChange( TextureChannel p_value )
	{
		TextureUnitSPtr l_unit = GetTexture();

		DoApplyChange( [p_value, l_unit]()
		{
			l_unit->SetChannel( p_value );
		} );
	}

	void TextureTreeItemProperty::OnImageChange( Castor::String const & p_value )
	{
		TextureUnitSPtr l_unit = GetTexture();

		DoApplyChange( [p_value, l_unit]()
		{
			if ( File::FileExists( Path{ p_value } ) )
			{
				// Absolute path
				l_unit->SetAutoMipmaps( true );
				auto l_texture = l_unit->GetEngine()->GetRenderSystem()->CreateTexture( TextureType::TwoDimensions, AccessType::Read, AccessType::Read );
				l_texture->GetImage().SetSource( Path{}, Path{ p_value } );
				l_unit->SetTexture( l_texture );
				l_unit->Initialise();
			}
		} );
	}
}
