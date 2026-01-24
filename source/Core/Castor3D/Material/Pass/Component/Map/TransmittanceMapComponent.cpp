#include "Castor3D/Material/Pass/Component/Map/TransmittanceMapComponent.hpp"

#include "Castor3D/Engine.hpp"
#include "Castor3D/Material/Pass/Pass.hpp"
#include "Castor3D/Miscellaneous/ConfigurationVisitor.hpp"
#include "Castor3D/Material/Pass/Component/PassComponentRegister.hpp"
#include "Castor3D/Material/Pass/Component/Lighting/TransmittanceComponent.hpp"
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
	class TextWriter< TransmittanceMapComponent >
		: public TextWriterT< TransmittanceMapComponent >
	{
	public:
		explicit TextWriter( String const & tabs
			, uint32_t mask = 0u )
			: TextWriterT< TransmittanceMapComponent >{ tabs }
			, m_mask{ mask }
		{
		}

		bool operator()( StringStream & file )const
		{
			return writeMask( file, cuT( "transmittance_mask" ), m_mask );
		}

		bool operator()( TransmittanceMapComponent const & object
			, StringStream & file )override
		{
			return true;
		}

	private:
		uint32_t m_mask;
	};

	//*********************************************************************************************

	namespace trscmp
	{
		static CU_ImplementAttributeParserBlock( parserUnitTransmittanceMask, TextureContext )
		{
			if ( params.empty() )
			{
				CU_ParsingError( cuT( "Missing parameter." ) );
			}
			else if ( !blockContext->pass )
			{
				auto & plugin = getEngine( *blockContext )->getPassComponentsRegister().getPlugin( TransmittanceMapComponent::TypeName );
				plugin.fillTextureConfiguration( blockContext->configuration
					, params[0]->get< uint32_t >() );
			}
			else
			{
				auto & plugin = blockContext->pass->pass->getComponentPlugin( TransmittanceMapComponent::TypeName );
				plugin.fillTextureConfiguration( blockContext->configuration
					, params[0]->get< uint32_t >() );
			}
		}
		CU_EndAttribute()

		static CU_ImplementAttributeParserBlock( parserTexRemapTransmittance, SceneImportContext )
		{
			auto & plugin = getEngine( *blockContext )->getPassComponentsRegister().getPlugin( TransmittanceMapComponent::TypeName );
			blockContext->textureRemapIt = blockContext->textureRemaps.try_emplace( plugin.getTextureFlags() ).first;
			blockContext->textureRemapIt->second = TextureConfiguration{};
		}
		CU_EndAttributePushBlock( CSCNSection::eTextureRemapChannel, blockContext )

		static CU_ImplementAttributeParserBlock( parserTexRemapTransmittanceMask, SceneImportContext )
		{
			if ( params.empty() )
			{
				CU_ParsingError( cuT( "Missing parameter." ) );
			}
			else
			{
				auto & plugin = getEngine( *blockContext )->getPassComponentsRegister().getPlugin( TransmittanceMapComponent::TypeName );
				plugin.fillTextureConfiguration( blockContext->textureRemapIt->second
					, params[0]->get< uint32_t >() );
			}
		}
		CU_EndAttribute()
	}

	//*********************************************************************************************

	void TransmittanceMapComponent::ComponentsShader::applyTexture( shader::PassShaders const & passShaders
		, shader::TextureConfigurations const & textureConfigs
		, shader::TextureAnimations const & textureAnims
		, sdw::Array< sdw::CombinedImage2DRgba32 > const & maps
		, shader::Material const & material
		, shader::BlendComponents & components
		, shader::SampleTexture const & sampleTexture )const
	{
		applyFloatComponent( cuT( "transmittance" )
			, passShaders
			, textureConfigs
			, textureAnims
			, material
			, components
			, sampleTexture );
	}

	//*********************************************************************************************

	void TransmittanceMapComponent::Plugin::createParsers( AttributeParsers & parsers
		, ChannelFillers & channelFillers )const
	{
		channelFillers.try_emplace( cuT( "transmittance" )
			, getTextureFlags()
			, []( TextureContext & blockContext )
			{
				auto const & component = getPassComponent< TransmittanceMapComponent >( blockContext );
				component.fillChannel( blockContext.configuration
					, 0xFF000000 );
			} );

		c3d::addParserT( parsers
			, CSCNSection::eTexture
			, cuT( "transmittance_mask" )
			, trscmp::parserUnitTransmittanceMask
			, { makeParameter< ParameterType::eUInt32 >() } );

		c3d::addParserT( parsers
			, CSCNSection::eTextureUnit
			, cuT( "transmittance_mask" )
			, trscmp::parserUnitTransmittanceMask
			, { makeParameter< ParameterType::eUInt32 >() } );

		c3d::addParserT( parsers
			, CSCNSection::eTextureRemap
			, CSCNSection::eTextureRemapChannel
			, cuT( "transmittance" )
			, trscmp::parserTexRemapTransmittance );

		c3d::addParserT( parsers
			, CSCNSection::eTextureRemapChannel
			, cuT( "transmittance_mask" )
			, trscmp::parserTexRemapTransmittanceMask
			, { makeParameter< ParameterType::eUInt32 >() } );
	}

	bool TransmittanceMapComponent::Plugin::isComponentNeeded( TextureCombine const & textures
		, ComponentModeFlags const & filter )const
	{
		return checkFlag( filter, ComponentModeFlag::eDiffuseLighting )
			|| checkFlag( filter, ComponentModeFlag::eSpecularLighting )
			|| hasAny( textures, getTextureFlags() );
	}

	void TransmittanceMapComponent::Plugin::createMapComponent( Pass & pass
		, Vector< PassComponentUPtr > & result )const
	{
		result.push_back( makeUniqueDerived< PassComponent, TransmittanceMapComponent >( pass ) );
	}

	bool TransmittanceMapComponent::Plugin::doWriteTextureConfig( TextureConfiguration const & configuration
		, uint32_t mask
		, String const & tabs
		, StringStream & file )const
	{
		return TextWriter< TransmittanceMapComponent >{ tabs, mask }( file );
	}

	//*********************************************************************************************

	String const TransmittanceMapComponent::TypeName = C3D_MakePassMapComponentName( "transmittance" );

	TransmittanceMapComponent::TransmittanceMapComponent( Pass & pass )
		: PassMapComponent{ pass
			, TypeName
			, Transmittance
			, { TransmittanceComponent::TypeName } }
	{
	}

	PassComponentUPtr TransmittanceMapComponent::doClone( Pass & pass )const
	{
		return makeUniqueDerived< PassComponent, TransmittanceMapComponent >( pass );
	}

	void TransmittanceMapComponent::doFillConfig( TextureConfiguration & configuration
		, ConfigurationVisitorBase & vis )const
	{
		vis.visit( cuT( "Transmittance" ) );
		vis.visit( cuT( "Map" ), getTextureFlags(), getFlagConfiguration( configuration, getTextureFlags() ), 1u );
	}

	//*********************************************************************************************
}
