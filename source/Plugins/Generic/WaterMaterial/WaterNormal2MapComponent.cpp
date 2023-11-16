#include "WaterMaterial/WaterNormal2MapComponent.hpp"
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
#include <Castor3D/Shader/Shaders/GlslUtils.hpp>

#include <CastorUtils/FileParser/ParserParameter.hpp>

CU_ImplementSmartPtr( water, WaterNormal2MapComponent )

namespace castor
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

		bool operator()( StringStream & file )
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
	using namespace castor3d;
	namespace c3d = castor3d::shader;

	//*********************************************************************************************

	namespace trscmp
	{
		static CU_ImplementAttributeParser( parserUnitWaterNormal2Mask )
		{
			auto & parsingContext = getParserContext( context );

			if ( params.empty() )
			{
				CU_ParsingError( cuT( "Missing parameter." ) );
			}
			else if ( !parsingContext.pass )
			{
				auto & plugin = parsingContext.parser->getEngine()->getPassComponentsRegister().getPlugin( WaterNormal2MapComponent::TypeName );
				plugin.fillTextureConfiguration( parsingContext.texture.configuration
					, params[0]->get< uint32_t >() );
			}
			else
			{
				auto & plugin = parsingContext.pass->getComponentPlugin( WaterNormal2MapComponent::TypeName );
				plugin.fillTextureConfiguration( parsingContext.texture.configuration
					, params[0]->get< uint32_t >() );
			}
		}
		CU_EndAttribute()

			static CU_ImplementAttributeParser( parserTexRemapWaterNormal2 )
		{
			auto & parsingContext = getParserContext( context );
			auto & plugin = parsingContext.parser->getEngine()->getPassComponentsRegister().getPlugin( WaterNormal2MapComponent::TypeName );
			parsingContext.sceneImportConfig.textureRemapIt = parsingContext.sceneImportConfig.textureRemaps.emplace( plugin.getTextureFlags(), TextureConfiguration{} ).first;
			parsingContext.sceneImportConfig.textureRemapIt->second = TextureConfiguration{};
		}
		CU_EndAttributePush( CSCNSection::eTextureRemapChannel )

			static CU_ImplementAttributeParser( parserTexRemapWaterNormal2Mask )
		{
			auto & parsingContext = getParserContext( context );

			if ( params.empty() )
			{
				CU_ParsingError( cuT( "Missing parameter." ) );
			}
			else
			{
				auto & plugin = parsingContext.parser->getEngine()->getPassComponentsRegister().getPlugin( WaterNormal2MapComponent::TypeName );
				plugin.fillTextureConfiguration( parsingContext.sceneImportConfig.textureRemapIt->second
					, params[0]->get< uint32_t >() );
			}
		}
		CU_EndAttribute()
	}

	//*********************************************************************************************

	void WaterNormal2MapComponent::ComponentsShader::fillComponents( castor3d::ComponentModeFlags componentsMask
		, sdw::type::BaseStruct & components
		, castor3d::shader::Materials const & materials
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
		, castor3d::shader::Materials const & materials
		, castor3d::shader::Material const * material
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

	void WaterNormal2MapComponent::ComponentsShader::blendComponents( castor3d::shader::Materials const & materials
		, sdw::Float const & passMultiplier
		, castor3d::shader::BlendComponents & res
		, castor3d::shader::BlendComponents const & src )const
	{
		if ( res.hasMember( "waterNormals2" ) )
		{
			res.getMember< sdw::Float >( "waterNormals2" ) += src.getMember< sdw::Float >( "waterNormals2" ) * passMultiplier;
		}
	}

	void WaterNormal2MapComponent::ComponentsShader::applyComponents( TextureCombine const & combine
		, PipelineFlags const * flags
		, c3d::TextureConfigData const & config
		, sdw::U32Vec3 const & imgCompConfig
		, sdw::Vec4 const & sampled
		, sdw::Vec2 const & uv
		, c3d::BlendComponents & components )const
	{
		if ( !components.hasMember( "waterNormals2" ) )
		{
			return;
		}

		auto & writer{ *sampled.getWriter() };

		IF( writer, imgCompConfig.x() == sdw::UInt{ getTextureFlags() } )
		{
			auto waterNormals2 = components.getMember< sdw::Vec3 >( "waterNormals2" );
			waterNormals2 = config.getVec3( sampled, imgCompConfig.z() ) * 2.0_f - 1.0_f;
			auto waterNormalMapCoords2 = components.getMember< sdw::Vec2 >( "waterNormalMapCoords2" );
			waterNormalMapCoords2 = uv;
		}
		FI;
	}

	void WaterNormal2MapComponent::ComponentsShader::updateComponent( TextureCombine const & combine
		, sdw::Array< sdw::CombinedImage2DRgba32 > const & maps
		, c3d::BlendComponents & components )const
	{
		if ( !components.hasMember( "waterNormals2" )
			|| !components.hasMember( "waterNormals1" ) )
		{
			return;
		}

		auto & writer{ *components.getWriter() };
		auto waterNormals1 = components.getMember< sdw::Vec3 >( "waterNormals1" );
		auto waterNormals2 = components.getMember< sdw::Vec3 >( "waterNormals2" );
		auto tbn = c3d::Utils::getTBN( components.getMember< sdw::Vec3 >( "normal" )
			, components.getMember< sdw::Vec4 >( "tangent" ).xyz()
			, components.getMember< sdw::Vec3 >( "bitangent" ) );
		auto finalNormal = writer.declLocale( "finalNormal"
			, normalize( tbn * waterNormals1.xyz() ) );
		finalNormal += normalize( tbn * waterNormals2.xyz() );
		components.getMember< sdw::Vec3 >( "normal" ) = normalize( finalNormal );
	}

	//*********************************************************************************************

	void WaterNormal2MapComponent::Plugin::createParsers( castor::AttributeParsers & parsers
		, ChannelFillers & channelFillers )const
	{
		channelFillers.emplace( "water_normal2", ChannelFiller{ getTextureFlags()
			, []( SceneFileContext & parsingContext )
			{
				auto & component = getPassComponent< WaterNormal2MapComponent >( parsingContext );
				component.fillChannel( parsingContext.texture.configuration
					, 0x00FFFFFFu );
			} } );

		castor::addParserT( parsers
			, CSCNSection::eTexture
			, cuT( "water_normal2_mask" )
			, trscmp::parserUnitWaterNormal2Mask
			, { castor::makeParameter< castor::ParameterType::eUInt32 >() } );

		castor::addParserT( parsers
			, CSCNSection::eTextureUnit
			, cuT( "water_normal2_mask" )
			, trscmp::parserUnitWaterNormal2Mask
			, { castor::makeParameter< castor::ParameterType::eUInt32 >() } );

		castor::addParserT( parsers
			, CSCNSection::eTextureRemap
			, cuT( "water_normal2" )
			, trscmp::parserTexRemapWaterNormal2 );

		castor::addParserT( parsers
			, CSCNSection::eTextureRemapChannel
			, cuT( "water_normal2_mask" )
			, trscmp::parserTexRemapWaterNormal2Mask
			, { castor::makeParameter< castor::ParameterType::eUInt32 >() } );
	}

	bool WaterNormal2MapComponent::Plugin::isComponentNeeded( TextureCombine const & textures
		, ComponentModeFlags const & filter )const
	{
		return checkFlag( filter, ComponentModeFlag::eSpecularLighting )
			|| hasAny( textures, getTextureFlags() );
	}

	void WaterNormal2MapComponent::Plugin::createMapComponent( Pass & pass
		, std::vector< PassComponentUPtr > & result )const
	{
		result.push_back( castor::makeUniqueDerived< PassComponent, WaterNormal2MapComponent >( pass ) );
	}

	bool WaterNormal2MapComponent::Plugin::doWriteTextureConfig( TextureConfiguration const & configuration
		, uint32_t mask
		, castor::String const & tabs
		, castor::StringStream & file )const
	{
		return castor::TextWriter< WaterNormal2MapComponent >{ tabs, mask }( file );
	}

	//*********************************************************************************************

	castor::String const WaterNormal2MapComponent::TypeName = C3D_MakePassMapComponentName( "water.normal2" );

	WaterNormal2MapComponent::WaterNormal2MapComponent( Pass & pass )
		: PassMapComponent{ pass
		, TypeName
		, Normal2
		, { WaterComponent::TypeName } }
	{
	}

	PassComponentUPtr WaterNormal2MapComponent::doClone( Pass & pass )const
	{
		return castor::makeUniqueDerived< PassComponent, WaterNormal2MapComponent >( pass );
	}

	void WaterNormal2MapComponent::doFillConfig( TextureConfiguration & configuration
		, ConfigurationVisitorBase & vis )const
	{
		vis.visit( cuT( "WaterNormal2" ) );
		vis.visit( cuT( "Map" ), getTextureFlags(), getFlagConfiguration( configuration, getTextureFlags() ), 1u );
	}

	//*********************************************************************************************
}
