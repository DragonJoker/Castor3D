#include "Castor3D/Material/Pass/Component/Map/SpecularFactorMapComponent.hpp"

#include "Castor3D/Engine.hpp"
#include "Castor3D/Material/Pass/Pass.hpp"
#include "Castor3D/Miscellaneous/ConfigurationVisitor.hpp"
#include "Castor3D/Material/Pass/Component/Lighting/SpecularFactorComponent.hpp"
#include "Castor3D/Material/Pass/Component/PassComponentRegister.hpp"
#include "Castor3D/Material/Texture/TextureConfiguration.hpp"
#include "Castor3D/Scene/SceneFileParserData.hpp"
#include "Castor3D/Scene/SceneImporter.hpp"
#include "Castor3D/Shader/Shaders/GlslBlendComponents.hpp"
#include "Castor3D/Shader/Shaders/GlslMaterial.hpp"
#include "Castor3D/Shader/Shaders/GlslTextureConfiguration.hpp"

#include <CastorUtils/FileParser/FileParser.hpp>

//*************************************************************************************************

namespace c3d
{
	template<>
	class TextWriter< SpecularFactorMapComponent >
		: public TextWriterT< SpecularFactorMapComponent >
	{
	public:
		explicit TextWriter( String const & tabs
			, uint32_t mask )
			: TextWriterT< SpecularFactorMapComponent >{ tabs }
			, m_mask{ mask }
		{
		}

		bool operator()( StringStream & file )const
		{
			return writeMask( file, cuT( "specular_factor_mask" ), m_mask );
		}

		bool operator()( SpecularFactorMapComponent const & object
			, StringStream & file )override
		{
			return writeMask( file, cuT( "specular_factor_mask" ), m_mask );
		}

	private:
		uint32_t m_mask;
	};

	//*********************************************************************************************

	namespace spcftcmp
	{
		static CU_ImplementAttributeParserBlock( parserUnitSpecularFactorMask, TextureContext )
		{
			if ( params.empty() )
			{
				CU_ParsingError( cuT( "Missing parameter." ) );
			}
			else if ( !blockContext->pass )
			{
				auto & plugin = getEngine( *blockContext )->getPassComponentsRegister().getPlugin( SpecularFactorMapComponent::TypeName );
				plugin.fillTextureConfiguration( blockContext->configuration
					, params[0]->get< uint32_t >() );
			}
			else
			{
				auto & plugin = blockContext->pass->pass->getComponentPlugin( SpecularFactorMapComponent::TypeName );
				plugin.fillTextureConfiguration( blockContext->configuration
					, params[0]->get< uint32_t >() );
			}
		}
		CU_EndAttribute()

		static CU_ImplementAttributeParserBlock( parserTexRemapSpecularFactor, SceneImportContext )
		{
			auto & plugin = getEngine( *blockContext )->getPassComponentsRegister().getPlugin( SpecularFactorMapComponent::TypeName );
			blockContext->textureRemapIt = blockContext->textureRemaps.try_emplace( plugin.getTextureFlags() ).first;
			blockContext->textureRemapIt->second = TextureConfiguration{};
		}
		CU_EndAttributePushBlock( CSCNSection::eTextureRemapChannel, blockContext )

		static CU_ImplementAttributeParserBlock( parserTexRemapSpecularFactorMask, SceneImportContext )
		{
			if ( params.empty() )
			{
				CU_ParsingError( cuT( "Missing parameter." ) );
			}
			else
			{
				auto & plugin = getEngine( *blockContext )->getPassComponentsRegister().getPlugin( SpecularFactorMapComponent::TypeName );
				plugin.fillTextureConfiguration( blockContext->textureRemapIt->second
					, params[0]->get< uint32_t >() );
			}
		}
		CU_EndAttribute()
	}

	//*********************************************************************************************

	void SpecularFactorMapComponent::ComponentsShader::applyTexture( shader::PassShaders const & passShaders
		, shader::TextureConfigurations const & textureConfigs
		, shader::TextureAnimations const & textureAnims
		, sdw::Array< sdw::CombinedImage2DRgba32 > const & maps
		, shader::Material const & material
		, shader::BlendComponents & components
		, shader::SampleTexture const & sampleTexture )const
	{
		applyFloatComponent( cuT( "specularFactor" )
			, passShaders
			, textureConfigs
			, textureAnims
			, material
			, components
			, sampleTexture );
	}

	//*********************************************************************************************

	void SpecularFactorMapComponent::Plugin::createParsers( AttributeParsers & parsers
		, ChannelFillers & channelFillers )const
	{
		channelFillers.try_emplace( cuT( "specular_factor" )
			, getTextureFlags()
			, []( TextureContext & blockContext )
			{
				auto const & component = getPassComponent< SpecularFactorMapComponent >( blockContext );
				component.fillChannel( blockContext.configuration
					, 0x00FFFFFF );
			} );

		c3d::addParserT( parsers
			, CSCNSection::eTexture
			, cuT( "specular_factor_mask" )
			, spcftcmp::parserUnitSpecularFactorMask
			, { makeParameter< ParameterType::eUInt32 >() } );

		c3d::addParserT( parsers
			, CSCNSection::eTextureUnit
			, cuT( "specular_factor_mask" )
			, spcftcmp::parserUnitSpecularFactorMask
			, { makeParameter< ParameterType::eUInt32 >() } );

		c3d::addParserT( parsers
			, CSCNSection::eTextureRemap
			, CSCNSection::eTextureRemapChannel
			, cuT( "specular_factor" )
			, spcftcmp::parserTexRemapSpecularFactor );

		c3d::addParserT( parsers
			, CSCNSection::eTextureRemapChannel
			, cuT( "specular_factor_mask" )
			, spcftcmp::parserTexRemapSpecularFactorMask
			, { makeParameter< ParameterType::eUInt32 >() } );
	}

	bool SpecularFactorMapComponent::Plugin::isComponentNeeded( TextureCombine const & textures
		, ComponentModeFlags const & filter )const
	{
		return checkFlag( filter, ComponentModeFlag::eSpecularLighting )
			|| hasAny( textures, getTextureFlags() );
	}

	void SpecularFactorMapComponent::Plugin::createMapComponent( Pass & pass
		, Vector< PassComponentUPtr > & result )const
	{
		result.push_back( makeUniqueDerived< PassComponent, SpecularFactorMapComponent >( pass ) );
	}

	bool SpecularFactorMapComponent::Plugin::doWriteTextureConfig( TextureConfiguration const & configuration
		, uint32_t mask
		, String const & tabs
		, StringStream & file )const
	{
		return TextWriter< SpecularFactorMapComponent >{ tabs, mask }( file );
	}

	//*********************************************************************************************

	String const SpecularFactorMapComponent::TypeName = C3D_MakePassMapComponentName( "specular_factor" );

	SpecularFactorMapComponent::SpecularFactorMapComponent( Pass & pass )
		: PassMapComponent{ pass
			, TypeName
			, Specular
			, { SpecularFactorComponent::TypeName } }
	{
	}

	PassComponentUPtr SpecularFactorMapComponent::doClone( Pass & pass )const
	{
		return makeUniqueDerived< PassComponent, SpecularFactorMapComponent >( pass );
	}

	void SpecularFactorMapComponent::doFillConfig( TextureConfiguration & configuration
		, ConfigurationVisitorBase & vis )const
	{
		vis.visit( cuT( "Specular Factor" ) );
		vis.visit( cuT( "Map" ), getTextureFlags(), getFlagConfiguration( configuration, getTextureFlags() ), 1u );
	}

	//*********************************************************************************************
}
