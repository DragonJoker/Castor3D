#include "Castor3D/Material/Pass/Component/Map/MetalnessMapComponent.hpp"

#include "Castor3D/Engine.hpp"
#include "Castor3D/Material/Pass/Pass.hpp"
#include "Castor3D/Miscellaneous/ConfigurationVisitor.hpp"
#include "Castor3D/Material/Pass/Component/PassComponentRegister.hpp"
#include "Castor3D/Material/Pass/Component/Map/SpecularMapComponent.hpp"
#include "Castor3D/Material/Pass/Component/Lighting/MetalnessComponent.hpp"
#include "Castor3D/Material/Pass/Component/PassComponentRegister.hpp"
#include "Castor3D/Material/Texture/TextureConfiguration.hpp"
#include "Castor3D/Scene/SceneFileParserData.hpp"
#include "Castor3D/Scene/SceneImporter.hpp"
#include "Castor3D/Shader/ShaderBuffers/PassBuffer.hpp"
#include "Castor3D/Shader/Shaders/GlslBlendComponents.hpp"
#include "Castor3D/Shader/Shaders/GlslLighting.hpp"
#include "Castor3D/Shader/Shaders/GlslMaterial.hpp"
#include "Castor3D/Shader/Shaders/GlslTextureConfiguration.hpp"

#include <CastorUtils/FileParser/FileParser.hpp>

namespace c3d
{
	template<>
	class TextWriter< MetalnessMapComponent >
		: public TextWriterT< MetalnessMapComponent >
	{
	public:
		explicit TextWriter( String const & tabs
			, uint32_t mask )
			: TextWriterT< MetalnessMapComponent >{ tabs }
			, m_mask{ mask }
		{
		}

		bool operator()( MetalnessMapComponent const & object
			, StringStream & file )override
		{
			return writeMask( file, cuT( "metalness_mask" ), m_mask );
		}

		bool operator()( StringStream & file )const
		{
			return writeMask( file, cuT( "metalness_mask" ), m_mask );
		}

	private:
		uint32_t m_mask;
	};

	//*********************************************************************************************

	namespace mtlcmp
	{
		static CU_ImplementAttributeParserBlock( parserUnitMetalnessMask, TextureContext )
		{
			if ( params.empty() )
			{
				CU_ParsingError( cuT( "Missing parameter." ) );
			}
			else if ( !blockContext->pass )
			{
				auto & plugin = getEngine( *blockContext )->getPassComponentsRegister().getPlugin( MetalnessMapComponent::TypeName );
				plugin.fillTextureConfiguration( blockContext->configuration
					, params[0]->get< uint32_t >() );
			}
			else
			{
				auto & plugin = blockContext->pass->pass->getComponentPlugin( MetalnessMapComponent::TypeName );
				plugin.fillTextureConfiguration( blockContext->configuration
					, params[0]->get< uint32_t >() );
			}
		}
		CU_EndAttribute()

		static CU_ImplementAttributeParserBlock( parserTexRemapMetalness, SceneImportContext )
		{
			auto & plugin = getEngine( *blockContext )->getPassComponentsRegister().getPlugin( MetalnessMapComponent::TypeName );
			blockContext->textureRemapIt = blockContext->textureRemaps.try_emplace( plugin.getTextureFlags() ).first;
			blockContext->textureRemapIt->second = TextureConfiguration{};
		}
		CU_EndAttributePushBlock( CSCNSection::eTextureRemapChannel, blockContext )

		static CU_ImplementAttributeParserBlock( parserTexRemapMetalnessMask, SceneImportContext )
		{
			if ( params.empty() )
			{
				CU_ParsingError( cuT( "Missing parameter." ) );
			}
			else
			{
				auto & plugin = getEngine( *blockContext )->getPassComponentsRegister().getPlugin( MetalnessMapComponent::TypeName );
				plugin.fillTextureConfiguration( blockContext->textureRemapIt->second
					, params[0]->get< uint32_t >() );
			}
		}
		CU_EndAttribute()
	}

	//*********************************************************************************************

	void MetalnessMapComponent::ComponentsShader::applyTexture( shader::PassShaders const & passShaders
		, shader::TextureConfigurations const & textureConfigs
		, shader::TextureAnimations const & textureAnims
		, sdw::Array< sdw::CombinedImage2DRgba32 > const & maps
		, shader::Material const & material
		, shader::BlendComponents & components
		, shader::SampleTexture const & sampleTexture )const
	{
		applyFloatComponent( cuT( "metalness" )
			, passShaders
			, textureConfigs
			, textureAnims
			, material
			, components
			, sampleTexture );
	}

	//*********************************************************************************************

	void MetalnessMapComponent::Plugin::createParsers( AttributeParsers & parsers
		, ChannelFillers & channelFillers )const
	{
		channelFillers.try_emplace( cuT( "metalness" )
			, getTextureFlags()
			, []( TextureContext & blockContext )
			{
				auto const & component = getPassComponent< MetalnessMapComponent >( blockContext );
				component.fillChannel( blockContext.configuration
					, 0x00FF0000 );
			} );

		c3d::addParserT( parsers
			, CSCNSection::eTexture
			, cuT( "metalness_mask" )
			, mtlcmp::parserUnitMetalnessMask
			, { makeParameter< ParameterType::eUInt32 >() } );

		c3d::addParserT( parsers
			, CSCNSection::eTextureUnit
			, cuT( "metalness_mask" )
			, mtlcmp::parserUnitMetalnessMask
			, { makeParameter< ParameterType::eUInt32 >() } );

		c3d::addParserT( parsers
			, CSCNSection::eTextureRemap
			, CSCNSection::eTextureRemapChannel
			, cuT( "metalness" )
			, mtlcmp::parserTexRemapMetalness );

		c3d::addParserT( parsers
			, CSCNSection::eTextureRemapChannel
			, cuT( "metalness_mask" )
			, mtlcmp::parserTexRemapMetalnessMask
			, { makeParameter< ParameterType::eUInt32 >() } );
	}

	bool MetalnessMapComponent::Plugin::isComponentNeeded( TextureCombine const & textures
		, ComponentModeFlags const & filter )const
	{
		return checkFlag( filter, ComponentModeFlag::eOpacity )
			|| checkFlag( filter, ComponentModeFlag::eSpecularLighting )
			|| hasAny( textures, getTextureFlags() );
	}

	void MetalnessMapComponent::Plugin::createMapComponent( Pass & pass
		, Vector< PassComponentUPtr > & result )const
	{
		result.push_back( makeUniqueDerived< PassComponent, MetalnessMapComponent >( pass ) );
	}

	bool MetalnessMapComponent::Plugin::doWriteTextureConfig( TextureConfiguration const & configuration
		, uint32_t mask
		, String const & tabs
		, StringStream & file )const
	{
		return TextWriter< MetalnessMapComponent >{ tabs, mask }( file );
	}

	void MetalnessMapComponent::Plugin::doUpdateComponent( PassComponentRegister const & passComponents
		, TextureCombine const & combine
		, shader::BlendComponents & components
		, bool isFrontCulled )
	{
		auto & mtlPlugin = passComponents.getPlugin< MetalnessMapComponent >();
		auto & spcPlugin = passComponents.getPlugin< SpecularMapComponent >();

		if ( !hasAny( combine, mtlPlugin.getTextureFlags() )
			&& hasAny( combine, spcPlugin.getTextureFlags() ) )
		{
			components.getMember< sdw::Float >( "metalness", true ) = length( components.getMember< sdw::Vec3 >( "specular", true ) );
		}
	}

	//*********************************************************************************************

	String const MetalnessMapComponent::TypeName = C3D_MakePassMapComponentName( "metalness" );

	MetalnessMapComponent::MetalnessMapComponent( Pass & pass )
		: PassMapComponent{ pass
			, TypeName
			, Metalness
			, { MetalnessComponent::TypeName } }
	{
	}

	PassComponentUPtr MetalnessMapComponent::doClone( Pass & pass )const
	{
		return makeUniqueDerived< PassComponent, MetalnessMapComponent >( pass );
	}

	void MetalnessMapComponent::doFillConfig( TextureConfiguration & configuration
		, ConfigurationVisitorBase & vis )const
	{
		vis.visit( cuT( "Metalness" ) );
		vis.visit( cuT( "Map" ), getTextureFlags(), getFlagConfiguration( configuration, getTextureFlags() ), 1u );
	}

	PassMapDefaultImageParams MetalnessMapComponent::createDefaultImage()const
	{
		String name{ cuT( "DefaultMetalness" ) };
		ByteArray data;
		data.resize( sizeof( float ) );
		float value = 1.0f;
		std::memcpy( data.data(), &value, data.size() );
		return { name
			, ImageCreateParams{ getFormatName( PixelFormat::eR32_SFLOAT ), data } };
	}

	//*********************************************************************************************
}
