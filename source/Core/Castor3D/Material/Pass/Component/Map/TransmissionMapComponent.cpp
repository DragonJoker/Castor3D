#include "Castor3D/Material/Pass/Component/Map/TransmissionMapComponent.hpp"

#include "Castor3D/Engine.hpp"
#include "Castor3D/Material/Pass/Pass.hpp"
#include "Castor3D/Material/Pass/PassVisitor.hpp"
#include "Castor3D/Material/Pass/Component/PassComponentRegister.hpp"
#include "Castor3D/Material/Pass/Component/Lighting/TransmissionComponent.hpp"
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
	class TextWriter< castor3d::TransmissionMapComponent >
		: public TextWriterT< castor3d::TransmissionMapComponent >
	{
	public:
		explicit TextWriter( String const & tabs
			, uint32_t mask = 0u )
			: TextWriterT< castor3d::TransmissionMapComponent >{ tabs }
			, m_mask{ mask }
		{
		}

		bool operator()( StringStream & file )
		{
			return writeMask( file, cuT( "transmission_mask" ), m_mask );
		}

		bool operator()( castor3d::TransmissionMapComponent const & object
			, StringStream & file )override
		{
			return true;
		}

	private:
		uint32_t m_mask;
	};
}

namespace castor3d
{
	//*********************************************************************************************

	namespace trscmp
	{
		static CU_ImplementAttributeParser( parserUnitTransmissionMask )
		{
			auto & parsingContext = getParserContext( context );

			if ( params.empty() )
			{
				CU_ParsingError( cuT( "Missing parameter." ) );
			}
			else
			{
				auto & plugin = parsingContext.pass->getComponentPlugin( TransmissionMapComponent::TypeName );
				plugin.fillTextureConfiguration( parsingContext.textureConfiguration
					, params[0]->get< uint32_t >() );
			}
		}
		CU_EndAttribute()

		static CU_ImplementAttributeParser( parserTexRemapTransmission )
		{
			auto & parsingContext = getParserContext( context );
			auto & plugin = parsingContext.parser->getEngine()->getPassComponentsRegister().getPlugin( TransmissionMapComponent::TypeName );
			parsingContext.sceneImportConfig.textureRemapIt = parsingContext.sceneImportConfig.textureRemaps.emplace( plugin.getTextureFlags(), TextureConfiguration{} ).first;
			parsingContext.sceneImportConfig.textureRemapIt->second = TextureConfiguration{};
		}
		CU_EndAttributePush( CSCNSection::eTextureRemapChannel )

		static CU_ImplementAttributeParser( parserTexRemapTransmissionMask )
		{
			auto & parsingContext = getParserContext( context );

			if ( params.empty() )
			{
				CU_ParsingError( cuT( "Missing parameter." ) );
			}
			else
			{
				auto & plugin = parsingContext.parser->getEngine()->getPassComponentsRegister().getPlugin( TransmissionMapComponent::TypeName );
				plugin.fillTextureConfiguration( parsingContext.sceneImportConfig.textureRemapIt->second
					, params[0]->get< uint32_t >() );
			}
		}
		CU_EndAttribute()
	}

	//*********************************************************************************************

	void TransmissionMapComponent::ComponentsShader::applyComponents( TextureCombine const & combine
		, PipelineFlags const * flags
		, shader::TextureConfigData const & config
		, sdw::U32Vec3 const & imgCompConfig
		, sdw::Vec4 const & sampled
		, shader::BlendComponents & components )const
	{
		if ( !components.hasMember(  "transmission" ) )
		{
			return;
		}

		auto & writer{ *sampled.getWriter() };

		IF( writer, imgCompConfig.x() == sdw::UInt{ getTextureFlags() } )
		{
			auto transmission = components.getMember< sdw::Float >( "transmission" );
			transmission *= config.getFloat( sampled, imgCompConfig.z() );
		}
		FI;
	}

	//*********************************************************************************************

	void TransmissionMapComponent::Plugin::createParsers( castor::AttributeParsers & parsers
		, ChannelFillers & channelFillers )const
	{
		channelFillers.emplace( "transmission", ChannelFiller{ getTextureFlags()
			, []( SceneFileContext & parsingContext )
			{
				auto & component = getPassComponent< TransmissionMapComponent >( parsingContext );
				component.fillChannel( parsingContext.textureConfiguration
					, 0x00FF0000 );
			} } );

		Pass::addParserT( parsers
			, CSCNSection::eTextureUnit
			, cuT( "transmission_mask" )
			, trscmp::parserUnitTransmissionMask
			, { castor::makeParameter< castor::ParameterType::eUInt32 >() } );

		Pass::addParserT( parsers
			, CSCNSection::eTextureRemap
			, cuT( "transmission" )
			, trscmp::parserTexRemapTransmission );

		Pass::addParserT( parsers
			, CSCNSection::eTextureRemapChannel
			, cuT( "transmission_mask" )
			, trscmp::parserTexRemapTransmissionMask
			, { castor::makeParameter< castor::ParameterType::eUInt32 >() } );
	}

	bool TransmissionMapComponent::Plugin::isComponentNeeded( TextureCombine const & textures
		, ComponentModeFlags const & filter )const
	{
		return checkFlag( filter, ComponentModeFlag::eOpacity )
			|| hasAny( textures, getTextureFlags() );
	}

	void TransmissionMapComponent::Plugin::createMapComponent( Pass & pass
		, std::vector< PassComponentUPtr > & result )const
	{
		result.push_back( std::make_unique< TransmissionMapComponent >( pass ) );
	}

	bool TransmissionMapComponent::Plugin::doWriteTextureConfig( TextureConfiguration const & configuration
		, uint32_t mask
		, castor::String const & tabs
		, castor::StringStream & file )const
	{
		return castor::TextWriter< TransmissionMapComponent >{ tabs, mask }( file );
	}

	//*********************************************************************************************

	castor::String const TransmissionMapComponent::TypeName = C3D_MakePassMapComponentName( "transmission" );

	TransmissionMapComponent::TransmissionMapComponent( Pass & pass )
		: PassMapComponent{ pass, TypeName }
	{
		if ( !pass.hasComponent< TransmissionComponent >() )
		{
			pass.createComponent< TransmissionComponent >();
		}
	}

	PassComponentUPtr TransmissionMapComponent::doClone( Pass & pass )const
	{
		return std::make_unique< TransmissionMapComponent >( pass );
	}

	void TransmissionMapComponent::doFillConfig( TextureConfiguration & configuration
		, PassVisitorBase & vis )const
	{
		vis.visit( cuT( "Transmission" ), getTextureFlags(), getFlagConfiguration( configuration, getTextureFlags() ), 1u );
	}

	//*********************************************************************************************
}
