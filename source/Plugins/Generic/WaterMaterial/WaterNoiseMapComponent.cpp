#include "WaterMaterial/WaterNoiseMapComponent.hpp"
#include "WaterMaterial/WaterComponent.hpp"

#include <Castor3D/Engine.hpp>
#include <Castor3D/Material/Pass/Pass.hpp>
#include <Castor3D/Miscellaneous/ConfigurationVisitor.hpp>
#include <Castor3D/Material/Pass/Component/PassComponentRegister.hpp>
#include <Castor3D/Material/Texture/TextureConfiguration.hpp>
#include <Castor3D/Scene/SceneFileParser.hpp>
#include <Castor3D/Shader/ShaderBuffers/PassBuffer.hpp>
#include <Castor3D/Shader/Shaders/GlslBlendComponents.hpp>
#include <Castor3D/Shader/Shaders/GlslLighting.hpp>
#include <Castor3D/Shader/Shaders/GlslMaterial.hpp>
#include <Castor3D/Shader/Shaders/GlslTextureConfiguration.hpp>

#include <CastorUtils/FileParser/ParserParameter.hpp>

CU_ImplementSmartPtr( water, WaterNoiseMapComponent )

namespace castor
{
	template<>
	class TextWriter< water::WaterNoiseMapComponent >
		: public TextWriterT< water::WaterNoiseMapComponent >
	{
	public:
		explicit TextWriter( String const & tabs
			, uint32_t mask = 0u )
			: TextWriterT< water::WaterNoiseMapComponent >{ tabs }
			, m_mask{ mask }
		{
		}

		bool operator()( StringStream & file )
		{
			return writeMask( file, cuT( "water_noise_mask" ), m_mask );
		}

		bool operator()( water::WaterNoiseMapComponent const & object
			, StringStream & file )override
		{
			return true;
		}

	private:
		uint32_t m_mask;
	};
}

namespace water
{
	using namespace castor3d;
	namespace c3d = castor3d::shader;

	//*********************************************************************************************

	namespace trscmp
	{
		static CU_ImplementAttributeParser( parserUnitWaterNoiseMask )
		{
			auto & parsingContext = getParserContext( context );

			if ( params.empty() )
			{
				CU_ParsingError( cuT( "Missing parameter." ) );
			}
			else if ( !parsingContext.pass )
			{
				auto & plugin = parsingContext.parser->getEngine()->getPassComponentsRegister().getPlugin( WaterNoiseMapComponent::TypeName );
				plugin.fillTextureConfiguration( parsingContext.texture.configuration
					, params[0]->get< uint32_t >() );
			}
			else
			{
				auto & plugin = parsingContext.pass->getComponentPlugin( WaterNoiseMapComponent::TypeName );
				plugin.fillTextureConfiguration( parsingContext.texture.configuration
					, params[0]->get< uint32_t >() );
			}
		}
		CU_EndAttribute()

			static CU_ImplementAttributeParser( parserTexRemapWaterNoise )
		{
			auto & parsingContext = getParserContext( context );
			auto & plugin = parsingContext.parser->getEngine()->getPassComponentsRegister().getPlugin( WaterNoiseMapComponent::TypeName );
			parsingContext.sceneImportConfig.textureRemapIt = parsingContext.sceneImportConfig.textureRemaps.emplace( plugin.getTextureFlags(), TextureConfiguration{} ).first;
			parsingContext.sceneImportConfig.textureRemapIt->second = TextureConfiguration{};
		}
		CU_EndAttributePush( CSCNSection::eTextureRemapChannel )

			static CU_ImplementAttributeParser( parserTexRemapWaterNoiseMask )
		{
			auto & parsingContext = getParserContext( context );

			if ( params.empty() )
			{
				CU_ParsingError( cuT( "Missing parameter." ) );
			}
			else
			{
				auto & plugin = parsingContext.parser->getEngine()->getPassComponentsRegister().getPlugin( WaterNoiseMapComponent::TypeName );
				plugin.fillTextureConfiguration( parsingContext.sceneImportConfig.textureRemapIt->second
					, params[0]->get< uint32_t >() );
			}
		}
		CU_EndAttribute()
	}

	//*********************************************************************************************

	void WaterNoiseMapComponent::ComponentsShader::applyComponents( TextureCombine const & combine
		, PipelineFlags const * flags
		, c3d::TextureConfigData const & config
		, sdw::U32Vec3 const & imgCompConfig
		, sdw::Vec4 const & sampled
		, c3d::BlendComponents & components )const
	{
		if ( !components.hasMember( "waterNoise" ) )
		{
			return;
		}

		auto & writer{ *sampled.getWriter() };

		IF( writer, imgCompConfig.x() == sdw::UInt{ getTextureFlags() } )
		{
			auto waterNoise = components.getMember< sdw::Float >( "waterNoise" );
			waterNoise = config.getFloat( sampled, imgCompConfig.z() );
		}
		FI;
	}

	//*********************************************************************************************

	void WaterNoiseMapComponent::Plugin::createParsers( castor::AttributeParsers & parsers
		, ChannelFillers & channelFillers )const
	{
		channelFillers.emplace( "clearcoat", ChannelFiller{ getTextureFlags()
			, []( SceneFileContext & parsingContext )
			{
				auto & component = getPassComponent< WaterNoiseMapComponent >( parsingContext );
				component.fillChannel( parsingContext.texture.configuration
					, 0x00FF0000u );
			} } );

		castor::addParserT( parsers
			, CSCNSection::eTexture
			, cuT( "water_noise_mask" )
			, trscmp::parserUnitWaterNoiseMask
			, { castor::makeParameter< castor::ParameterType::eUInt32 >() } );

		castor::addParserT( parsers
			, CSCNSection::eTextureUnit
			, cuT( "water_noise_mask" )
			, trscmp::parserUnitWaterNoiseMask
			, { castor::makeParameter< castor::ParameterType::eUInt32 >() } );

		castor::addParserT( parsers
			, CSCNSection::eTextureRemap
			, cuT( "water_noise" )
			, trscmp::parserTexRemapWaterNoise );

		castor::addParserT( parsers
			, CSCNSection::eTextureRemapChannel
			, cuT( "water_noise_mask" )
			, trscmp::parserTexRemapWaterNoiseMask
			, { castor::makeParameter< castor::ParameterType::eUInt32 >() } );
	}

	bool WaterNoiseMapComponent::Plugin::isComponentNeeded( TextureCombine const & textures
		, ComponentModeFlags const & filter )const
	{
		return checkFlag( filter, ComponentModeFlag::eSpecularLighting )
			|| hasAny( textures, getTextureFlags() );
	}

	void WaterNoiseMapComponent::Plugin::createMapComponent( Pass & pass
		, std::vector< PassComponentUPtr > & result )const
	{
		result.push_back( castor::makeUniqueDerived< PassComponent, WaterNoiseMapComponent >( pass ) );
	}

	bool WaterNoiseMapComponent::Plugin::doWriteTextureConfig( TextureConfiguration const & configuration
		, uint32_t mask
		, castor::String const & tabs
		, castor::StringStream & file )const
	{
		return castor::TextWriter< WaterNoiseMapComponent >{ tabs, mask }( file );
	}

	//*********************************************************************************************

	castor::String const WaterNoiseMapComponent::TypeName = C3D_MakePassMapComponentName( "water.noise" );

	WaterNoiseMapComponent::WaterNoiseMapComponent( Pass & pass )
		: PassMapComponent{ pass
		, TypeName
		, Noise
		, { WaterComponent::TypeName } }
	{
	}

	PassComponentUPtr WaterNoiseMapComponent::doClone( Pass & pass )const
	{
		return castor::makeUniqueDerived< PassComponent, WaterNoiseMapComponent >( pass );
	}

	void WaterNoiseMapComponent::doFillConfig( TextureConfiguration & configuration
		, ConfigurationVisitorBase & vis )const
	{
		vis.visit( cuT( "WaterNoise" ) );
		vis.visit( cuT( "Map" ), getTextureFlags(), getFlagConfiguration( configuration, getTextureFlags() ), 1u );
	}

	//*********************************************************************************************
}
