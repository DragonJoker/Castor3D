#include "Castor3D/Material/Pass/Component/Map/MetalnessMapComponent.hpp"

#include "Castor3D/Material/Pass/Pass.hpp"
#include "Castor3D/Material/Pass/PassVisitor.hpp"
#include "Castor3D/Material/Pass/Component/Lighting/MetalnessComponent.hpp"
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
	class TextWriter< castor3d::MetalnessMapComponent >
		: public TextWriterT< castor3d::MetalnessMapComponent >
	{
	public:
		explicit TextWriter( String const & tabs
			, uint32_t mask )
			: TextWriterT< castor3d::MetalnessMapComponent >{ tabs }
			, m_mask{ mask }
		{
		}

		bool operator()( castor3d::MetalnessMapComponent const & object
			, StringStream & file )override
		{
			return writeNamedSub( file, cuT( "metalness_mask" ), m_mask );
		}

		bool operator()( StringStream & file )
		{
			return writeNamedSub( file, cuT( "metalness_mask" ), m_mask );
		}

	private:
		uint32_t m_mask;
	};
}

namespace castor3d
{
	//*********************************************************************************************

	namespace mtlcmp
	{
		static CU_ImplementAttributeParser( parserUnitMetalnessMask )
		{
			auto & parsingContext = getParserContext( context );

			if ( params.empty() )
			{
				CU_ParsingError( cuT( "Missing parameter." ) );
			}
			else
			{
				auto & component = getPassComponent< MetalnessMapComponent >( parsingContext );
				component.fillChannel( parsingContext.textureConfiguration
					, params[0]->get< uint32_t >() );
			}
		}
		CU_EndAttribute()

		static CU_ImplementAttributeParser( parserTexRemapMetalness )
		{
			auto & parsingContext = getParserContext( context );
			parsingContext.sceneImportConfig.textureRemapIt = parsingContext.sceneImportConfig.textureRemaps.emplace( TextureFlag::eMetalness, TextureConfiguration{} ).first;
			parsingContext.sceneImportConfig.textureRemapIt->second = TextureConfiguration{};
		}
		CU_EndAttributePush( CSCNSection::eTextureRemapChannel )

		static CU_ImplementAttributeParser( parserTexRemapMetalnessMask )
		{
			auto & parsingContext = getParserContext( context );

			if ( params.empty() )
			{
				CU_ParsingError( cuT( "Missing parameter." ) );
			}
			else
			{
				MetalnessMapComponent::fillRemapMask( params[0]->get< uint32_t >()
					, parsingContext.sceneImportConfig.textureRemapIt->second );
			}
		}
		CU_EndAttribute()
	}

	//*********************************************************************************************

	void MetalnessMapComponent::ComponentsShader::applyComponents( TextureFlags const & texturesFlags
		, PipelineFlags const * flags
		, shader::TextureConfigData const & config
		, sdw::Vec4 const & sampled
		, shader::BlendComponents const & components )const
	{
		if ( !components.hasMember( "metalness" )
			|| !checkFlag( texturesFlags, TextureFlag::eMetalness ) )
		{
			return;
		}

		auto & writer{ *sampled.getWriter() };

		IF( writer, config.metEnbl() != 0.0_f )
		{
			components.getMember< sdw::Float >( "metalness" ) *= config.getFloat( sampled, config.metMask() );
		}
		FI;
	}

	//*********************************************************************************************

	castor::String const MetalnessMapComponent::TypeName = C3D_MakePassMapComponentName( "metalness" );

	MetalnessMapComponent::MetalnessMapComponent( Pass & pass )
		: PassMapComponent{ pass, TypeName }
	{
		if ( !pass.hasComponent< MetalnessComponent >() )
		{
			pass.createComponent< MetalnessComponent >();
		}
	}

	void MetalnessMapComponent::createParsers( castor::AttributeParsers & parsers
		, ChannelFillers & channelFillers )
	{
		channelFillers.emplace( "metalness", ChannelFiller{ uint32_t( TextureFlag::eMetalness )
			, []( SceneFileContext & parsingContext )
			{
				auto & component = getPassComponent< MetalnessMapComponent >( parsingContext );
				component.fillChannel( parsingContext.textureConfiguration
					, 0x00FF0000 );
			} } );

		Pass::addParserT( parsers
			, CSCNSection::eTextureUnit
			, cuT( "metalness_mask" )
			, mtlcmp::parserUnitMetalnessMask
			, { castor::makeParameter< castor::ParameterType::eUInt32 >() } );

		Pass::addParserT( parsers
			, CSCNSection::eTextureRemap
			, cuT( "metalness" )
			, mtlcmp::parserTexRemapMetalness );

		Pass::addParserT( parsers
			, CSCNSection::eTextureRemapChannel
			, cuT( "metalness_mask" )
			, mtlcmp::parserTexRemapMetalnessMask
			, { castor::makeParameter< castor::ParameterType::eUInt32 >() } );
	}

	void MetalnessMapComponent::fillRemapMask( uint32_t maskValue
		, TextureConfiguration & configuration )
	{
		configuration.metalnessMask[0] = maskValue;
	}

	bool MetalnessMapComponent::writeTextureConfig( TextureConfiguration const & configuration
		, castor::String const & tabs
		, castor::StringStream & file )
	{
		return castor::TextWriter< MetalnessMapComponent >{ tabs, configuration.metalnessMask[0] }( file );
	}

	bool MetalnessMapComponent::isComponentNeeded( TextureFlags const & textures
		, ComponentModeFlags const & filter )
	{
		return checkFlag( filter, ComponentModeFlag::eSpecularLighting )
			|| checkFlag( textures, TextureFlag::eMetalness );
	}

	bool MetalnessMapComponent::needsMapComponent( TextureConfiguration const & configuration )
	{
		return configuration.metalnessMask[0] != 0u;
	}

	void MetalnessMapComponent::createMapComponent( Pass & pass
		, std::vector< PassComponentUPtr > & result )
	{
		result.push_back( std::make_unique< MetalnessMapComponent >( pass ) );
	}

	void MetalnessMapComponent::updateComponent( TextureFlags const & texturesFlags
		, shader::BlendComponents const & components )
	{
		if ( !checkFlag( texturesFlags, TextureFlag::eMetalness )
			&& checkFlag( texturesFlags, TextureFlag::eSpecular ) )
		{
			components.getMember< sdw::Float >( "metalness", true ) = length( components.getMember< sdw::Vec3 >( "specular", true ) );
		}
	}

	void MetalnessMapComponent::mergeImages( TextureUnitDataSet & result )
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

	void MetalnessMapComponent::fillChannel( TextureConfiguration & configuration
		, uint32_t mask )
	{
		configuration.textureSpace |= TextureSpace::eNormalised;
		configuration.metalnessMask[0] = mask;
	}

	void MetalnessMapComponent::fillConfig( TextureConfiguration & configuration
		, PassVisitorBase & vis )
	{
		vis.visit( cuT( "Metalness" ), castor3d::TextureFlag::eMetalness, configuration.metalnessMask, 1u );
	}

	PassComponentUPtr MetalnessMapComponent::doClone( Pass & pass )const
	{
		return std::make_unique< MetalnessMapComponent >( pass );
	}

	//*********************************************************************************************
}
