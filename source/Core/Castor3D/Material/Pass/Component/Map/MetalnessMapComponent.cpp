#include "Castor3D/Material/Pass/Component/Map/MetalnessMapComponent.hpp"

#include "Castor3D/Engine.hpp"
#include "Castor3D/Material/Pass/Pass.hpp"
#include "Castor3D/Material/Pass/PassVisitor.hpp"
#include "Castor3D/Material/Pass/Component/PassComponentRegister.hpp"
#include "Castor3D/Material/Pass/Component/Map/SpecularMapComponent.hpp"
#include "Castor3D/Material/Pass/Component/Lighting/MetalnessComponent.hpp"
#include "Castor3D/Material/Pass/Component/PassComponentRegister.hpp"
#include "Castor3D/Material/Texture/TextureConfiguration.hpp"
#include "Castor3D/Scene/SceneFileParser.hpp"
#include "Castor3D/Shader/ShaderBuffers/PassBuffer.hpp"
#include "Castor3D/Shader/Shaders/GlslBlendComponents.hpp"
#include "Castor3D/Shader/Shaders/GlslLighting.hpp"
#include "Castor3D/Shader/Shaders/GlslMaterial.hpp"
#include "Castor3D/Shader/Shaders/GlslTextureConfiguration.hpp"

#include <CastorUtils/FileParser/ParserParameter.hpp>

CU_ImplementSmartPtr( castor3d, MetalnessMapComponent )

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
			return writeMask( file, cuT( "metalness_mask" ), m_mask );
		}

		bool operator()( StringStream & file )
		{
			return writeMask( file, cuT( "metalness_mask" ), m_mask );
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
				auto & plugin = parsingContext.pass->getComponentPlugin( MetalnessMapComponent::TypeName );
				plugin.fillTextureConfiguration( parsingContext.textureConfiguration
					, params[0]->get< uint32_t >() );
			}
		}
		CU_EndAttribute()

		static CU_ImplementAttributeParser( parserTexRemapMetalness )
		{
			auto & parsingContext = getParserContext( context );
			auto & plugin = parsingContext.parser->getEngine()->getPassComponentsRegister().getPlugin( MetalnessMapComponent::TypeName );
			parsingContext.sceneImportConfig.textureRemapIt = parsingContext.sceneImportConfig.textureRemaps.emplace( plugin.getTextureFlags(), TextureConfiguration{} ).first;
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
				auto & plugin = parsingContext.parser->getEngine()->getPassComponentsRegister().getPlugin( MetalnessMapComponent::TypeName );
				plugin.fillTextureConfiguration( parsingContext.sceneImportConfig.textureRemapIt->second
					, params[0]->get< uint32_t >() );
			}
		}
		CU_EndAttribute()
	}

	//*********************************************************************************************

	void MetalnessMapComponent::ComponentsShader::applyComponents( TextureCombine const & combine
		, PipelineFlags const * flags
		, shader::TextureConfigData const & config
		, sdw::U32Vec3 const & imgCompConfig
		, sdw::Vec4 const & sampled
		, shader::BlendComponents & components )const
	{
		applyFloatComponent( "metalness"
			, getTextureFlags()
			, config
			, imgCompConfig
			, sampled
			, components );
	}

	//*********************************************************************************************

	void MetalnessMapComponent::Plugin::createParsers( castor::AttributeParsers & parsers
		, ChannelFillers & channelFillers )const
	{
		channelFillers.emplace( "metalness", ChannelFiller{ getTextureFlags()
			, []( SceneFileContext & parsingContext )
			{
				auto & component = getPassComponent< MetalnessMapComponent >( parsingContext );
				component.fillChannel( parsingContext.textureConfiguration
					, 0x00FF0000 );
			} } );

		castor::addParserT( parsers
			, CSCNSection::eTextureUnit
			, cuT( "metalness_mask" )
			, mtlcmp::parserUnitMetalnessMask
			, { castor::makeParameter< castor::ParameterType::eUInt32 >() } );

		castor::addParserT( parsers
			, CSCNSection::eTextureRemap
			, cuT( "metalness" )
			, mtlcmp::parserTexRemapMetalness );

		castor::addParserT( parsers
			, CSCNSection::eTextureRemapChannel
			, cuT( "metalness_mask" )
			, mtlcmp::parserTexRemapMetalnessMask
			, { castor::makeParameter< castor::ParameterType::eUInt32 >() } );
	}

	bool MetalnessMapComponent::Plugin::isComponentNeeded( TextureCombine const & textures
		, ComponentModeFlags const & filter )const
	{
		return checkFlag( filter, ComponentModeFlag::eOpacity )
			|| checkFlag( filter, ComponentModeFlag::eSpecularLighting )
			|| hasAny( textures, getTextureFlags() );
	}

	void MetalnessMapComponent::Plugin::createMapComponent( Pass & pass
		, std::vector< PassComponentUPtr > & result )const
	{
		result.push_back( castor::makeUniqueDerived< PassComponent, MetalnessMapComponent >( pass ) );
	}

	bool MetalnessMapComponent::Plugin::doWriteTextureConfig( TextureConfiguration const & configuration
		, uint32_t mask
		, castor::String const & tabs
		, castor::StringStream & file )const
	{
		return castor::TextWriter< MetalnessMapComponent >{ tabs, mask }( file );
	}

	void MetalnessMapComponent::Plugin::doUpdateComponent( PassComponentRegister const & passComponents
		, TextureCombine const & combine
		, shader::BlendComponents & components )
	{
		auto & plugin = passComponents.getPlugin< MetalnessMapComponent >();
		auto & spcPlugin = passComponents.getPlugin< SpecularMapComponent >();

		if ( !hasAny( combine, plugin.getTextureFlags() )
			&& hasAny( combine, spcPlugin.getTextureFlags() ) )
		{
			components.getMember< sdw::Float >( "metalness", true ) = length( components.getMember< sdw::Vec3 >( "specular", true ) );
		}
	}

	//*********************************************************************************************

	castor::String const MetalnessMapComponent::TypeName = C3D_MakePassMapComponentName( "metalness" );

	MetalnessMapComponent::MetalnessMapComponent( Pass & pass )
		: PassMapComponent{ pass
			, TypeName
			, Metalness
			, { MetalnessComponent::TypeName } }
	{
	}

	PassComponentUPtr MetalnessMapComponent::doClone( Pass & pass )const
	{
		return castor::makeUniqueDerived< PassComponent, MetalnessMapComponent >( pass );
	}

	void MetalnessMapComponent::doFillConfig( TextureConfiguration & configuration
		, PassVisitorBase & vis )const
	{
		vis.visit( cuT( "Metalness" ) );
		vis.visit( cuT( "Map" ), getTextureFlags(), getFlagConfiguration( configuration, getTextureFlags() ), 1u );
	}

	//*********************************************************************************************
}
