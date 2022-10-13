#include "Castor3D/Material/Pass/Component/Map/OpacityMapComponent.hpp"

#include "Castor3D/Engine.hpp"
#include "Castor3D/Material/Pass/Pass.hpp"
#include "Castor3D/Material/Pass/PassVisitor.hpp"
#include "Castor3D/Material/Pass/Component/PassComponentRegister.hpp"
#include "Castor3D/Material/Pass/Component/Map/ColourMapComponent.hpp"
#include "Castor3D/Material/Pass/Component/Other/OpacityComponent.hpp"
#include "Castor3D/Material/Texture/TextureConfiguration.hpp"
#include "Castor3D/Scene/SceneFileParser.hpp"
#include "Castor3D/Shader/ShaderBuffers/PassBuffer.hpp"
#include "Castor3D/Shader/Shaders/GlslBlendComponents.hpp"
#include "Castor3D/Shader/Shaders/GlslLighting.hpp"
#include "Castor3D/Shader/Shaders/GlslMaterial.hpp"
#include "Castor3D/Shader/Shaders/GlslTextureConfiguration.hpp"

#include <CastorUtils/FileParser/ParserParameter.hpp>
#include <CastorUtils/Data/Text/TextRgbColour.hpp>

namespace castor
{
	template<>
	class TextWriter< castor3d::OpacityMapComponent >
		: public TextWriterT< castor3d::OpacityMapComponent >
	{
	public:
		explicit TextWriter( String const & tabs
			, uint32_t mask )
			: TextWriterT< castor3d::OpacityMapComponent >{ tabs }
			, m_mask{ mask }
		{
		}

		bool operator()( castor3d::OpacityMapComponent const & object
			, StringStream & file )override
		{
			return writeMask( file, cuT( "opacity_mask" ), m_mask );
		}

		bool operator()( StringStream & file )
		{
			return writeMask( file, cuT( "opacity_mask" ), m_mask );
		}

	private:
		uint32_t m_mask;
	};
}

namespace castor3d
{
	//*********************************************************************************************

	namespace opacmp
	{
		static CU_ImplementAttributeParser( parserUnitOpacityMask )
		{
			auto & parsingContext = getParserContext( context );

			if ( params.empty() )
			{
				CU_ParsingError( cuT( "Missing parameter." ) );
			}
			else
			{
				auto & plugin = parsingContext.pass->getComponentPlugin( OpacityMapComponent::TypeName );
				plugin.fillTextureConfiguration( parsingContext.textureConfiguration
					, params[0]->get< uint32_t >() );
			}
		}
		CU_EndAttribute()

		static CU_ImplementAttributeParser( parserTexRemapOpacity )
		{
			auto & parsingContext = getParserContext( context );
			auto & plugin = parsingContext.parser->getEngine()->getPassComponentsRegister().getPlugin( OpacityMapComponent::TypeName );
			parsingContext.sceneImportConfig.textureRemapIt = parsingContext.sceneImportConfig.textureRemaps.emplace( plugin.getTextureFlags(), TextureConfiguration{} ).first;
			parsingContext.sceneImportConfig.textureRemapIt->second = TextureConfiguration{};
		}
		CU_EndAttributePush( CSCNSection::eTextureRemapChannel )

		static CU_ImplementAttributeParser( parserTexRemapOpacityMask )
		{
			auto & parsingContext = getParserContext( context );

			if ( params.empty() )
			{
				CU_ParsingError( cuT( "Missing parameter." ) );
			}
			else
			{
				auto & plugin = parsingContext.parser->getEngine()->getPassComponentsRegister().getPlugin( OpacityMapComponent::TypeName );
				plugin.fillTextureConfiguration( parsingContext.sceneImportConfig.textureRemapIt->second
					, params[0]->get< uint32_t >() );
			}
		}
		CU_EndAttribute()
	}

	//*********************************************************************************************

	void OpacityMapComponent::ComponentsShader::applyComponents( TextureFlagsArray const & texturesFlags
		, PipelineFlags const * flags
		, shader::TextureConfigData const & config
		, sdw::U32Vec3 const & imgCompConfig
		, sdw::Vec4 const & sampled
		, shader::BlendComponents & components )const
	{
		if ( !components.hasMember( "opacity" )
			|| texturesFlags.end() == checkFlags( texturesFlags, getTextureFlags() ) )
		{
			return;
		}

		auto & writer{ *sampled.getWriter() };

		IF( writer, imgCompConfig.y() != 0_u )
		{
			components.getMember< sdw::Float >( "opacity" ) *= config.getFloat( sampled, imgCompConfig.z() );
		}
		FI;
	}

	//*********************************************************************************************

	void OpacityMapComponent::Plugin::createParsers( castor::AttributeParsers & parsers
		, ChannelFillers & channelFillers )const
	{
		channelFillers.emplace( "opacity", ChannelFiller{ getTextureFlags()
			, []( SceneFileContext & parsingContext )
			{
				auto & component = getPassComponent< OpacityMapComponent >( parsingContext );
				component.fillChannel( parsingContext.textureConfiguration
					, 0xFF000000 );
			} } );

		Pass::addParserT( parsers
			, CSCNSection::eTextureUnit
			, cuT( "opacity_mask" )
			, opacmp::parserUnitOpacityMask
			, { castor::makeParameter< castor::ParameterType::eUInt32 >() } );

		Pass::addParserT( parsers
			, CSCNSection::eTextureRemap
			, cuT( "opacity" )
			, opacmp::parserTexRemapOpacity );

		Pass::addParserT( parsers
			, CSCNSection::eTextureRemapChannel
			, cuT( "opacity_mask" )
			, opacmp::parserTexRemapOpacityMask
			, { castor::makeParameter< castor::ParameterType::eUInt32 >() } );
	}

	bool OpacityMapComponent::Plugin::writeTextureConfig( TextureConfiguration const & configuration
		, castor::String const & tabs
		, castor::StringStream & file )const
	{
		auto it = checkFlags( configuration.components, getTextureFlags() );

		if ( it == configuration.components.end() )
		{
			return true;
		}

		return castor::TextWriter< OpacityMapComponent >{ tabs, it->componentsMask }( file );
	}

	bool OpacityMapComponent::Plugin::isComponentNeeded( TextureFlagsArray const & textures
		, ComponentModeFlags const & filter )const
	{
		return checkFlag( filter, ComponentModeFlag::eOpacity )
			|| textures.end() != checkFlags( textures, getTextureFlags() );
	}

	void OpacityMapComponent::Plugin::createMapComponent( Pass & pass
		, std::vector< PassComponentUPtr > & result )const
	{
		result.push_back( std::make_unique< OpacityMapComponent >( pass ) );
	}

	//*********************************************************************************************

	castor::String const OpacityMapComponent::TypeName = C3D_MakePassMapComponentName( "opacity" );

	OpacityMapComponent::OpacityMapComponent( Pass & pass )
		: PassMapComponent{ pass, TypeName }
	{
		if ( !pass.hasComponent< OpacityComponent >() )
		{
			pass.createComponent< OpacityComponent >();
		}
	}

	void OpacityMapComponent::fillConfig( TextureConfiguration & configuration
		, PassVisitorBase & vis )const
	{
		vis.visit( cuT( "Opacity" ), getTextureFlags(), getFlagConfiguration( configuration, getTextureFlags() ), 1u );
	}

	PassComponentUPtr OpacityMapComponent::doClone( Pass & pass )const
	{
		return std::make_unique< OpacityMapComponent >( pass );
	}

	//*********************************************************************************************
}
