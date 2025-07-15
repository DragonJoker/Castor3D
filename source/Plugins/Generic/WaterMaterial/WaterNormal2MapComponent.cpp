#include "WaterMaterial/WaterNormal2MapComponent.hpp"
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
#include <Castor3D/Shader/Shaders/GlslDerivativeValue.hpp>
#include <Castor3D/Shader/Shaders/GlslLighting.hpp>
#include <Castor3D/Shader/Shaders/GlslMaterial.hpp>
#include <Castor3D/Shader/Shaders/GlslTextureConfiguration.hpp>
#include <Castor3D/Shader/Shaders/GlslUtils.hpp>

#include <CastorUtils/FileParser/FileParser.hpp>

CU_ImplementSmartPtr( water, WaterNormal2MapComponent )

namespace c3d
{
	template<>
	class TextWriter< water::WaterNormal2MapComponent >
		: public TextWriterT< water::WaterNormal2MapComponent >
	{
	public:
		explicit TextWriter( String const & tabs
			, uint32_t mask = 0u )
			: TextWriterT< water::WaterNormal2MapComponent >{ tabs }
			, m_mask{ mask }
		{
		}

		bool operator()( StringStream & file )const
		{
			return writeMask( file, cuT( "water_normal2_mask" ), m_mask );
		}

		bool operator()( water::WaterNormal2MapComponent const & object
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
	using namespace c3d;
	namespace c3ds = c3d::shader;

	//*********************************************************************************************

	namespace trscmp
	{
		static CU_ImplementAttributeParserBlock( parserUnitWaterNormal2Mask, TextureContext )
		{
			if ( params.empty() )
			{
				CU_ParsingError( cuT( "Missing parameter." ) );
			}
			else if ( !blockContext->pass )
			{
				auto & plugin = getEngine( *blockContext )->getPassComponentsRegister().getPlugin( WaterNormal2MapComponent::TypeName );
				plugin.fillTextureConfiguration( blockContext->configuration
					, params[0]->get< uint32_t >() );
			}
			else
			{
				auto & plugin = blockContext->pass->pass->getComponentPlugin( WaterNormal2MapComponent::TypeName );
				plugin.fillTextureConfiguration( blockContext->configuration
					, params[0]->get< uint32_t >() );
			}
		}
		CU_EndAttribute()

		static CU_ImplementAttributeParserBlock( parserTexRemapWaterNormal2, SceneImportContext )
		{
			auto & plugin = getEngine( *blockContext )->getPassComponentsRegister().getPlugin( WaterNormal2MapComponent::TypeName );
			blockContext->textureRemapIt = blockContext->textureRemaps.try_emplace( plugin.getTextureFlags() ).first;
			blockContext->textureRemapIt->second = TextureConfiguration{};
		}
		CU_EndAttributePushBlock( CSCNSection::eTextureRemapChannel, blockContext )

		static CU_ImplementAttributeParserBlock( parserTexRemapWaterNormal2Mask, SceneImportContext )
		{
			if ( params.empty() )
			{
				CU_ParsingError( cuT( "Missing parameter." ) );
			}
			else
			{
				auto & plugin = getEngine( *blockContext )->getPassComponentsRegister().getPlugin( WaterNormal2MapComponent::TypeName );
				plugin.fillTextureConfiguration( blockContext->textureRemapIt->second
					, params[0]->get< uint32_t >() );
			}
		}
		CU_EndAttribute()
	}

	//*********************************************************************************************

	void WaterNormal2MapComponent::ComponentsShader::fillComponents( c3d::ComponentModeFlags componentsMask
		, sdw::type::BaseStruct & components
		, c3d::shader::Materials const & materials
		, sdw::StructInstance const * surface )const
	{
		if ( !WaterComponent::isComponentAvailable( componentsMask, materials ) )
		{
			return;
		}

		if ( !components.hasMember( "waterNormalMapCoords2" ) )
		{
			components.declMember( "waterNormalMapCoords2", sdw::type::Kind::eVec2F );
			components.declMember( "waterNormals2", sdw::type::Kind::eVec3F );
		}
	}

	void WaterNormal2MapComponent::ComponentsShader::fillComponentsInits( sdw::type::BaseStruct const & components
		, c3d::shader::Materials const & materials
		, c3d::shader::Material const * material
		, sdw::StructInstance const * surface
		, sdw::Vec4 const * clrCot
		, sdw::expr::ExprList & inits )const
	{
		if ( !components.hasMember( "waterNormalMapCoords2" ) )
		{
			return;
		}

		inits.emplace_back( sdw::makeExpr( vec2( 0.0_f ) ) );
		inits.emplace_back( sdw::makeExpr( vec3( 0.0_f ) ) );
	}

	void WaterNormal2MapComponent::ComponentsShader::blendComponents( c3d::shader::Materials const & materials
		, sdw::Float const & passMultiplier
		, c3d::shader::BlendComponents & res
		, c3d::shader::BlendComponents const & src )const
	{
		if ( res.hasMember( "waterNormals2" ) )
		{
			res.getMember< sdw::Float >( "waterNormals2" ) += src.getMember< sdw::Float >( "waterNormals2" ) * passMultiplier;
		}
	}

	void WaterNormal2MapComponent::ComponentsShader::applyTexture( c3d::shader::PassShaders const & passShaders
		, c3d::shader::TextureConfigurations const & textureConfigs
		, c3d::shader::TextureAnimations const & textureAnims
		, sdw::Array< sdw::CombinedImage2DRgba32 > const & maps
		, c3d::shader::Material const & material
		, c3d::shader::BlendComponents & components
		, c3d::shader::SampleTexture const & sampleTexture )const
	{
		c3d::MbString valueName = "waterNormals2";
		c3d::MbString mapName = "waterNormal2";
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
		auto value = components.getMember< sdw::Vec3 >( valueName );

		auto config = writer.declLocale( valueName + "Config"
			, textureConfigs.getTextureConfiguration( map ) );
		auto uv = components.getMember< sdw::Vec2 >( "waterNormalMapCoords2" );
		auto anim = writer.declLocale( valueName + "Anim"
			, textureAnims.getTextureAnimation( map ) );
		passShaders.computeTexcoords( textureConfigs
			, config
			, anim
			, components );

		if ( checkFlag( passShaders.getFilter(), ComponentModeFlag::eDerivTex ) )
		{
			auto texCoords = components.getMember< c3d::shader::DerivTex >( "texCoords" );
			uv = config.getUv( texCoords );
		}
		else if ( passShaders.getPassCombine().baseId == 0u )
		{
			auto texCoords = components.getMember< sdw::Vec2 >( "texCoords" );
			uv = texCoords;
		}
		else
		{
			auto texCoords = components.getMember< sdw::Vec3 >( "texCoords" );
			uv = config.getUv( texCoords );
		}

		auto sampled = writer.declLocale( valueName + "Sampled"
			, sampleTexture( map, config, components ) );
		value = c3d::shader::TextureConfigData::getVec3( sampled, mask ) * 2.0_f - 1.0_f;
	}

	//*********************************************************************************************

	void WaterNormal2MapComponent::Plugin::createParsers( c3d::AttributeParsers & parsers
		, ChannelFillers & channelFillers )const
	{
		channelFillers.try_emplace( cuT( "water_normal2" )
			, getTextureFlags()
			, []( TextureContext & blockContext )
			{
				auto const & component = getPassComponent< WaterNormal2MapComponent >( blockContext );
				component.fillChannel( blockContext.configuration
					, 0x00FFFFFFu );
			} );

		c3d::addParserT( parsers
			, CSCNSection::eTexture
			, cuT( "water_normal2_mask" )
			, trscmp::parserUnitWaterNormal2Mask
			, { c3d::makeParameter< c3d::ParameterType::eUInt32 >() } );

		c3d::addParserT( parsers
			, CSCNSection::eTextureUnit
			, cuT( "water_normal2_mask" )
			, trscmp::parserUnitWaterNormal2Mask
			, { c3d::makeParameter< c3d::ParameterType::eUInt32 >() } );

		c3d::addParserT( parsers
			, CSCNSection::eTextureRemap
			, CSCNSection::eTextureRemapChannel
			, cuT( "water_normal2" )
			, trscmp::parserTexRemapWaterNormal2 );

		c3d::addParserT( parsers
			, CSCNSection::eTextureRemapChannel
			, cuT( "water_normal2_mask" )
			, trscmp::parserTexRemapWaterNormal2Mask
			, { c3d::makeParameter< c3d::ParameterType::eUInt32 >() } );
	}

	bool WaterNormal2MapComponent::Plugin::isComponentNeeded( TextureCombine const & textures
		, ComponentModeFlags const & filter )const
	{
		return checkFlag( filter, ComponentModeFlag::eSpecularLighting )
			|| hasAny( textures, getTextureFlags() );
	}

	void WaterNormal2MapComponent::Plugin::createMapComponent( Pass & pass
		, c3d::Vector< PassComponentUPtr > & result )const
	{
		result.push_back( c3d::makeUniqueDerived< PassComponent, WaterNormal2MapComponent >( pass ) );
	}

	bool WaterNormal2MapComponent::Plugin::doWriteTextureConfig( TextureConfiguration const & configuration
		, uint32_t mask
		, c3d::String const & tabs
		, c3d::StringStream & file )const
	{
		return c3d::TextWriter< WaterNormal2MapComponent >{ tabs, mask }( file );
	}

	//*********************************************************************************************

	c3d::String const WaterNormal2MapComponent::TypeName = C3D_PluginMakePassMapComponentName( "water", "normal2" );

	WaterNormal2MapComponent::WaterNormal2MapComponent( Pass & pass )
		: PassMapComponent{ pass
		, TypeName
		, Normal2
		, { WaterComponent::TypeName } }
	{
	}

	PassComponentUPtr WaterNormal2MapComponent::doClone( Pass & pass )const
	{
		return c3d::makeUniqueDerived< PassComponent, WaterNormal2MapComponent >( pass );
	}

	void WaterNormal2MapComponent::doFillConfig( TextureConfiguration & configuration
		, ConfigurationVisitorBase & vis )const
	{
		vis.visit( cuT( "WaterNormal2" ) );
		vis.visit( cuT( "Map" ), getTextureFlags(), getFlagConfiguration( configuration, getTextureFlags() ), 1u );
	}

	//*********************************************************************************************
}
