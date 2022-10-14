#include "Castor3D/Material/Pass/Component/Map/EmissiveMapComponent.hpp"

#include "Castor3D/Engine.hpp"
#include "Castor3D/Material/Pass/Pass.hpp"
#include "Castor3D/Material/Pass/PassVisitor.hpp"
#include "Castor3D/Material/Pass/Component/Lighting/EmissiveComponent.hpp"
#include "Castor3D/Material/Pass/Component/PassComponentRegister.hpp"
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
	class TextWriter< castor3d::EmissiveMapComponent >
		: public TextWriterT< castor3d::EmissiveMapComponent >
	{
	public:
		explicit TextWriter( String const & tabs
			, uint32_t mask )
			: TextWriterT< castor3d::EmissiveMapComponent >{ tabs }
			, m_mask{ mask }
		{
		}

		bool operator()( castor3d::EmissiveMapComponent const & object
			, StringStream & file )override
		{
			return writeMask( file, cuT( "emissive_mask" ), m_mask );
		}

		bool operator()( StringStream & file )
		{
			return writeMask( file, cuT( "emissive_mask" ), m_mask );
		}

	private:
		uint32_t m_mask;
	};
}

namespace castor3d
{
	//*********************************************************************************************

	namespace emscmp
	{
		static CU_ImplementAttributeParser( parserUnitEmissiveMask )
		{
			auto & parsingContext = getParserContext( context );

			if ( params.empty() )
			{
				CU_ParsingError( cuT( "Missing parameter." ) );
			}
			else
			{
				auto & plugin = parsingContext.pass->getComponentPlugin( EmissiveMapComponent::TypeName );
				plugin.fillTextureConfiguration( parsingContext.textureConfiguration
					, params[0]->get< uint32_t >() );
			}
		}
		CU_EndAttribute()

		static CU_ImplementAttributeParser( parserTexRemapEmissive )
		{
			auto & parsingContext = getParserContext( context );
			auto & plugin = parsingContext.parser->getEngine()->getPassComponentsRegister().getPlugin( EmissiveMapComponent::TypeName );
			parsingContext.sceneImportConfig.textureRemapIt = parsingContext.sceneImportConfig.textureRemaps.emplace( plugin.getTextureFlags(), TextureConfiguration{} ).first;
			parsingContext.sceneImportConfig.textureRemapIt->second = TextureConfiguration{};
		}
		CU_EndAttributePush( CSCNSection::eTextureRemapChannel )

		static CU_ImplementAttributeParser( parserTexRemapEmissiveMask )
		{
			auto & parsingContext = getParserContext( context );

			if ( params.empty() )
			{
				CU_ParsingError( cuT( "Missing parameter." ) );
			}
			else
			{
				auto & plugin = parsingContext.parser->getEngine()->getPassComponentsRegister().getPlugin( EmissiveMapComponent::TypeName );
				plugin.fillTextureConfiguration( parsingContext.sceneImportConfig.textureRemapIt->second
					, params[0]->get< uint32_t >() );
			}
		}
		CU_EndAttribute()
	}

	//*********************************************************************************************

	void EmissiveMapComponent::ComponentsShader::applyComponents( TextureCombine const & combine
		, PipelineFlags const * flags
		, shader::TextureConfigData const & config
		, sdw::U32Vec3 const & imgCompConfig
		, sdw::Vec4 const & sampled
		, shader::BlendComponents & components )const
	{
		if ( !components.hasMember( "emissive" ) )
		{
			return;
		}

		auto & writer{ *sampled.getWriter() };

		IF( writer, imgCompConfig.x() == sdw::UInt{ getTextureFlags() } )
		{
			components.getMember< sdw::Vec3 >( "emissive" ) *= config.getVec3( sampled, imgCompConfig.z() );
		}
		FI;
	}

	//*********************************************************************************************

	void EmissiveMapComponent::Plugin::createParsers( castor::AttributeParsers & parsers
		, ChannelFillers & channelFillers )const
	{
		channelFillers.emplace( "emissive", ChannelFiller{ getTextureFlags()
			, []( SceneFileContext & parsingContext )
			{
				auto & component = getPassComponent< EmissiveMapComponent >( parsingContext );
				component.fillChannel( parsingContext.textureConfiguration
					, 0x00FFFFFF );
			} } );

		Pass::addParserT( parsers
			, CSCNSection::eTextureUnit
			, cuT( "emissive_mask" )
			, emscmp::parserUnitEmissiveMask
			, { castor::makeParameter< castor::ParameterType::eUInt32 >() } );

		Pass::addParserT( parsers
			, CSCNSection::eTextureRemap
			, cuT( "emissive" )
			, emscmp::parserTexRemapEmissive );

		Pass::addParserT( parsers
			, CSCNSection::eTextureRemapChannel
			, cuT( "emissive_mask" )
			, emscmp::parserTexRemapEmissiveMask
			, { castor::makeParameter< castor::ParameterType::eUInt32 >() } );
	}

	bool EmissiveMapComponent::Plugin::writeTextureConfig( TextureConfiguration const & configuration
		, castor::String const & tabs
		, castor::StringStream & file )const
	{
		auto it = checkFlags( configuration.components, getTextureFlags() );

		if ( it == configuration.components.end() )
		{
			return true;
		}

		return castor::TextWriter< EmissiveMapComponent >{ tabs, it->componentsMask }( file );
	}

	bool EmissiveMapComponent::Plugin::isComponentNeeded( TextureCombine const & textures
		, ComponentModeFlags const & filter )const
	{
		return checkFlag( filter, ComponentModeFlag::eDiffuseLighting )
			|| checkFlag( filter, ComponentModeFlag::eSpecularLighting )
			|| checkFlags( textures, getTextureFlags() ) != textures.flags.end();
	}

	void EmissiveMapComponent::Plugin::createMapComponent( Pass & pass
		, std::vector< PassComponentUPtr > & result )const
	{
		result.push_back( std::make_unique< EmissiveMapComponent >( pass ) );
	}

	void EmissiveMapComponent::Plugin::doUpdateComponent( PassComponentRegister const & passComponents
		, TextureCombine const & combine
		, shader::BlendComponents & components )
	{
		auto & plugin = passComponents.getPlugin< EmissiveMapComponent >();

		if ( checkFlags( combine, plugin.getTextureFlags() ) == combine.flags.end() )
		{
			components.getMember< sdw::Vec3 >( "emissive", true ) *= components.getMember< sdw::Vec3 >( "colour", true );
		}
	}

	//*********************************************************************************************

	castor::String const EmissiveMapComponent::TypeName = C3D_MakePassMapComponentName( "emissive" );

	EmissiveMapComponent::EmissiveMapComponent( Pass & pass )
		: PassMapComponent{ pass, TypeName }
	{
		if ( !pass.hasComponent< EmissiveComponent >() )
		{
			pass.createComponent< EmissiveComponent >();
		}
	}

	void EmissiveMapComponent::fillConfig( TextureConfiguration & configuration
		, PassVisitorBase & vis )const
	{
		vis.visit( cuT( "Emissive" ), getTextureFlags(), getFlagConfiguration( configuration, getTextureFlags() ), 3u );
	}

	PassComponentUPtr EmissiveMapComponent::doClone( Pass & pass )const
	{
		return std::make_unique< EmissiveMapComponent >( pass );
	}

	//*********************************************************************************************
}
