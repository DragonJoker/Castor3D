#include "GuiCommon/TextureTreeItemProperty.hpp"

#include "GuiCommon/AdditionalProperties.hpp"

#include <Castor3D/Engine.hpp>
#include <Castor3D/Event/Frame/FunctorEvent.hpp>
#include <Castor3D/Render/RenderSystem.hpp>
#include <Castor3D/Material/Texture/TextureLayout.hpp>
#include <Castor3D/Material/Texture/TextureUnit.hpp>

#include <wx/propgrid/advprops.h>

using namespace castor3d;
using namespace castor;

namespace GuiCommon
{
	namespace
	{
		static wxString PROPERTY_CATEGORY_TEXTURE = _( "Texture" );
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
		static wxString PROPERTY_COMPONENT_R = wxT( "R" );
		static wxString PROPERTY_COMPONENT_G = wxT( "G" );
		static wxString PROPERTY_COMPONENT_B = wxT( "B" );
		static wxString PROPERTY_COMPONENT_A = wxT( "A" );
		static wxString PROPERTY_COMPONENT_RGB = wxT( "RGB" );
		static wxString PROPERTY_COMPONENT_GBA = wxT( "GBA" );

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
		: TreeItemProperty( texture->getEngine(), editable, ePROPERTY_DATA_TYPE_TEXTURE )
		, m_texture{ texture }
		, m_configuration{ texture->getConfiguration()}
		, m_materialType{ type }
	{
		CU_Ensure( m_configuration.environment == 0u );
		PROPERTY_CATEGORY_TEXTURE = _( "Texture" );
		PROPERTY_TEXTURE_IMAGE = _( "Image" );
		PROPERTY_FLAG_DIFFUSE = _( "Diffuse" );
		PROPERTY_FLAG_SPECULAR = _( "Specular" );
		PROPERTY_FLAG_METALLIC = _( "Metallic" );
		PROPERTY_FLAG_ALBEDO = _( "Albedo" );
		PROPERTY_FLAG_SHININESS = _( "Shininess" );
		PROPERTY_FLAG_GLOSSINESS = _( "Glossiness" );
		PROPERTY_FLAG_ROUGHNESS = _( "Roughness" );
		PROPERTY_FLAG_NORMAL = _( "Normal" );
		PROPERTY_FLAG_OPACITY = _( "Opacity" );
		PROPERTY_FLAG_HEIGHT = _( "Height" );
		PROPERTY_FLAG_EMISSIVE = _( "Emissive" );
		PROPERTY_FLAG_OCCLUSION = _( "Occlusion" );
		PROPERTY_FLAG_TRANSMITTANCE = _( "Transmittance" );
		PROPERTY_IS_DIFFUSE = _( "Diffuse Map" );
		PROPERTY_IS_ALBEDO = _( "Albedo Map" );
		PROPERTY_IS_SPECULAR = _( "Specular Map" );
		PROPERTY_IS_METALLIC = _( "Metallic Map" );
		PROPERTY_IS_SHININESS = _( "Shininess Map" );
		PROPERTY_IS_GLOSSINESS = _( "Glossiness Map" );
		PROPERTY_IS_ROUGHNESS = _( "Roughness Map" );
		PROPERTY_IS_NORMAL = _( "Normal Map" );
		PROPERTY_IS_OPACITY = _( "Opacity Map" );
		PROPERTY_IS_HEIGHT = _( "Height Map" );
		PROPERTY_IS_EMISSIVE = _( "Emissive Map" );
		PROPERTY_IS_OCCLUSION = _( "Occlusion Map" );
		PROPERTY_IS_TRANSMITTANCE = _( "Transmittance Map" );
		PROPERTY_COMP_DIFFUSE = _( "Diffuse Components" );
		PROPERTY_COMP_ALBEDO = _( "Albedo Components" );
		PROPERTY_COMP_SPECULAR = _( "Specular Components" );
		PROPERTY_COMP_METALLIC = _( "Metallic Component" );
		PROPERTY_COMP_SHININESS = _( "Shininess Component" );
		PROPERTY_COMP_GLOSSINESS = _( "Glossiness Component" );
		PROPERTY_COMP_ROUGHNESS = _( "Roughness Component" );
		PROPERTY_COMP_NORMAL = _( "Normal Components" );
		PROPERTY_COMP_OPACITY = _( "Opacity Component" );
		PROPERTY_COMP_HEIGHT = _( "Height Component" );
		PROPERTY_COMP_EMISSIVE = _( "Emissive Components" );
		PROPERTY_COMP_OCCLUSION = _( "Occlusion Component" );
		PROPERTY_COMP_TRANSMITTANCE = _( "Transmittance Component" );
		PROPERTY_COMPONENT = _( "Component" );
		PROPERTY_COMPONENTS = _( "Components" );
		PROPERTY_FACTOR_NORMAL = _( "Normal Factor" );
		PROPERTY_FACTOR_HEIGHT = _( "Height Factor" );
		PROPERTY_DIRECTX_NORMAL = _( "Normal DirectX" );
		PROPERTY_COMPONENT_R = wxT( "R" );
		PROPERTY_COMPONENT_G = wxT( "G" );
		PROPERTY_COMPONENT_B = wxT( "B" );
		PROPERTY_COMPONENT_A = wxT( "A" );
		PROPERTY_COMPONENT_RGB = wxT( "RGB" );
		PROPERTY_COMPONENT_GBA = wxT( "GBA" );
		CreateTreeItemMenu();
	}

	TextureTreeItemProperty::~TextureTreeItemProperty()
	{
	}

	void TextureTreeItemProperty::addProperty( wxPropertyGrid * grid
		, wxString const & flagName
		, wxString const & isName
		, wxString const & compName
		, castor3d::TextureFlag flag
		, castor::Point2ui & mask
		, uint32_t componentsCount )
	{
		grid->Append( new wxPropertyCategory( flagName ) );
		auto isProp = grid->Append( new wxBoolProperty( isName, isName ) );
		isProp->SetValue( mask[0] != 0 );
		wxString name;
		wxString selected;
		wxPGChoices choices;

		if ( componentsCount == 1u )
		{
			choices.Add( PROPERTY_COMPONENT_R );
			choices.Add( PROPERTY_COMPONENT_G );
			choices.Add( PROPERTY_COMPONENT_B );
			choices.Add( PROPERTY_COMPONENT_A );
			selected = choices.GetLabels()[getComponent( mask )];
		}
		else
		{
			choices.Add( PROPERTY_COMPONENT_RGB );
			choices.Add( PROPERTY_COMPONENT_GBA );
			selected = choices.GetLabels()[getComponent( mask )];
		}

		auto compProp = grid->Append( new wxEnumProperty( compName, compName, choices ) );
		compProp->SetValue( selected );
		compProp->Enable( mask[0] != 0 );
		m_properties.emplace( flag, PropertyPair{ isProp, compProp, mask } );
	}

	void TextureTreeItemProperty::doCreateProperties( wxPGEditor * editor, wxPropertyGrid * grid )
	{
		TextureUnitSPtr unit = getTexture();

		if ( unit )
		{
			grid->Append( new wxPropertyCategory( PROPERTY_CATEGORY_TEXTURE ) );

			if ( m_materialType == MaterialType::ePhong )
			{
				addProperty( grid, PROPERTY_FLAG_DIFFUSE, PROPERTY_IS_DIFFUSE, PROPERTY_COMP_DIFFUSE, TextureFlag::eDiffuse, m_configuration.colourMask, 3u );
				addProperty( grid, PROPERTY_FLAG_SPECULAR, PROPERTY_IS_SPECULAR, PROPERTY_COMP_SPECULAR, TextureFlag::eSpecular, m_configuration.specularMask, 3u );
				addProperty( grid, PROPERTY_FLAG_SHININESS, PROPERTY_IS_SHININESS, PROPERTY_COMP_SHININESS, TextureFlag::eShininess, m_configuration.glossinessMask, 1u );
			}
			else if ( m_materialType == MaterialType::eSpecularGlossiness)
			{
				addProperty( grid, PROPERTY_FLAG_DIFFUSE, PROPERTY_IS_ALBEDO, PROPERTY_COMP_ALBEDO, TextureFlag::eAlbedo, m_configuration.colourMask, 3u );
				addProperty( grid, PROPERTY_FLAG_SPECULAR, PROPERTY_IS_SPECULAR, PROPERTY_COMP_SPECULAR, TextureFlag::eSpecular, m_configuration.specularMask, 3u );
				addProperty( grid, PROPERTY_FLAG_GLOSSINESS, PROPERTY_IS_GLOSSINESS, PROPERTY_COMP_GLOSSINESS, TextureFlag::eGlossiness, m_configuration.glossinessMask, 1u );
			}
			else
			{
				addProperty( grid, PROPERTY_FLAG_ALBEDO, PROPERTY_IS_ALBEDO, PROPERTY_COMP_ALBEDO, TextureFlag::eAlbedo, m_configuration.colourMask, 3u );
				addProperty( grid, PROPERTY_FLAG_METALLIC, PROPERTY_IS_METALLIC, PROPERTY_COMP_METALLIC, TextureFlag::eMetalness, m_configuration.specularMask, 1u );
				addProperty( grid, PROPERTY_FLAG_ROUGHNESS, PROPERTY_IS_ROUGHNESS, PROPERTY_COMP_ROUGHNESS, TextureFlag::eRoughness, m_configuration.glossinessMask, 1u );
			}

			addProperty( grid, PROPERTY_FLAG_EMISSIVE, PROPERTY_IS_EMISSIVE, PROPERTY_COMP_EMISSIVE, TextureFlag::eEmissive, m_configuration.emissiveMask, 3u );
			addProperty( grid, PROPERTY_FLAG_OPACITY, PROPERTY_IS_OPACITY, PROPERTY_COMP_OPACITY, TextureFlag::eOpacity, m_configuration.opacityMask, 1u );
			addProperty( grid, PROPERTY_FLAG_OCCLUSION, PROPERTY_IS_OCCLUSION, PROPERTY_COMP_OCCLUSION, TextureFlag::eOcclusion, m_configuration.occlusionMask, 1u );
			addProperty( grid, PROPERTY_FLAG_TRANSMITTANCE, PROPERTY_IS_TRANSMITTANCE, PROPERTY_COMP_TRANSMITTANCE, TextureFlag::eTransmittance, m_configuration.transmittanceMask, 1u );
			addProperty( grid, PROPERTY_FLAG_NORMAL, PROPERTY_IS_NORMAL, PROPERTY_COMP_NORMAL, TextureFlag::eNormal, m_configuration.normalMask, 3u );
			grid->Append( new wxFloatProperty( PROPERTY_FACTOR_NORMAL, PROPERTY_FACTOR_NORMAL ) )->SetValue( m_configuration.normalFactor );
			grid->Append( new wxBoolProperty( PROPERTY_DIRECTX_NORMAL, PROPERTY_DIRECTX_NORMAL ) )->SetValue( m_configuration.normalGMultiplier != 1.0f );
			addProperty( grid, PROPERTY_FLAG_HEIGHT, PROPERTY_IS_HEIGHT, PROPERTY_COMP_HEIGHT, TextureFlag::eHeight, m_configuration.heightMask, 1u );
			grid->Append( new wxFloatProperty( PROPERTY_FACTOR_HEIGHT, PROPERTY_FACTOR_HEIGHT ) )->SetValue( m_configuration.heightFactor );

			if ( unit->getTexture()->getDefaultImage().isStaticSource() )
			{
				Path path{ unit->getTexture()->getDefaultImage().toString() };
				grid->Append( new wxImageFileProperty( PROPERTY_TEXTURE_IMAGE ) )->SetValue( path );
			}
		}
	}

	void TextureTreeItemProperty::doPropertyChange( wxPropertyGridEvent & event )
	{
		TextureUnitSPtr unit = getTexture();
		wxPGProperty * property = event.GetProperty();

		if ( property && unit )
		{
			auto propertyName = property->GetLabel();

			if ( propertyName == PROPERTY_IS_DIFFUSE
				|| propertyName == PROPERTY_COMP_DIFFUSE )
			{
				onChange( TextureFlag::eDiffuse, 3u );
			}
			else if ( propertyName == PROPERTY_IS_ALBEDO
				|| propertyName == PROPERTY_COMP_ALBEDO )
			{
				onChange( TextureFlag::eAlbedo, 3u );
			}
			else if ( propertyName == PROPERTY_IS_SPECULAR
				|| propertyName == PROPERTY_COMP_SPECULAR )
			{
				onChange( TextureFlag::eSpecular, 3u );
			}
			else if ( propertyName == PROPERTY_IS_METALLIC
				|| propertyName == PROPERTY_COMP_METALLIC )
			{
				onChange( TextureFlag::eMetalness, 1u );
			}
			else if ( propertyName == PROPERTY_IS_SHININESS
				|| propertyName == PROPERTY_COMP_SHININESS )
			{
				onChange( TextureFlag::eShininess, 1u );
			}
			else if ( propertyName == PROPERTY_IS_GLOSSINESS
				|| propertyName == PROPERTY_COMP_GLOSSINESS )
			{
				onChange( TextureFlag::eGlossiness, 1u );
			}
			else if ( propertyName == PROPERTY_IS_ROUGHNESS
				|| propertyName == PROPERTY_COMP_ROUGHNESS )
			{
				onChange( TextureFlag::eRoughness, 1u );
			}
			else if ( propertyName == PROPERTY_IS_EMISSIVE
				|| propertyName == PROPERTY_COMP_EMISSIVE )
			{
				onChange( TextureFlag::eEmissive, 3u );
			}
			else if ( propertyName == PROPERTY_IS_OPACITY
				|| propertyName == PROPERTY_COMP_OPACITY )
			{
				onChange( TextureFlag::eOpacity, 1u );
			}
			else if ( propertyName == PROPERTY_IS_OCCLUSION
				|| propertyName == PROPERTY_COMP_OCCLUSION )
			{
				onChange( TextureFlag::eOcclusion, 1u );
			}
			else if ( propertyName == PROPERTY_IS_TRANSMITTANCE
				|| propertyName == PROPERTY_COMP_TRANSMITTANCE )
			{
				onChange( TextureFlag::eTransmittance, 1u );
			}
			else if ( propertyName == PROPERTY_IS_NORMAL
				|| propertyName == PROPERTY_COMP_NORMAL )
			{
				onChange( TextureFlag::eNormal, 3u );
			}
			else if ( propertyName == PROPERTY_IS_HEIGHT
				|| propertyName == PROPERTY_COMP_HEIGHT )
			{
				onChange( TextureFlag::eHeight, 1u );
			}
			else if ( propertyName == PROPERTY_FACTOR_NORMAL )
			{
				onNormalFactorChange( property->GetValue() );
			}
			else if ( propertyName == PROPERTY_DIRECTX_NORMAL )
			{
				onNormalDirectXChange( property->GetValue() );
			}
			else if ( propertyName == PROPERTY_FACTOR_HEIGHT )
			{
				onHeightFactorChange( property->GetValue() );
			}
			else if ( property->GetName() == PROPERTY_TEXTURE_IMAGE )
			{
				onImageChange( castor::Path( make_String( property->GetValueAsString() ) ) );
			}
		}
	}

	void TextureTreeItemProperty::onChange( TextureFlag flag
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

	void TextureTreeItemProperty::onNormalFactorChange( double value )
	{
		TextureUnitSPtr unit = getTexture();
		m_configuration.normalFactor = float( value );
		unit->setConfiguration( m_configuration );
	}

	void TextureTreeItemProperty::onHeightFactorChange( double value )
	{
		TextureUnitSPtr unit = getTexture();
		m_configuration.heightFactor = float( value );
		unit->setConfiguration( m_configuration );
	}

	void TextureTreeItemProperty::onNormalDirectXChange( bool value )
	{
		TextureUnitSPtr unit = getTexture();
		m_configuration.normalGMultiplier = value
			? -1.0f
			: 1.0f;
		unit->setConfiguration( m_configuration );
	}

	void TextureTreeItemProperty::onImageChange( castor::Path const & value )
	{
		TextureUnitSPtr unit = getTexture();

		doApplyChange( [value, unit]()
		{
			Path path{ value };

			if ( File::fileExists( path ) )
			{
				// Absolute path
				unit->setAutoMipmaps( true );
				
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
				auto texture = std::make_shared< TextureLayout >( *unit->getEngine()->getRenderSystem()
					, image
					, VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT
					, path );
				texture->getImage().initialiseSource( Path{}, path );
				unit->setTexture( texture );
				unit->initialise();
			}
		} );
	}
}
