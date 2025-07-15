#include "AnisotropicMaterial/AnisotropyStrengthMapComponent.hpp"
#include "AnisotropicMaterial/AnisotropyComponent.hpp"

#include <Castor3D/Engine.hpp>
#include <Castor3D/Material/Pass/Pass.hpp>
#include <Castor3D/Miscellaneous/ConfigurationVisitor.hpp>
#include <Castor3D/Material/Pass/Component/PassComponentRegister.hpp>
#include <Castor3D/Material/Texture/TextureConfiguration.hpp>
#include <Castor3D/Scene/SceneFileParserData.hpp>
#include <Castor3D/Scene/SceneImporter.hpp>
#include <Castor3D/Shader/ShaderBuffers/PassBuffer.hpp>
#include <Castor3D/Shader/Shaders/GlslBlendComponents.hpp>
#include <Castor3D/Shader/Shaders/GlslDerivativeValue.hpp>
#include <Castor3D/Shader/Shaders/GlslLighting.hpp>
#include <Castor3D/Shader/Shaders/GlslMaterial.hpp>
#include <Castor3D/Shader/Shaders/GlslTextureConfiguration.hpp>

#include <CastorUtils/FileParser/FileParser.hpp>

CU_ImplementSmartPtr( anisotropy, AnisotropyStrengthMapComponent )

namespace c3d
{
	template<>
	class TextWriter< anisotropy::AnisotropyStrengthMapComponent >
		: public TextWriterT< anisotropy::AnisotropyStrengthMapComponent >
	{
	public:
		explicit TextWriter( String const & tabs
			, uint32_t mask = 0u )
			: TextWriterT< anisotropy::AnisotropyStrengthMapComponent >{ tabs }
			, m_mask{ mask }
		{
		}

		bool operator()( StringStream & file )const
		{
			return writeMask( file, cuT( "anisotropy_strength_mask" ), m_mask );
		}

		bool operator()( anisotropy::AnisotropyStrengthMapComponent const & object
			, StringStream & file )override
		{
			return true;
		}

	private:
		uint32_t m_mask;
	};
}

namespace anisotropy
{
	using namespace c3d;
	namespace c3ds = c3d::shader;

	//*********************************************************************************************

	namespace trscmp
	{
		static CU_ImplementAttributeParserBlock( parserUnitAnisotropyStrengthMask, TextureContext )
		{
			if ( params.empty() )
			{
				CU_ParsingError( cuT( "Missing parameter." ) );
			}
			else if ( !blockContext->pass )
			{
				auto & plugin = getEngine( *blockContext )->getPassComponentsRegister().getPlugin( AnisotropyStrengthMapComponent::TypeName );
				plugin.fillTextureConfiguration( blockContext->configuration
					, params[0]->get< uint32_t >() );
			}
			else
			{
				auto & plugin = blockContext->pass->pass->getComponentPlugin( AnisotropyStrengthMapComponent::TypeName );
				plugin.fillTextureConfiguration( blockContext->configuration
					, params[0]->get< uint32_t >() );
			}
		}
		CU_EndAttribute()

		static CU_ImplementAttributeParserBlock( parserTexRemapAnisotropyStrength, SceneImportContext )
		{
			auto & plugin = getEngine( *blockContext )->getPassComponentsRegister().getPlugin( AnisotropyStrengthMapComponent::TypeName );
			blockContext->textureRemapIt = blockContext->textureRemaps.try_emplace( plugin.getTextureFlags() ).first;
			blockContext->textureRemapIt->second = TextureConfiguration{};
		}
		CU_EndAttributePushBlock( CSCNSection::eTextureRemapChannel, blockContext )

		static CU_ImplementAttributeParserBlock( parserTexRemapAnisotropyStrengthMask, SceneImportContext )
		{
			if ( params.empty() )
			{
				CU_ParsingError( cuT( "Missing parameter." ) );
			}
			else
			{
				auto & plugin = getEngine( *blockContext )->getPassComponentsRegister().getPlugin( AnisotropyStrengthMapComponent::TypeName );
				plugin.fillTextureConfiguration( blockContext->textureRemapIt->second
					, params[0]->get< uint32_t >() );
			}
		}
		CU_EndAttribute()
	}

	//*********************************************************************************************

	void AnisotropyStrengthMapComponent::ComponentsShader::applyTexture( c3d::shader::PassShaders const & passShaders
		, c3d::shader::TextureConfigurations const & textureConfigs
		, c3d::shader::TextureAnimations const & textureAnims
		, sdw::Array< sdw::CombinedImage2DRgba32 > const & maps
		, c3d::shader::Material const & material
		, c3d::shader::BlendComponents & components
		, c3d::shader::SampleTexture const & sampleTexture )const
	{
		applyFloatComponent( cuT( "anisotropyStrength" )
			, passShaders
			, textureConfigs
			, textureAnims
			, material
			, components
			, sampleTexture );
	}

	//*********************************************************************************************

	void AnisotropyStrengthMapComponent::Plugin::createParsers( c3d::AttributeParsers & parsers
		, ChannelFillers & channelFillers )const
	{
		channelFillers.try_emplace( cuT( "anisotropy_strength" )
			, getTextureFlags()
			, []( TextureContext & blockContext )
			{
				auto const & component = getPassComponent< AnisotropyStrengthMapComponent >( blockContext );
				component.fillChannel( blockContext.configuration
					, 0x000000FFu );
			} );

		c3d::addParserT( parsers
			, CSCNSection::eTexture
			, cuT( "anisotropy_strength_mask" )
			, trscmp::parserUnitAnisotropyStrengthMask
			, { c3d::makeParameter< c3d::ParameterType::eUInt32 >() } );

		c3d::addParserT( parsers
			, CSCNSection::eTextureUnit
			, cuT( "anisotropy_strength_mask" )
			, trscmp::parserUnitAnisotropyStrengthMask
			, { c3d::makeParameter< c3d::ParameterType::eUInt32 >() } );

		c3d::addParserT( parsers
			, CSCNSection::eTextureRemap
			, CSCNSection::eTextureRemapChannel
			, cuT( "anisotropy_strength" )
			, trscmp::parserTexRemapAnisotropyStrength );

		c3d::addParserT( parsers
			, CSCNSection::eTextureRemapChannel
			, cuT( "anisotropy_strength_mask" )
			, trscmp::parserTexRemapAnisotropyStrengthMask
			, { c3d::makeParameter< c3d::ParameterType::eUInt32 >() } );
	}

	bool AnisotropyStrengthMapComponent::Plugin::isComponentNeeded( TextureCombine const & textures
		, ComponentModeFlags const & filter )const
	{
		return checkFlag( filter, ComponentModeFlag::eSpecularLighting )
			|| hasAny( textures, getTextureFlags() );
	}

	void AnisotropyStrengthMapComponent::Plugin::createMapComponent( Pass & pass
		, c3d::Vector< PassComponentUPtr > & result )const
	{
		result.push_back( c3d::makeUniqueDerived< PassComponent, AnisotropyStrengthMapComponent >( pass ) );
	}

	bool AnisotropyStrengthMapComponent::Plugin::doWriteTextureConfig( TextureConfiguration const & configuration
		, uint32_t mask
		, c3d::String const & tabs
		, c3d::StringStream & file )const
	{
		return c3d::TextWriter< AnisotropyStrengthMapComponent >{ tabs, mask }( file );
	}

	//*********************************************************************************************

	c3d::String const AnisotropyStrengthMapComponent::TypeName = C3D_PluginMakePassMapComponentName( "anisotropy", "strength" );

	AnisotropyStrengthMapComponent::AnisotropyStrengthMapComponent( Pass & pass )
		: PassMapComponent{ pass
		, TypeName
		, Strength
		, { AnisotropyComponent::TypeName } }
	{
	}

	PassComponentUPtr AnisotropyStrengthMapComponent::doClone( Pass & pass )const
	{
		return c3d::makeUniqueDerived< PassComponent, AnisotropyStrengthMapComponent >( pass );
	}

	void AnisotropyStrengthMapComponent::doFillConfig( TextureConfiguration & configuration
		, ConfigurationVisitorBase & vis )const
	{
		vis.visit( cuT( "Anisotropy Strength" ) );
		vis.visit( cuT( "Map" ), getTextureFlags(), getFlagConfiguration( configuration, getTextureFlags() ), 1u );
	}

	//*********************************************************************************************
}
