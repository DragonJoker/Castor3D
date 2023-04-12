#include "Castor3D/Material/Pass/Component/Map/ColourMapComponent.hpp"

#include "Castor3D/Engine.hpp"
#include "Castor3D/Material/Pass/Pass.hpp"
#include "Castor3D/Material/Pass/PassVisitor.hpp"
#include "Castor3D/Material/Pass/Component/PassComponentRegister.hpp"
#include "Castor3D/Material/Pass/Component/Other/ColourComponent.hpp"
#include "Castor3D/Material/Texture/TextureConfiguration.hpp"
#include "Castor3D/Scene/SceneFileParser.hpp"
#include "Castor3D/Shader/ShaderBuffers/PassBuffer.hpp"
#include "Castor3D/Shader/Shaders/GlslBlendComponents.hpp"
#include "Castor3D/Shader/Shaders/GlslLighting.hpp"
#include "Castor3D/Shader/Shaders/GlslMaterial.hpp"
#include "Castor3D/Shader/Shaders/GlslTextureConfiguration.hpp"

#include <CastorUtils/FileParser/ParserParameter.hpp>
#include <CastorUtils/Data/Text/TextRgbColour.hpp>

CU_ImplementSmartPtr( castor3d, ColourMapComponent )

namespace castor
{
	template<>
	class TextWriter< castor3d::ColourMapComponent >
		: public TextWriterT< castor3d::ColourMapComponent >
	{
	public:
		explicit TextWriter( String const & tabs
			, uint32_t mask )
			: TextWriterT< castor3d::ColourMapComponent >{ tabs }
			, m_mask{ mask }
		{
		}

		bool operator()( castor3d::ColourMapComponent const & object
			, StringStream & file )override
		{
			return writeMask( file, cuT( "colour_mask" ), m_mask );
		}

		bool operator()( StringStream & file )
		{
			return writeMask( file, cuT( "colour_mask" ), m_mask );
		}

	private:
		uint32_t m_mask;
	};
}

namespace castor3d
{
	//*********************************************************************************************

	namespace colcmp
	{
		static CU_ImplementAttributeParser( parserUnitColourMask )
		{
			auto & parsingContext = getParserContext( context );

			if ( params.empty() )
			{
				CU_ParsingError( cuT( "Missing parameter." ) );
			}
			else
			{
				auto & plugin = parsingContext.pass->getComponentPlugin( ColourMapComponent::TypeName );
				plugin.fillTextureConfiguration( parsingContext.textureConfiguration
					, params[0]->get< uint32_t >() );
			}
		}
		CU_EndAttribute()

		static CU_ImplementAttributeParser( parserTexRemapColour )
		{
			auto & parsingContext = getParserContext( context );
			auto & plugin = parsingContext.parser->getEngine()->getPassComponentsRegister().getPlugin( ColourMapComponent::TypeName );
			parsingContext.sceneImportConfig.textureRemapIt = parsingContext.sceneImportConfig.textureRemaps.emplace( plugin.getTextureFlags(), TextureConfiguration{} ).first;
			parsingContext.sceneImportConfig.textureRemapIt->second = TextureConfiguration{};
		}
		CU_EndAttributePush( CSCNSection::eTextureRemapChannel )

		static CU_ImplementAttributeParser( parserTexRemapColourMask )
		{
			auto & parsingContext = getParserContext( context );

			if ( params.empty() )
			{
				CU_ParsingError( cuT( "Missing parameter." ) );
			}
			else
			{
				auto & plugin = parsingContext.parser->getEngine()->getPassComponentsRegister().getPlugin( ColourMapComponent::TypeName );
				plugin.fillTextureConfiguration( parsingContext.sceneImportConfig.textureRemapIt->second
					, params[0]->get< uint32_t >() );
			}
		}
		CU_EndAttribute()
	}

	//*********************************************************************************************

	void ColourMapComponent::ComponentsShader::applyComponents( TextureCombine const & combine
		, PipelineFlags const * flags
		, shader::TextureConfigData const & config
		, sdw::U32Vec3 const & imgCompConfig
		, sdw::Vec4 const & sampled
		, shader::BlendComponents & components )const
	{
		applyVec3Component( "colour"
			, getTextureFlags()
			, config
			, imgCompConfig
			, sampled
			, components );
	}

	//*********************************************************************************************

	void ColourMapComponent::Plugin::createParsers( castor::AttributeParsers & parsers
		, ChannelFillers & channelFillers )const
	{
		channelFillers.emplace( "diffuse", ChannelFiller{ getTextureFlags()
			, []( SceneFileContext & parsingContext )
			{
				auto & component = getPassComponent< ColourMapComponent >( parsingContext );
				component.fillChannel( parsingContext.textureConfiguration
					, 0x00FFFFFF );
			} } );
		channelFillers.emplace( "albedo", ChannelFiller{ getTextureFlags()
			, []( SceneFileContext & parsingContext )
			{
				auto & component = getPassComponent< ColourMapComponent >( parsingContext );
				component.fillChannel( parsingContext.textureConfiguration
					, 0x00FFFFFF );
			} } );
		channelFillers.emplace( "colour", ChannelFiller{ getTextureFlags()
			, []( SceneFileContext & parsingContext )
			{
				auto & component = getPassComponent< ColourMapComponent >( parsingContext );
				component.fillChannel( parsingContext.textureConfiguration
					, 0x00FFFFFF );
			} } );

		castor::addParserT( parsers
			, CSCNSection::eTextureUnit
			, cuT( "colour_mask" )
			, colcmp::parserUnitColourMask
			, { castor::makeParameter< castor::ParameterType::eUInt32 >() } );
		castor::addParserT( parsers
			, CSCNSection::eTextureUnit
			, cuT( "albedo_mask" )
			, colcmp::parserUnitColourMask
			, { castor::makeParameter< castor::ParameterType::eUInt32 >() } );
		castor::addParserT( parsers
			, CSCNSection::eTextureUnit
			, cuT( "diffuse_mask" )
			, colcmp::parserUnitColourMask
			, { castor::makeParameter< castor::ParameterType::eUInt32 >() } );

		castor::addParserT( parsers
			, CSCNSection::eTextureRemap
			, cuT( "colour" )
			, colcmp::parserTexRemapColour );
		castor::addParserT( parsers
			, CSCNSection::eTextureRemap
			, cuT( "albedo" )
			, colcmp::parserTexRemapColour );
		castor::addParserT( parsers
			, CSCNSection::eTextureRemap
			, cuT( "diffuse" )
			, colcmp::parserTexRemapColour );

		castor::addParserT( parsers
			, CSCNSection::eTextureRemapChannel
			, cuT( "colour_mask" )
			, colcmp::parserTexRemapColourMask
			, { castor::makeParameter< castor::ParameterType::eUInt32 >() } );
		castor::addParserT( parsers
			, CSCNSection::eTextureRemapChannel
			, cuT( "albedo_mask" )
			, colcmp::parserTexRemapColourMask
			, { castor::makeParameter< castor::ParameterType::eUInt32 >() } );
		castor::addParserT( parsers
			, CSCNSection::eTextureRemapChannel
			, cuT( "diffuse_mask" )
			, colcmp::parserTexRemapColourMask
			, { castor::makeParameter< castor::ParameterType::eUInt32 >() } );
	}

	bool ColourMapComponent::Plugin::doWriteTextureConfig( TextureConfiguration const & configuration
		, uint32_t mask
		, castor::String const & tabs
		, castor::StringStream & file )const
	{
		return castor::TextWriter< ColourMapComponent >{ tabs, mask }( file );
	}

	bool ColourMapComponent::Plugin::isComponentNeeded( TextureCombine const & textures
		, ComponentModeFlags const & filter )const
	{
		return checkFlag( filter, ComponentModeFlag::eColour )
			|| hasAny( textures, getTextureFlags() );
	}

	void ColourMapComponent::Plugin::createMapComponent( Pass & pass
		, std::vector< PassComponentUPtr > & result )const
	{
		result.push_back( castor::makeUniqueDerived< PassComponent, ColourMapComponent >( pass ) );
	}

	//*********************************************************************************************

	castor::String const ColourMapComponent::TypeName = C3D_MakePassMapComponentName( "colour" );

	ColourMapComponent::ColourMapComponent( Pass & pass )
		: PassMapComponent{ pass
			, TypeName
			, Colour
			, { ColourComponent::TypeName } }
	{
	}

	PassComponentUPtr ColourMapComponent::doClone( Pass & pass )const
	{
		return castor::makeUniqueDerived< PassComponent, ColourMapComponent >( pass );
	}

	void ColourMapComponent::doFillConfig( TextureConfiguration & configuration
		, PassVisitorBase & vis )const
	{
		vis.visit( cuT( "Colour" ) );
		vis.visit( cuT( "Map" ), getTextureFlags(), getFlagConfiguration( configuration, getTextureFlags() ), 3u );
	}

	//*********************************************************************************************
}
