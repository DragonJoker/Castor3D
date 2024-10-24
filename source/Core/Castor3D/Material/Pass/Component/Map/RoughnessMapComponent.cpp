#include "Castor3D/Material/Pass/Component/Map/RoughnessMapComponent.hpp"

#include "Castor3D/Engine.hpp"
#include "Castor3D/Material/Pass/Pass.hpp"
#include "Castor3D/Miscellaneous/ConfigurationVisitor.hpp"
#include "Castor3D/Material/Pass/Component/PassComponentRegister.hpp"
#include "Castor3D/Material/Pass/Component/Map/MetalnessMapComponent.hpp"
#include "Castor3D/Material/Pass/Component/Lighting/RoughnessComponent.hpp"
#include "Castor3D/Material/Texture/TextureConfiguration.hpp"
#include "Castor3D/Scene/SceneFileParserData.hpp"
#include "Castor3D/Scene/SceneImporter.hpp"
#include "Castor3D/Shader/ShaderBuffers/PassBuffer.hpp"
#include "Castor3D/Shader/Shaders/GlslBlendComponents.hpp"
#include "Castor3D/Shader/Shaders/GlslLighting.hpp"
#include "Castor3D/Shader/Shaders/GlslMaterial.hpp"
#include "Castor3D/Shader/Shaders/GlslTextureConfiguration.hpp"

#include <CastorUtils/FileParser/FileParser.hpp>

CU_ImplementSmartPtr( castor3d, RoughnessMapComponent )

namespace castor
{
	template<>
	class TextWriter< castor3d::RoughnessMapComponent >
		: public TextWriterT< castor3d::RoughnessMapComponent >
	{
	public:
		explicit TextWriter( String const & tabs
			, uint32_t mask )
			: TextWriterT< castor3d::RoughnessMapComponent >{ tabs }
			, m_mask{ mask }
		{
		}

		bool operator()( StringStream & file )const
		{
			return writeMask( file, cuT( "roughness_mask" ), m_mask );
		}

		bool operator()( castor3d::RoughnessMapComponent const & object
			, StringStream & file )override
		{
			return writeMask( file, cuT( "roughness_mask" ), m_mask );
		}

	private:
		uint32_t m_mask;
	};
}

namespace castor3d
{
	//*********************************************************************************************

	namespace rghcmp
	{
		static CU_ImplementAttributeParserBlock( parserUnitRoughnessMask, TextureContext )
		{
			if ( params.empty() )
			{
				CU_ParsingError( cuT( "Missing parameter." ) );
			}
			else if ( !blockContext->pass )
			{
				auto & plugin = getEngine( *blockContext )->getPassComponentsRegister().getPlugin( RoughnessMapComponent::TypeName );
				plugin.fillTextureConfiguration( blockContext->configuration
					, params[0]->get< uint32_t >() );
			}
			else
			{
				auto & plugin = blockContext->pass->pass->getComponentPlugin( RoughnessMapComponent::TypeName );
				plugin.fillTextureConfiguration( blockContext->configuration
					, params[0]->get< uint32_t >() );
			}
		}
		CU_EndAttribute()

		static CU_ImplementAttributeParserBlock( parserTexRemapRoughness, SceneImportContext )
		{
			auto & plugin = getEngine( *blockContext )->getPassComponentsRegister().getPlugin( RoughnessMapComponent::TypeName );
			blockContext->textureRemapIt = blockContext->textureRemaps.try_emplace( plugin.getTextureFlags() ).first;
			blockContext->textureRemapIt->second = TextureConfiguration{};
		}
		CU_EndAttributePushBlock( CSCNSection::eTextureRemapChannel, blockContext )

		static CU_ImplementAttributeParserBlock( parserTexRemapRoughnessMask, SceneImportContext )
		{
			if ( params.empty() )
			{
				CU_ParsingError( cuT( "Missing parameter." ) );
			}
			else
			{
				auto & plugin = getEngine( *blockContext )->getPassComponentsRegister().getPlugin( RoughnessMapComponent::TypeName );
				plugin.fillTextureConfiguration( blockContext->textureRemapIt->second
					, params[0]->get< uint32_t >() );
			}
		}
		CU_EndAttribute()
	}

	//*********************************************************************************************

	void RoughnessMapComponent::ComponentsShader::applyTexture( shader::PassShaders const & passShaders
		, shader::TextureConfigurations const & textureConfigs
		, shader::TextureAnimations const & textureAnims
		, sdw::Array< sdw::CombinedImage2DRgba32 > const & maps
		, shader::Material const & material
		, shader::BlendComponents & components
		, shader::SampleTexture const & sampleTexture )const
	{
		auto mbMapName = castor::toUtf8( cuT( "roughness" ) );
		auto mbValueName = castor::toUtf8( cuT( "roughness" ) );
		auto textureName = mbMapName + "MapAndMask";

		if ( !material.hasMember( textureName )
			|| !components.hasMember( mbValueName ) )
		{
			return;
		}

		auto & writer{ *material.getWriter() };
		auto map = writer.declLocale( mbMapName + "Map"
			, material.getMember< sdw::UInt >( textureName ) >> 16u );
		auto mask = writer.declLocale( mbMapName + "Mask"
			, material.getMember< sdw::UInt >( textureName ) & 0xFFFFu );
		auto value = components.getMember< sdw::Float >( mbValueName );
		auto roughnessMode = components.getMember< sdw::UInt32 >( "roughnessMode" );

		auto config = writer.declLocale( mbValueName + "Config"
			, textureConfigs.getTextureConfiguration( map ) );
		auto anim = writer.declLocale( mbValueName + "Anim"
			, textureAnims.getTextureAnimation( map ) );
		passShaders.computeTexcoords( textureConfigs
			, config
			, anim
			, components );
		auto sampled = writer.declLocale( mbValueName + "Sampled"
			, sampleTexture( map, config, components ) );

		IF( writer, roughnessMode != 0_u )
		{
			auto gloss = 1.0_f - value;
			value = 1.0_f - ( gloss * shader::TextureConfigData::getFloat( sampled, mask ) );
		}
		ELSE
		{
			value *= shader::TextureConfigData::getFloat( sampled, mask );
		}
		FI;
	}

	//*********************************************************************************************

	void RoughnessMapComponent::Plugin::createParsers( castor::AttributeParsers & parsers
		, ChannelFillers & channelFillers )const
	{
		channelFillers.try_emplace( cuT( "roughness" )
			, getTextureFlags()
			, []( TextureContext & blockContext )
			{
				auto const & component = getPassComponent< RoughnessMapComponent >( blockContext );
				component.fillChannel( blockContext.configuration
					, 0x00FF0000 );
			} );

		castor::addParserT( parsers
			, CSCNSection::eTexture
			, cuT( "roughness_mask" )
			, rghcmp::parserUnitRoughnessMask
			, { castor::makeParameter< castor::ParameterType::eUInt32 >() } );
		castor::addParserT( parsers
			, CSCNSection::eTexture
			, cuT( "glossiness_mask" )
			, rghcmp::parserUnitRoughnessMask
			, { castor::makeParameter< castor::ParameterType::eUInt32 >() } );
		castor::addParserT( parsers
			, CSCNSection::eTexture
			, cuT( "shininess_mask" )
			, rghcmp::parserUnitRoughnessMask
			, { castor::makeParameter< castor::ParameterType::eUInt32 >() } );

		castor::addParserT( parsers
			, CSCNSection::eTextureUnit
			, cuT( "roughness_mask" )
			, rghcmp::parserUnitRoughnessMask
			, { castor::makeParameter< castor::ParameterType::eUInt32 >() } );
		castor::addParserT( parsers
			, CSCNSection::eTextureUnit
			, cuT( "glossiness_mask" )
			, rghcmp::parserUnitRoughnessMask
			, { castor::makeParameter< castor::ParameterType::eUInt32 >() } );
		castor::addParserT( parsers
			, CSCNSection::eTextureUnit
			, cuT( "shininess_mask" )
			, rghcmp::parserUnitRoughnessMask
			, { castor::makeParameter< castor::ParameterType::eUInt32 >() } );

		castor::addParserT( parsers
			, CSCNSection::eTextureRemap
			, CSCNSection::eTextureRemapChannel
			, cuT( "roughness" )
			, rghcmp::parserTexRemapRoughness );
		castor::addParserT( parsers
			, CSCNSection::eTextureRemap
			, CSCNSection::eTextureRemapChannel
			, cuT( "glossiness" )
			, rghcmp::parserTexRemapRoughness );
		castor::addParserT( parsers
			, CSCNSection::eTextureRemap
			, CSCNSection::eTextureRemapChannel
			, cuT( "shininess" )
			, rghcmp::parserTexRemapRoughness );

		castor::addParserT( parsers
			, CSCNSection::eTextureRemapChannel
			, cuT( "roughness_mask" )
			, rghcmp::parserTexRemapRoughnessMask
			, { castor::makeParameter< castor::ParameterType::eUInt32 >() } );
		castor::addParserT( parsers
			, CSCNSection::eTextureRemapChannel
			, cuT( "glossiness_mask" )
			, rghcmp::parserTexRemapRoughnessMask
			, { castor::makeParameter< castor::ParameterType::eUInt32 >() } );
		castor::addParserT( parsers
			, CSCNSection::eTextureRemapChannel
			, cuT( "shininess_mask" )
			, rghcmp::parserTexRemapRoughnessMask
			, { castor::makeParameter< castor::ParameterType::eUInt32 >() } );
	}

	bool RoughnessMapComponent::Plugin::isComponentNeeded( TextureCombine const & textures
		, ComponentModeFlags const & filter )const
	{
		return checkFlag( filter, ComponentModeFlag::eDiffuseLighting )
			|| checkFlag( filter, ComponentModeFlag::eSpecularLighting )
			|| hasAny( textures, getTextureFlags() );
	}

	void RoughnessMapComponent::Plugin::createMapComponent( Pass & pass
		, castor::Vector< PassComponentUPtr > & result )const
	{
		result.push_back( castor::makeUniqueDerived< PassComponent, RoughnessMapComponent >( pass ) );
	}

	bool RoughnessMapComponent::Plugin::doWriteTextureConfig( TextureConfiguration const & configuration
		, uint32_t mask
		, castor::String const & tabs
		, castor::StringStream & file )const
	{
		return castor::TextWriter< RoughnessMapComponent >{ tabs, mask }( file );
	}

	//*********************************************************************************************

	castor::String const RoughnessMapComponent::TypeName = C3D_MakePassMapComponentName( "roughness" );

	RoughnessMapComponent::RoughnessMapComponent( Pass & pass )
		: PassMapComponent{ pass
			, TypeName
			, Roughness
			, { RoughnessComponent::TypeName } }
	{
	}

	PassComponentUPtr RoughnessMapComponent::doClone( Pass & pass )const
	{
		return castor::makeUniqueDerived< PassComponent, RoughnessMapComponent >( pass );
	}

	void RoughnessMapComponent::doFillConfig( TextureConfiguration & configuration
		, ConfigurationVisitorBase & vis )const
	{
		vis.visit( cuT( "Roughness" ) );
		vis.visit( cuT( "Map" ), getTextureFlags(), getFlagConfiguration( configuration, getTextureFlags() ), 1u );
	}

	PassMapDefaultImageParams RoughnessMapComponent::createDefaultImage( Engine & engine )const
	{
		castor::String name{ cuT( "DefaultRoughness" ) };
		castor::ByteArray data;
		data.resize( sizeof( float ) );
		float value{ 1.0f };
		std::memcpy( data.data(), &value, data.size() );
		return { name
			, castor::ImageCreateParams{ getFormatName( castor::PixelFormat::eR32_SFLOAT ), data } };
	}

	//*********************************************************************************************
}
