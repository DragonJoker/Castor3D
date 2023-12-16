#include "Castor3D/Material/Pass/Component/Map/GlossinessMapComponent.hpp"

#include "Castor3D/Limits.hpp"
#include "Castor3D/Engine.hpp"
#include "Castor3D/Material/Pass/Pass.hpp"
#include "Castor3D/Material/Pass/PassFactory.hpp"
#include "Castor3D/Miscellaneous/ConfigurationVisitor.hpp"
#include "Castor3D/Material/Pass/PbrPass.hpp"
#include "Castor3D/Material/Pass/Component/PassComponentRegister.hpp"
#include "Castor3D/Material/Pass/Component/Map/SpecularMapComponent.hpp"
#include "Castor3D/Material/Pass/Component/Lighting/MetalnessComponent.hpp"
#include "Castor3D/Material/Pass/Component/Lighting/RoughnessComponent.hpp"
#include "Castor3D/Material/Pass/Component/Lighting/SpecularComponent.hpp"
#include "Castor3D/Material/Texture/TextureConfiguration.hpp"
#include "Castor3D/Scene/SceneFileParser.hpp"
#include "Castor3D/Shader/Shaders/GlslBlendComponents.hpp"
#include "Castor3D/Shader/Shaders/GlslLighting.hpp"
#include "Castor3D/Shader/Shaders/GlslMaterial.hpp"
#include "Castor3D/Shader/Shaders/GlslTextureConfiguration.hpp"

#include <CastorUtils/FileParser/ParserParameter.hpp>

//*************************************************************************************************

CU_ImplementSmartPtr( castor3d, GlossinessMapComponent )

namespace castor
{
	template<>
	class TextWriter< castor3d::GlossinessMapComponent >
		: public TextWriterT< castor3d::GlossinessMapComponent >
	{
	public:
		explicit TextWriter( String const & tabs
			, uint32_t mask )
			: TextWriterT< castor3d::GlossinessMapComponent >{ tabs }
			, m_mask{ mask }
		{
		}

		bool operator()( castor3d::GlossinessMapComponent const & object
			, StringStream & file )override
		{
			return writeMask( file, cuT( "glossiness_mask" ), m_mask );
		}

		bool operator()( StringStream & file )
		{
			return writeMask( file, cuT( "glossiness_mask" ), m_mask );
		}

	private:
		uint32_t m_mask;
	};
}

namespace castor3d
{
	//*********************************************************************************************

	namespace glscmp
	{
		static CU_ImplementAttributeParserBlock( parserUnitGlossinessMask, TextureContext )
		{
			if ( params.empty() )
			{
				CU_ParsingError( cuT( "Missing parameter." ) );
			}
			else if ( !blockContext->pass )
			{
				auto & plugin = blockContext->root->engine->getPassComponentsRegister().getPlugin( GlossinessMapComponent::TypeName );
				plugin.fillTextureConfiguration( blockContext->configuration
					, params[0]->get< uint32_t >() );
			}
			else
			{
				auto & plugin = blockContext->pass->pass->getComponentPlugin( GlossinessMapComponent::TypeName );
				plugin.fillTextureConfiguration( blockContext->configuration
					, params[0]->get< uint32_t >() );
			}
		}
		CU_EndAttribute()

		static CU_ImplementAttributeParserBlock( parserTexRemapGlossiness, SceneImportContext )
		{
			auto & plugin = blockContext->root->engine->getPassComponentsRegister().getPlugin( GlossinessMapComponent::TypeName );
			blockContext->textureRemapIt = blockContext->textureRemaps.emplace( plugin.getTextureFlags(), TextureConfiguration{} ).first;
			blockContext->textureRemapIt->second = TextureConfiguration{};
		}
		CU_EndAttributePushBlock( CSCNSection::eTextureRemapChannel, blockContext )

		static CU_ImplementAttributeParserBlock( parserTexRemapGlossinessMask, SceneImportContext )
		{
			if ( params.empty() )
			{
				CU_ParsingError( cuT( "Missing parameter." ) );
			}
			else
			{
				auto & plugin = blockContext->root->engine->getPassComponentsRegister().getPlugin( GlossinessMapComponent::TypeName );
				plugin.fillTextureConfiguration( blockContext->textureRemapIt->second
					, params[0]->get< uint32_t >() );
			}
		}
		CU_EndAttribute()
	}

	//*********************************************************************************************

	void GlossinessMapComponent::ComponentsShader::applyTexture( shader::PassShaders const & passShaders
		, shader::TextureConfigurations const & textureConfigs
		, shader::TextureAnimations const & textureAnims
		, sdw::Array< sdw::CombinedImage2DRgba32 > const & maps
		, shader::Material const & material
		, shader::BlendComponents & components
		, shader::SampleTexture const & sampleTexture )const
	{
		std::string mapName = "glossiness";
		auto textureName = mapName + "MapAndMask";

		if ( !material.hasMember( textureName )
			|| !components.hasMember( "roughness" ) )
		{
			return;
		}

		auto & writer{ *material.getWriter() };
		auto map = writer.declLocale( mapName + "Map"
			, material.getMember< sdw::UInt >( textureName ) >> 16u );
		auto mask = writer.declLocale( mapName + "Mask"
			, material.getMember< sdw::UInt >( textureName ) & 0xFFFFu );
		auto value = components.getMember< sdw::Float >( "roughness" );

		auto config = writer.declLocale( "glossinessConfig"
			, textureConfigs.getTextureConfiguration( map ) );
		auto anim = writer.declLocale( "glossinessAnim"
			, textureAnims.getTextureAnimation( map ) );
		passShaders.computeTexcoords( textureConfigs
			, config
			, anim
			, components );
		auto sampled = writer.declLocale( "glossinessSampled"
			, sampleTexture( map, config, components ) );
		auto gloss = writer.declLocale( "gloss"
			, ( 1.0_f - value ) );
		gloss *= shader::TextureConfigData::getFloat( sampled, mask );
		value = ( 1.0_f - gloss );
	}

	//*********************************************************************************************

	void GlossinessMapComponent::Plugin::createParsers( castor::AttributeParsers & parsers
		, ChannelFillers & channelFillers )const
	{
		channelFillers.emplace( "glossiness", ChannelFiller{ getTextureFlags()
			, []( TextureContext & blockContext )
			{
				auto & component = getPassComponent< GlossinessMapComponent >( blockContext );
				component.fillChannel( blockContext.configuration
					, 0x00FF0000 );
			} } );
		channelFillers.emplace( "shininess", ChannelFiller{ getTextureFlags()
			, []( TextureContext & blockContext )
			{
				auto & component = getPassComponent< GlossinessMapComponent >( blockContext );
				component.fillChannel( blockContext.configuration
					, 0x00FF0000 );
			} } );

		castor::addParserT( parsers
			, CSCNSection::eTexture
			, cuT( "glossiness_mask" )
			, glscmp::parserUnitGlossinessMask
			, { castor::makeParameter< castor::ParameterType::eUInt32 >() } );
		castor::addParserT( parsers
			, CSCNSection::eTexture
			, cuT( "shininess_mask" )
			, glscmp::parserUnitGlossinessMask
			, { castor::makeParameter< castor::ParameterType::eUInt32 >() } );

		castor::addParserT( parsers
			, CSCNSection::eTextureUnit
			, cuT( "glossiness_mask" )
			, glscmp::parserUnitGlossinessMask
			, { castor::makeParameter< castor::ParameterType::eUInt32 >() } );
		castor::addParserT( parsers
			, CSCNSection::eTextureUnit
			, cuT( "shininess_mask" )
			, glscmp::parserUnitGlossinessMask
			, { castor::makeParameter< castor::ParameterType::eUInt32 >() } );

		castor::addParserT( parsers
			, CSCNSection::eTextureRemap
			, cuT( "glossiness" )
			, glscmp::parserTexRemapGlossiness );

		castor::addParserT( parsers
			, CSCNSection::eTextureRemapChannel
			, cuT( "glossiness_mask" )
			, glscmp::parserTexRemapGlossinessMask
			, { castor::makeParameter< castor::ParameterType::eUInt32 >() } );
		castor::addParserT( parsers
			, CSCNSection::eTextureRemapChannel
			, cuT( "shininess_mask" )
			, glscmp::parserUnitGlossinessMask
			, { castor::makeParameter< castor::ParameterType::eUInt32 >() } );
	}

	bool GlossinessMapComponent::Plugin::isComponentNeeded( TextureCombine const & textures
		, ComponentModeFlags const & filter )const
	{
		return checkFlag( filter, ComponentModeFlag::eDiffuseLighting )
			|| checkFlag( filter, ComponentModeFlag::eSpecularLighting )
			|| hasAny( textures, getTextureFlags() );
	}

	void GlossinessMapComponent::Plugin::createMapComponent( Pass & pass
		, std::vector< PassComponentUPtr > & result )const
	{
		result.push_back( castor::makeUniqueDerived< PassComponent, GlossinessMapComponent >( pass ) );
	}

	bool GlossinessMapComponent::Plugin::doWriteTextureConfig( TextureConfiguration const & configuration
		, uint32_t mask
		, castor::String const & tabs
		, castor::StringStream & file )const
	{
		return castor::TextWriter< GlossinessMapComponent >{ tabs, mask }( file );
	}

	//*********************************************************************************************

	castor::String const GlossinessMapComponent::TypeName = C3D_MakePassMapComponentName( "glossiness" );

	GlossinessMapComponent::GlossinessMapComponent( Pass & pass )
		: PassMapComponent{ pass
			, TypeName
			, Glossiness
			, { SpecularComponent::TypeName, RoughnessComponent::TypeName } }
	{
	}

	PassComponentUPtr GlossinessMapComponent::doClone( Pass & pass )const
	{
		return castor::makeUniqueDerived< PassComponent, GlossinessMapComponent >( pass );
	}

	void GlossinessMapComponent::doFillConfig( TextureConfiguration & configuration
		, ConfigurationVisitorBase & vis )const
	{
		vis.visit( cuT( "Glossiness" ) );
		vis.visit( cuT( "Map" ), getTextureFlags(), getFlagConfiguration( configuration, getTextureFlags() ), 1u );
	}

	//*********************************************************************************************
}
