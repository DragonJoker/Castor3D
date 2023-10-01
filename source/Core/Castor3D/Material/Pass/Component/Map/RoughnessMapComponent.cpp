#include "Castor3D/Material/Pass/Component/Map/RoughnessMapComponent.hpp"

#include "Castor3D/Engine.hpp"
#include "Castor3D/Material/Pass/Pass.hpp"
#include "Castor3D/Miscellaneous/ConfigurationVisitor.hpp"
#include "Castor3D/Material/Pass/Component/PassComponentRegister.hpp"
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

		bool operator()( StringStream & file )
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
		static CU_ImplementAttributeParser( parserUnitRoughnessMask )
		{
			auto & parsingContext = getParserContext( context );

			if ( params.empty() )
			{
				CU_ParsingError( cuT( "Missing parameter." ) );
			}
			else if ( !parsingContext.pass )
			{
				auto & plugin = parsingContext.parser->getEngine()->getPassComponentsRegister().getPlugin( RoughnessMapComponent::TypeName );
				plugin.fillTextureConfiguration( parsingContext.texture.configuration
					, params[0]->get< uint32_t >() );
			}
			else
			{
				auto & plugin = parsingContext.pass->getComponentPlugin( RoughnessMapComponent::TypeName );
				plugin.fillTextureConfiguration( parsingContext.texture.configuration
					, params[0]->get< uint32_t >() );
			}
		}
		CU_EndAttribute()

		static CU_ImplementAttributeParser( parserTexRemapRoughness )
		{
			auto & parsingContext = getParserContext( context );
			auto & plugin = parsingContext.parser->getEngine()->getPassComponentsRegister().getPlugin( RoughnessMapComponent::TypeName );
			parsingContext.sceneImportConfig.textureRemapIt = parsingContext.sceneImportConfig.textureRemaps.emplace( plugin.getTextureFlags(), TextureConfiguration{} ).first;
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
				auto & plugin = parsingContext.parser->getEngine()->getPassComponentsRegister().getPlugin( RoughnessMapComponent::TypeName );
				plugin.fillTextureConfiguration( parsingContext.sceneImportConfig.textureRemapIt->second
					, params[0]->get< uint32_t >() );
			}
		}
		CU_EndAttribute()
	}

	//*********************************************************************************************

	void RoughnessMapComponent::ComponentsShader::applyComponents( TextureCombine const & combine
		, PipelineFlags const * flags
		, shader::TextureConfigData const & config
		, sdw::U32Vec3 const & imgCompConfig
		, sdw::Vec4 const & sampled
		, shader::BlendComponents & components )const
	{
		applyFloatComponent( "roughness"
			, getTextureFlags()
			, config
			, imgCompConfig
			, sampled
			, components );
	}

	//*********************************************************************************************

	void RoughnessMapComponent::Plugin::createParsers( castor::AttributeParsers & parsers
		, ChannelFillers & channelFillers )const
	{
		channelFillers.emplace( "roughness", ChannelFiller{ getTextureFlags()
			, []( SceneFileContext & parsingContext )
			{
				auto & component = getPassComponent< RoughnessMapComponent >( parsingContext );
				component.fillChannel( parsingContext.texture.configuration
					, 0x00FF0000 );
			} } );

		castor::addParserT( parsers
			, CSCNSection::eTexture
			, cuT( "roughness_mask" )
			, rghcmp::parserUnitRoughnessMask
			, { castor::makeParameter< castor::ParameterType::eUInt32 >() } );

		castor::addParserT( parsers
			, CSCNSection::eTextureUnit
			, cuT( "roughness_mask" )
			, rghcmp::parserUnitRoughnessMask
			, { castor::makeParameter< castor::ParameterType::eUInt32 >() } );

		castor::addParserT( parsers
			, CSCNSection::eTextureRemap
			, cuT( "roughness" )
			, rghcmp::parserTexRemapRoughness );

		castor::addParserT( parsers
			, CSCNSection::eTextureRemapChannel
			, cuT( "roughness_mask" )
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
		, std::vector< PassComponentUPtr > & result )const
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

	//*********************************************************************************************
}
