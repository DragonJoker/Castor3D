#include "Castor3D/Material/Pass/Component/Map/ColourMapComponent.hpp"

#include "Castor3D/Engine.hpp"
#include "Castor3D/Material/Pass/Pass.hpp"
#include "Castor3D/Miscellaneous/ConfigurationVisitor.hpp"
#include "Castor3D/Material/Pass/Component/PassComponentRegister.hpp"
#include "Castor3D/Material/Pass/Component/Other/ColourComponent.hpp"
#include "Castor3D/Material/Texture/TextureConfiguration.hpp"
#include "Castor3D/Scene/SceneFileParserData.hpp"
#include "Castor3D/Scene/SceneImporter.hpp"
#include "Castor3D/Shader/ShaderBuffers/PassBuffer.hpp"
#include "Castor3D/Shader/Shaders/GlslBlendComponents.hpp"
#include "Castor3D/Shader/Shaders/GlslLighting.hpp"
#include "Castor3D/Shader/Shaders/GlslMaterial.hpp"
#include "Castor3D/Shader/Shaders/GlslTextureConfiguration.hpp"

#include <CastorUtils/FileParser/FileParser.hpp>
#include <CastorUtils/Data/Text/TextRgbColour.hpp>

namespace c3d
{
	template<>
	class TextWriter< ColourMapComponent >
		: public TextWriterT< ColourMapComponent >
	{
	public:
		explicit TextWriter( String const & tabs
			, uint32_t mask )
			: TextWriterT< ColourMapComponent >{ tabs }
			, m_mask{ mask }
		{
		}

		bool operator()( ColourMapComponent const & object
			, StringStream & file )override
		{
			return writeMask( file, cuT( "colour_mask" ), m_mask );
		}

		bool operator()( StringStream & file )const
		{
			return writeMask( file, cuT( "colour_mask" ), m_mask );
		}

	private:
		uint32_t m_mask;
	};
}

namespace c3d
{
	//*********************************************************************************************

	namespace colcmp
	{
		static CU_ImplementAttributeParserBlock( parserUnitColourMask, TextureContext )
		{
			if ( params.empty() )
			{
				CU_ParsingError( cuT( "Missing parameter." ) );
			}
			else if ( !blockContext->pass )
			{
				auto & plugin = getEngine( *blockContext )->getPassComponentsRegister().getPlugin( ColourMapComponent::TypeName );
				plugin.fillTextureConfiguration( blockContext->configuration
					, params[0]->get< uint32_t >() );
			}
			else
			{
				auto & plugin = blockContext->pass->pass->getComponentPlugin( ColourMapComponent::TypeName );
				plugin.fillTextureConfiguration( blockContext->configuration
					, params[0]->get< uint32_t >() );
			}
		}
		CU_EndAttribute()

		static CU_ImplementAttributeParserBlock( parserTexRemapColour, SceneImportContext )
		{
			auto & plugin = getEngine( *blockContext )->getPassComponentsRegister().getPlugin( ColourMapComponent::TypeName );
			blockContext->textureRemapIt = blockContext->textureRemaps.try_emplace( plugin.getTextureFlags() ).first;
			blockContext->textureRemapIt->second = TextureConfiguration{};
		}
		CU_EndAttributePushBlock( CSCNSection::eTextureRemapChannel, blockContext )

		static CU_ImplementAttributeParserBlock( parserTexRemapColourMask, SceneImportContext )
		{
			if ( params.empty() )
			{
				CU_ParsingError( cuT( "Missing parameter." ) );
			}
			else
			{
				auto & plugin = getEngine( *blockContext )->getPassComponentsRegister().getPlugin( ColourMapComponent::TypeName );
				plugin.fillTextureConfiguration( blockContext->textureRemapIt->second
					, params[0]->get< uint32_t >() );
			}
		}
		CU_EndAttribute()
	}

	//*********************************************************************************************

	void ColourMapComponent::ComponentsShader::applyTexture( shader::PassShaders const & passShaders
		, shader::TextureConfigurations const & textureConfigs
		, shader::TextureAnimations const & textureAnims
		, sdw::Array< sdw::CombinedImage2DRgba32 > const & maps
		, shader::Material const & material
		, shader::BlendComponents & components
		, shader::SampleTexture const & sampleTexture )const
	{
		applyVec3Component( cuT( "baseColour" )
			, passShaders
			, textureConfigs
			, textureAnims
			, material
			, components
			, sampleTexture );
	}

	//*********************************************************************************************

	void ColourMapComponent::Plugin::createParsers( AttributeParsers & parsers
		, ChannelFillers & channelFillers )const
	{
		channelFillers.try_emplace( cuT( "diffuse" )
			, getTextureFlags()
			, []( TextureContext & blockContext )
			{
				auto const & component = getPassComponent< ColourMapComponent >( blockContext );
				component.fillChannel( blockContext.configuration
					, 0x00FFFFFF );
			} );
		channelFillers.try_emplace( cuT( "albedo" )
			, getTextureFlags()
			, []( TextureContext & blockContext )
			{
				auto const & component = getPassComponent< ColourMapComponent >( blockContext );
				component.fillChannel( blockContext.configuration
					, 0x00FFFFFF );
			} );
		channelFillers.try_emplace( cuT( "colour" )
			, getTextureFlags()
			, []( TextureContext & blockContext )
			{
				auto const & component = getPassComponent< ColourMapComponent >( blockContext );
				component.fillChannel( blockContext.configuration
					, 0x00FFFFFF );
			} );

		c3d::addParserT( parsers
			, CSCNSection::eTexture
			, cuT( "colour_mask" )
			, colcmp::parserUnitColourMask
			, { makeParameter< ParameterType::eUInt32 >() } );
		c3d::addParserT( parsers
			, CSCNSection::eTexture
			, cuT( "albedo_mask" )
			, colcmp::parserUnitColourMask
			, { makeParameter< ParameterType::eUInt32 >() } );
		c3d::addParserT( parsers
			, CSCNSection::eTexture
			, cuT( "diffuse_mask" )
			, colcmp::parserUnitColourMask
			, { makeParameter< ParameterType::eUInt32 >() } );

		c3d::addParserT( parsers
			, CSCNSection::eTextureUnit
			, cuT( "colour_mask" )
			, colcmp::parserUnitColourMask
			, { makeParameter< ParameterType::eUInt32 >() } );
		c3d::addParserT( parsers
			, CSCNSection::eTextureUnit
			, cuT( "albedo_mask" )
			, colcmp::parserUnitColourMask
			, { makeParameter< ParameterType::eUInt32 >() } );
		c3d::addParserT( parsers
			, CSCNSection::eTextureUnit
			, cuT( "diffuse_mask" )
			, colcmp::parserUnitColourMask
			, { makeParameter< ParameterType::eUInt32 >() } );

		c3d::addParserT( parsers
			, CSCNSection::eTextureRemap
			, CSCNSection::eTextureRemapChannel
			, cuT( "colour" )
			, colcmp::parserTexRemapColour );
		c3d::addParserT( parsers
			, CSCNSection::eTextureRemap
			, CSCNSection::eTextureRemapChannel
			, cuT( "albedo" )
			, colcmp::parserTexRemapColour );
		c3d::addParserT( parsers
			, CSCNSection::eTextureRemap
			, CSCNSection::eTextureRemapChannel
			, cuT( "diffuse" )
			, colcmp::parserTexRemapColour );

		c3d::addParserT( parsers
			, CSCNSection::eTextureRemapChannel
			, cuT( "colour_mask" )
			, colcmp::parserTexRemapColourMask
			, { makeParameter< ParameterType::eUInt32 >() } );
		c3d::addParserT( parsers
			, CSCNSection::eTextureRemapChannel
			, cuT( "albedo_mask" )
			, colcmp::parserTexRemapColourMask
			, { makeParameter< ParameterType::eUInt32 >() } );
		c3d::addParserT( parsers
			, CSCNSection::eTextureRemapChannel
			, cuT( "diffuse_mask" )
			, colcmp::parserTexRemapColourMask
			, { makeParameter< ParameterType::eUInt32 >() } );
	}

	bool ColourMapComponent::Plugin::doWriteTextureConfig( TextureConfiguration const & configuration
		, uint32_t mask
		, String const & tabs
		, StringStream & file )const
	{
		return TextWriter< ColourMapComponent >{ tabs, mask }( file );
	}

	bool ColourMapComponent::Plugin::isComponentNeeded( TextureCombine const & textures
		, ComponentModeFlags const & filter )const
	{
		return checkFlag( filter, ComponentModeFlag::eColour )
			|| hasAny( textures, getTextureFlags() );
	}

	void ColourMapComponent::Plugin::createMapComponent( Pass & pass
		, Vector< PassComponentUPtr > & result )const
	{
		result.push_back( makeUniqueDerived< PassComponent, ColourMapComponent >( pass ) );
	}

	//*********************************************************************************************

	String const ColourMapComponent::TypeName = C3D_MakePassMapComponentName( "colour" );

	ColourMapComponent::ColourMapComponent( Pass & pass )
		: PassMapComponent{ pass
			, TypeName
			, Colour
			, { ColourComponent::TypeName } }
	{
	}

	PassComponentUPtr ColourMapComponent::doClone( Pass & pass )const
	{
		return makeUniqueDerived< PassComponent, ColourMapComponent >( pass );
	}

	void ColourMapComponent::doFillConfig( TextureConfiguration & configuration
		, ConfigurationVisitorBase & vis )const
	{
		vis.visit( cuT( "Colour" ) );
		vis.visit( cuT( "Map" ), getTextureFlags(), getFlagConfiguration( configuration, getTextureFlags() ), 3u );
	}

	PassMapDefaultImageParams ColourMapComponent::createDefaultImage()const
	{
		String name{ cuT( "DefaultColour" ) };
		ByteArray data{ 255u, 255u, 255u, 0u };
		return { name
			, ImageCreateParams{ getFormatName( PixelFormat::eR8G8B8A8_UNORM ), data } };
	}

	//*********************************************************************************************
}
