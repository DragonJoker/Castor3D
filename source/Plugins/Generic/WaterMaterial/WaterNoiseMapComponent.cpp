#include "WaterMaterial/WaterNoiseMapComponent.hpp"
#include "WaterMaterial/WaterComponent.hpp"

#include <Castor3D/Engine.hpp>
#include <Castor3D/Material/Pass/Pass.hpp>
#include <Castor3D/Miscellaneous/ConfigurationVisitor.hpp>
#include <Castor3D/Material/Pass/Component/PassComponentRegister.hpp>
#include <Castor3D/Material/Texture/TextureConfiguration.hpp>
#include <Castor3D/Scene/SceneFileParserData.hpp>
#include <Castor3D/Scene/SceneImporter.hpp>
#include <Castor3D/Shader/ShaderBuffers/PassBuffer.hpp>
#include <Castor3D/Shader/Shaders/GlslBlendComponents.hpp>
#include <Castor3D/Shader/Shaders/GlslLighting.hpp>
#include <Castor3D/Shader/Shaders/GlslMaterial.hpp>
#include <Castor3D/Shader/Shaders/GlslTextureConfiguration.hpp>

#include <CastorUtils/FileParser/FileParser.hpp>

CU_ImplementSmartPtr( water, WaterNoiseMapComponent )

namespace castor
{
	template<>
	class TextWriter< water::WaterNoiseMapComponent >
		: public TextWriterT< water::WaterNoiseMapComponent >
	{
	public:
		explicit TextWriter( String const & tabs
			, uint32_t mask = 0u )
			: TextWriterT< water::WaterNoiseMapComponent >{ tabs }
			, m_mask{ mask }
		{
		}

		bool operator()( StringStream & file )const
		{
			return writeMask( file, cuT( "water_noise_mask" ), m_mask );
		}

		bool operator()( water::WaterNoiseMapComponent const & object
			, StringStream & file )override
		{
			return true;
		}

	private:
		uint32_t m_mask;
	};
}

namespace water
{
	using namespace castor3d;
	namespace c3d = castor3d::shader;

	//*********************************************************************************************

	namespace trscmp
	{
		static CU_ImplementAttributeParserBlock( parserUnitWaterNoiseMask, TextureContext )
		{
			if ( params.empty() )
			{
				CU_ParsingError( cuT( "Missing parameter." ) );
			}
			else if ( !blockContext->pass )
			{
				auto & plugin = getEngine( *blockContext )->getPassComponentsRegister().getPlugin( WaterNoiseMapComponent::TypeName );
				plugin.fillTextureConfiguration( blockContext->configuration
					, params[0]->get< uint32_t >() );
			}
			else
			{
				auto & plugin = blockContext->pass->pass->getComponentPlugin( WaterNoiseMapComponent::TypeName );
				plugin.fillTextureConfiguration( blockContext->configuration
					, params[0]->get< uint32_t >() );
			}
		}
		CU_EndAttribute()

		static CU_ImplementAttributeParserBlock( parserTexRemapWaterNoise, SceneImportContext )
		{
			auto & plugin = getEngine( *blockContext )->getPassComponentsRegister().getPlugin( WaterNoiseMapComponent::TypeName );
			blockContext->textureRemapIt = blockContext->textureRemaps.try_emplace( plugin.getTextureFlags() ).first;
			blockContext->textureRemapIt->second = TextureConfiguration{};
		}
		CU_EndAttributePushBlock( CSCNSection::eTextureRemapChannel, blockContext )

		static CU_ImplementAttributeParserBlock( parserTexRemapWaterNoiseMask, SceneImportContext )
		{
			if ( params.empty() )
			{
				CU_ParsingError( cuT( "Missing parameter." ) );
			}
			else
			{
				auto & plugin = getEngine( *blockContext )->getPassComponentsRegister().getPlugin( WaterNoiseMapComponent::TypeName );
				plugin.fillTextureConfiguration( blockContext->textureRemapIt->second
					, params[0]->get< uint32_t >() );
			}
		}
		CU_EndAttribute()
	}

	//*********************************************************************************************

	void WaterNoiseMapComponent::ComponentsShader::fillComponents( castor3d::ComponentModeFlags componentsMask
		, sdw::type::BaseStruct & components
		, castor3d::shader::Materials const & materials
		, sdw::StructInstance const * surface )const
	{
		if ( !WaterComponent::isComponentAvailable( componentsMask, materials ) )
		{
			return;
		}

		if ( !components.hasMember( "waterNoise" ) )
		{
			components.declMember( "waterNoise", sdw::type::Kind::eFloat );
		}
	}

	void WaterNoiseMapComponent::ComponentsShader::fillComponentsInits( sdw::type::BaseStruct const & components
		, castor3d::shader::Materials const & materials
		, castor3d::shader::Material const * material
		, sdw::StructInstance const * surface
		, sdw::Vec4 const * clrCot
		, sdw::expr::ExprList & inits )const
	{
		if ( !components.hasMember( "waterNoise" ) )
		{
			return;
		}

		inits.emplace_back( sdw::makeExpr( 0.0_f ) );
	}

	void WaterNoiseMapComponent::ComponentsShader::blendComponents( castor3d::shader::Materials const & materials
		, sdw::Float const & passMultiplier
		, castor3d::shader::BlendComponents & res
		, castor3d::shader::BlendComponents const & src )const
	{
		if ( res.hasMember( "waterNoise" ) )
		{
			res.getMember< sdw::Float >( "waterNoise" ) += src.getMember< sdw::Float >( "waterNoise" ) * passMultiplier;
		}
	}

	void WaterNoiseMapComponent::ComponentsShader::applyTexture( castor3d::shader::PassShaders const & passShaders
		, castor3d::shader::TextureConfigurations const & textureConfigs
		, castor3d::shader::TextureAnimations const & textureAnims
		, sdw::Array< sdw::CombinedImage2DRgba32 > const & maps
		, castor3d::shader::Material const & material
		, castor3d::shader::BlendComponents & components
		, castor3d::shader::SampleTexture const & sampleTexture )const
	{
		castor::MbString valueName = "waterNoise";
		castor::MbString mapName = "waterNoise";
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
		auto value = components.getMember< sdw::Float >( valueName );
		auto tiling = components.getMember< sdw::Float >( "noiseTiling" );

		auto config = writer.declLocale( valueName + "Config"
			, textureConfigs.getTextureConfiguration( map ) );
		auto anim = writer.declLocale( valueName + "Anim"
			, textureAnims.getTextureAnimation( map ) );
		passShaders.computeTexcoords( textureConfigs
			, config
			, anim
			, components );

		if ( checkFlag( passShaders.getFilter(), ComponentModeFlag::eDerivTex ) )
		{
			auto texCoords = components.getMember< castor3d::shader::DerivTex >( "texCoords" );
			texCoords.uv() *= 0.5_f * tiling;
		}
		else if ( passShaders.getPassCombine().baseId == 0u )
		{
			auto texCoords = components.getMember< sdw::Vec2 >( "texCoords" );
			texCoords *= 0.5_f * tiling;
		}
		else
		{
			auto texCoords = components.getMember< sdw::Vec3 >( "texCoords" );
			texCoords *= 0.5_f * tiling;
		}

		auto sampled = writer.declLocale( valueName + "Sampled"
			, sampleTexture( map, config, components ) );
		value = c3d::TextureConfigData::getFloat( sampled, mask );
	}

	void WaterNoiseMapComponent::ComponentsShader::updateComponent( sdw::Array< sdw::CombinedImage2DRgba32 > const & maps
		, c3d::Material const & material
		, c3d::BlendComponents & components
		, bool isFrontCulled )const
	{
		castor::MbString valueName = "waterNoise";
		castor::MbString mapName = "waterNoise";
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
		auto value = components.getMember< sdw::Float >( valueName );
		auto tiling = components.getMember< sdw::Float >( "noiseTiling" );

		if ( components.hasMember( "waterNormalMapCoords1" ) )
		{
			auto waterNormalMapCoords1 = components.getMember< sdw::Vec2 >( "waterNormalMapCoords1" );
			value *= c3d::TextureConfigData::getFloat( maps[nonuniform( map - 1_u )].lod( waterNormalMapCoords1 * tiling * 0.5_f, 0.0_f )
				, mask );
		}

		if ( components.hasMember( "waterNormalMapCoords2" ) )
		{
			auto waterNormalMapCoords2 = components.getMember< sdw::Vec2 >( "waterNormalMapCoords2" );
			value *= c3d::TextureConfigData::getFloat( maps[nonuniform( map - 1_u )].lod( waterNormalMapCoords2 * tiling * 0.5_f, 0.0_f )
				, mask );
		}

		components.specular *= value;
	}

	//*********************************************************************************************

	void WaterNoiseMapComponent::Plugin::createParsers( castor::AttributeParsers & parsers
		, ChannelFillers & channelFillers )const
	{
		channelFillers.try_emplace( cuT( "water_noise" )
			, getTextureFlags()
			, []( TextureContext & blockContext )
			{
				auto const & component = getPassComponent< WaterNoiseMapComponent >( blockContext );
				component.fillChannel( blockContext.configuration
					, 0x00FF0000u );
			} );

		castor::addParserT( parsers
			, CSCNSection::eTexture
			, cuT( "water_noise_mask" )
			, trscmp::parserUnitWaterNoiseMask
			, { castor::makeParameter< castor::ParameterType::eUInt32 >() } );

		castor::addParserT( parsers
			, CSCNSection::eTextureUnit
			, cuT( "water_noise_mask" )
			, trscmp::parserUnitWaterNoiseMask
			, { castor::makeParameter< castor::ParameterType::eUInt32 >() } );

		castor::addParserT( parsers
			, CSCNSection::eTextureRemap
			, cuT( "water_noise" )
			, trscmp::parserTexRemapWaterNoise );

		castor::addParserT( parsers
			, CSCNSection::eTextureRemapChannel
			, cuT( "water_noise_mask" )
			, trscmp::parserTexRemapWaterNoiseMask
			, { castor::makeParameter< castor::ParameterType::eUInt32 >() } );
	}

	bool WaterNoiseMapComponent::Plugin::isComponentNeeded( TextureCombine const & textures
		, ComponentModeFlags const & filter )const
	{
		return checkFlag( filter, ComponentModeFlag::eSpecularLighting )
			|| hasAny( textures, getTextureFlags() );
	}

	void WaterNoiseMapComponent::Plugin::createMapComponent( Pass & pass
		, castor::Vector< PassComponentUPtr > & result )const
	{
		result.push_back( castor::makeUniqueDerived< PassComponent, WaterNoiseMapComponent >( pass ) );
	}

	bool WaterNoiseMapComponent::Plugin::hasTexcoordModif( PassComponentRegister const & passComponents
		, PipelineFlags const * flags )const
	{
		if ( !flags )
		{
			return false;
		}

		auto & plugin = passComponents.getPlugin< WaterNoiseMapComponent >();
		return hasAny( flags->textures, getTextureFlags() )
			&& hasAny( flags->pass, plugin.getComponentFlags() );
	}

	bool WaterNoiseMapComponent::Plugin::doWriteTextureConfig( TextureConfiguration const & configuration
		, uint32_t mask
		, castor::String const & tabs
		, castor::StringStream & file )const
	{
		return castor::TextWriter< WaterNoiseMapComponent >{ tabs, mask }( file );
	}

	//*********************************************************************************************

	castor::String const WaterNoiseMapComponent::TypeName = C3D_PluginMakePassMapComponentName( "water", "noise" );

	WaterNoiseMapComponent::WaterNoiseMapComponent( Pass & pass )
		: PassMapComponent{ pass
		, TypeName
		, Noise
		, { WaterComponent::TypeName } }
	{
	}

	PassComponentUPtr WaterNoiseMapComponent::doClone( Pass & pass )const
	{
		return castor::makeUniqueDerived< PassComponent, WaterNoiseMapComponent >( pass );
	}

	void WaterNoiseMapComponent::doFillConfig( TextureConfiguration & configuration
		, ConfigurationVisitorBase & vis )const
	{
		vis.visit( cuT( "WaterNoise" ) );
		vis.visit( cuT( "Map" ), getTextureFlags(), getFlagConfiguration( configuration, getTextureFlags() ), 1u );
	}

	//*********************************************************************************************
}
