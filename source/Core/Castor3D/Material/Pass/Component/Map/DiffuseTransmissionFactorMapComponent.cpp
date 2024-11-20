#include "Castor3D/Material/Pass/Component/Map/DiffuseTransmissionFactorMapComponent.hpp"

#include "Castor3D/Engine.hpp"
#include "Castor3D/Material/Pass/Pass.hpp"
#include "Castor3D/Miscellaneous/ConfigurationVisitor.hpp"
#include "Castor3D/Material/Pass/Component/PassComponentRegister.hpp"
#include "Castor3D/Material/Pass/Component/Lighting/TransmissionComponent.hpp"
#include "Castor3D/Material/Texture/TextureConfiguration.hpp"
#include "Castor3D/Scene/SceneFileParserData.hpp"
#include "Castor3D/Scene/SceneImporter.hpp"
#include "Castor3D/Shader/ShaderBuffers/PassBuffer.hpp"
#include "Castor3D/Shader/Shaders/GlslBlendComponents.hpp"
#include "Castor3D/Shader/Shaders/GlslLighting.hpp"
#include "Castor3D/Shader/Shaders/GlslMaterial.hpp"
#include "Castor3D/Shader/Shaders/GlslTextureConfiguration.hpp"

#include <CastorUtils/FileParser/FileParser.hpp>

namespace castor
{
	template<>
	class TextWriter< castor3d::DiffuseTransmissionFactorMapComponent >
		: public TextWriterT< castor3d::DiffuseTransmissionFactorMapComponent >
	{
	public:
		explicit TextWriter( String const & tabs
			, uint32_t mask = 0u )
			: TextWriterT< castor3d::DiffuseTransmissionFactorMapComponent >{ tabs }
			, m_mask{ mask }
		{
		}

		bool operator()( StringStream & file )const
		{
			return writeMask( file, cuT( "diffuse_transmission_factor_mask" ), m_mask );
		}

		bool operator()( castor3d::DiffuseTransmissionFactorMapComponent const & object
			, StringStream & file )override
		{
			return true;
		}

	private:
		uint32_t m_mask;
	};
}

namespace castor3d
{
	//*********************************************************************************************

	namespace dftrftmk
	{
		static CU_ImplementAttributeParserBlock( parserUnitTransmissionMask, TextureContext )
		{
			if ( params.empty() )
			{
				CU_ParsingError( cuT( "Missing parameter." ) );
			}
			else if ( !blockContext->pass )
			{
				auto & plugin = getEngine( *blockContext )->getPassComponentsRegister().getPlugin( DiffuseTransmissionFactorMapComponent::TypeName );
				plugin.fillTextureConfiguration( blockContext->configuration
					, params[0]->get< uint32_t >() );
			}
			else
			{
				auto & plugin = blockContext->pass->pass->getComponentPlugin( DiffuseTransmissionFactorMapComponent::TypeName );
				plugin.fillTextureConfiguration( blockContext->configuration
					, params[0]->get< uint32_t >() );
			}
		}
		CU_EndAttribute()

		static CU_ImplementAttributeParserBlock( parserTexRemapTransmission, SceneImportContext )
		{
			auto & plugin = getEngine( *blockContext )->getPassComponentsRegister().getPlugin( DiffuseTransmissionFactorMapComponent::TypeName );
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
				auto & plugin = getEngine( *blockContext )->getPassComponentsRegister().getPlugin( DiffuseTransmissionFactorMapComponent::TypeName );
				plugin.fillTextureConfiguration( blockContext->textureRemapIt->second
					, params[0]->get< uint32_t >() );
			}
		}
		CU_EndAttribute()
	}

	//*********************************************************************************************

	void DiffuseTransmissionFactorMapComponent::ComponentsShader::applyTexture( shader::PassShaders const & passShaders
		, shader::TextureConfigurations const & textureConfigs
		, shader::TextureAnimations const & textureAnims
		, sdw::Array< sdw::CombinedImage2DRgba32 > const & maps
		, shader::Material const & material
		, shader::BlendComponents & components
		, shader::SampleTexture const & sampleTexture )const
	{
		applyFloatComponent( cuT( "diffuseTransmissionFactor" )
			, passShaders
			, textureConfigs
			, textureAnims
			, material
			, components
			, sampleTexture );
	}

	//*********************************************************************************************

	void DiffuseTransmissionFactorMapComponent::Plugin::createParsers( castor::AttributeParsers & parsers
		, ChannelFillers & channelFillers )const
	{
		channelFillers.try_emplace( cuT( "diffuse_transmission_factor" )
			, getTextureFlags()
			, []( TextureContext & blockContext )
			{
				auto const & component = getPassComponent< DiffuseTransmissionFactorMapComponent >( blockContext );
				component.fillChannel( blockContext.configuration
					, 0xFF000000 );
			} );

		castor::addParserT( parsers
			, CSCNSection::eTexture
			, cuT( "diffuse_transmission_factor_mask" )
			, dftrftmk::parserUnitTransmissionMask
			, { castor::makeParameter< castor::ParameterType::eUInt32 >() } );

		castor::addParserT( parsers
			, CSCNSection::eTextureUnit
			, cuT( "diffuse_transmission_factor_mask" )
			, dftrftmk::parserUnitTransmissionMask
			, { castor::makeParameter< castor::ParameterType::eUInt32 >() } );

		castor::addParserT( parsers
			, CSCNSection::eTextureRemap
			, CSCNSection::eTextureRemapChannel
			, cuT( "diffuse_transmission_factor" )
			, dftrftmk::parserTexRemapTransmission );

		castor::addParserT( parsers
			, CSCNSection::eTextureRemapChannel
			, cuT( "diffuse_transmission_factor_mask" )
			, dftrftmk::parserTexRemapTransmissionMask
			, { castor::makeParameter< castor::ParameterType::eUInt32 >() } );
	}

	bool DiffuseTransmissionFactorMapComponent::Plugin::isComponentNeeded( TextureCombine const & textures
		, ComponentModeFlags const & filter )const
	{
		return checkFlag( filter, ComponentModeFlag::eDiffuseLighting )
			|| hasAny( textures, getTextureFlags() );
	}

	void DiffuseTransmissionFactorMapComponent::Plugin::createMapComponent( Pass & pass
		, castor::Vector< PassComponentUPtr > & result )const
	{
		result.push_back( castor::makeUniqueDerived< PassComponent, DiffuseTransmissionFactorMapComponent >( pass ) );
	}

	bool DiffuseTransmissionFactorMapComponent::Plugin::doWriteTextureConfig( TextureConfiguration const & configuration
		, uint32_t mask
		, castor::String const & tabs
		, castor::StringStream & file )const
	{
		return castor::TextWriter< DiffuseTransmissionFactorMapComponent >{ tabs, mask }( file );
	}

	//*********************************************************************************************

	castor::String const DiffuseTransmissionFactorMapComponent::TypeName = C3D_MakePassMapComponentName( "diffuse_transmission_factor" );

	DiffuseTransmissionFactorMapComponent::DiffuseTransmissionFactorMapComponent( Pass & pass )
		: PassMapComponent{ pass
			, TypeName
			, Transmission
			, { TransmissionComponent::TypeName } }
	{
	}

	PassComponentUPtr DiffuseTransmissionFactorMapComponent::doClone( Pass & pass )const
	{
		return castor::makeUniqueDerived< PassComponent, DiffuseTransmissionFactorMapComponent >( pass );
	}

	void DiffuseTransmissionFactorMapComponent::doFillConfig( TextureConfiguration & configuration
		, ConfigurationVisitorBase & vis )const
	{
		vis.visit( cuT( "DiffuseTransmissionFactor" ) );
		vis.visit( cuT( "Map" ), getTextureFlags(), getFlagConfiguration( configuration, getTextureFlags() ), 1u );
	}

	//*********************************************************************************************
}
