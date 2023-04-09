#include "Castor3D/Material/Pass/Component/Map/ClearcoatNormalMapComponent.hpp"

#include "Castor3D/Engine.hpp"
#include "Castor3D/Material/Pass/Pass.hpp"
#include "Castor3D/Material/Pass/PassVisitor.hpp"
#include "Castor3D/Material/Pass/Component/PassComponentRegister.hpp"
#include "Castor3D/Material/Pass/Component/Lighting/ClearcoatComponent.hpp"
#include "Castor3D/Material/Texture/TextureConfiguration.hpp"
#include "Castor3D/Scene/SceneFileParser.hpp"
#include "Castor3D/Shader/ShaderBuffers/PassBuffer.hpp"
#include "Castor3D/Shader/Shaders/GlslBlendComponents.hpp"
#include "Castor3D/Shader/Shaders/GlslLighting.hpp"
#include "Castor3D/Shader/Shaders/GlslMaterial.hpp"
#include "Castor3D/Shader/Shaders/GlslTextureConfiguration.hpp"

#include <CastorUtils/FileParser/ParserParameter.hpp>

CU_ImplementSmartPtr( castor3d, ClearcoatNormalMapComponent )

namespace castor
{
	template<>
	class TextWriter< castor3d::ClearcoatNormalMapComponent >
		: public TextWriterT< castor3d::ClearcoatNormalMapComponent >
	{
	public:
		explicit TextWriter( String const & tabs
			, uint32_t mask = 0u )
			: TextWriterT< castor3d::ClearcoatNormalMapComponent >{ tabs }
			, m_mask{ mask }
		{
		}

		bool operator()( StringStream & file )
		{
			return writeMask( file, cuT( "clearcoat_normal_mask" ), m_mask );
		}

		bool operator()( castor3d::ClearcoatNormalMapComponent const & object
			, StringStream & file )override
		{
			return true;
		}

	private:
		uint32_t m_mask;
	};
}

namespace castor3d
{
	//*********************************************************************************************

	namespace trscmp
	{
		static CU_ImplementAttributeParser( parserUnitClearcoatNormalMask )
		{
			auto & parsingContext = getParserContext( context );

			if ( params.empty() )
			{
				CU_ParsingError( cuT( "Missing parameter." ) );
			}
			else
			{
				auto & plugin = parsingContext.pass->getComponentPlugin( ClearcoatNormalMapComponent::TypeName );
				plugin.fillTextureConfiguration( parsingContext.textureConfiguration
					, params[0]->get< uint32_t >() );
			}
		}
		CU_EndAttribute()

		static CU_ImplementAttributeParser( parserTexRemapClearcoatNormal )
		{
			auto & parsingContext = getParserContext( context );
			auto & plugin = parsingContext.parser->getEngine()->getPassComponentsRegister().getPlugin( ClearcoatNormalMapComponent::TypeName );
			parsingContext.sceneImportConfig.textureRemapIt = parsingContext.sceneImportConfig.textureRemaps.emplace( plugin.getTextureFlags(), TextureConfiguration{} ).first;
			parsingContext.sceneImportConfig.textureRemapIt->second = TextureConfiguration{};
		}
		CU_EndAttributePush( CSCNSection::eTextureRemapChannel )

		static CU_ImplementAttributeParser( parserTexRemapClearcoatNormalMask )
		{
			auto & parsingContext = getParserContext( context );

			if ( params.empty() )
			{
				CU_ParsingError( cuT( "Missing parameter." ) );
			}
			else
			{
				auto & plugin = parsingContext.parser->getEngine()->getPassComponentsRegister().getPlugin( ClearcoatNormalMapComponent::TypeName );
				plugin.fillTextureConfiguration( parsingContext.sceneImportConfig.textureRemapIt->second
					, params[0]->get< uint32_t >() );
			}
		}
		CU_EndAttribute()
	}

	//*********************************************************************************************

	void ClearcoatNormalMapComponent::ComponentsShader::applyComponents( TextureCombine const & combine
		, PipelineFlags const * flags
		, shader::TextureConfigData const & config
		, sdw::U32Vec3 const & imgCompConfig
		, sdw::Vec4 const & sampled
		, shader::BlendComponents & components )const
	{
		if ( !components.hasMember(  "clearcoatNormal" ) )
		{
			return;
		}

		auto & writer{ *sampled.getWriter() };

		IF( writer, imgCompConfig.x() == sdw::UInt{ getTextureFlags() } )
		{
			auto tbn = shader::Utils::getTBN( components.clearcoatNormal
				, components.getMember< sdw::Vec3 >( "tangent" )
				, components.getMember< sdw::Vec3 >( "bitangent" ) );
			components.clearcoatNormal = normalize( tbn
				* fma( config.getVec3( sampled, imgCompConfig.z() )
					, vec3( 2.0_f )
					, -vec3( 1.0_f ) ) );
		}
		FI;
	}

	//*********************************************************************************************

	void ClearcoatNormalMapComponent::Plugin::createParsers( castor::AttributeParsers & parsers
		, ChannelFillers & channelFillers )const
	{
		channelFillers.emplace( "clearcoat_normal", ChannelFiller{ getTextureFlags()
			, []( SceneFileContext & parsingContext )
			{
				auto & component = getPassComponent< ClearcoatNormalMapComponent >( parsingContext );
				component.fillChannel( parsingContext.textureConfiguration
					, 0x00FFFFFFu );
			} } );

		castor::addParserT( parsers
			, CSCNSection::eTextureUnit
			, cuT( "clearcoat_normal_mask" )
			, trscmp::parserUnitClearcoatNormalMask
			, { castor::makeParameter< castor::ParameterType::eUInt32 >() } );

		castor::addParserT( parsers
			, CSCNSection::eTextureRemap
			, cuT( "clearcoat_normal" )
			, trscmp::parserTexRemapClearcoatNormal );

		castor::addParserT( parsers
			, CSCNSection::eTextureRemapChannel
			, cuT( "clearcoat_normal_mask" )
			, trscmp::parserTexRemapClearcoatNormalMask
			, { castor::makeParameter< castor::ParameterType::eUInt32 >() } );
	}

	bool ClearcoatNormalMapComponent::Plugin::isComponentNeeded( TextureCombine const & textures
		, ComponentModeFlags const & filter )const
	{
		return checkFlag( filter, ComponentModeFlag::eSpecularLighting )
			|| hasAny( textures, getTextureFlags() );
	}

	void ClearcoatNormalMapComponent::Plugin::createMapComponent( Pass & pass
		, std::vector< PassComponentUPtr > & result )const
	{
		result.push_back( castor::makeUniqueDerived< PassComponent, ClearcoatNormalMapComponent >( pass ) );
	}

	bool ClearcoatNormalMapComponent::Plugin::doWriteTextureConfig( TextureConfiguration const & configuration
		, uint32_t mask
		, castor::String const & tabs
		, castor::StringStream & file )const
	{
		return castor::TextWriter< ClearcoatNormalMapComponent >{ tabs, mask }( file );
	}

	//*********************************************************************************************

	castor::String const ClearcoatNormalMapComponent::TypeName = C3D_MakePassMapComponentName( "clearcoat_normal" );

	ClearcoatNormalMapComponent::ClearcoatNormalMapComponent( Pass & pass )
		: PassMapComponent{ pass, TypeName }
	{
		if ( !pass.hasComponent< ClearcoatComponent >() )
		{
			pass.createComponent< ClearcoatComponent >();
		}
	}

	PassComponentUPtr ClearcoatNormalMapComponent::doClone( Pass & pass )const
	{
		return castor::makeUniqueDerived< PassComponent, ClearcoatNormalMapComponent >( pass );
	}

	void ClearcoatNormalMapComponent::doFillConfig( TextureConfiguration & configuration
		, PassVisitorBase & vis )const
	{
		vis.visit( cuT( "Clearcoat Normal" ), getTextureFlags(), getFlagConfiguration( configuration, getTextureFlags() ), 1u );
	}

	//*********************************************************************************************
}
