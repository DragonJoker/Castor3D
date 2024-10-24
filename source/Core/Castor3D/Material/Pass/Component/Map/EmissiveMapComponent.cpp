#include "Castor3D/Material/Pass/Component/Map/EmissiveMapComponent.hpp"

#include "Castor3D/Engine.hpp"
#include "Castor3D/Material/Pass/Pass.hpp"
#include "Castor3D/Miscellaneous/ConfigurationVisitor.hpp"
#include "Castor3D/Material/Pass/Component/Lighting/EmissiveComponent.hpp"
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

CU_ImplementSmartPtr( castor3d, EmissiveMapComponent )

namespace castor
{
	template<>
	class TextWriter< castor3d::EmissiveMapComponent >
		: public TextWriterT< castor3d::EmissiveMapComponent >
	{
	public:
		explicit TextWriter( String const & tabs
			, uint32_t mask )
			: TextWriterT< castor3d::EmissiveMapComponent >{ tabs }
			, m_mask{ mask }
		{
		}

		bool operator()( castor3d::EmissiveMapComponent const & object
			, StringStream & file )override
		{
			return writeMask( file, cuT( "emissive_mask" ), m_mask );
		}

		bool operator()( StringStream & file )const
		{
			return writeMask( file, cuT( "emissive_mask" ), m_mask );
		}

	private:
		uint32_t m_mask;
	};
}

namespace castor3d
{
	//*********************************************************************************************

	namespace emscmp
	{
		static CU_ImplementAttributeParserBlock( parserUnitEmissiveMask, TextureContext )
		{
			if ( params.empty() )
			{
				CU_ParsingError( cuT( "Missing parameter." ) );
			}
			else if ( !blockContext->pass )
			{
				auto & plugin = getEngine( *blockContext )->getPassComponentsRegister().getPlugin( EmissiveMapComponent::TypeName );
				plugin.fillTextureConfiguration( blockContext->configuration
					, params[0]->get< uint32_t >() );
			}
			else
			{
				auto & plugin = blockContext->pass->pass->getComponentPlugin( EmissiveMapComponent::TypeName );
				plugin.fillTextureConfiguration( blockContext->configuration
					, params[0]->get< uint32_t >() );
			}
		}
		CU_EndAttribute()

		static CU_ImplementAttributeParserBlock( parserTexRemapEmissive, SceneImportContext )
		{
			auto & plugin = getEngine( *blockContext )->getPassComponentsRegister().getPlugin( EmissiveMapComponent::TypeName );
			blockContext->textureRemapIt = blockContext->textureRemaps.try_emplace( plugin.getTextureFlags() ).first;
			blockContext->textureRemapIt->second = TextureConfiguration{};
		}
		CU_EndAttributePushBlock( CSCNSection::eTextureRemapChannel, blockContext )

		static CU_ImplementAttributeParserBlock( parserTexRemapEmissiveMask, SceneImportContext )
		{
			if ( params.empty() )
			{
				CU_ParsingError( cuT( "Missing parameter." ) );
			}
			else
			{
				auto & plugin = getEngine( *blockContext )->getPassComponentsRegister().getPlugin( EmissiveMapComponent::TypeName );
				plugin.fillTextureConfiguration( blockContext->textureRemapIt->second
					, params[0]->get< uint32_t >() );
			}
		}
		CU_EndAttribute()
	}

	//*********************************************************************************************

	void EmissiveMapComponent::ComponentsShader::applyTexture( shader::PassShaders const & passShaders
		, shader::TextureConfigurations const & textureConfigs
		, shader::TextureAnimations const & textureAnims
		, sdw::Array< sdw::CombinedImage2DRgba32 > const & maps
		, shader::Material const & material
		, shader::BlendComponents & components
		, shader::SampleTexture const & sampleTexture )const
	{
		applyVec3Component( cuT( "emissiveColour" )
			, passShaders
			, textureConfigs
			, textureAnims
			, material
			, components
			, sampleTexture );
	}

	//*********************************************************************************************

	void EmissiveMapComponent::Plugin::createParsers( castor::AttributeParsers & parsers
		, ChannelFillers & channelFillers )const
	{
		channelFillers.try_emplace( cuT( "emissive" )
			, getTextureFlags()
			, []( TextureContext & blockContext )
			{
				auto const & component = getPassComponent< EmissiveMapComponent >( blockContext );
				component.fillChannel( blockContext.configuration
					, 0x00FFFFFF );
			} );

		castor::addParserT( parsers
			, CSCNSection::eTexture
			, cuT( "emissive_mask" )
			, emscmp::parserUnitEmissiveMask
			, { castor::makeParameter< castor::ParameterType::eUInt32 >() } );

		castor::addParserT( parsers
			, CSCNSection::eTextureUnit
			, cuT( "emissive_mask" )
			, emscmp::parserUnitEmissiveMask
			, { castor::makeParameter< castor::ParameterType::eUInt32 >() } );

		castor::addParserT( parsers
			, CSCNSection::eTextureRemap
			, cuT( "emissive" )
			, emscmp::parserTexRemapEmissive );

		castor::addParserT( parsers
			, CSCNSection::eTextureRemapChannel
			, cuT( "emissive_mask" )
			, emscmp::parserTexRemapEmissiveMask
			, { castor::makeParameter< castor::ParameterType::eUInt32 >() } );
	}

	bool EmissiveMapComponent::Plugin::isComponentNeeded( TextureCombine const & textures
		, ComponentModeFlags const & filter )const
	{
		return checkFlag( filter, ComponentModeFlag::eDiffuseLighting )
			|| checkFlag( filter, ComponentModeFlag::eSpecularLighting )
			|| hasAny( textures, getTextureFlags() );
	}

	void EmissiveMapComponent::Plugin::createMapComponent( Pass & pass
		, castor::Vector< PassComponentUPtr > & result )const
	{
		result.push_back( castor::makeUniqueDerived< PassComponent, EmissiveMapComponent >( pass ) );
	}

	bool EmissiveMapComponent::Plugin::doWriteTextureConfig( TextureConfiguration const & configuration
		, uint32_t mask
		, castor::String const & tabs
		, castor::StringStream & file )const
	{
		return castor::TextWriter< EmissiveMapComponent >{ tabs, mask }( file );
	}

	//*********************************************************************************************

	castor::String const EmissiveMapComponent::TypeName = C3D_MakePassMapComponentName( "emissive" );

	EmissiveMapComponent::EmissiveMapComponent( Pass & pass )
		: PassMapComponent{ pass
			, TypeName
			, Emissive
			, { EmissiveComponent::TypeName } }
	{
	}

	PassComponentUPtr EmissiveMapComponent::doClone( Pass & pass )const
	{
		return castor::makeUniqueDerived< PassComponent, EmissiveMapComponent >( pass );
	}

	void EmissiveMapComponent::doFillConfig( TextureConfiguration & configuration
		, ConfigurationVisitorBase & vis )const
	{
		vis.visit( cuT( "Emissive" ) );
		vis.visit( cuT( "Map" ), getTextureFlags(), getFlagConfiguration( configuration, getTextureFlags() ), 3u );
	}

	//*********************************************************************************************
}
