#include "GuiCommon/Properties/TreeItems/TextureTreeItemProperty.hpp"

#include "GuiCommon/Properties/AdditionalProperties.hpp"

#include <Castor3D/Engine.hpp>
#include <Castor3D/Render/RenderSystem.hpp>
#include <Castor3D/Material/Texture/TextureLayout.hpp>
#include <Castor3D/Material/Texture/TextureUnit.hpp>
#include <Castor3D/Material/Texture/Animation/TextureAnimation.hpp>

#include <wx/propgrid/advprops.h>

using namespace castor3d;
using namespace castor;

namespace GuiCommon
{
	namespace
	{
		uint32_t getComponent( castor::Point2ui const & flag )
		{
			for ( uint32_t i = 0u; i < 4u; ++i )
			{
				if ( ( flag[0] >> ( i * 8u ) ) & 0x01 )
				{
					return i;
				}
			}

			return 0u;
		}

		uint32_t getMask( bool enabled
			, long component
			, uint32_t componentsCount )
		{
			uint32_t result = 0u;

			if ( enabled )
			{
				if ( componentsCount == 1u )
				{
					switch ( component )
					{
					case 0:
						result = 0x000000FF;
						break;
					case 1:
						result = 0x0000FF00;
						break;
					case 2:
						result = 0x00FF0000;
						break;
					case 3:
						result = 0xFF000000;
						break;
					}
				}
				else
				{
					switch ( component )
					{
					case 0:
						result = 0x00FFFFFF;
						break;
					case 1:
						result = 0xFFFFFF00;
						break;
					}
				}
			}

			return result;
		}
	}

	TextureTreeItemProperty::TextureTreeItemProperty( bool editable
		, TextureUnitSPtr texture
		, MaterialType type )
		: TreeItemProperty( texture->getEngine(), editable )
		, m_texture{ texture }
		, m_configuration{ texture->getConfiguration()}
		, m_materialType{ type }
	{
		CreateTreeItemMenu();
	}

	TextureTreeItemProperty::~TextureTreeItemProperty()
	{
	}

	void TextureTreeItemProperty::doAddProperty( wxPropertyGrid * grid
		, wxString const & flagName
		, wxString const & isName
		, wxString const & compName
		, castor3d::TextureFlag flag
		, castor::Point2ui & mask
		, uint32_t componentsCount
		, PropertyChangeHandler handler )
	{
		static wxString PROPERTY_COMPONENT_R = wxT( "R" );
		static wxString PROPERTY_COMPONENT_G = wxT( "G" );
		static wxString PROPERTY_COMPONENT_B = wxT( "B" );
		static wxString PROPERTY_COMPONENT_A = wxT( "A" );
		static wxString PROPERTY_COMPONENT_RGB = wxT( "RGB" );
		static wxString PROPERTY_COMPONENT_GBA = wxT( "GBA" );

		grid->Append( new wxPropertyCategory( flagName ) );
		addProperty( grid, flagName );
		auto isProp = addProperty( grid
			, isName
			, mask[0] != 0
			, handler );
		wxString name;
		wxString selected;
		wxArrayString choices;

		if ( componentsCount == 1u )
		{
			choices.Add( PROPERTY_COMPONENT_R );
			choices.Add( PROPERTY_COMPONENT_G );
			choices.Add( PROPERTY_COMPONENT_B );
			choices.Add( PROPERTY_COMPONENT_A );
			selected = choices[getComponent( mask )];
		}
		else
		{
			choices.Add( PROPERTY_COMPONENT_RGB );
			choices.Add( PROPERTY_COMPONENT_GBA );
			selected = choices[getComponent( mask )];
		}

		auto compProp = addProperty( grid
			, compName
			, choices
			, selected
			, handler );
		compProp->Enable( mask[0] != 0 );
		m_properties.emplace( flag, PropertyPair{ isProp, compProp, mask } );
	}

	void TextureTreeItemProperty::doCreateProperties( wxPGEditor * editor, wxPropertyGrid * grid )
	{
		static wxString CATEGORY_TEXTURE = _( "Texture" );
		static wxString PROPERTY_TEXTURE_IMAGE = _( "Image" );
		static wxString PROPERTY_FLAG_DIFFUSE = _( "Diffuse" );
		static wxString PROPERTY_FLAG_SPECULAR = _( "Specular" );
		static wxString PROPERTY_FLAG_METALLIC = _( "Metallic" );
		static wxString PROPERTY_FLAG_ALBEDO = _( "Albedo" );
		static wxString PROPERTY_FLAG_SHININESS = _( "Shininess" );
		static wxString PROPERTY_FLAG_GLOSSINESS = _( "Glossiness" );
		static wxString PROPERTY_FLAG_ROUGHNESS = _( "Roughness" );
		static wxString PROPERTY_FLAG_NORMAL = _( "Normal" );
		static wxString PROPERTY_FLAG_OPACITY = _( "Opacity" );
		static wxString PROPERTY_FLAG_HEIGHT = _( "Height" );
		static wxString PROPERTY_FLAG_EMISSIVE = _( "Emissive" );
		static wxString PROPERTY_FLAG_OCCLUSION = _( "Occlusion" );
		static wxString PROPERTY_FLAG_TRANSMITTANCE = _( "Transmittance" );
		static wxString PROPERTY_IS_DIFFUSE = _( "Diffuse Map" );
		static wxString PROPERTY_IS_ALBEDO = _( "Albedo Map" );
		static wxString PROPERTY_IS_SPECULAR = _( "Specular Map" );
		static wxString PROPERTY_IS_METALLIC = _( "Metallic Map" );
		static wxString PROPERTY_IS_SHININESS = _( "Shininess Map" );
		static wxString PROPERTY_IS_GLOSSINESS = _( "Glossiness Map" );
		static wxString PROPERTY_IS_ROUGHNESS = _( "Roughness Map" );
		static wxString PROPERTY_IS_NORMAL = _( "Normal Map" );
		static wxString PROPERTY_IS_OPACITY = _( "Opacity Map" );
		static wxString PROPERTY_IS_HEIGHT = _( "Height Map" );
		static wxString PROPERTY_IS_EMISSIVE = _( "Emissive Map" );
		static wxString PROPERTY_IS_OCCLUSION = _( "Occlusion Map" );
		static wxString PROPERTY_IS_TRANSMITTANCE = _( "Transmittance Map" );
		static wxString PROPERTY_COMP_DIFFUSE = _( "Diffuse Components" );
		static wxString PROPERTY_COMP_ALBEDO = _( "Albedo Components" );
		static wxString PROPERTY_COMP_SPECULAR = _( "Specular Components" );
		static wxString PROPERTY_COMP_METALLIC = _( "Metallic Component" );
		static wxString PROPERTY_COMP_SHININESS = _( "Shininess Component" );
		static wxString PROPERTY_COMP_GLOSSINESS = _( "Glossiness Component" );
		static wxString PROPERTY_COMP_ROUGHNESS = _( "Roughness Component" );
		static wxString PROPERTY_COMP_NORMAL = _( "Normal Components" );
		static wxString PROPERTY_COMP_OPACITY = _( "Opacity Component" );
		static wxString PROPERTY_COMP_HEIGHT = _( "Height Component" );
		static wxString PROPERTY_COMP_EMISSIVE = _( "Emissive Components" );
		static wxString PROPERTY_COMP_OCCLUSION = _( "Occlusion Component" );
		static wxString PROPERTY_COMP_TRANSMITTANCE = _( "Transmittance Component" );
		static wxString PROPERTY_COMPONENT = _( "Component" );
		static wxString PROPERTY_COMPONENTS = _( "Components" );
		static wxString PROPERTY_FACTOR_NORMAL = _( "Normal Factor" );
		static wxString PROPERTY_FACTOR_HEIGHT = _( "Height Factor" );
		static wxString PROPERTY_DIRECTX_NORMAL = _( "Normal DirectX" );

		TextureUnitSPtr unit = getTexture();

		if ( unit )
		{
			grid->Append( new wxPropertyCategory( CATEGORY_TEXTURE ) );

			if ( unit->getTexture()->isStatic() )
			{
				addProperty( grid, PROPERTY_TEXTURE_IMAGE, Path{ unit->getTexture()->getPath() }
					, [this]( wxVariant const & var )
					{
						onImageChange( var );
					} );
			}

			if ( m_materialType == MaterialType::ePhong )
			{
				doAddProperty( grid, PROPERTY_FLAG_DIFFUSE, PROPERTY_IS_DIFFUSE, PROPERTY_COMP_DIFFUSE, TextureFlag::eDiffuse, m_configuration.colourMask, 3u
					, [this]( wxVariant const & var ){ onChange( var, TextureFlag::eDiffuse, 3u ); } );
				doAddProperty( grid, PROPERTY_FLAG_SPECULAR, PROPERTY_IS_SPECULAR, PROPERTY_COMP_SPECULAR, TextureFlag::eSpecular, m_configuration.specularMask, 3u
					, [this]( wxVariant const & var ){ onChange( var, TextureFlag::eSpecular, 3u ); } );
				doAddProperty( grid, PROPERTY_FLAG_SHININESS, PROPERTY_IS_SHININESS, PROPERTY_COMP_SHININESS, TextureFlag::eShininess, m_configuration.glossinessMask, 1u
					, [this]( wxVariant const & var ){ onChange( var, TextureFlag::eShininess, 1u ); } );
			}
			else if ( m_materialType == MaterialType::eSpecularGlossiness)
			{
				doAddProperty( grid, PROPERTY_FLAG_DIFFUSE, PROPERTY_IS_ALBEDO, PROPERTY_COMP_ALBEDO, TextureFlag::eAlbedo, m_configuration.colourMask, 3u
					, [this]( wxVariant const & var ){ onChange( var, TextureFlag::eAlbedo, 3u ); } );
				doAddProperty( grid, PROPERTY_FLAG_SPECULAR, PROPERTY_IS_SPECULAR, PROPERTY_COMP_SPECULAR, TextureFlag::eSpecular, m_configuration.specularMask, 3u
					, [this]( wxVariant const & var ){ onChange( var, TextureFlag::eSpecular, 3u ); } );
				doAddProperty( grid, PROPERTY_FLAG_GLOSSINESS, PROPERTY_IS_GLOSSINESS, PROPERTY_COMP_GLOSSINESS, TextureFlag::eGlossiness, m_configuration.glossinessMask, 1u
					, [this]( wxVariant const & var ){ onChange( var, TextureFlag::eGlossiness, 1u ); } );
			}
			else
			{
				doAddProperty( grid, PROPERTY_FLAG_ALBEDO, PROPERTY_IS_ALBEDO, PROPERTY_COMP_ALBEDO, TextureFlag::eAlbedo, m_configuration.colourMask, 3u
					, [this]( wxVariant const & var ){ onChange( var, TextureFlag::eAlbedo, 3u ); } );
				doAddProperty( grid, PROPERTY_FLAG_METALLIC, PROPERTY_IS_METALLIC, PROPERTY_COMP_METALLIC, TextureFlag::eMetalness, m_configuration.specularMask, 1u
					, [this]( wxVariant const & var ){ onChange( var, TextureFlag::eMetalness, 1u ); } );
				doAddProperty( grid, PROPERTY_FLAG_ROUGHNESS, PROPERTY_IS_ROUGHNESS, PROPERTY_COMP_ROUGHNESS, TextureFlag::eRoughness, m_configuration.glossinessMask, 1u
					, [this]( wxVariant const & var ){ onChange( var, TextureFlag::eRoughness, 1u ); } );
			}

			doAddProperty( grid, PROPERTY_FLAG_EMISSIVE, PROPERTY_IS_EMISSIVE, PROPERTY_COMP_EMISSIVE, TextureFlag::eEmissive, m_configuration.emissiveMask, 3u
					, [this]( wxVariant const & var ){ onChange( var, TextureFlag::eEmissive, 3u ); } );
			doAddProperty( grid, PROPERTY_FLAG_OPACITY, PROPERTY_IS_OPACITY, PROPERTY_COMP_OPACITY, TextureFlag::eOpacity, m_configuration.opacityMask, 1u
					, [this]( wxVariant const & var ){ onChange( var, TextureFlag::eOpacity, 1u ); } );
			doAddProperty( grid, PROPERTY_FLAG_OCCLUSION, PROPERTY_IS_OCCLUSION, PROPERTY_COMP_OCCLUSION, TextureFlag::eOcclusion, m_configuration.occlusionMask, 1u
					, [this]( wxVariant const & var ){ onChange( var, TextureFlag::eOcclusion, 1u ); } );
			doAddProperty( grid, PROPERTY_FLAG_TRANSMITTANCE, PROPERTY_IS_TRANSMITTANCE, PROPERTY_COMP_TRANSMITTANCE, TextureFlag::eTransmittance, m_configuration.transmittanceMask, 1u
					, [this]( wxVariant const & var ){ onChange( var, TextureFlag::eTransmittance, 1u ); } );
			doAddProperty( grid, PROPERTY_FLAG_NORMAL, PROPERTY_IS_NORMAL, PROPERTY_COMP_NORMAL, TextureFlag::eNormal, m_configuration.normalMask, 3u
					, [this]( wxVariant const & var ){ onChange( var, TextureFlag::eNormal, 3u ); } );
			addPropertyT( grid, PROPERTY_FACTOR_NORMAL, &m_configuration.normalFactor );
			addProperty( grid, PROPERTY_DIRECTX_NORMAL, m_configuration.normalGMultiplier != 1.0f
				, [this]( wxVariant const & var ){ m_configuration.normalGMultiplier = var.GetBool() ? -1.0f : 1.0f; } );
			doAddProperty( grid, PROPERTY_FLAG_HEIGHT, PROPERTY_IS_HEIGHT, PROPERTY_COMP_HEIGHT, TextureFlag::eHeight, m_configuration.heightMask, 1u 
					, [this]( wxVariant const & var ){ onChange( var, TextureFlag::eHeight, 1u ); } );
			addPropertyT( grid, PROPERTY_FACTOR_HEIGHT, &m_configuration.heightFactor );

			if ( unit->hasAnimation() )
			{
				static wxString CATEGORY_ANIMATION = _( "Animation" );
				static wxString PROPERTY_ANIMATION_TRANSLATE = _( "Translate" );
				static wxString PROPERTY_ANIMATION_ROTATE = _( "Rotate" );
				static wxString PROPERTY_ANIMATION_SCALE = _( "Scale" );

				auto & anim = unit->getAnimation();
				grid->Append( new wxPropertyCategory( CATEGORY_ANIMATION ) );
				addPropertyT( grid, PROPERTY_ANIMATION_TRANSLATE, anim.getTranslateSpeed(), &anim, &TextureAnimation::setTranslateSpeed );
				addPropertyT( grid, PROPERTY_ANIMATION_ROTATE, anim.getRotateSpeed(), &anim, &TextureAnimation::setRotateSpeed );
				addPropertyT( grid, PROPERTY_ANIMATION_SCALE, anim.getScaleSpeed(), &anim, &TextureAnimation::setScaleSpeed );
			}
		}
	}

	void TextureTreeItemProperty::onChange( wxVariant const & var
		, TextureFlag flag
		, uint32_t componentsCount )
	{
		TextureUnitSPtr unit = getTexture();
		auto it = m_properties.find( flag );
		CU_Require( it != m_properties.end() );
		bool isMap = it->second.isMap->GetValue();
		long components = it->second.components->GetValue();
		it->second.components->Enable( isMap );
		it->second.mask[0] = getMask( isMap, components, componentsCount );
		unit->setConfiguration( m_configuration );
	}

	void TextureTreeItemProperty::onImageChange( wxVariant const & var )
	{
		TextureUnitSPtr unit = getTexture();
		Path path{ make_String( var.GetString() ) };

		// Absolute path
		if ( File::fileExists( path ) )
		{
			ashes::ImageCreateInfo image
			{
				0u,
				VK_IMAGE_TYPE_2D,
				VK_FORMAT_R32_SFLOAT,
				{ 1u, 1u, 1u },
				1u,
				1u,
				VK_SAMPLE_COUNT_1_BIT,
				VK_IMAGE_TILING_OPTIMAL,
				( VK_IMAGE_USAGE_SAMPLED_BIT
					| VK_IMAGE_USAGE_TRANSFER_DST_BIT ),
			};
			auto & device = unit->getDevice();
			auto texture = std::make_shared< TextureLayout >( *unit->getEngine()->getRenderSystem()
				, image
				, VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT
				, path );
			texture->setSource( Path{}, path );
			unit->setTexture( texture );

			unit->initialise( device );
		}
	}
}
