#include "Castor3D/Material/Pass/Component/Map/RoughnessMapComponent.hpp"

#include "Castor3D/Material/Pass/Pass.hpp"
#include "Castor3D/Material/Pass/PassVisitor.hpp"
#include "Castor3D/Material/Pass/Component/Map/MetalnessMapComponent.hpp"
#include "Castor3D/Material/Pass/Component/Lighting/RoughnessComponent.hpp"
#include "Castor3D/Material/Texture/TextureConfiguration.hpp"
#include "Castor3D/Scene/SceneFileParser.hpp"
#include "Castor3D/Shader/ShaderBuffers/PassBuffer.hpp"
#include "Castor3D/Shader/Shaders/GlslBlendComponents.hpp"
#include "Castor3D/Shader/Shaders/GlslLighting.hpp"
#include "Castor3D/Shader/Shaders/GlslMaterial.hpp"
#include "Castor3D/Shader/Shaders/GlslTextureConfiguration.hpp"

#include <CastorUtils/FileParser/ParserParameter.hpp>

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

		bool operator()( StringStream & file )
		{
			return writeNamedSub( file, cuT( "roughness_mask" ), m_mask );
		}

		bool operator()( castor3d::RoughnessMapComponent const & object
			, StringStream & file )override
		{
			return writeNamedSub( file, cuT( "roughness_mask" ), m_mask );
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
		static CU_ImplementAttributeParser( parserUnitRoughnessMask )
		{
			auto & parsingContext = getParserContext( context );

			if ( params.empty() )
			{
				CU_ParsingError( cuT( "Missing parameter." ) );
			}
			else
			{
				auto & component = getPassComponent< RoughnessMapComponent >( parsingContext );
				component.fillChannel( parsingContext.textureConfiguration
					, params[0]->get< uint32_t >() );
			}
		}
		CU_EndAttribute()

		static CU_ImplementAttributeParser( parserTexRemapRoughness )
		{
			auto & parsingContext = getParserContext( context );
			parsingContext.sceneImportConfig.textureRemapIt = parsingContext.sceneImportConfig.textureRemaps.emplace( TextureFlag::eRoughness, TextureConfiguration{} ).first;
			parsingContext.sceneImportConfig.textureRemapIt->second = TextureConfiguration{};
		}
		CU_EndAttributePush( CSCNSection::eTextureRemapChannel )

		static CU_ImplementAttributeParser( parserTexRemapRoughnessMask )
		{
			auto & parsingContext = getParserContext( context );

			if ( params.empty() )
			{
				CU_ParsingError( cuT( "Missing parameter." ) );
			}
			else
			{
				parsingContext.sceneImportConfig.textureRemapIt->second.roughnessMask[0] = params[0]->get< uint32_t >();
			}
		}
		CU_EndAttribute()
	}

	//*********************************************************************************************

	void RoughnessMapComponent::ComponentsShader::applyComponents( TextureFlags const & texturesFlags
		, PipelineFlags const * flags
		, shader::TextureConfigData const & config
		, sdw::Vec4 const & sampled
		, shader::BlendComponents const & components )const
	{
		if ( !components.hasMember( "roughness" )
			|| !checkFlag( texturesFlags, TextureFlag::eRoughness ) )
		{
			return;
		}

		auto & writer{ *sampled.getWriter() };

		IF( writer, config.rghEnbl() != 0.0_f )
		{
			components.getMember< sdw::Float >( "roughness" ) *= config.getFloat( sampled, config.rghMask() );
		}
		FI;
	}

	//*********************************************************************************************

	void RoughnessMapComponent::Plugin::createParsers( castor::AttributeParsers & parsers
		, ChannelFillers & channelFillers )const
	{
		channelFillers.emplace( "roughness", ChannelFiller{ uint32_t( TextureFlag::eRoughness )
			, []( SceneFileContext & parsingContext )
			{
				auto & component = getPassComponent< RoughnessMapComponent >( parsingContext );
				component.fillChannel( parsingContext.textureConfiguration
					, 0x00FF0000 );
			} } );

		Pass::addParserT( parsers
			, CSCNSection::eTextureUnit
			, cuT( "roughness_mask" )
			, rghcmp::parserUnitRoughnessMask
			, { castor::makeParameter< castor::ParameterType::eUInt32 >() } );

		Pass::addParserT( parsers
			, CSCNSection::eTextureRemap
			, cuT( "roughness" )
			, rghcmp::parserTexRemapRoughness );

		Pass::addParserT( parsers
			, CSCNSection::eTextureRemapChannel
			, cuT( "roughness_mask" )
			, rghcmp::parserTexRemapRoughnessMask
			, { castor::makeParameter< castor::ParameterType::eUInt32 >() } );
	}

	bool RoughnessMapComponent::Plugin::writeTextureConfig( TextureConfiguration const & configuration
		, castor::String const & tabs
		, castor::StringStream & file )const
	{
		return castor::TextWriter< RoughnessMapComponent >{ tabs, configuration.roughnessMask[0] }( file );
	}

	bool RoughnessMapComponent::Plugin::isComponentNeeded( TextureFlags const & textures
		, ComponentModeFlags const & filter )const
	{
		return checkFlag( filter, ComponentModeFlag::eDiffuseLighting )
			|| checkFlag( filter, ComponentModeFlag::eSpecularLighting )
			|| checkFlag( textures, TextureFlag::eRoughness );
	}

	bool RoughnessMapComponent::Plugin::needsMapComponent( TextureConfiguration const & configuration )const
	{
		return configuration.roughnessMask[0] != 0u;
	}

	void RoughnessMapComponent::Plugin::createMapComponent( Pass & pass
		, std::vector< PassComponentUPtr > & result )const
	{
		result.push_back( std::make_unique< RoughnessMapComponent >( pass ) );
	}

	//*********************************************************************************************

	castor::String const RoughnessMapComponent::TypeName = C3D_MakePassMapComponentName( "roughness" );

	RoughnessMapComponent::RoughnessMapComponent( Pass & pass )
		: PassMapComponent{ pass, TypeName }
	{
		if ( !pass.hasComponent< RoughnessComponent >() )
		{
			pass.createComponent< RoughnessComponent >();
		}
	}

	void RoughnessMapComponent::mergeImages( TextureUnitDataSet & result )
	{
		if ( !getOwner()->hasComponent< MetalnessMapComponent >() )
		{
			getOwner()->mergeImages( TextureFlag::eMetalness
				, offsetof( TextureConfiguration, metalnessMask )
				, 0x00FF0000
				, TextureFlag::eRoughness
				, offsetof( TextureConfiguration, roughnessMask )
				, 0x0000FF00
				, "MtlRgh"
				, result );
		}
	}

	void RoughnessMapComponent::fillChannel( TextureConfiguration & configuration
		, uint32_t mask )
	{
		configuration.textureSpace |= TextureSpace::eNormalised;
		configuration.roughnessMask[0] = mask;
	}

	void RoughnessMapComponent::fillConfig( TextureConfiguration & configuration
		, PassVisitorBase & vis )
	{
		vis.visit( cuT( "Roughness" ), castor3d::TextureFlag::eRoughness, configuration.roughnessMask, 1u );
	}

	PassComponentUPtr RoughnessMapComponent::doClone( Pass & pass )const
	{
		return std::make_unique< RoughnessMapComponent >( pass );
	}

	//*********************************************************************************************
}
