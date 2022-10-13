#include "Castor3D/Material/Pass/Component/Map/GlossinessMapComponent.hpp"

#include "Castor3D/Limits.hpp"
#include "Castor3D/Engine.hpp"
#include "Castor3D/Material/Pass/Pass.hpp"
#include "Castor3D/Material/Pass/PassFactory.hpp"
#include "Castor3D/Material/Pass/Component/Map/SpecularMapComponent.hpp"
#include "Castor3D/Material/Pass/Component/Lighting/MetalnessComponent.hpp"
#include "Castor3D/Material/Pass/Component/Lighting/RoughnessComponent.hpp"
#include "Castor3D/Material/Pass/Component/Lighting/SpecularComponent.hpp"
#include "Castor3D/Material/Pass/PBR/PbrPass.hpp"
#include "Castor3D/Material/Pass/PassVisitor.hpp"
#include "Castor3D/Material/Texture/TextureConfiguration.hpp"
#include "Castor3D/Scene/SceneFileParser.hpp"
#include "Castor3D/Shader/Shaders/GlslBlendComponents.hpp"
#include "Castor3D/Shader/Shaders/GlslLighting.hpp"
#include "Castor3D/Shader/Shaders/GlslMaterial.hpp"
#include "Castor3D/Shader/Shaders/GlslTextureConfiguration.hpp"

#include <CastorUtils/FileParser/ParserParameter.hpp>

//*************************************************************************************************

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
		static CU_ImplementAttributeParser( parserUnitGlossinessMask )
		{
			auto & parsingContext = getParserContext( context );

			if ( params.empty() )
			{
				CU_ParsingError( cuT( "Missing parameter." ) );
			}
			else
			{
				auto & plugin = parsingContext.pass->getComponentPlugin( GlossinessMapComponent::TypeName );
				plugin.fillTextureConfiguration( parsingContext.textureConfiguration
					, params[0]->get< uint32_t >() );
			}
		}
		CU_EndAttribute()

		static CU_ImplementAttributeParser( parserTexRemapGlossiness )
		{
			auto & parsingContext = getParserContext( context );
			auto & plugin = parsingContext.parser->getEngine()->getPassComponentsRegister().getPlugin( GlossinessMapComponent::TypeName );
			parsingContext.sceneImportConfig.textureRemapIt = parsingContext.sceneImportConfig.textureRemaps.emplace( plugin.getTextureFlags(), TextureConfiguration{} ).first;
			parsingContext.sceneImportConfig.textureRemapIt->second = TextureConfiguration{};
		}
		CU_EndAttributePush( CSCNSection::eTextureRemapChannel )

		static CU_ImplementAttributeParser( parserTexRemapGlossinessMask )
		{
			auto & parsingContext = getParserContext( context );

			if ( params.empty() )
			{
				CU_ParsingError( cuT( "Missing parameter." ) );
			}
			else
			{
				auto & plugin = parsingContext.parser->getEngine()->getPassComponentsRegister().getPlugin( GlossinessMapComponent::TypeName );
				plugin.fillTextureConfiguration( parsingContext.sceneImportConfig.textureRemapIt->second
					, params[0]->get< uint32_t >() );
			}
		}
		CU_EndAttribute()
	}

	//*********************************************************************************************

	void GlossinessMapComponent::ComponentsShader::applyComponents( TextureFlagsArray const & texturesFlags
		, PipelineFlags const * flags
		, shader::TextureConfigData const & config
		, sdw::U32Vec3 const & imgCompConfig
		, sdw::Vec4 const & sampled
		, shader::BlendComponents & components )const
	{
		if ( !components.hasMember( "roughness" )
			|| checkFlags( texturesFlags, getTextureFlags() ) == texturesFlags.end() )
		{
			return;
		}

		auto & writer{ *sampled.getWriter() };

		IF( writer, imgCompConfig.y() != 0_u )
		{
			auto roughness = components.getMember< sdw::Float >( "roughness" );
			auto gloss = writer.declLocale( "gloss"
				, ( 1.0_f - roughness ) );
			gloss *= config.getFloat( sampled, imgCompConfig.z() );
			components.getMember< sdw::Float >( "roughness" ) = ( 1.0_f - gloss );
		}
		FI;
	}

	//*********************************************************************************************

	void GlossinessMapComponent::Plugin::createParsers( castor::AttributeParsers & parsers
		, ChannelFillers & channelFillers )const
	{
		channelFillers.emplace( "glossiness", ChannelFiller{ getTextureFlags()
			, []( SceneFileContext & parsingContext )
			{
				auto & component = getPassComponent< GlossinessMapComponent >( parsingContext );
				component.fillChannel( parsingContext.textureConfiguration
					, 0x00FF0000 );
			} } );
		channelFillers.emplace( "shininess", ChannelFiller{ getTextureFlags()
			, []( SceneFileContext & parsingContext )
			{
				auto & component = getPassComponent< GlossinessMapComponent >( parsingContext );
				component.fillChannel( parsingContext.textureConfiguration
					, 0x00FF0000 );
			} } );

		Pass::addParserT( parsers
			, CSCNSection::eTextureUnit
			, cuT( "glossiness_mask" )
			, glscmp::parserUnitGlossinessMask
			, { castor::makeParameter< castor::ParameterType::eUInt32 >() }
			, "The specular glossiness (or shininess exponent) channels mask for the texture" );
		Pass::addParserT( parsers
			, CSCNSection::eTextureUnit
			, cuT( "shininess_mask" )
			, glscmp::parserUnitGlossinessMask
			, { castor::makeParameter< castor::ParameterType::eUInt32 >() }
			, "The specular shininess exponent (or glossiness) channels mask for the texture" );

		Pass::addParserT( parsers
			, CSCNSection::eTextureRemap
			, cuT( "glossiness" )
			, glscmp::parserTexRemapGlossiness );

		Pass::addParserT( parsers
			, CSCNSection::eTextureRemapChannel
			, cuT( "glossiness_mask" )
			, glscmp::parserTexRemapGlossinessMask
			, { castor::makeParameter< castor::ParameterType::eUInt32 >() }
			, "The specular glossiness (or shininess exponent) remapping channels mask for the texture" );
		Pass::addParserT( parsers
			, CSCNSection::eTextureRemapChannel
			, cuT( "shininess_mask" )
			, glscmp::parserUnitGlossinessMask
			, { castor::makeParameter< castor::ParameterType::eUInt32 >() }
			, "The specular shininess exponent (or glossiness) remapping channels mask for the texture" );
	}

	bool GlossinessMapComponent::Plugin::writeTextureConfig( TextureConfiguration const & configuration
		, castor::String const & tabs
		, castor::StringStream & file )const
	{
		auto it = checkFlags( configuration.components, getTextureFlags() );

		if ( it == configuration.components.end() )
		{
			return true;
		}

		return castor::TextWriter< GlossinessMapComponent >{ tabs, it->componentsMask }( file );
	}

	bool GlossinessMapComponent::Plugin::isComponentNeeded( TextureFlagsArray const & textures
		, ComponentModeFlags const & filter )const
	{
		return checkFlag( filter, ComponentModeFlag::eDiffuseLighting )
			|| checkFlag( filter, ComponentModeFlag::eSpecularLighting )
			|| checkFlags( textures, getTextureFlags() ) != textures.end();
	}

	void GlossinessMapComponent::Plugin::createMapComponent( Pass & pass
		, std::vector< PassComponentUPtr > & result )const
	{
		result.push_back( std::make_unique< GlossinessMapComponent >( pass ) );
	}

	//*********************************************************************************************

	castor::String const GlossinessMapComponent::TypeName = C3D_MakePassMapComponentName( "glossiness" );

	GlossinessMapComponent::GlossinessMapComponent( Pass & pass )
		: PassMapComponent{ pass, TypeName }
	{
		if ( !pass.hasComponent< SpecularComponent >() )
		{
			pass.createComponent< SpecularComponent >();
		}

		if ( !pass.hasComponent< RoughnessComponent >() )
		{
			pass.createComponent< RoughnessComponent >();
		}
	}

	void GlossinessMapComponent::fillConfig( TextureConfiguration & configuration
		, PassVisitorBase & vis )const
	{
		vis.visit( cuT( "Glossiness" ), getTextureFlags(), getFlagConfiguration( configuration, getTextureFlags() ), 1u );
	}

	PassComponentUPtr GlossinessMapComponent::doClone( Pass & pass )const
	{
		return std::make_unique< GlossinessMapComponent >( pass );
	}

	//*********************************************************************************************
}
