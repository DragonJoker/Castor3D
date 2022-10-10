#include "Castor3D/Material/Pass/Component/Map/ColourMapComponent.hpp"

#include "Castor3D/Engine.hpp"
#include "Castor3D/Material/Pass/Pass.hpp"
#include "Castor3D/Material/Pass/PassVisitor.hpp"
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
			return writeNamedSub( file, cuT( "colour_mask" ), m_mask );
		}

		bool operator()( StringStream & file )
		{
			return writeNamedSub( file, cuT( "colour_mask" ), m_mask );
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
				auto & component = getPassComponent< ColourMapComponent >( parsingContext );
				component.fillChannel( parsingContext.textureConfiguration
					, params[0]->get< uint32_t >() );
			}
		}
		CU_EndAttribute()

		static CU_ImplementAttributeParser( parserTexRemapColour )
		{
			auto & parsingContext = getParserContext( context );
			parsingContext.sceneImportConfig.textureRemapIt = parsingContext.sceneImportConfig.textureRemaps.emplace( TextureFlag::eColour, TextureConfiguration{} ).first;
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
				parsingContext.sceneImportConfig.textureRemapIt->second.colourMask[0] = params[0]->get< uint32_t >();
			}
		}
		CU_EndAttribute()
	}

	//*********************************************************************************************

	void ColourMapComponent::ComponentsShader::applyComponents( TextureFlags const & texturesFlags
		, PipelineFlags const * flags
		, shader::TextureConfigData const & config
		, sdw::Vec4 const & sampled
		, shader::BlendComponents const & components )const
	{
		if ( !components.hasMember( "colour" )
			|| !checkFlag( texturesFlags, TextureFlag::eColour ) )
		{
			return;
		}

		auto & writer{ *sampled.getWriter() };

		IF( writer, config.colEnbl() != 0.0_f )
		{
			components.getMember< sdw::Vec3 >( "colour" ) *= config.getVec3( sampled, config.colMask() );
		}
		FI;
	}

	//*********************************************************************************************

	void ColourMapComponent::Plugin::createParsers( castor::AttributeParsers & parsers
		, ChannelFillers & channelFillers )const
	{
		channelFillers.emplace( "diffuse", ChannelFiller{ uint32_t( TextureFlag::eColour )
			, []( SceneFileContext & parsingContext )
			{
				auto & component = getPassComponent< ColourMapComponent >( parsingContext );
				component.fillChannel( parsingContext.textureConfiguration
					, 0x00FFFFFF );
			} } );
		channelFillers.emplace( "albedo", ChannelFiller{ uint32_t( TextureFlag::eColour )
			, []( SceneFileContext & parsingContext )
			{
				auto & component = getPassComponent< ColourMapComponent >( parsingContext );
				component.fillChannel( parsingContext.textureConfiguration
					, 0x00FFFFFF );
			} } );
		channelFillers.emplace( "colour", ChannelFiller{ uint32_t( TextureFlag::eColour )
			, []( SceneFileContext & parsingContext )
			{
				auto & component = getPassComponent< ColourMapComponent >( parsingContext );
				component.fillChannel( parsingContext.textureConfiguration
					, 0x00FFFFFF );
			} } );

		Pass::addParserT( parsers
			, CSCNSection::eTextureUnit
			, cuT( "colour_mask" )
			, colcmp::parserUnitColourMask
			, { castor::makeParameter< castor::ParameterType::eUInt32 >() } );
		Pass::addParserT( parsers
			, CSCNSection::eTextureUnit
			, cuT( "albedo_mask" )
			, colcmp::parserUnitColourMask
			, { castor::makeParameter< castor::ParameterType::eUInt32 >() } );
		Pass::addParserT( parsers
			, CSCNSection::eTextureUnit
			, cuT( "diffuse_mask" )
			, colcmp::parserUnitColourMask
			, { castor::makeParameter< castor::ParameterType::eUInt32 >() } );

		Pass::addParserT( parsers
			, CSCNSection::eTextureRemap
			, cuT( "colour" )
			, colcmp::parserTexRemapColour );
		Pass::addParserT( parsers
			, CSCNSection::eTextureRemap
			, cuT( "albedo" )
			, colcmp::parserTexRemapColour );
		Pass::addParserT( parsers
			, CSCNSection::eTextureRemap
			, cuT( "diffuse" )
			, colcmp::parserTexRemapColour );

		Pass::addParserT( parsers
			, CSCNSection::eTextureRemapChannel
			, cuT( "colour_mask" )
			, colcmp::parserTexRemapColourMask
			, { castor::makeParameter< castor::ParameterType::eUInt32 >() } );
		Pass::addParserT( parsers
			, CSCNSection::eTextureRemapChannel
			, cuT( "albedo_mask" )
			, colcmp::parserTexRemapColourMask
			, { castor::makeParameter< castor::ParameterType::eUInt32 >() } );
		Pass::addParserT( parsers
			, CSCNSection::eTextureRemapChannel
			, cuT( "diffuse_mask" )
			, colcmp::parserTexRemapColourMask
			, { castor::makeParameter< castor::ParameterType::eUInt32 >() } );
	}

	bool ColourMapComponent::Plugin::writeTextureConfig( TextureConfiguration const & configuration
		, castor::String const & tabs
		, castor::StringStream & file )const
	{
		return castor::TextWriter< ColourMapComponent >{ tabs, configuration.colourMask[0] }( file );
	}

	bool ColourMapComponent::Plugin::isComponentNeeded( TextureFlags const & textures
		, ComponentModeFlags const & filter )const
	{
		return checkFlag( filter, ComponentModeFlag::eColour )
			|| checkFlag( textures, TextureFlag::eColour );
	}

	bool ColourMapComponent::Plugin::needsMapComponent( TextureConfiguration const & configuration )const
	{
		return configuration.colourMask[0] != 0u;
	}

	void ColourMapComponent::Plugin::createMapComponent( Pass & pass
		, std::vector< PassComponentUPtr > & result )const
	{
		result.push_back( std::make_unique< ColourMapComponent >( pass ) );
	}

	//*********************************************************************************************

	castor::String const ColourMapComponent::TypeName = C3D_MakePassMapComponentName( "colour" );

	ColourMapComponent::ColourMapComponent( Pass & pass )
		: PassMapComponent{ pass, TypeName }
	{
		if ( !pass.hasComponent< ColourComponent >() )
		{
			pass.createComponent< ColourComponent >();
		}
	}

	void ColourMapComponent::mergeImages( TextureUnitDataSet & result )
	{
		getOwner()->mergeImages( TextureFlag::eColour
			, offsetof( TextureConfiguration, colourMask )
			, 0x00FFFFFF
			, TextureFlag::eOpacity
			, offsetof( TextureConfiguration, opacityMask )
			, 0xFF000000
			, "ColOpa"
			, result );
	}

	void ColourMapComponent::fillChannel( TextureConfiguration & configuration
		, uint32_t mask )
	{
		configuration.textureSpace |= TextureSpace::eColour;
		configuration.textureSpace |= TextureSpace::eAllowSRGB;
		configuration.colourMask[0] = mask;
	}

	void ColourMapComponent::fillConfig( TextureConfiguration & configuration
		, PassVisitorBase & vis )
	{
		vis.visit( cuT( "Colour" ), TextureFlag::eColour, configuration.colourMask, 3u );
	}

	PassComponentUPtr ColourMapComponent::doClone( Pass & pass )const
	{
		return std::make_unique< ColourMapComponent >( pass );
	}

	//*********************************************************************************************
}
