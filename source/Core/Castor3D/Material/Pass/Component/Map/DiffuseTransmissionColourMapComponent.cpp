#include "Castor3D/Material/Pass/Component/Map/DiffuseTransmissionColourMapComponent.hpp"

#include "Castor3D/Engine.hpp"
#include "Castor3D/Material/Pass/Pass.hpp"
#include "Castor3D/Miscellaneous/ConfigurationVisitor.hpp"
#include "Castor3D/Material/Pass/Component/PassComponentRegister.hpp"
#include "Castor3D/Material/Pass/Component/Lighting/DiffuseTransmissionComponent.hpp"
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
	class TextWriter< DiffuseTransmissionColourMapComponent >
		: public TextWriterT< DiffuseTransmissionColourMapComponent >
	{
	public:
		explicit TextWriter( String const & tabs
			, uint32_t mask = 0u )
			: TextWriterT< DiffuseTransmissionColourMapComponent >{ tabs }
			, m_mask{ mask }
		{
		}

		bool operator()( StringStream & file )const
		{
			return writeMask( file, cuT( "diffuse_transmission_colour_mask" ), m_mask );
		}

		bool operator()( DiffuseTransmissionColourMapComponent const & object
			, StringStream & file )override
		{
			return true;
		}

	private:
		uint32_t m_mask;
	};

	//*********************************************************************************************

	namespace dftrclmk
	{
		static CU_ImplementAttributeParserBlock( parserUnitTransmissionMask, TextureContext )
		{
			if ( params.empty() )
			{
				CU_ParsingError( cuT( "Missing parameter." ) );
			}
			else if ( !blockContext->pass )
			{
				auto & plugin = getEngine( *blockContext )->getPassComponentsRegister().getPlugin( DiffuseTransmissionColourMapComponent::TypeName );
				plugin.fillTextureConfiguration( blockContext->configuration
					, params[0]->get< uint32_t >() );
			}
			else
			{
				auto & plugin = blockContext->pass->pass->getComponentPlugin( DiffuseTransmissionColourMapComponent::TypeName );
				plugin.fillTextureConfiguration( blockContext->configuration
					, params[0]->get< uint32_t >() );
			}
		}
		CU_EndAttribute()

		static CU_ImplementAttributeParserBlock( parserTexRemapTransmission, SceneImportContext )
		{
			auto & plugin = getEngine( *blockContext )->getPassComponentsRegister().getPlugin( DiffuseTransmissionColourMapComponent::TypeName );
			blockContext->textureRemapIt = blockContext->textureRemaps.try_emplace( plugin.getTextureFlags() ).first;
			blockContext->textureRemapIt->second = TextureConfiguration{};
		}
		CU_EndAttributePushBlock( CSCNSection::eTextureRemapChannel, blockContext )

		static CU_ImplementAttributeParserBlock( parserTexRemapTransmissionMask, SceneImportContext )
		{
			if ( params.empty() )
			{
				CU_ParsingError( cuT( "Missing parameter." ) );
			}
			else
			{
				auto & plugin = getEngine( *blockContext )->getPassComponentsRegister().getPlugin( DiffuseTransmissionColourMapComponent::TypeName );
				plugin.fillTextureConfiguration( blockContext->textureRemapIt->second
					, params[0]->get< uint32_t >() );
			}
		}
		CU_EndAttribute()
	}

	//*********************************************************************************************

	void DiffuseTransmissionColourMapComponent::ComponentsShader::applyTexture( shader::PassShaders const & passShaders
		, shader::TextureConfigurations const & textureConfigs
		, shader::TextureAnimations const & textureAnims
		, sdw::Array< sdw::CombinedImage2DRgba32 > const & maps
		, shader::Material const & material
		, shader::BlendComponents & components
		, shader::SampleTexture const & sampleTexture )const
	{
		applyVec3Component( cuT( "diffuseTransmissionColour" )
			, passShaders
			, textureConfigs
			, textureAnims
			, material
			, components
			, sampleTexture );
	}

	//*********************************************************************************************

	void DiffuseTransmissionColourMapComponent::Plugin::createParsers( AttributeParsers & parsers
		, ChannelFillers & channelFillers )const
	{
		channelFillers.try_emplace( cuT( "diffuse_transmission_colour" )
			, getTextureFlags()
			, []( TextureContext & blockContext )
			{
				auto const & component = getPassComponent< DiffuseTransmissionColourMapComponent >( blockContext );
				component.fillChannel( blockContext.configuration
					, 0x00FFFFFF );
			} );

		c3d::addParserT( parsers
			, CSCNSection::eTexture
			, cuT( "diffuse_transmission_colour_mask" )
			, dftrclmk::parserUnitTransmissionMask
			, { makeParameter< ParameterType::eUInt32 >() } );

		c3d::addParserT( parsers
			, CSCNSection::eTextureUnit
			, cuT( "diffuse_transmission_colour_mask" )
			, dftrclmk::parserUnitTransmissionMask
			, { makeParameter< ParameterType::eUInt32 >() } );

		c3d::addParserT( parsers
			, CSCNSection::eTextureRemap
			, CSCNSection::eTextureRemapChannel
			, cuT( "diffuse_transmission_colour" )
			, dftrclmk::parserTexRemapTransmission );

		c3d::addParserT( parsers
			, CSCNSection::eTextureRemapChannel
			, cuT( "diffuse_transmission_colour_mask" )
			, dftrclmk::parserTexRemapTransmissionMask
			, { makeParameter< ParameterType::eUInt32 >() } );
	}

	bool DiffuseTransmissionColourMapComponent::Plugin::isComponentNeeded( TextureCombine const & textures
		, ComponentModeFlags const & filter )const
	{
		return checkFlag( filter, ComponentModeFlag::eDiffuseLighting )
			|| hasAny( textures, getTextureFlags() );
	}

	void DiffuseTransmissionColourMapComponent::Plugin::createMapComponent( Pass & pass
		, Vector< PassComponentUPtr > & result )const
	{
		result.push_back( makeUniqueDerived< PassComponent, DiffuseTransmissionColourMapComponent >( pass ) );
	}

	bool DiffuseTransmissionColourMapComponent::Plugin::doWriteTextureConfig( TextureConfiguration const & configuration
		, uint32_t mask
		, String const & tabs
		, StringStream & file )const
	{
		return TextWriter< DiffuseTransmissionColourMapComponent >{ tabs, mask }( file );
	}

	//*********************************************************************************************

	String const DiffuseTransmissionColourMapComponent::TypeName = C3D_MakePassMapComponentName( "diffuse_transmission_colour" );

	DiffuseTransmissionColourMapComponent::DiffuseTransmissionColourMapComponent( Pass & pass )
		: PassMapComponent{ pass
			, TypeName
			, Transmission
			, { DiffuseTransmissionComponent::TypeName } }
	{
	}

	PassComponentUPtr DiffuseTransmissionColourMapComponent::doClone( Pass & pass )const
	{
		return makeUniqueDerived< PassComponent, DiffuseTransmissionColourMapComponent >( pass );
	}

	void DiffuseTransmissionColourMapComponent::doFillConfig( TextureConfiguration & configuration
		, ConfigurationVisitorBase & vis )const
	{
		vis.visit( cuT( "DiffuseTransmissionColour" ) );
		vis.visit( cuT( "Map" ), getTextureFlags(), getFlagConfiguration( configuration, getTextureFlags() ), 3u );
	}

	//*********************************************************************************************
}
