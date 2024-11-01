#include "AnisotropicMaterial/AnisotropyDirectionMapComponent.hpp"
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

CU_ImplementSmartPtr( anisotropy, AnisotropyDirectionMapComponent )

namespace castor
{
	template<>
	class TextWriter< anisotropy::AnisotropyDirectionMapComponent >
		: public TextWriterT< anisotropy::AnisotropyDirectionMapComponent >
	{
	public:
		explicit TextWriter( String const & tabs
			, uint32_t mask = 0u )
			: TextWriterT< anisotropy::AnisotropyDirectionMapComponent >{ tabs }
			, m_mask{ mask }
		{
		}

		bool operator()( StringStream & file )const
		{
			return writeMask( file, cuT( "anisotropy_direction_mask" ), m_mask );
		}

		bool operator()( anisotropy::AnisotropyDirectionMapComponent const & object
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
	using namespace castor3d;
	namespace c3d = castor3d::shader;

	//*********************************************************************************************

	namespace trscmp
	{
		static CU_ImplementAttributeParserBlock( parserUnitAnisotropyDirectionMask, TextureContext )
		{
			if ( params.empty() )
			{
				CU_ParsingError( cuT( "Missing parameter." ) );
			}
			else if ( !blockContext->pass )
			{
				auto & plugin = getEngine( *blockContext )->getPassComponentsRegister().getPlugin( AnisotropyDirectionMapComponent::TypeName );
				plugin.fillTextureConfiguration( blockContext->configuration
					, params[0]->get< uint32_t >() );
			}
			else
			{
				auto & plugin = blockContext->pass->pass->getComponentPlugin( AnisotropyDirectionMapComponent::TypeName );
				plugin.fillTextureConfiguration( blockContext->configuration
					, params[0]->get< uint32_t >() );
			}
		}
		CU_EndAttribute()

		static CU_ImplementAttributeParserBlock( parserTexRemapAnisotropyDirection, SceneImportContext )
		{
			auto & plugin = getEngine( *blockContext )->getPassComponentsRegister().getPlugin( AnisotropyDirectionMapComponent::TypeName );
			blockContext->textureRemapIt = blockContext->textureRemaps.try_emplace( plugin.getTextureFlags() ).first;
			blockContext->textureRemapIt->second = TextureConfiguration{};
		}
		CU_EndAttributePushBlock( CSCNSection::eTextureRemapChannel, blockContext )

		static CU_ImplementAttributeParserBlock( parserTexRemapAnisotropyDirectionMask, SceneImportContext )
		{
			if ( params.empty() )
			{
				CU_ParsingError( cuT( "Missing parameter." ) );
			}
			else
			{
				auto & plugin = getEngine( *blockContext )->getPassComponentsRegister().getPlugin( AnisotropyDirectionMapComponent::TypeName );
				plugin.fillTextureConfiguration( blockContext->textureRemapIt->second
					, params[0]->get< uint32_t >() );
			}
		}
		CU_EndAttribute()
	}

	//*********************************************************************************************

	void AnisotropyDirectionMapComponent::ComponentsShader::applyTexture( castor3d::shader::PassShaders const & passShaders
		, castor3d::shader::TextureConfigurations const & textureConfigs
		, castor3d::shader::TextureAnimations const & textureAnims
		, sdw::Array< sdw::CombinedImage2DRgba32 > const & maps
		, castor3d::shader::Material const & material
		, castor3d::shader::BlendComponents & components
		, castor3d::shader::SampleTexture const & sampleTexture )const
	{
		castor::MbString valueName = "anisotropyDirection";
		castor::MbString mapName = "anisotropyDirection";
		auto textureName = mapName + "MapAndMask";

		if ( !material.hasMember( textureName )
			|| !components.hasMember( valueName ) )
		{
			return;
		}

		auto & writer{ *material.getWriter() };
		auto map = writer.declLocale( mapName + "Map"
			, material.getMember< sdw::UInt >( textureName ) >> 16u );
		auto mask = writer.declLocale( mapName + "Mask"
			, material.getMember< sdw::UInt >( textureName ) & 0xFFFFu );
		auto value = components.getMember< sdw::Vec2 >( valueName );

		auto config = writer.declLocale( valueName + "Config"
			, textureConfigs.getTextureConfiguration( map ) );
		auto anim = writer.declLocale( valueName + "Anim"
			, textureAnims.getTextureAnimation( map ) );
		passShaders.computeTexcoords( textureConfigs
			, config
			, anim
			, components );
		auto sampled = writer.declLocale( valueName + "Sampled"
			, sampleTexture( map, config, components ) );
		value = mat2( vec2( value.x(), value.y() ), vec2( -value.y(), value.x() ) )
			* normalize( c3d::TextureConfigData::getVec2( sampled, mask ) * 2.0_f - vec2( 1.0_f ) );
	}

	//*********************************************************************************************

	void AnisotropyDirectionMapComponent::Plugin::createParsers( castor::AttributeParsers & parsers
		, ChannelFillers & channelFillers )const
	{
		channelFillers.try_emplace( cuT( "anisotropy_direction" )
			, getTextureFlags()
			, []( TextureContext & blockContext )
			{
				auto const & component = getPassComponent< AnisotropyDirectionMapComponent >( blockContext );
				component.fillChannel( blockContext.configuration
					, 0x00FFFF00u );
			} );

		castor::addParserT( parsers
			, CSCNSection::eTexture
			, cuT( "anisotropy_direction_mask" )
			, trscmp::parserUnitAnisotropyDirectionMask
			, { castor::makeParameter< castor::ParameterType::eUInt32 >() } );

		castor::addParserT( parsers
			, CSCNSection::eTextureUnit
			, cuT( "anisotropy_direction_mask" )
			, trscmp::parserUnitAnisotropyDirectionMask
			, { castor::makeParameter< castor::ParameterType::eUInt32 >() } );

		castor::addParserT( parsers
			, CSCNSection::eTextureRemap
			, CSCNSection::eTextureRemapChannel
			, cuT( "anisotropy_direction" )
			, trscmp::parserTexRemapAnisotropyDirection );

		castor::addParserT( parsers
			, CSCNSection::eTextureRemapChannel
			, cuT( "anisotropy_direction_mask" )
			, trscmp::parserTexRemapAnisotropyDirectionMask
			, { castor::makeParameter< castor::ParameterType::eUInt32 >() } );
	}

	bool AnisotropyDirectionMapComponent::Plugin::isComponentNeeded( TextureCombine const & textures
		, ComponentModeFlags const & filter )const
	{
		return checkFlag( filter, ComponentModeFlag::eSpecularLighting )
			|| hasAny( textures, getTextureFlags() );
	}

	void AnisotropyDirectionMapComponent::Plugin::createMapComponent( Pass & pass
		, castor::Vector< PassComponentUPtr > & result )const
	{
		result.push_back( castor::makeUniqueDerived< PassComponent, AnisotropyDirectionMapComponent >( pass ) );
	}

	bool AnisotropyDirectionMapComponent::Plugin::doWriteTextureConfig( TextureConfiguration const & configuration
		, uint32_t mask
		, castor::String const & tabs
		, castor::StringStream & file )const
	{
		return castor::TextWriter< AnisotropyDirectionMapComponent >{ tabs, mask }( file );
	}

	//*********************************************************************************************

	castor::String const AnisotropyDirectionMapComponent::TypeName = C3D_PluginMakePassMapComponentName( "anisotropy", "direction" );

	AnisotropyDirectionMapComponent::AnisotropyDirectionMapComponent( Pass & pass )
		: PassMapComponent{ pass
		, TypeName
		, Direction
		, { AnisotropyComponent::TypeName } }
	{
	}

	PassComponentUPtr AnisotropyDirectionMapComponent::doClone( Pass & pass )const
	{
		return castor::makeUniqueDerived< PassComponent, AnisotropyDirectionMapComponent >( pass );
	}

	void AnisotropyDirectionMapComponent::doFillConfig( TextureConfiguration & configuration
		, ConfigurationVisitorBase & vis )const
	{
		vis.visit( cuT( "Anisotropy Direction" ) );
		vis.visit( cuT( "Map" ), getTextureFlags(), getFlagConfiguration( configuration, getTextureFlags() ), 2u );
	}

	//*********************************************************************************************
}
