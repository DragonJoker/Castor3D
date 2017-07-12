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
		: TreeItemProperty( p_texture->GetEngine(), p_editable, ePROPERTY_DATA_TYPE_TEXTURE )
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

	void TextureTreeItemProperty::DoCreateProperties( wxPGEditor * p_editor, wxPropertyGrid * p_grid )
	{
		TextureUnitSPtr l_unit = GetTexture();

		if ( l_unit )
		{
			wxPGChoices l_choices;
			wxString l_selected;

			if ( m_materialType == MaterialType::eLegacy )
			{
				l_choices.Add( PROPERTY_CHANNEL_DIFFUSE );
				l_choices.Add( PROPERTY_CHANNEL_NORMAL );
				l_choices.Add( PROPERTY_CHANNEL_OPACITY );
				l_choices.Add( PROPERTY_CHANNEL_SPECULAR );
				l_choices.Add( PROPERTY_CHANNEL_EMISSIVE );
				l_choices.Add( PROPERTY_CHANNEL_HEIGHT );
				l_choices.Add( PROPERTY_CHANNEL_GLOSS );
				l_choices.Add( PROPERTY_CHANNEL_REFLECTION );
				l_choices.Add( PROPERTY_CHANNEL_REFRACTION );

				switch ( l_unit->GetChannel() )
				{
				case TextureChannel::eDiffuse:
					l_selected = PROPERTY_CHANNEL_DIFFUSE;
					break;

				case TextureChannel::eNormal:
					l_selected = PROPERTY_CHANNEL_NORMAL;
					break;

				case TextureChannel::eOpacity:
					l_selected = PROPERTY_CHANNEL_OPACITY;
					break;

				case TextureChannel::eSpecular:
					l_selected = PROPERTY_CHANNEL_SPECULAR;
					break;

				case TextureChannel::eEmissive:
					l_selected = PROPERTY_CHANNEL_EMISSIVE;
					break;

				case TextureChannel::eHeight:
					l_selected = PROPERTY_CHANNEL_HEIGHT;
					break;

				case TextureChannel::eGloss:
					l_selected = PROPERTY_CHANNEL_GLOSS;
					break;

				case TextureChannel::eReflection:
					l_selected = PROPERTY_CHANNEL_REFLECTION;
					break;

				case TextureChannel::eRefraction:
					l_selected = PROPERTY_CHANNEL_REFRACTION;
					break;
				}
			}
			else
			{
				l_choices.Add( PROPERTY_CHANNEL_ALBEDO );
				l_choices.Add( PROPERTY_CHANNEL_NORMAL );
				l_choices.Add( PROPERTY_CHANNEL_OPACITY );
				l_choices.Add( PROPERTY_CHANNEL_ROUGHNESS );
				l_choices.Add( PROPERTY_CHANNEL_EMISSIVE );
				l_choices.Add( PROPERTY_CHANNEL_HEIGHT );
				l_choices.Add( PROPERTY_CHANNEL_METALLIC );
				l_choices.Add( PROPERTY_CHANNEL_REFLECTION );
				l_choices.Add( PROPERTY_CHANNEL_REFRACTION );

				switch ( l_unit->GetChannel() )
				{
				case TextureChannel::eDiffuse:
					l_selected = PROPERTY_CHANNEL_ALBEDO;
					break;

				case TextureChannel::eNormal:
					l_selected = PROPERTY_CHANNEL_NORMAL;
					break;

				case TextureChannel::eOpacity:
					l_selected = PROPERTY_CHANNEL_OPACITY;
					break;

				case TextureChannel::eSpecular:
					l_selected = PROPERTY_CHANNEL_ROUGHNESS;
					break;

				case TextureChannel::eEmissive:
					l_selected = PROPERTY_CHANNEL_EMISSIVE;
					break;

				case TextureChannel::eHeight:
					l_selected = PROPERTY_CHANNEL_HEIGHT;
					break;

				case TextureChannel::eGloss:
					l_selected = PROPERTY_CHANNEL_METALLIC;
					break;

				case TextureChannel::eReflection:
					l_selected = PROPERTY_CHANNEL_REFLECTION;
					break;

				case TextureChannel::eRefraction:
					l_selected = PROPERTY_CHANNEL_REFRACTION;
					break;
				}
			}

			p_grid->Append( new wxPropertyCategory( PROPERTY_CATEGORY_TEXTURE ) );
			p_grid->Append( new wxEnumProperty( PROPERTY_CHANNEL, PROPERTY_CHANNEL, l_choices ) )->SetValue( l_selected );

			if ( l_unit->GetChannel() != TextureChannel::eReflection
				&& l_unit->GetChannel() != TextureChannel::eRefraction )
			{
				if ( l_unit->GetTexture()->GetImage().IsStaticSource() )
				{
					Path l_path{ l_unit->GetTexture()->GetImage().ToString() };
					p_grid->Append( new wxImageFileProperty( PROPERTY_TEXTURE_IMAGE ) )->SetValue( l_path );
				}
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
				if ( l_property->GetValueAsString() == PROPERTY_CHANNEL_DIFFUSE )
				{
					OnChannelChange( TextureChannel::eDiffuse );
				}

				if ( l_property->GetValueAsString() == PROPERTY_CHANNEL_NORMAL )
				{
					OnChannelChange( TextureChannel::eNormal );
				}

				if ( l_property->GetValueAsString() == PROPERTY_CHANNEL_OPACITY )
				{
					OnChannelChange( TextureChannel::eOpacity );
				}

				if ( l_property->GetValueAsString() == PROPERTY_CHANNEL_SPECULAR )
				{
					OnChannelChange( TextureChannel::eSpecular );
				}

				if ( l_property->GetValueAsString() == PROPERTY_CHANNEL_EMISSIVE )
				{
					OnChannelChange( TextureChannel::eEmissive );
				}

				if ( l_property->GetValueAsString() == PROPERTY_CHANNEL_HEIGHT )
				{
					OnChannelChange( TextureChannel::eHeight );
				}

				if ( l_property->GetValueAsString() == PROPERTY_CHANNEL_GLOSS )
				{
					OnChannelChange( TextureChannel::eGloss );
				}

				if ( l_property->GetValueAsString() == PROPERTY_CHANNEL_REFLECTION )
				{
					OnChannelChange( TextureChannel::eReflection );
				}

				if ( l_property->GetValueAsString() == PROPERTY_CHANNEL_REFRACTION )
				{
					OnChannelChange( TextureChannel::eRefraction );
				}

				if ( l_property->GetValueAsString() == PROPERTY_CHANNEL_ALBEDO )
				{
					OnChannelChange( TextureChannel::eAlbedo );
				}

				if ( l_property->GetValueAsString() == PROPERTY_CHANNEL_ROUGHNESS )
				{
					OnChannelChange( TextureChannel::eRoughness );
				}

				if ( l_property->GetValueAsString() == PROPERTY_CHANNEL_METALLIC )
				{
					OnChannelChange( TextureChannel::eMetallic );
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
				auto l_texture = l_unit->GetEngine()->GetRenderSystem()->CreateTexture( TextureType::eTwoDimensions
					, AccessType::eRead
					, AccessType::eRead );
				l_texture->GetImage().InitialiseSource( Path{}, Path{ p_value } );
				l_unit->SetTexture( l_texture );
				l_unit->Initialise();
			}
		} );
	}
}
