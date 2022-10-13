#include "Castor3D/Material/Pass/Component/Map/NormalMapComponent.hpp"

#include "Castor3D/Engine.hpp"
#include "Castor3D/Material/Pass/Pass.hpp"
#include "Castor3D/Material/Pass/PassVisitor.hpp"
#include "Castor3D/Material/Pass/Component/Base/NormalComponent.hpp"
#include "Castor3D/Material/Texture/TextureConfiguration.hpp"
#include "Castor3D/Scene/SceneFileParser.hpp"
#include "Castor3D/Shader/ShaderBuffers/PassBuffer.hpp"
#include "Castor3D/Shader/Shaders/GlslBlendComponents.hpp"
#include "Castor3D/Shader/Shaders/GlslLighting.hpp"
#include "Castor3D/Shader/Shaders/GlslMaterial.hpp"
#include "Castor3D/Shader/Shaders/GlslSurface.hpp"
#include "Castor3D/Shader/Shaders/GlslTextureConfiguration.hpp"

#include <CastorUtils/FileParser/ParserParameter.hpp>

namespace castor
{
	template<>
	class TextWriter< castor3d::NormalMapComponent >
		: public TextWriterT< castor3d::NormalMapComponent >
	{
	public:
		explicit TextWriter( String const & tabs
			, castor3d::TextureConfiguration const & configuration )
			: TextWriterT< castor3d::NormalMapComponent >{ tabs }
			, m_configuration{ configuration }
		{
		}

		bool operator()( castor3d::NormalMapComponent const & object
			, StringStream & file )override
		{
			return writeMask( file, cuT( "normal_mask" ), getComponentsMask( m_configuration, object.getTextureFlags() ) )
				&& writeOpt( file, cuT( "normal_factor" ), m_configuration.normalFactor, 1.0f )
				&& writeOpt( file, cuT( "normal_directx" ), m_configuration.normalGMultiplier != 1.0f );
		}

		bool operator()( StringStream & file
			, uint32_t mask )
		{
			return writeMask( file, cuT( "normal_mask" ), mask )
				&& writeOpt( file, cuT( "normal_factor" ), m_configuration.normalFactor, 1.0f )
				&& writeOpt( file, cuT( "normal_directx" ), m_configuration.normalGMultiplier != 1.0f );
		}

	private:
		castor3d::TextureConfiguration const & m_configuration;
	};
}

namespace castor3d
{
	//*********************************************************************************************

	namespace nmlcmp
	{
		static CU_ImplementAttributeParser( parserUnitNormalMask )
		{
			auto & parsingContext = getParserContext( context );

			if ( params.empty() )
			{
				CU_ParsingError( cuT( "Missing parameter." ) );
			}
			else
			{
				auto & component = getPassComponent< NormalMapComponent >( parsingContext );
				component.fillChannel( parsingContext.textureConfiguration
					, params[0]->get< uint32_t >() );
			}
		}
		CU_EndAttribute()

		static CU_ImplementAttributeParser( parserUnitNormalFactor )
		{
			auto & parsingContext = getParserContext( context );

			if ( params.empty() )
			{
				CU_ParsingError( cuT( "Missing parameter." ) );
			}
			else
			{
				getPassComponent< NormalMapComponent >( parsingContext );
				params[0]->get( parsingContext.textureConfiguration.normalFactor );
			}
		}
		CU_EndAttribute()

		static CU_ImplementAttributeParser( parserUnitNormalDirectX )
		{
			auto & parsingContext = getParserContext( context );

			if ( params.empty() )
			{
				CU_ParsingError( cuT( "Missing parameter." ) );
			}
			else
			{
				bool isDirectX;
				params[0]->get( isDirectX );
				getPassComponent< NormalMapComponent >( parsingContext );
				parsingContext.textureConfiguration.normalGMultiplier = isDirectX
					? -1.0f
					: 1.0f;
			}
		}
		CU_EndAttribute()

		static CU_ImplementAttributeParser( parserTexRemapNormal )
		{
			auto & parsingContext = getParserContext( context );
			auto & plugin = parsingContext.parser->getEngine()->getPassComponentsRegister().getPlugin( NormalMapComponent::TypeName );
			parsingContext.sceneImportConfig.textureRemapIt = parsingContext.sceneImportConfig.textureRemaps.emplace( plugin.getTextureFlags(), TextureConfiguration{} ).first;
			parsingContext.sceneImportConfig.textureRemapIt->second = TextureConfiguration{};
		}
		CU_EndAttributePush( CSCNSection::eTextureRemapChannel )

		static CU_ImplementAttributeParser( parserTexRemapNormalMask )
		{
			auto & parsingContext = getParserContext( context );

			if ( params.empty() )
			{
				CU_ParsingError( cuT( "Missing parameter." ) );
			}
			else
			{
				auto & plugin = parsingContext.parser->getEngine()->getPassComponentsRegister().getPlugin( NormalMapComponent::TypeName );
				plugin.fillTextureConfiguration( parsingContext.sceneImportConfig.textureRemapIt->second
					, params[0]->get< uint32_t >() );
			}
		}
		CU_EndAttribute()

		static CU_ImplementAttributeParser( parserTexRemapNormalDirectX )
		{
			auto & parsingContext = getParserContext( context );

			if ( params.empty() )
			{
				CU_ParsingError( cuT( "Missing parameter." ) );
			}
			else
			{
				bool isDirectX;
				params[0]->get( isDirectX );
				parsingContext.sceneImportConfig.textureRemapIt->second.normalGMultiplier = isDirectX
					? -1.0f
					: 1.0f;
			}
		}
		CU_EndAttribute()
	}

	//*********************************************************************************************

	void NormalMapComponent::ComponentsShader::applyComponents( TextureFlagsArray const & texturesFlags
		, PipelineFlags const * flags
		, shader::TextureConfigData const & config
		, sdw::U32Vec3 const & imgCompConfig
		, sdw::Vec4 const & sampled
		, shader::BlendComponents & components )const
	{
		if ( !components.hasMember( "normal" )
			|| texturesFlags.end() == checkFlags( texturesFlags, getTextureFlags() )
			|| !flags
			|| !flags->enableTangentSpace() )
		{
			return;
		}

		auto & writer{ *sampled.getWriter() };

		IF( writer, imgCompConfig.y() != 0_u )
		{
			auto tbn = shader::Utils::getTBN( components.getMember< sdw::Vec3 >( "normal" )
				, components.getMember< sdw::Vec3 >( "tangent" )
				, components.getMember< sdw::Vec3 >( "bitangent" ) );
			components.getMember< sdw::Vec3 >( "normal" ) = normalize( tbn
				* fma( config.getVec3( sampled, imgCompConfig.z() )
					, vec3( 2.0_f )
					, -vec3( 1.0_f ) ) );
		}
		FI;
	}

	//*********************************************************************************************

	void NormalMapComponent::Plugin::createParsers( castor::AttributeParsers & parsers
		, ChannelFillers & channelFillers )const
	{
		channelFillers.emplace( "normal", ChannelFiller{ getTextureFlags()
			, []( SceneFileContext & parsingContext )
			{
				auto & component = getPassComponent< NormalMapComponent >( parsingContext );
				component.fillChannel( parsingContext.textureConfiguration
					, 0x00FFFFFF );
			} } );

		Pass::addParserT( parsers
			, CSCNSection::eTextureUnit
			, cuT( "normal_mask" )
			, nmlcmp::parserUnitNormalMask
			, { castor::makeParameter< castor::ParameterType::eUInt32 >() } );
		Pass::addParserT( parsers
			, CSCNSection::eTextureUnit
			, cuT( "normal_factor" )
			, nmlcmp::parserUnitNormalFactor
			, { castor::makeParameter< castor::ParameterType::eFloat >() } );
		Pass::addParserT( parsers
			, CSCNSection::eTextureUnit
			, cuT( "normal_directx" )
			, nmlcmp::parserUnitNormalDirectX
			, { castor::makeParameter< castor::ParameterType::eBool >() } );

		Pass::addParserT( parsers
			, CSCNSection::eTextureRemap
			, cuT( "normal" )
			, nmlcmp::parserTexRemapNormal );

		Pass::addParserT( parsers
			, CSCNSection::eTextureRemapChannel
			, cuT( "normal_mask" )
			, nmlcmp::parserTexRemapNormalMask
			, { castor::makeParameter< castor::ParameterType::eUInt32 >() } );
		Pass::addParserT( parsers
			, CSCNSection::eTextureRemapChannel
			, cuT( "normal_directx" )
			, nmlcmp::parserTexRemapNormalDirectX
			, { castor::makeParameter< castor::ParameterType::eBool >() } );
	}

	bool NormalMapComponent::Plugin::writeTextureConfig( TextureConfiguration const & configuration
		, castor::String const & tabs
		, castor::StringStream & file )const
	{
		auto it = checkFlags( configuration.components, getTextureFlags() );

		if ( it == configuration.components.end() )
		{
			return true;
		}

		return castor::TextWriter< NormalMapComponent >{ tabs, configuration }( file, it->componentsMask );
	}

	bool NormalMapComponent::Plugin::isComponentNeeded( TextureFlagsArray const & textures
		, ComponentModeFlags const & filter )const
	{
		return checkFlag( filter, ComponentModeFlag::eGeometry )
			|| checkFlag( filter, ComponentModeFlag::eDiffuseLighting )
			|| checkFlag( filter, ComponentModeFlag::eSpecularLighting )
			|| checkFlag( filter, ComponentModeFlag::eOcclusion )
			|| textures.end() != checkFlags( textures, getTextureFlags() );
	}

	void NormalMapComponent::Plugin::createMapComponent( Pass & pass
		, std::vector< PassComponentUPtr > & result )const
	{
		result.push_back( std::make_unique< NormalMapComponent >( pass ) );
	}

	//*********************************************************************************************

	castor::String const NormalMapComponent::TypeName = C3D_MakePassMapComponentName( "normal" );

	NormalMapComponent::NormalMapComponent( Pass & pass )
		: PassMapComponent{ pass, TypeName }
	{
			if ( !pass.hasComponent< NormalComponent >() )
			{
				pass.createComponent< NormalComponent >();
			}
	}

	void NormalMapComponent::fillConfig( TextureConfiguration & configuration
		, PassVisitorBase & vis )const
	{
		vis.visit( cuT( "Normal" ), getTextureFlags(), getFlagConfiguration( configuration, getTextureFlags() ), 3u );
		vis.visit( cuT( "Normal factor" ), configuration.normalFactor );
		vis.visit( cuT( "Normal DirectX" ), configuration.normalGMultiplier );
	}

	PassComponentUPtr NormalMapComponent::doClone( Pass & pass )const
	{
		return std::make_unique< NormalMapComponent >( pass );
	}

	//*********************************************************************************************
}
