#include "TextureTreeItemProperty.hpp"

#include <Engine.hpp>

#include <Event/Frame/FunctorEvent.hpp>
#include <Render/RenderSystem.hpp>
#include <Texture/TextureLayout.hpp>
#include <Texture/TextureUnit.hpp>

#include "AdditionalProperties.hpp"
#include <wx/propgrid/advprops.h>

using namespace castor3d;
using namespace castor;

namespace GuiCommon
{
	namespace
	{
		static wxString PROPERTY_CATEGORY_TEXTURE = _( "Texture" );
		static wxString PROPERTY_TEXTURE_IMAGE = _( "Image" );
		static wxString PROPERTY_CHANNEL = _( "Channel" );
		static wxString PROPERTY_CHANNEL_DIFFUSE = _( "Diffuse" );
		static wxString PROPERTY_CHANNEL_NORMAL = _( "Normal" );
		static wxString PROPERTY_CHANNEL_OPACITY = _( "Opacity" );
		static wxString PROPERTY_CHANNEL_SPECULAR = _( "Specular" );
		static wxString PROPERTY_CHANNEL_HEIGHT = _( "Height" );
		static wxString PROPERTY_CHANNEL_AMBIENT = _( "Ambient" );
		static wxString PROPERTY_CHANNEL_GLOSS = _( "Gloss" );
		static wxString PROPERTY_CHANNEL_EMISSIVE = _( "Emissive" );
		static wxString PROPERTY_CHANNEL_REFLECTION = _( "Reflection" );
		static wxString PROPERTY_CHANNEL_REFRACTION = _( "Refraction" );
		static wxString PROPERTY_CHANNEL_ALBEDO = _( "Albedo" );
		static wxString PROPERTY_CHANNEL_ROUGHNESS = _( "Roughness" );
		static wxString PROPERTY_CHANNEL_METALLIC = _( "Metallic" );
	}

	TextureTreeItemProperty::TextureTreeItemProperty( bool p_editable
		, TextureUnitSPtr p_texture
		, MaterialType p_type )
		: TreeItemProperty( p_texture->getEngine(), p_editable, ePROPERTY_DATA_TYPE_TEXTURE )
		, m_texture{ p_texture }
		, m_materialType{ p_type }
	{
		PROPERTY_CATEGORY_TEXTURE = _( "Texture" );
		PROPERTY_TEXTURE_IMAGE = _( "Image" );
		PROPERTY_CHANNEL = _( "Channel" );
		PROPERTY_CHANNEL_DIFFUSE = _( "Diffuse" );
		PROPERTY_CHANNEL_NORMAL = _( "Normal" );
		PROPERTY_CHANNEL_OPACITY = _( "Opacity" );
		PROPERTY_CHANNEL_SPECULAR = _( "Specular" );
		PROPERTY_CHANNEL_HEIGHT = _( "Height" );
		PROPERTY_CHANNEL_GLOSS = _( "Gloss" );
		PROPERTY_CHANNEL_EMISSIVE = _( "Emissive" );
		PROPERTY_CHANNEL_REFLECTION = _( "Reflection" );
		PROPERTY_CHANNEL_REFRACTION = _( "Refraction" );
		PROPERTY_CHANNEL_ALBEDO = _( "Albedo" );
		PROPERTY_CHANNEL_ROUGHNESS = _( "Roughness" );
		PROPERTY_CHANNEL_METALLIC = _( "Metallic" );

		CreateTreeItemMenu();
	}

	TextureTreeItemProperty::~TextureTreeItemProperty()
	{
	}

	void TextureTreeItemProperty::doCreateProperties( wxPGEditor * p_editor, wxPropertyGrid * p_grid )
	{
		TextureUnitSPtr unit = getTexture();

		if ( unit )
		{
			wxPGChoices choices;
			wxString selected;

			if ( m_materialType == MaterialType::eLegacy )
			{
				choices.Add( PROPERTY_CHANNEL_DIFFUSE );
				choices.Add( PROPERTY_CHANNEL_NORMAL );
				choices.Add( PROPERTY_CHANNEL_OPACITY );
				choices.Add( PROPERTY_CHANNEL_SPECULAR );
				choices.Add( PROPERTY_CHANNEL_EMISSIVE );
				choices.Add( PROPERTY_CHANNEL_HEIGHT );
				choices.Add( PROPERTY_CHANNEL_GLOSS );
				choices.Add( PROPERTY_CHANNEL_REFLECTION );
				choices.Add( PROPERTY_CHANNEL_REFRACTION );

				switch ( unit->getChannel() )
				{
				case TextureChannel::eDiffuse:
					selected = PROPERTY_CHANNEL_DIFFUSE;
					break;

				case TextureChannel::eNormal:
					selected = PROPERTY_CHANNEL_NORMAL;
					break;

				case TextureChannel::eOpacity:
					selected = PROPERTY_CHANNEL_OPACITY;
					break;

				case TextureChannel::eSpecular:
					selected = PROPERTY_CHANNEL_SPECULAR;
					break;

				case TextureChannel::eEmissive:
					selected = PROPERTY_CHANNEL_EMISSIVE;
					break;

				case TextureChannel::eHeight:
					selected = PROPERTY_CHANNEL_HEIGHT;
					break;

				case TextureChannel::eGloss:
					selected = PROPERTY_CHANNEL_GLOSS;
					break;

				case TextureChannel::eReflection:
					selected = PROPERTY_CHANNEL_REFLECTION;
					break;

				case TextureChannel::eRefraction:
					selected = PROPERTY_CHANNEL_REFRACTION;
					break;
				}
			}
			else
			{
				choices.Add( PROPERTY_CHANNEL_ALBEDO );
				choices.Add( PROPERTY_CHANNEL_NORMAL );
				choices.Add( PROPERTY_CHANNEL_OPACITY );
				choices.Add( PROPERTY_CHANNEL_ROUGHNESS );
				choices.Add( PROPERTY_CHANNEL_EMISSIVE );
				choices.Add( PROPERTY_CHANNEL_HEIGHT );
				choices.Add( PROPERTY_CHANNEL_METALLIC );
				choices.Add( PROPERTY_CHANNEL_REFLECTION );
				choices.Add( PROPERTY_CHANNEL_REFRACTION );

				switch ( unit->getChannel() )
				{
				case TextureChannel::eDiffuse:
					selected = PROPERTY_CHANNEL_ALBEDO;
					break;

				case TextureChannel::eNormal:
					selected = PROPERTY_CHANNEL_NORMAL;
					break;

				case TextureChannel::eOpacity:
					selected = PROPERTY_CHANNEL_OPACITY;
					break;

				case TextureChannel::eSpecular:
					selected = PROPERTY_CHANNEL_ROUGHNESS;
					break;

				case TextureChannel::eEmissive:
					selected = PROPERTY_CHANNEL_EMISSIVE;
					break;

				case TextureChannel::eHeight:
					selected = PROPERTY_CHANNEL_HEIGHT;
					break;

				case TextureChannel::eGloss:
					selected = PROPERTY_CHANNEL_METALLIC;
					break;

				case TextureChannel::eReflection:
					selected = PROPERTY_CHANNEL_REFLECTION;
					break;

				case TextureChannel::eRefraction:
					selected = PROPERTY_CHANNEL_REFRACTION;
					break;
				}
			}

			p_grid->Append( new wxPropertyCategory( PROPERTY_CATEGORY_TEXTURE ) );
			p_grid->Append( new wxEnumProperty( PROPERTY_CHANNEL, PROPERTY_CHANNEL, choices ) )->SetValue( selected );

			if ( unit->getChannel() != TextureChannel::eReflection
				&& unit->getChannel() != TextureChannel::eRefraction )
			{
				if ( unit->getTexture()->getImage().isStaticSource() )
				{
					Path path{ unit->getTexture()->getImage().toString() };
					p_grid->Append( new wxImageFileProperty( PROPERTY_TEXTURE_IMAGE ) )->SetValue( path );
				}
			}
		}
	}

	void TextureTreeItemProperty::doPropertyChange( wxPropertyGridEvent & p_event )
	{
		TextureUnitSPtr unit = getTexture();
		wxPGProperty * property = p_event.GetProperty();

		if ( property && unit )
		{
			if ( property->GetName() == PROPERTY_CHANNEL )
			{
				if ( property->GetValueAsString() == PROPERTY_CHANNEL_DIFFUSE )
				{
					OnChannelChange( TextureChannel::eDiffuse );
				}

				if ( property->GetValueAsString() == PROPERTY_CHANNEL_NORMAL )
				{
					OnChannelChange( TextureChannel::eNormal );
				}

				if ( property->GetValueAsString() == PROPERTY_CHANNEL_OPACITY )
				{
					OnChannelChange( TextureChannel::eOpacity );
				}

				if ( property->GetValueAsString() == PROPERTY_CHANNEL_SPECULAR )
				{
					OnChannelChange( TextureChannel::eSpecular );
				}

				if ( property->GetValueAsString() == PROPERTY_CHANNEL_EMISSIVE )
				{
					OnChannelChange( TextureChannel::eEmissive );
				}

				if ( property->GetValueAsString() == PROPERTY_CHANNEL_HEIGHT )
				{
					OnChannelChange( TextureChannel::eHeight );
				}

				if ( property->GetValueAsString() == PROPERTY_CHANNEL_GLOSS )
				{
					OnChannelChange( TextureChannel::eGloss );
				}

				if ( property->GetValueAsString() == PROPERTY_CHANNEL_REFLECTION )
				{
					OnChannelChange( TextureChannel::eReflection );
				}

				if ( property->GetValueAsString() == PROPERTY_CHANNEL_REFRACTION )
				{
					OnChannelChange( TextureChannel::eRefraction );
				}

				if ( property->GetValueAsString() == PROPERTY_CHANNEL_ALBEDO )
				{
					OnChannelChange( TextureChannel::eAlbedo );
				}

				if ( property->GetValueAsString() == PROPERTY_CHANNEL_ROUGHNESS )
				{
					OnChannelChange( TextureChannel::eRoughness );
				}

				if ( property->GetValueAsString() == PROPERTY_CHANNEL_METALLIC )
				{
					OnChannelChange( TextureChannel::eMetallic );
				}
			}
			else if ( property->GetName() == PROPERTY_TEXTURE_IMAGE )
			{
				OnImageChange( String( property->GetValueAsString() ) );
			}
		}
	}

	void TextureTreeItemProperty::OnChannelChange( TextureChannel p_value )
	{
		TextureUnitSPtr unit = getTexture();

		doApplyChange( [p_value, unit]()
		{
			unit->setChannel( p_value );
		} );
	}

	void TextureTreeItemProperty::OnImageChange( castor::String const & p_value )
	{
		TextureUnitSPtr unit = getTexture();

		doApplyChange( [p_value, unit]()
		{
			if ( File::fileExists( Path{ p_value } ) )
			{
				// Absolute path
				unit->setAutoMipmaps( true );
				auto texture = unit->getEngine()->getRenderSystem()->createTexture( TextureType::eTwoDimensions
					, AccessType::eRead
					, AccessType::eRead );
				texture->getImage().initialiseSource( Path{}, Path{ p_value } );
				unit->setTexture( texture );
				unit->initialise();
			}
		} );
	}
}
