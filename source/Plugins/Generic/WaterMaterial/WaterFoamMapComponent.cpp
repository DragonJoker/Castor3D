#include "WaterMaterial/WaterFoamMapComponent.hpp"
#include "WaterMaterial/WaterComponent.hpp"

#include <Castor3D/Engine.hpp>
#include <Castor3D/Material/Pass/Pass.hpp>
#include <Castor3D/Miscellaneous/ConfigurationVisitor.hpp>
#include <Castor3D/Material/Pass/Component/PassComponentRegister.hpp>
#include <Castor3D/Material/Texture/TextureConfiguration.hpp>
#include <Castor3D/Scene/SceneFileParser.hpp>
#include <Castor3D/Shader/ShaderBuffers/PassBuffer.hpp>
#include <Castor3D/Shader/Shaders/GlslBlendComponents.hpp>
#include <Castor3D/Shader/Shaders/GlslLighting.hpp>
#include <Castor3D/Shader/Shaders/GlslMaterial.hpp>
#include <Castor3D/Shader/Shaders/GlslTextureConfiguration.hpp>

#include <CastorUtils/FileParser/ParserParameter.hpp>

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
		static CU_ImplementAttributeParser( parserUnitWaterFoamMask )
		{
			auto & parsingContext = getParserContext( context );

			if ( params.empty() )
			{
				CU_ParsingError( cuT( "Missing parameter." ) );
			}
			else if ( !parsingContext.pass )
			{
				auto & plugin = parsingContext.parser->getEngine()->getPassComponentsRegister().getPlugin( WaterFoamMapComponent::TypeName );
				plugin.fillTextureConfiguration( parsingContext.texture.configuration
					, params[0]->get< uint32_t >() );
			}
			else
			{
				auto & plugin = parsingContext.pass->getComponentPlugin( WaterFoamMapComponent::TypeName );
				plugin.fillTextureConfiguration( parsingContext.texture.configuration
					, params[0]->get< uint32_t >() );
			}
		}
		CU_EndAttribute()

			static CU_ImplementAttributeParser( parserTexRemapWaterFoam )
		{
			auto & parsingContext = getParserContext( context );
			auto & plugin = parsingContext.parser->getEngine()->getPassComponentsRegister().getPlugin( WaterFoamMapComponent::TypeName );
			parsingContext.sceneImportConfig.textureRemapIt = parsingContext.sceneImportConfig.textureRemaps.emplace( plugin.getTextureFlags(), TextureConfiguration{} ).first;
			parsingContext.sceneImportConfig.textureRemapIt->second = TextureConfiguration{};
		}
		CU_EndAttributePush( CSCNSection::eTextureRemapChannel )

			static CU_ImplementAttributeParser( parserTexRemapWaterFoamMask )
		{
			auto & parsingContext = getParserContext( context );

			if ( params.empty() )
			{
				CU_ParsingError( cuT( "Missing parameter." ) );
			}
			else
			{
				auto & plugin = parsingContext.parser->getEngine()->getPassComponentsRegister().getPlugin( WaterFoamMapComponent::TypeName );
				plugin.fillTextureConfiguration( parsingContext.sceneImportConfig.textureRemapIt->second
					, params[0]->get< uint32_t >() );
			}
		}
		CU_EndAttribute()
	}

	//*********************************************************************************************

	void WaterFoamMapComponent::ComponentsShader::computeTexcoord( PipelineFlags const & flags
		, castor3d::shader::TextureConfigData const & config
		, sdw::U32Vec3 const & imgCompConfig
		, sdw::CombinedImage2DRgba32 const & map
		, sdw::Vec3 & texCoords
		, sdw::Vec2 & texCoord
		, sdw::UInt const & mapId
		, castor3d::shader::BlendComponents & components )const
	{
		if ( !components.hasMember( "waterFoam" ) )
		{
			return;
		}

		components.getMember< sdw::UInt >( "waterFoamMapId" ) = mapId;
		components.getMember< sdw::UInt >( "waterFoamMapMask" ) = imgCompConfig.z();
		components.getMember< sdw::Vec2 >( "waterFoamNoiseUV" ) = texCoord;

		if ( components.hasMember( "foamNoiseTiling" ) )
		{
			auto foamNoiseTiling = components.getMember< sdw::Float >( "foamNoiseTiling" );
			components.getMember< sdw::Vec2 >( "waterFoamNoiseUV" ) *= foamNoiseTiling;
		}
	}

	void WaterFoamMapComponent::ComponentsShader::computeTexcoord( PipelineFlags const & flags
		, castor3d::shader::TextureConfigData const & config
		, sdw::U32Vec3 const & imgCompConfig
		, sdw::CombinedImage2DRgba32 const & map
		, castor3d::shader::DerivTex & texCoords
		, castor3d::shader::DerivTex & texCoord
		, sdw::UInt const & mapId
		, castor3d::shader::BlendComponents & components )const
	{
		if ( !components.hasMember( "waterFoam" ) )
		{
			return;
		}

		components.getMember< sdw::UInt >( "waterFoamMapId" ) = mapId;
		components.getMember< sdw::UInt >( "waterFoamMapMask" ) = imgCompConfig.z();
		components.getMember< sdw::Vec2 >( "waterFoamNoiseUV" ) = texCoord.uv();

		if ( components.hasMember( "foamNoiseTiling" ) )
		{
			auto foamNoiseTiling = components.getMember< sdw::Float >( "foamNoiseTiling" );
			components.getMember< sdw::Vec2 >( "waterFoamNoiseUV" ) *= foamNoiseTiling;
		}
	}

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
			components.declMember( "waterFoamMapId", sdw::type::Kind::eUInt );
			components.declMember( "waterFoamMapMask", sdw::type::Kind::eUInt );
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
		inits.emplace_back( sdw::makeExpr( 0_u ) );
		inits.emplace_back( sdw::makeExpr( 0_u ) );
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

	void WaterFoamMapComponent::ComponentsShader::updateComponent( TextureCombine const & combine
		, sdw::Array< sdw::CombinedImage2DRgba32 > const & maps
		, c3d::BlendComponents & components )const
	{
		if ( !components.hasMember( "waterFoam" )
			|| !components.hasMember( "foamTiling" )
			|| !components.hasMember( "waterNormalMapCoords1" )
			|| !components.hasMember( "waterNormalMapCoords2" ) )
		{
			return;
		}

		auto & writer = *components.getWriter();
		auto waterFoam = components.getMember< sdw::Vec3 >( "waterFoam" );
		auto waterFoamNoise = components.getMember< sdw::Float >( "waterFoamNoise" );
		auto foamTiling = components.getMember< sdw::Float >( "foamTiling" );
		auto waterFoamMapId = components.getMember< sdw::UInt >( "waterFoamMapId" );
		auto waterFoamMapMask = components.getMember< sdw::UInt >( "waterFoamMapMask" );
		auto waterNormalMapCoords1 = components.getMember< sdw::Vec2 >( "waterNormalMapCoords1" );
		auto waterNormalMapCoords2 = components.getMember< sdw::Vec2 >( "waterNormalMapCoords2" );
		auto foamSampled = writer.declLocale( "foamSampled"
			, maps[nonuniform( waterFoamMapId - 1_u )].lod( ( waterNormalMapCoords1 + waterNormalMapCoords2 ) * foamTiling, 0.0_f ) );
		waterFoam = c3d::TextureConfigData::getVec3( foamSampled, waterFoamMapMask );

		if ( components.hasMember( "waterNoiseMapId" )
			&& components.hasMember( "waterNoiseMapMask" ) )
		{
			auto waterNoiseMapId = components.getMember< sdw::UInt >( "waterNoiseMapId" );
			auto waterNoiseMapMask = components.getMember< sdw::UInt >( "waterNoiseMapMask" );
			auto waterFoamNoiseUV = components.getMember< sdw::Vec2 >( "waterFoamNoiseUV" );
			waterFoamNoise = c3d::TextureConfigData::getFloat( maps[nonuniform( waterNoiseMapId - 1_u )].lod( waterFoamNoiseUV * foamTiling, 0.0_f )
				, waterNoiseMapMask );
		}
	}

	//*********************************************************************************************

	void WaterFoamMapComponent::Plugin::createParsers( castor::AttributeParsers & parsers
		, ChannelFillers & channelFillers )const
	{
		channelFillers.emplace( "water_foam", ChannelFiller{ getTextureFlags()
			, []( SceneFileContext & parsingContext )
			{
				auto & component = getPassComponent< WaterFoamMapComponent >( parsingContext );
				component.fillChannel( parsingContext.texture.configuration
					, 0x00FF0000u );
			} } );

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
			|| hasAny( textures, getTextureFlags() );
	}

	void WaterFoamMapComponent::Plugin::createMapComponent( Pass & pass
		, std::vector< PassComponentUPtr > & result )const
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
