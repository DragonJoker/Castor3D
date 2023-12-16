#include "Castor3D/Material/Pass/Component/Map/ClearcoatMapComponent.hpp"

#include "Castor3D/Engine.hpp"
#include "Castor3D/Material/Pass/Pass.hpp"
#include "Castor3D/Miscellaneous/ConfigurationVisitor.hpp"
#include "Castor3D/Material/Pass/Component/PassComponentRegister.hpp"
#include "Castor3D/Material/Pass/Component/Lighting/ClearcoatComponent.hpp"
#include "Castor3D/Material/Texture/TextureConfiguration.hpp"
#include "Castor3D/Scene/SceneFileParser.hpp"
#include "Castor3D/Shader/ShaderBuffers/PassBuffer.hpp"
#include "Castor3D/Shader/Shaders/GlslBlendComponents.hpp"
#include "Castor3D/Shader/Shaders/GlslLighting.hpp"
#include "Castor3D/Shader/Shaders/GlslMaterial.hpp"
#include "Castor3D/Shader/Shaders/GlslTextureConfiguration.hpp"

#include <CastorUtils/FileParser/ParserParameter.hpp>

CU_ImplementSmartPtr( castor3d, ClearcoatMapComponent )

namespace castor
{
	template<>
	class TextWriter< castor3d::ClearcoatMapComponent >
		: public TextWriterT< castor3d::ClearcoatMapComponent >
	{
	public:
		explicit TextWriter( String const & tabs
			, uint32_t mask = 0u )
			: TextWriterT< castor3d::ClearcoatMapComponent >{ tabs }
			, m_mask{ mask }
		{
		}

		bool operator()( StringStream & file )
		{
			return writeMask( file, cuT( "clearcoat_mask" ), m_mask );
		}

		bool operator()( castor3d::ClearcoatMapComponent const & object
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

	namespace trscmp
	{
		static CU_ImplementAttributeParserBlock( parserUnitClearcoatMask, TextureContext )
		{
			if ( params.empty() )
			{
				CU_ParsingError( cuT( "Missing parameter." ) );
			}
			else if ( !blockContext->pass )
			{
				auto & plugin = blockContext->root->engine->getPassComponentsRegister().getPlugin( ClearcoatMapComponent::TypeName );
				plugin.fillTextureConfiguration( blockContext->configuration
					, params[0]->get< uint32_t >() );
			}
			else
			{
				auto & plugin = blockContext->pass->pass->getComponentPlugin( ClearcoatMapComponent::TypeName );
				plugin.fillTextureConfiguration( blockContext->configuration
					, params[0]->get< uint32_t >() );
			}
		}
		CU_EndAttribute()

		static CU_ImplementAttributeParserBlock( parserTexRemapClearcoat, SceneImportContext )
		{
			auto & plugin = blockContext->root->engine->getPassComponentsRegister().getPlugin( ClearcoatMapComponent::TypeName );
			blockContext->textureRemapIt = blockContext->textureRemaps.emplace( plugin.getTextureFlags(), TextureConfiguration{} ).first;
			blockContext->textureRemapIt->second = TextureConfiguration{};
		}
		CU_EndAttributePushBlock( CSCNSection::eTextureRemapChannel, blockContext )

		static CU_ImplementAttributeParserBlock( parserTexRemapClearcoatMask, SceneImportContext )
		{
			if ( params.empty() )
			{
				CU_ParsingError( cuT( "Missing parameter." ) );
			}
			else
			{
				auto & plugin = blockContext->root->engine->getPassComponentsRegister().getPlugin( ClearcoatMapComponent::TypeName );
				plugin.fillTextureConfiguration( blockContext->textureRemapIt->second
					, params[0]->get< uint32_t >() );
			}
		}
		CU_EndAttribute()
	}

	//*********************************************************************************************

	void ClearcoatMapComponent::ComponentsShader::applyTexture( shader::PassShaders const & passShaders
		, shader::TextureConfigurations const & textureConfigs
		, shader::TextureAnimations const & textureAnims
		, sdw::Array< sdw::CombinedImage2DRgba32 > const & maps
		, shader::Material const & material
		, shader::BlendComponents & components
		, shader::SampleTexture const & sampleTexture )const
	{
		applyFloatComponent( "clearcoatFactor"
			, passShaders
			, textureConfigs
			, textureAnims
			, material
			, components
			, sampleTexture );
	}

	//*********************************************************************************************

	void ClearcoatMapComponent::Plugin::createParsers( castor::AttributeParsers & parsers
		, ChannelFillers & channelFillers )const
	{
		channelFillers.emplace( "clearcoat", ChannelFiller{ getTextureFlags()
			, []( TextureContext & blockContext )
			{
				auto & component = getPassComponent< ClearcoatMapComponent >( blockContext );
				component.fillChannel( blockContext.configuration
					, 0x00FF0000u );
			} } );

		castor::addParserT( parsers
			, CSCNSection::eTexture
			, cuT( "clearcoat_mask" )
			, trscmp::parserUnitClearcoatMask
			, { castor::makeParameter< castor::ParameterType::eUInt32 >() } );

		castor::addParserT( parsers
			, CSCNSection::eTextureUnit
			, cuT( "clearcoat_mask" )
			, trscmp::parserUnitClearcoatMask
			, { castor::makeParameter< castor::ParameterType::eUInt32 >() } );

		castor::addParserT( parsers
			, CSCNSection::eTextureRemap
			, cuT( "clearcoat" )
			, trscmp::parserTexRemapClearcoat );

		castor::addParserT( parsers
			, CSCNSection::eTextureRemapChannel
			, cuT( "clearcoat_mask" )
			, trscmp::parserTexRemapClearcoatMask
			, { castor::makeParameter< castor::ParameterType::eUInt32 >() } );
	}

	bool ClearcoatMapComponent::Plugin::isComponentNeeded( TextureCombine const & textures
		, ComponentModeFlags const & filter )const
	{
		return checkFlag( filter, ComponentModeFlag::eSpecularLighting )
			|| hasAny( textures, getTextureFlags() );
	}

	void ClearcoatMapComponent::Plugin::createMapComponent( Pass & pass
		, std::vector< PassComponentUPtr > & result )const
	{
		result.push_back( castor::makeUniqueDerived< PassComponent, ClearcoatMapComponent >( pass ) );
	}

	bool ClearcoatMapComponent::Plugin::doWriteTextureConfig( TextureConfiguration const & configuration
		, uint32_t mask
		, castor::String const & tabs
		, castor::StringStream & file )const
	{
		return castor::TextWriter< ClearcoatMapComponent >{ tabs, mask }( file );
	}

	//*********************************************************************************************

	castor::String const ClearcoatMapComponent::TypeName = C3D_MakePassMapComponentName( "clearcoat" );

	ClearcoatMapComponent::ClearcoatMapComponent( Pass & pass )
		: PassMapComponent{ pass
			, TypeName
			, Clearcoat
			, { ClearcoatComponent::TypeName } }
	{
	}

	PassComponentUPtr ClearcoatMapComponent::doClone( Pass & pass )const
	{
		return castor::makeUniqueDerived< PassComponent, ClearcoatMapComponent >( pass );
	}

	void ClearcoatMapComponent::doFillConfig( TextureConfiguration & configuration
		, ConfigurationVisitorBase & vis )const
	{
		vis.visit( cuT( "Clearcoat" ) );
		vis.visit( cuT( "Map" ), getTextureFlags(), getFlagConfiguration( configuration, getTextureFlags() ), 1u );
	}

	//*********************************************************************************************
}
