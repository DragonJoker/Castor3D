#include "Castor3D/Material/Pass/Component/Map/ClearcoatMapComponent.hpp"

#include "Castor3D/Engine.hpp"
#include "Castor3D/Material/Pass/Pass.hpp"
#include "Castor3D/Miscellaneous/ConfigurationVisitor.hpp"
#include "Castor3D/Material/Pass/Component/PassComponentRegister.hpp"
#include "Castor3D/Material/Pass/Component/Lighting/ClearcoatComponent.hpp"
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
	class TextWriter< ClearcoatMapComponent >
		: public TextWriterT< ClearcoatMapComponent >
	{
	public:
		explicit TextWriter( String const & tabs
			, uint32_t mask = 0u )
			: TextWriterT< ClearcoatMapComponent >{ tabs }
			, m_mask{ mask }
		{
		}

		bool operator()( StringStream & file )const
		{
			return writeMask( file, cuT( "clearcoat_mask" ), m_mask );
		}

		bool operator()( ClearcoatMapComponent const & object
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
		static CU_ImplementAttributeParserBlock( parserUnitClearcoatMask, TextureContext )
		{
			if ( params.empty() )
			{
				CU_ParsingError( cuT( "Missing parameter." ) );
			}
			else if ( !blockContext->pass )
			{
				auto & plugin = getEngine( *blockContext )->getPassComponentsRegister().getPlugin( ClearcoatMapComponent::TypeName );
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
			auto & plugin = getEngine( *blockContext )->getPassComponentsRegister().getPlugin( ClearcoatMapComponent::TypeName );
			blockContext->textureRemapIt = blockContext->textureRemaps.try_emplace( plugin.getTextureFlags() ).first;
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
				auto & plugin = getEngine( *blockContext )->getPassComponentsRegister().getPlugin( ClearcoatMapComponent::TypeName );
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
		applyFloatComponent( cuT( "clearcoatFactor" )
			, passShaders
			, textureConfigs
			, textureAnims
			, material
			, components
			, sampleTexture );
	}

	//*********************************************************************************************

	void ClearcoatMapComponent::Plugin::createParsers( AttributeParsers & parsers
		, ChannelFillers & channelFillers )const
	{
		channelFillers.try_emplace( cuT( "clearcoat" )
			, getTextureFlags()
			, []( TextureContext & blockContext )
			{
				auto const & component = getPassComponent< ClearcoatMapComponent >( blockContext );
				component.fillChannel( blockContext.configuration
					, 0x00FF0000u );
			} );

		c3d::addParserT( parsers
			, CSCNSection::eTexture
			, cuT( "clearcoat_mask" )
			, trscmp::parserUnitClearcoatMask
			, { makeParameter< ParameterType::eUInt32 >() } );

		c3d::addParserT( parsers
			, CSCNSection::eTextureUnit
			, cuT( "clearcoat_mask" )
			, trscmp::parserUnitClearcoatMask
			, { makeParameter< ParameterType::eUInt32 >() } );

		c3d::addParserT( parsers
			, CSCNSection::eTextureRemap
			, CSCNSection::eTextureRemapChannel
			, cuT( "clearcoat" )
			, trscmp::parserTexRemapClearcoat );

		c3d::addParserT( parsers
			, CSCNSection::eTextureRemapChannel
			, cuT( "clearcoat_mask" )
			, trscmp::parserTexRemapClearcoatMask
			, { makeParameter< ParameterType::eUInt32 >() } );
	}

	bool ClearcoatMapComponent::Plugin::isComponentNeeded( TextureCombine const & textures
		, ComponentModeFlags const & filter )const
	{
		return checkFlag( filter, ComponentModeFlag::eSpecularLighting )
			|| hasAny( textures, getTextureFlags() );
	}

	void ClearcoatMapComponent::Plugin::createMapComponent( Pass & pass
		, Vector< PassComponentUPtr > & result )const
	{
		result.push_back( makeUniqueDerived< PassComponent, ClearcoatMapComponent >( pass ) );
	}

	bool ClearcoatMapComponent::Plugin::doWriteTextureConfig( TextureConfiguration const & configuration
		, uint32_t mask
		, String const & tabs
		, StringStream & file )const
	{
		return TextWriter< ClearcoatMapComponent >{ tabs, mask }( file );
	}

	//*********************************************************************************************

	String const ClearcoatMapComponent::TypeName = C3D_MakePassMapComponentName( "clearcoat" );

	ClearcoatMapComponent::ClearcoatMapComponent( Pass & pass )
		: PassMapComponent{ pass
			, TypeName
			, Clearcoat
			, { ClearcoatComponent::TypeName } }
	{
	}

	PassComponentUPtr ClearcoatMapComponent::doClone( Pass & pass )const
	{
		return makeUniqueDerived< PassComponent, ClearcoatMapComponent >( pass );
	}

	void ClearcoatMapComponent::doFillConfig( TextureConfiguration & configuration
		, ConfigurationVisitorBase & vis )const
	{
		vis.visit( cuT( "Clearcoat" ) );
		vis.visit( cuT( "Map" ), getTextureFlags(), getFlagConfiguration( configuration, getTextureFlags() ), 1u );
	}

	//*********************************************************************************************
}
