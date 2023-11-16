#include "WaterMaterial/WaterNormal1MapComponent.hpp"
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

CU_ImplementSmartPtr( water, WaterNormal1MapComponent )

namespace castor
{
	template<>
	class TextWriter< water::WaterNormal1MapComponent >
		: public TextWriterT< water::WaterNormal1MapComponent >
	{
	public:
		explicit TextWriter( String const & tabs
			, uint32_t mask = 0u )
			: TextWriterT< water::WaterNormal1MapComponent >{ tabs }
			, m_mask{ mask }
		{
		}

		bool operator()( StringStream & file )
		{
			return writeMask( file, cuT( "water_normal1_mask" ), m_mask );
		}

		bool operator()( water::WaterNormal1MapComponent const & object
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
		static CU_ImplementAttributeParser( parserUnitWaterNormal1Mask )
		{
			auto & parsingContext = getParserContext( context );

			if ( params.empty() )
			{
				CU_ParsingError( cuT( "Missing parameter." ) );
			}
			else if ( !parsingContext.pass )
			{
				auto & plugin = parsingContext.parser->getEngine()->getPassComponentsRegister().getPlugin( WaterNormal1MapComponent::TypeName );
				plugin.fillTextureConfiguration( parsingContext.texture.configuration
					, params[0]->get< uint32_t >() );
			}
			else
			{
				auto & plugin = parsingContext.pass->getComponentPlugin( WaterNormal1MapComponent::TypeName );
				plugin.fillTextureConfiguration( parsingContext.texture.configuration
					, params[0]->get< uint32_t >() );
			}
		}
		CU_EndAttribute()

		static CU_ImplementAttributeParser( parserTexRemapWaterNormal1 )
		{
			auto & parsingContext = getParserContext( context );
			auto & plugin = parsingContext.parser->getEngine()->getPassComponentsRegister().getPlugin( WaterNormal1MapComponent::TypeName );
			parsingContext.sceneImportConfig.textureRemapIt = parsingContext.sceneImportConfig.textureRemaps.emplace( plugin.getTextureFlags(), TextureConfiguration{} ).first;
			parsingContext.sceneImportConfig.textureRemapIt->second = TextureConfiguration{};
		}
		CU_EndAttributePush( CSCNSection::eTextureRemapChannel )

		static CU_ImplementAttributeParser( parserTexRemapWaterNormal1Mask )
		{
			auto & parsingContext = getParserContext( context );

			if ( params.empty() )
			{
				CU_ParsingError( cuT( "Missing parameter." ) );
			}
			else
			{
				auto & plugin = parsingContext.parser->getEngine()->getPassComponentsRegister().getPlugin( WaterNormal1MapComponent::TypeName );
				plugin.fillTextureConfiguration( parsingContext.sceneImportConfig.textureRemapIt->second
					, params[0]->get< uint32_t >() );
			}
		}
		CU_EndAttribute()
	}

	//*********************************************************************************************

	void WaterNormal1MapComponent::ComponentsShader::fillComponents( castor3d::ComponentModeFlags componentsMask
		, sdw::type::BaseStruct & components
		, castor3d::shader::Materials const & materials
		, sdw::StructInstance const * surface )const
	{
		if ( !WaterComponent::isComponentAvailable( componentsMask, materials ) )
		{
			return;
		}

		if ( !components.hasMember( "waterNormalMapCoords1" ) )
		{
			components.declMember( "waterNormalMapCoords1", sdw::type::Kind::eVec2F );
			components.declMember( "waterNormals1", sdw::type::Kind::eVec3F );
		}
	}

	void WaterNormal1MapComponent::ComponentsShader::fillComponentsInits( sdw::type::BaseStruct const & components
		, castor3d::shader::Materials const & materials
		, castor3d::shader::Material const * material
		, sdw::StructInstance const * surface
		, sdw::Vec4 const * clrCot
		, sdw::expr::ExprList & inits )const
	{
		if ( !components.hasMember( "waterNormalMapCoords1" ) )
		{
			return;
		}

		inits.emplace_back( sdw::makeExpr( vec2( 0.0_f ) ) );
		inits.emplace_back( sdw::makeExpr( vec3( 0.0_f ) ) );
	}

	void WaterNormal1MapComponent::ComponentsShader::blendComponents( castor3d::shader::Materials const & materials
		, sdw::Float const & passMultiplier
		, castor3d::shader::BlendComponents & res
		, castor3d::shader::BlendComponents const & src )const
	{
		if ( res.hasMember( "waterNormals1" ) )
		{
			res.getMember< sdw::Float >( "waterNormals1" ) += src.getMember< sdw::Float >( "waterNormals1" ) * passMultiplier;
		}
	}

	void WaterNormal1MapComponent::ComponentsShader::applyComponents( TextureCombine const & combine
		, PipelineFlags const * flags
		, c3d::TextureConfigData const & config
		, sdw::U32Vec3 const & imgCompConfig
		, sdw::Vec4 const & sampled
		, sdw::Vec2 const & uv
		, c3d::BlendComponents & components )const
	{
		if ( !components.hasMember( "waterNormals1" ) )
		{
			return;
		}

		auto & writer{ *sampled.getWriter() };

		IF( writer, imgCompConfig.x() == sdw::UInt{ getTextureFlags() } )
		{
			auto waterNormals1 = components.getMember< sdw::Vec3 >( "waterNormals1" );
			waterNormals1 = config.getVec3( sampled, imgCompConfig.z() ) * 2.0_f - 1.0_f;
			auto waterNormalMapCoords1 = components.getMember< sdw::Vec2 >( "waterNormalMapCoords1" );
			waterNormalMapCoords1 = uv;
		}
		FI;
	}

	//*********************************************************************************************

	void WaterNormal1MapComponent::Plugin::createParsers( castor::AttributeParsers & parsers
		, ChannelFillers & channelFillers )const
	{
		channelFillers.emplace( "water_normal1", ChannelFiller{ getTextureFlags()
			, []( SceneFileContext & parsingContext )
			{
				auto & component = getPassComponent< WaterNormal1MapComponent >( parsingContext );
				component.fillChannel( parsingContext.texture.configuration
					, 0x00FFFFFFu );
			} } );

		castor::addParserT( parsers
			, CSCNSection::eTexture
			, cuT( "water_normal1_mask" )
			, trscmp::parserUnitWaterNormal1Mask
			, { castor::makeParameter< castor::ParameterType::eUInt32 >() } );

		castor::addParserT( parsers
			, CSCNSection::eTextureUnit
			, cuT( "water_normal1_mask" )
			, trscmp::parserUnitWaterNormal1Mask
			, { castor::makeParameter< castor::ParameterType::eUInt32 >() } );

		castor::addParserT( parsers
			, CSCNSection::eTextureRemap
			, cuT( "water_normal1" )
			, trscmp::parserTexRemapWaterNormal1 );

		castor::addParserT( parsers
			, CSCNSection::eTextureRemapChannel
			, cuT( "water_normal1_mask" )
			, trscmp::parserTexRemapWaterNormal1Mask
			, { castor::makeParameter< castor::ParameterType::eUInt32 >() } );
	}

	bool WaterNormal1MapComponent::Plugin::isComponentNeeded( TextureCombine const & textures
		, ComponentModeFlags const & filter )const
	{
		return checkFlag( filter, ComponentModeFlag::eSpecularLighting )
			|| hasAny( textures, getTextureFlags() );
	}

	void WaterNormal1MapComponent::Plugin::createMapComponent( Pass & pass
		, std::vector< PassComponentUPtr > & result )const
	{
		result.push_back( castor::makeUniqueDerived< PassComponent, WaterNormal1MapComponent >( pass ) );
	}

	bool WaterNormal1MapComponent::Plugin::doWriteTextureConfig( TextureConfiguration const & configuration
		, uint32_t mask
		, castor::String const & tabs
		, castor::StringStream & file )const
	{
		return castor::TextWriter< WaterNormal1MapComponent >{ tabs, mask }( file );
	}

	//*********************************************************************************************

	castor::String const WaterNormal1MapComponent::TypeName = C3D_PluginMakePassMapComponentName( "water", "normal1" );

	WaterNormal1MapComponent::WaterNormal1MapComponent( Pass & pass )
		: PassMapComponent{ pass
			, TypeName
			, Normal1
			, { WaterComponent::TypeName } }
	{
	}

	PassComponentUPtr WaterNormal1MapComponent::doClone( Pass & pass )const
	{
		return castor::makeUniqueDerived< PassComponent, WaterNormal1MapComponent >( pass );
	}

	void WaterNormal1MapComponent::doFillConfig( TextureConfiguration & configuration
		, ConfigurationVisitorBase & vis )const
	{
		vis.visit( cuT( "WaterNormal1" ) );
		vis.visit( cuT( "Map" ), getTextureFlags(), getFlagConfiguration( configuration, getTextureFlags() ), 1u );
	}

	//*********************************************************************************************
}
