#include "Castor3D/Material/Pass/Component/Map/AmbientColourMapComponent.hpp"

#include "Castor3D/Engine.hpp"
#include "Castor3D/Material/Pass/Pass.hpp"
#include "Castor3D/Miscellaneous/ConfigurationVisitor.hpp"
#include "Castor3D/Material/Pass/Component/PassComponentRegister.hpp"
#include "Castor3D/Material/Pass/Component/Lighting/AmbientComponent.hpp"
#include "Castor3D/Material/Texture/TextureConfiguration.hpp"
#include "Castor3D/Scene/SceneFileParserData.hpp"
#include "Castor3D/Scene/SceneImporter.hpp"
#include "Castor3D/Shader/Shaders/GlslBlendComponents.hpp"
#include "Castor3D/Shader/Shaders/GlslMaterial.hpp"
#include "Castor3D/Shader/Shaders/GlslTextureConfiguration.hpp"

#include <CastorUtils/FileParser/FileParser.hpp>

namespace c3d
{
	template<>
	class TextWriter< AmbientColourMapComponent >
		: public TextWriterT< AmbientColourMapComponent >
	{
	public:
		explicit TextWriter( String const & tabs
			, uint32_t mask )
			: TextWriterT< AmbientColourMapComponent >{ tabs }
			, m_mask{ mask }
		{
		}

		bool operator()( AmbientColourMapComponent const & object
			, StringStream & file )override
		{
			return writeMask( file, cuT( "ambient_colour_mask" ), m_mask );
		}

		bool operator()( StringStream & file )const
		{
			return writeMask( file, cuT( "ambient_colour_mask" ), m_mask );
		}

	private:
		uint32_t m_mask;
	};
}

namespace c3d
{
	//*********************************************************************************************

	namespace ambcolmap
	{
		static CU_ImplementAttributeParserBlock( parserUnitMask, TextureContext )
		{
			if ( params.empty() )
			{
				CU_ParsingError( cuT( "Missing parameter." ) );
			}
			else if ( !blockContext->pass )
			{
				auto & plugin = getEngine( *blockContext )->getPassComponentsRegister().getPlugin( AmbientColourMapComponent::TypeName );
				plugin.fillTextureConfiguration( blockContext->configuration
					, params[0]->get< uint32_t >() );
			}
			else
			{
				auto & plugin = blockContext->pass->pass->getComponentPlugin( AmbientColourMapComponent::TypeName );
				plugin.fillTextureConfiguration( blockContext->configuration
					, params[0]->get< uint32_t >() );
			}
		}
		CU_EndAttribute()

		static CU_ImplementAttributeParserBlock( parserTexRemap, SceneImportContext )
		{
			auto & plugin = getEngine( *blockContext )->getPassComponentsRegister().getPlugin( AmbientColourMapComponent::TypeName );
			blockContext->textureRemapIt = blockContext->textureRemaps.try_emplace( plugin.getTextureFlags() ).first;
			blockContext->textureRemapIt->second = TextureConfiguration{};
		}
		CU_EndAttributePushBlock( CSCNSection::eTextureRemapChannel, blockContext )

		static CU_ImplementAttributeParserBlock( parserTexRemapMask, SceneImportContext )
		{
			if ( params.empty() )
			{
				CU_ParsingError( cuT( "Missing parameter." ) );
			}
			else
			{
				auto & plugin = getEngine( *blockContext )->getPassComponentsRegister().getPlugin( AmbientColourMapComponent::TypeName );
				plugin.fillTextureConfiguration( blockContext->textureRemapIt->second
					, params[0]->get< uint32_t >() );
			}
		}
		CU_EndAttribute()
	}

	//*********************************************************************************************

	void AmbientColourMapComponent::ComponentsShader::applyTexture( shader::PassShaders const & passShaders
		, shader::TextureConfigurations const & textureConfigs
		, shader::TextureAnimations const & textureAnims
		, sdw::Array< sdw::CombinedImage2DRgba32 > const & maps
		, shader::Material const & material
		, shader::BlendComponents & components
		, shader::SampleTexture const & sampleTexture )const
	{
		applyVec3Component( cuT( "ambientColour" )
			, passShaders
			, textureConfigs
			, textureAnims
			, material
			, components
			, sampleTexture );
	}

	//*********************************************************************************************

	void AmbientColourMapComponent::Plugin::createParsers( AttributeParsers & parsers
		, ChannelFillers & channelFillers )const
	{
		channelFillers.try_emplace( cuT( "ambient_colour" )
			, getTextureFlags()
			, []( TextureContext & blockContext )
			{
				auto const & component = getPassComponent< AmbientColourMapComponent >( blockContext );
				component.fillChannel( blockContext.configuration, DefaultMask );
			} );

		c3d::addParserT( parsers
			, CSCNSection::eTexture
			, cuT( "ambient_colour_mask" )
			, ambcolmap::parserUnitMask
			, { makeParameter< ParameterType::eUInt32 >() } );

		c3d::addParserT( parsers
			, CSCNSection::eTextureUnit
			, cuT( "ambient_colour_mask" )
			, ambcolmap::parserUnitMask
			, { makeParameter< ParameterType::eUInt32 >() } );

		c3d::addParserT( parsers
			, CSCNSection::eTextureRemap
			, CSCNSection::eTextureRemapChannel
			, cuT( "ambient_colour" )
			, ambcolmap::parserTexRemap );

		c3d::addParserT( parsers
			, CSCNSection::eTextureRemapChannel
			, cuT( "ambient_colour_mask" )
			, ambcolmap::parserTexRemapMask
			, { makeParameter< ParameterType::eUInt32 >() } );
	}

	bool AmbientColourMapComponent::Plugin::doWriteTextureConfig( TextureConfiguration const & configuration
		, uint32_t mask
		, String const & tabs
		, StringStream & file )const
	{
		return TextWriter< AmbientColourMapComponent >{ tabs, mask }( file );
	}

	bool AmbientColourMapComponent::Plugin::isComponentNeeded( TextureCombine const & textures
		, ComponentModeFlags const & filter )const
	{
		return checkFlag( filter, ComponentModeFlag::eDiffuseLighting )
			|| hasAny( textures, getTextureFlags() );
	}

	void AmbientColourMapComponent::Plugin::createMapComponent( Pass & pass
		, Vector< PassComponentUPtr > & result )const
	{
		result.push_back( makeUniqueDerived< PassComponent, AmbientColourMapComponent >( pass ) );
	}

	//*********************************************************************************************

	String const AmbientColourMapComponent::TypeName = C3D_MakePassMapComponentName( "ambient_colour" );

	AmbientColourMapComponent::AmbientColourMapComponent( Pass & pass )
		: PassMapComponent{ pass
			, TypeName
			, Ambient
			, { AmbientComponent::TypeName } }
	{
	}

	PassComponentUPtr AmbientColourMapComponent::doClone( Pass & pass )const
	{
		return makeUniqueDerived< PassComponent, AmbientColourMapComponent >( pass );
	}

	void AmbientColourMapComponent::doFillConfig( TextureConfiguration & configuration
		, ConfigurationVisitorBase & vis )const
	{
		vis.visit( cuT( "AmbientColour" ) );
		vis.visit( cuT( "Map" ), getTextureFlags(), getFlagConfiguration( configuration, getTextureFlags() ), ComponentCount );
	}

	PassMapDefaultImageParams AmbientColourMapComponent::createDefaultImage()const
	{
		String name{ cuT( "DefaultAmbientColour" ) };
		ByteArray data{ 0u, 0u, 0u, 0u };
		return { name
			, ImageCreateParams{ getFormatName( PixelFormat::eR8G8B8A8_UNORM ), data } };
	}

	//*********************************************************************************************
}
