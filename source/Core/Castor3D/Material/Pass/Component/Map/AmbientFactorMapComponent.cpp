#include "Castor3D/Material/Pass/Component/Map/AmbientFactorMapComponent.hpp"

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
	class TextWriter< AmbientFactorMapComponent >
		: public TextWriterT< AmbientFactorMapComponent >
	{
	public:
		explicit TextWriter( String const & tabs
			, uint32_t mask )
			: TextWriterT< AmbientFactorMapComponent >{ tabs }
			, m_mask{ mask }
		{
		}

		bool operator()( AmbientFactorMapComponent const & object
			, StringStream & file )override
		{
			return writeMask( file, cuT( "ambient_factor_mask" ), m_mask );
		}

		bool operator()( StringStream & file )const
		{
			return writeMask( file, cuT( "ambient_factor_mask" ), m_mask );
		}

	private:
		uint32_t m_mask;
	};
}

namespace c3d
{
	//*********************************************************************************************

	namespace ambfacmap
	{
		static CU_ImplementAttributeParserBlock( parserUnitMask, TextureContext )
		{
			if ( params.empty() )
			{
				CU_ParsingError( cuT( "Missing parameter." ) );
			}
			else if ( !blockContext->pass )
			{
				auto & plugin = getEngine( *blockContext )->getPassComponentsRegister().getPlugin( AmbientFactorMapComponent::TypeName );
				plugin.fillTextureConfiguration( blockContext->configuration
					, params[0]->get< uint32_t >() );
			}
			else
			{
				auto & plugin = blockContext->pass->pass->getComponentPlugin( AmbientFactorMapComponent::TypeName );
				plugin.fillTextureConfiguration( blockContext->configuration
					, params[0]->get< uint32_t >() );
			}
		}
		CU_EndAttribute()

		static CU_ImplementAttributeParserBlock( parserTexRemap, SceneImportContext )
		{
			auto & plugin = getEngine( *blockContext )->getPassComponentsRegister().getPlugin( AmbientFactorMapComponent::TypeName );
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
				auto & plugin = getEngine( *blockContext )->getPassComponentsRegister().getPlugin( AmbientFactorMapComponent::TypeName );
				plugin.fillTextureConfiguration( blockContext->textureRemapIt->second
					, params[0]->get< uint32_t >() );
			}
		}
		CU_EndAttribute()
	}

	//*********************************************************************************************

	void AmbientFactorMapComponent::ComponentsShader::applyTexture( shader::PassShaders const & passShaders
		, shader::TextureConfigurations const & textureConfigs
		, shader::TextureAnimations const & textureAnims
		, sdw::Array< sdw::CombinedImage2DRgba32 > const & maps
		, shader::Material const & material
		, shader::BlendComponents & components
		, shader::SampleTexture const & sampleTexture )const
	{
		applyFloatComponent( cuT( "ambientFactor" )
			, passShaders
			, textureConfigs
			, textureAnims
			, material
			, components
			, sampleTexture );
	}

	//*********************************************************************************************

	void AmbientFactorMapComponent::Plugin::createParsers( AttributeParsers & parsers
		, ChannelFillers & channelFillers )const
	{
		channelFillers.try_emplace( cuT( "ambient_factor" )
			, getTextureFlags()
			, []( TextureContext & blockContext )
			{
				auto const & component = getPassComponent< AmbientFactorMapComponent >( blockContext );
				component.fillChannel( blockContext.configuration, DefaultMask );
			} );

		c3d::addParserT( parsers
			, CSCNSection::eTexture
			, cuT( "ambient_factor_mask" )
			, ambfacmap::parserUnitMask
			, { makeParameter< ParameterType::eUInt32 >() } );

		c3d::addParserT( parsers
			, CSCNSection::eTextureUnit
			, cuT( "ambient_factor_mask" )
			, ambfacmap::parserUnitMask
			, { makeParameter< ParameterType::eUInt32 >() } );

		c3d::addParserT( parsers
			, CSCNSection::eTextureRemap
			, CSCNSection::eTextureRemapChannel
			, cuT( "ambient_factor" )
			, ambfacmap::parserTexRemap );

		c3d::addParserT( parsers
			, CSCNSection::eTextureRemapChannel
			, cuT( "ambient_factor_mask" )
			, ambfacmap::parserTexRemapMask
			, { makeParameter< ParameterType::eUInt32 >() } );
	}

	bool AmbientFactorMapComponent::Plugin::doWriteTextureConfig( TextureConfiguration const & configuration
		, uint32_t mask
		, String const & tabs
		, StringStream & file )const
	{
		return TextWriter< AmbientFactorMapComponent >{ tabs, mask }( file );
	}

	bool AmbientFactorMapComponent::Plugin::isComponentNeeded( TextureCombine const & textures
		, ComponentModeFlags const & filter )const
	{
		return checkFlag( filter, ComponentModeFlag::eDiffuseLighting )
			|| hasAny( textures, getTextureFlags() );
	}

	void AmbientFactorMapComponent::Plugin::createMapComponent( Pass & pass
		, Vector< PassComponentUPtr > & result )const
	{
		result.push_back( makeUniqueDerived< PassComponent, AmbientFactorMapComponent >( pass ) );
	}

	//*********************************************************************************************

	String const AmbientFactorMapComponent::TypeName = C3D_MakePassMapComponentName( "ambient_factor" );

	AmbientFactorMapComponent::AmbientFactorMapComponent( Pass & pass )
		: PassMapComponent{ pass
			, TypeName
			, Ambient
			, { AmbientComponent::TypeName } }
	{
	}

	PassComponentUPtr AmbientFactorMapComponent::doClone( Pass & pass )const
	{
		return makeUniqueDerived< PassComponent, AmbientFactorMapComponent >( pass );
	}

	void AmbientFactorMapComponent::doFillConfig( TextureConfiguration & configuration
		, ConfigurationVisitorBase & vis )const
	{
		vis.visit( cuT( "AmbientFactor" ) );
		vis.visit( cuT( "Map" ), getTextureFlags(), getFlagConfiguration( configuration, getTextureFlags() ), ComponentCount );
	}

	PassMapDefaultImageParams AmbientFactorMapComponent::createDefaultImage()const
	{
		String name{ cuT( "DefaultAmbientFactor" ) };
		ByteArray data{ 0u };
		return { name
			, ImageCreateParams{ getFormatName( PixelFormat::eR8_UNORM ), data } };
	}

	//*********************************************************************************************
}
