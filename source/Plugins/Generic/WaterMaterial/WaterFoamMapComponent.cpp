#include "WaterMaterial/WaterFoamMapComponent.hpp"
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

CU_ImplementSmartPtr( water, WaterFoamMapComponent )

namespace castor
{
	template<>
	class TextWriter< water::WaterFoamMapComponent >
		: public TextWriterT< water::WaterFoamMapComponent >
	{
	public:
		explicit TextWriter( String const & tabs
			, uint32_t mask = 0u )
			: TextWriterT< water::WaterFoamMapComponent >{ tabs }
			, m_mask{ mask }
		{
		}

		bool operator()( StringStream & file )
		{
			return writeMask( file, cuT( "water_foam_mask" ), m_mask );
		}

		bool operator()( water::WaterFoamMapComponent const & object
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
		static CU_ImplementAttributeParserBlock( parserUnitWaterFoamMask, TextureContext )
		{
			if ( params.empty() )
			{
				CU_ParsingError( cuT( "Missing parameter." ) );
			}
			else if ( !blockContext->pass )
			{
				auto & plugin = getEngine( *blockContext )->getPassComponentsRegister().getPlugin( WaterFoamMapComponent::TypeName );
				plugin.fillTextureConfiguration( blockContext->configuration
					, params[0]->get< uint32_t >() );
			}
			else
			{
				auto & plugin = blockContext->pass->pass->getComponentPlugin( WaterFoamMapComponent::TypeName );
				plugin.fillTextureConfiguration( blockContext->configuration
					, params[0]->get< uint32_t >() );
			}
		}
		CU_EndAttribute()

		static CU_ImplementAttributeParserBlock( parserTexRemapWaterFoam, SceneImportContext )
		{
			auto & plugin = getEngine( *blockContext )->getPassComponentsRegister().getPlugin( WaterFoamMapComponent::TypeName );
			blockContext->textureRemapIt = blockContext->textureRemaps.try_emplace( plugin.getTextureFlags() ).first;
			blockContext->textureRemapIt->second = TextureConfiguration{};
		}
		CU_EndAttributePushBlock( CSCNSection::eTextureRemapChannel, blockContext )

		static CU_ImplementAttributeParserBlock( parserTexRemapWaterFoamMask, SceneImportContext )
		{
			if ( params.empty() )
			{
				CU_ParsingError( cuT( "Missing parameter." ) );
			}
			else
			{
				auto & plugin = getEngine( *blockContext )->getPassComponentsRegister().getPlugin( WaterFoamMapComponent::TypeName );
				plugin.fillTextureConfiguration( blockContext->textureRemapIt->second
					, params[0]->get< uint32_t >() );
			}
		}
		CU_EndAttribute()
	}

	//*********************************************************************************************

	void WaterFoamMapComponent::ComponentsShader::fillComponents( castor3d::ComponentModeFlags componentsMask
		, sdw::type::BaseStruct & components
		, castor3d::shader::Materials const & materials
		, sdw::StructInstance const * surface )const
	{
		if ( !WaterComponent::isComponentAvailable( componentsMask, materials ) )
		{
			return;
		}

		if ( !components.hasMember( "waterFoam" ) )
		{
			components.declMember( "waterFoam", sdw::type::Kind::eVec3F );
			components.declMember( "waterFoamNoiseUV", sdw::type::Kind::eVec2F );
			components.declMember( "waterFoamNoise", sdw::type::Kind::eFloat );
		}
	}

	void WaterFoamMapComponent::ComponentsShader::fillComponentsInits( sdw::type::BaseStruct const & components
		, castor3d::shader::Materials const & materials
		, castor3d::shader::Material const * material
		, sdw::StructInstance const * surface
		, sdw::Vec4 const * clrCot
		, sdw::expr::ExprList & inits )const
	{
		if ( !components.hasMember( "waterFoam" ) )
		{
			return;
		}

		inits.emplace_back( sdw::makeExpr( vec3( 0.0_f ) ) );
		inits.emplace_back( sdw::makeExpr( vec2( 0.0_f ) ) );
		inits.emplace_back( sdw::makeExpr( 1.0_f ) );
	}

	void WaterFoamMapComponent::ComponentsShader::blendComponents( castor3d::shader::Materials const & materials
		, sdw::Float const & passMultiplier
		, castor3d::shader::BlendComponents & res
		, castor3d::shader::BlendComponents const & src )const
	{
		if ( res.hasMember( "waterFoam" ) )
		{
			res.getMember< sdw::Float >( "waterFoam" ) += src.getMember< sdw::Float >( "waterFoam" ) * passMultiplier;
		}
	}

	void WaterFoamMapComponent::ComponentsShader::applyTexture( castor3d::shader::PassShaders const & passShaders
		, castor3d::shader::TextureConfigurations const & textureConfigs
		, castor3d::shader::TextureAnimations const & textureAnims
		, sdw::Array< sdw::CombinedImage2DRgba32 > const & maps
		, castor3d::shader::Material const & material
		, castor3d::shader::BlendComponents & components
		, castor3d::shader::SampleTexture const & sampleTexture )const
	{
		castor::MbString mapName = "waterFoamMap";
		castor::MbString valueName = "waterFoam";

		if ( !material.hasMember( mapName )
			|| !components.hasMember( valueName ) )
		{
			return;
		}

		auto & writer{ *material.getWriter() };
		auto map = material.getMember< sdw::UInt >( mapName );
		auto value = components.getMember< sdw::Vec3 >( valueName );
		auto waterFoamNoiseUV = components.getMember< sdw::Vec2 >( "waterFoamNoiseUV" );

		auto config = writer.declLocale( valueName + "Config"
			, textureConfigs.getTextureConfiguration( map ) );
		auto anim = writer.declLocale( valueName + "Anim"
			, textureAnims.getTextureAnimation( map ) );
		passShaders.computeTexcoords( textureConfigs, config, anim, components );

		if ( checkFlag( passShaders.getFilter(), ComponentModeFlag::eDerivTex ) )
		{
			auto texCoords = components.getMember< castor3d::shader::DerivTex >( "texCoords" );
			waterFoamNoiseUV = texCoords.value();
		}
		else if ( passShaders.getPassCombine().baseId == 0u )
		{
			auto texCoords = components.getMember< sdw::Vec2 >( "texCoords" );
			waterFoamNoiseUV = texCoords;
		}
		else
		{
			auto texCoords = components.getMember< sdw::Vec3 >( "texCoords" );
			waterFoamNoiseUV = texCoords.xy();
		}

		if ( components.hasMember( "foamNoiseTiling" ) )
		{
			auto foamNoiseTiling = components.getMember< sdw::Float >( "foamNoiseTiling" );
			waterFoamNoiseUV *= foamNoiseTiling;
		}
	}

	void WaterFoamMapComponent::ComponentsShader::updateComponent( sdw::Array< sdw::CombinedImage2DRgba32 > const & maps
		, c3d::Material const & material
		, c3d::BlendComponents & components
		, bool isFrontCulled )const
	{
		castor::MbString valueName = "waterFoam";
		castor::MbString mapName = "waterFoam";
		auto textureName = mapName + "MapAndMask";

		if ( !material.hasMember( textureName )
			|| !components.hasMember( valueName )
			|| !components.hasMember( "foamTiling" )
			|| !components.hasMember( "waterNormalMapCoords1" )
			|| !components.hasMember( "waterNormalMapCoords2" ) )
		{
			return;
		}

		auto & writer = *components.getWriter();
		auto map = writer.declLocale( mapName + "Map"
			, material.getMember< sdw::UInt >( textureName ) >> 16u );
		auto mask = writer.declLocale( mapName + "Mask"
			, material.getMember< sdw::UInt >( textureName ) & 0xFFFFu );
		auto value = components.getMember< sdw::Vec3 >( valueName );
		auto waterFoamNoise = components.getMember< sdw::Float >( "waterFoamNoise" );
		auto foamTiling = components.getMember< sdw::Float >( "foamTiling" );
		auto waterNormalMapCoords1 = components.getMember< sdw::Vec2 >( "waterNormalMapCoords1" );
		auto waterNormalMapCoords2 = components.getMember< sdw::Vec2 >( "waterNormalMapCoords2" );
		auto foamSampled = writer.declLocale( "foamSampled"
			, maps[nonuniform( map - 1_u )].lod( ( waterNormalMapCoords1 + waterNormalMapCoords2 ) * foamTiling, 0.0_f ) );
		value = c3d::TextureConfigData::getVec3( foamSampled, mask );

		mapName = "waterNoise";
		textureName = mapName + "MapAndMask";
		if ( material.hasMember( textureName ) )
		{
			auto waterNoiseMap = writer.declLocale( mapName + "FoamMap"
				, material.getMember< sdw::UInt >( textureName ) >> 16u );
			auto waterNoiseMask = writer.declLocale( mapName + "FoamMask"
				, material.getMember< sdw::UInt >( textureName ) & 0xFFFFu );
			auto waterFoamNoiseUV = components.getMember< sdw::Vec2 >( "waterFoamNoiseUV" );
			waterFoamNoise = c3d::TextureConfigData::getFloat( maps[nonuniform( waterNoiseMap - 1_u )].lod( waterFoamNoiseUV * foamTiling, 0.0_f )
				, waterNoiseMask );
		}
	}

	//*********************************************************************************************

	void WaterFoamMapComponent::Plugin::createParsers( castor::AttributeParsers & parsers
		, ChannelFillers & channelFillers )const
	{
		channelFillers.try_emplace( cuT( "water_foam" )
			, getTextureFlags()
			, []( TextureContext & blockContext )
			{
				auto const & component = getPassComponent< WaterFoamMapComponent >( blockContext );
				component.fillChannel( blockContext.configuration
					, 0x00FF0000u );
			} );

		castor::addParserT( parsers
			, CSCNSection::eTexture
			, cuT( "water_foam_mask" )
			, trscmp::parserUnitWaterFoamMask
			, { castor::makeParameter< castor::ParameterType::eUInt32 >() } );

		castor::addParserT( parsers
			, CSCNSection::eTextureUnit
			, cuT( "water_foam_mask" )
			, trscmp::parserUnitWaterFoamMask
			, { castor::makeParameter< castor::ParameterType::eUInt32 >() } );

		castor::addParserT( parsers
			, CSCNSection::eTextureRemap
			, cuT( "water_foam" )
			, trscmp::parserTexRemapWaterFoam );

		castor::addParserT( parsers
			, CSCNSection::eTextureRemapChannel
			, cuT( "water_foam_mask" )
			, trscmp::parserTexRemapWaterFoamMask
			, { castor::makeParameter< castor::ParameterType::eUInt32 >() } );
	}

	bool WaterFoamMapComponent::Plugin::isComponentNeeded( TextureCombine const & textures
		, ComponentModeFlags const & filter )const
	{
		return checkFlag( filter, ComponentModeFlag::eSpecularLighting )
			|| checkFlag( filter, ComponentModeFlag::eDiffuseLighting )
			|| checkFlag( filter, ComponentModeFlag::eColour )
			|| hasAny( textures, getTextureFlags() );
	}

	void WaterFoamMapComponent::Plugin::createMapComponent( Pass & pass
		, castor::Vector< PassComponentUPtr > & result )const
	{
		result.push_back( castor::makeUniqueDerived< PassComponent, WaterFoamMapComponent >( pass ) );
	}

	bool WaterFoamMapComponent::Plugin::hasTexcoordModif( PassComponentRegister const & passComponents
		, PipelineFlags const * flags )const
	{
		if ( !flags )
		{
			return false;
		}

		auto & plugin = passComponents.getPlugin< WaterFoamMapComponent >();
		return hasAny( flags->textures, getTextureFlags() )
			&& hasAny( flags->pass, plugin.getComponentFlags() );
	}

	bool WaterFoamMapComponent::Plugin::doWriteTextureConfig( TextureConfiguration const & configuration
		, uint32_t mask
		, castor::String const & tabs
		, castor::StringStream & file )const
	{
		return castor::TextWriter< WaterFoamMapComponent >{ tabs, mask }( file );
	}

	//*********************************************************************************************

	castor::String const WaterFoamMapComponent::TypeName = C3D_PluginMakePassMapComponentName( "water", "foam" );

	WaterFoamMapComponent::WaterFoamMapComponent( Pass & pass )
		: PassMapComponent{ pass
		, TypeName
		, Foam
		, { WaterComponent::TypeName } }
	{
	}

	PassComponentUPtr WaterFoamMapComponent::doClone( Pass & pass )const
	{
		return castor::makeUniqueDerived< PassComponent, WaterFoamMapComponent >( pass );
	}

	void WaterFoamMapComponent::doFillConfig( TextureConfiguration & configuration
		, ConfigurationVisitorBase & vis )const
	{
		vis.visit( cuT( "WaterFoam" ) );
		vis.visit( cuT( "Map" ), getTextureFlags(), getFlagConfiguration( configuration, getTextureFlags() ), 1u );
	}

	//*********************************************************************************************
}
