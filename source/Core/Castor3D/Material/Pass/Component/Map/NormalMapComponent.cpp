#include "Castor3D/Material/Pass/Component/Map/NormalMapComponent.hpp"

#include "Castor3D/Engine.hpp"
#include "Castor3D/Material/Pass/Pass.hpp"
#include "Castor3D/Miscellaneous/ConfigurationVisitor.hpp"
#include "Castor3D/Material/Pass/Component/PassComponentRegister.hpp"
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

CU_ImplementSmartPtr( castor3d, NormalMapComponent )

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
				&& writeOpt( file, cuT( "normal_directx" ), m_configuration.normalDirectX )
				&& writeOpt( file, cuT( "normal_2channels" ), m_configuration.normal2Channels );
		}

		bool operator()( StringStream & file
			, uint32_t mask )
		{
			return writeMask( file, cuT( "normal_mask" ), mask )
				&& writeOpt( file, cuT( "normal_factor" ), m_configuration.normalFactor, 1.0f )
				&& writeOpt( file, cuT( "normal_directx" ), m_configuration.normalDirectX )
				&& writeOpt( file, cuT( "normal_2channels" ), m_configuration.normal2Channels );
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
				getPassComponent< NormalMapComponent >( parsingContext );
				params[0]->get( parsingContext.textureConfiguration.normalDirectX );
			}
		}
		CU_EndAttribute()

		static CU_ImplementAttributeParser( parserUnitNormal2Channels )
		{
			auto & parsingContext = getParserContext( context );

			if ( params.empty() )
			{
				CU_ParsingError( cuT( "Missing parameter." ) );
			}
			else
			{
				getPassComponent< NormalMapComponent >( parsingContext );
				params[0]->get( parsingContext.textureConfiguration.normal2Channels );
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
				params[0]->get( parsingContext.sceneImportConfig.textureRemapIt->second.normalDirectX );
			}
		}
		CU_EndAttribute()

		static CU_ImplementAttributeParser( parserTexRemapNormal2Channels )
		{
			auto & parsingContext = getParserContext( context );

			if ( params.empty() )
			{
				CU_ParsingError( cuT( "Missing parameter." ) );
			}
			else
			{
				params[0]->get( parsingContext.sceneImportConfig.textureRemapIt->second.normal2Channels );
			}
		}
		CU_EndAttribute()
	}

	//*********************************************************************************************

	void NormalMapComponent::ComponentsShader::applyComponents( TextureCombine const & combine
		, PipelineFlags const * flags
		, shader::TextureConfigData const & config
		, sdw::U32Vec3 const & imgCompConfig
		, sdw::Vec4 const & sampled
		, shader::BlendComponents & components )const
	{
		if ( !components.hasMember( "normal" )
			|| !flags
			|| !flags->enableTangentSpace() )
		{
			return;
		}

		auto & writer{ *sampled.getWriter() };

		IF( writer, imgCompConfig.x() == sdw::UInt{ getTextureFlags() } )
		{
			computeMikktNormal( config, imgCompConfig, components, sampled
				, components.getMember< sdw::Vec3 >( "normal" ) );
		}
		FI;
	}

	void NormalMapComponent::ComponentsShader::computeMikktNormal( shader::TextureConfigData const & config
		, sdw::U32Vec3 const & imgCompConfig
		, shader::BlendComponents & components
		, sdw::Vec4 const & sampled
		, sdw::Vec3 normal )
	{
		auto & writer{ *sampled.getWriter() };
		auto tbn = shader::Utils::getTBN( components.getMember< sdw::Vec3 >( "normal" )
			, components.getMember< sdw::Vec4 >( "tangent" ).xyz()
			, components.getMember< sdw::Vec3 >( "bitangent" ) );
		sampled[imgCompConfig.z() + 1u] = config.nmlGMul() * sampled[imgCompConfig.z() + 1u];
		components.getMember< sdw::Vec3 >( "normal" ) = normalize( tbn
			* fma( vec3( 2.0_f )
				, writer.ternary( config.nml2Chan() != 0_u
					, shader::Utils::reconstructNormal( sampled[imgCompConfig.z()], sampled[imgCompConfig.z() + 1u] )
					, config.getVec3( sampled, imgCompConfig.z() ) )
				, -vec3( 1.0_f ) ) );
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

		castor::addParserT( parsers
			, CSCNSection::eTextureUnit
			, cuT( "normal_mask" )
			, nmlcmp::parserUnitNormalMask
			, { castor::makeParameter< castor::ParameterType::eUInt32 >() } );
		castor::addParserT( parsers
			, CSCNSection::eTextureUnit
			, cuT( "normal_factor" )
			, nmlcmp::parserUnitNormalFactor
			, { castor::makeParameter< castor::ParameterType::eFloat >() } );
		castor::addParserT( parsers
			, CSCNSection::eTextureUnit
			, cuT( "normal_directx" )
			, nmlcmp::parserUnitNormalDirectX
			, { castor::makeParameter< castor::ParameterType::eBool >() } );
		castor::addParserT( parsers
			, CSCNSection::eTextureUnit
			, cuT( "normal_2channels" )
			, nmlcmp::parserUnitNormal2Channels
			, { castor::makeParameter< castor::ParameterType::eBool >() } );

		castor::addParserT( parsers
			, CSCNSection::eTextureRemap
			, cuT( "normal" )
			, nmlcmp::parserTexRemapNormal );

		castor::addParserT( parsers
			, CSCNSection::eTextureRemapChannel
			, cuT( "normal_mask" )
			, nmlcmp::parserTexRemapNormalMask
			, { castor::makeParameter< castor::ParameterType::eUInt32 >() } );
		castor::addParserT( parsers
			, CSCNSection::eTextureRemapChannel
			, cuT( "normal_directx" )
			, nmlcmp::parserTexRemapNormalDirectX
			, { castor::makeParameter< castor::ParameterType::eBool >() } );
		castor::addParserT( parsers
			, CSCNSection::eTextureRemapChannel
			, cuT( "normal_2channels" )
			, nmlcmp::parserTexRemapNormal2Channels
			, { castor::makeParameter< castor::ParameterType::eBool >() } );
	}

	bool NormalMapComponent::Plugin::isComponentNeeded( TextureCombine const & textures
		, ComponentModeFlags const & filter )const
	{
		return checkFlag( filter, ComponentModeFlag::eNormals )
			|| checkFlag( filter, ComponentModeFlag::eDiffuseLighting )
			|| checkFlag( filter, ComponentModeFlag::eSpecularLighting )
			|| checkFlag( filter, ComponentModeFlag::eOcclusion )
			|| hasAny( textures, getTextureFlags() );
	}

	void NormalMapComponent::Plugin::createMapComponent( Pass & pass
		, std::vector< PassComponentUPtr > & result )const
	{
		result.push_back( castor::makeUniqueDerived< PassComponent, NormalMapComponent >( pass ) );
	}

	bool NormalMapComponent::Plugin::doWriteTextureConfig( TextureConfiguration const & configuration
		, uint32_t mask
		, castor::String const & tabs
		, castor::StringStream & file )const
	{
		return castor::TextWriter< NormalMapComponent >{ tabs, configuration }( file, mask );
	}

	//*********************************************************************************************

	castor::String const NormalMapComponent::TypeName = C3D_MakePassMapComponentName( "normal" );

	NormalMapComponent::NormalMapComponent( Pass & pass )
		: PassMapComponent{ pass
			, TypeName
			, Normal
			, { NormalComponent::TypeName } }
	{
	}

	PassComponentUPtr NormalMapComponent::doClone( Pass & pass )const
	{
		return castor::makeUniqueDerived< PassComponent, NormalMapComponent >( pass );
	}

	void NormalMapComponent::doFillConfig( TextureConfiguration & configuration
		, ConfigurationVisitorBase & vis )const
	{
		vis.visit( cuT( "Normal" ) );
		vis.visit( cuT( "Map" ), getTextureFlags(), getFlagConfiguration( configuration, getTextureFlags() ), 3u );
		vis.visit( cuT( "Factor" ), configuration.normalFactor );
		vis.visit( cuT( "DirectX" ), configuration.normalDirectX );
		vis.visit( cuT( "2 Channels" ), configuration.normal2Channels );
	}

	//*********************************************************************************************
}
