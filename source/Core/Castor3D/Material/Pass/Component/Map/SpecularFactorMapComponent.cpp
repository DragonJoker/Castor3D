#include "Castor3D/Material/Pass/Component/Map/SpecularFactorMapComponent.hpp"

#include "Castor3D/Limits.hpp"
#include "Castor3D/Engine.hpp"
#include "Castor3D/Material/Pass/Pass.hpp"
#include "Castor3D/Material/Pass/PassFactory.hpp"
#include "Castor3D/Miscellaneous/ConfigurationVisitor.hpp"
#include "Castor3D/Material/Pass/PbrPass.hpp"
#include "Castor3D/Material/Pass/Component/Map/MetalnessMapComponent.hpp"
#include "Castor3D/Material/Pass/Component/Lighting/SpecularFactorComponent.hpp"
#include "Castor3D/Material/Pass/Component/PassComponentRegister.hpp"
#include "Castor3D/Material/Texture/TextureConfiguration.hpp"
#include "Castor3D/Scene/SceneFileParser.hpp"
#include "Castor3D/Shader/Shaders/GlslBlendComponents.hpp"
#include "Castor3D/Shader/Shaders/GlslLighting.hpp"
#include "Castor3D/Shader/Shaders/GlslMaterial.hpp"
#include "Castor3D/Shader/Shaders/GlslTextureConfiguration.hpp"

#include <CastorUtils/FileParser/ParserParameter.hpp>

//*************************************************************************************************

CU_ImplementSmartPtr( castor3d, SpecularFactorMapComponent )

namespace castor
{
	template<>
	class TextWriter< castor3d::SpecularFactorMapComponent >
		: public TextWriterT< castor3d::SpecularFactorMapComponent >
	{
	public:
		explicit TextWriter( String const & tabs
			, uint32_t mask )
			: TextWriterT< castor3d::SpecularFactorMapComponent >{ tabs }
			, m_mask{ mask }
		{
		}

		bool operator()( StringStream & file )
		{
			return writeMask( file, cuT( "specular_factor_mask" ), m_mask );
		}

		bool operator()( castor3d::SpecularFactorMapComponent const & object
			, StringStream & file )override
		{
			return writeMask( file, cuT( "specular_factor_mask" ), m_mask );
		}

	private:
		uint32_t m_mask;
	};
}

namespace castor3d
{
	//*********************************************************************************************

	namespace spcftcmp
	{
		static CU_ImplementAttributeParser( parserUnitSpecularFactorMask )
		{
			auto & parsingContext = getParserContext( context );

			if ( params.empty() )
			{
				CU_ParsingError( cuT( "Missing parameter." ) );
			}
			else if ( !parsingContext.pass )
			{
				auto & plugin = parsingContext.parser->getEngine()->getPassComponentsRegister().getPlugin( SpecularFactorMapComponent::TypeName );
				plugin.fillTextureConfiguration( parsingContext.texture.configuration
					, params[0]->get< uint32_t >() );
			}
			else
			{
				auto & plugin = parsingContext.pass->getComponentPlugin( SpecularFactorMapComponent::TypeName );
				plugin.fillTextureConfiguration( parsingContext.texture.configuration
					, params[0]->get< uint32_t >() );
			}
		}
		CU_EndAttribute()

		static CU_ImplementAttributeParser( parserTexRemapSpecularFactor )
		{
			auto & parsingContext = getParserContext( context );
			auto & plugin = parsingContext.parser->getEngine()->getPassComponentsRegister().getPlugin( SpecularFactorMapComponent::TypeName );
			parsingContext.sceneImportConfig.textureRemapIt = parsingContext.sceneImportConfig.textureRemaps.emplace( plugin.getTextureFlags(), TextureConfiguration{} ).first;
			parsingContext.sceneImportConfig.textureRemapIt->second = TextureConfiguration{};
		}
		CU_EndAttributePush( CSCNSection::eTextureRemapChannel )

		static CU_ImplementAttributeParser( parserTexRemapSpecularFactorMask )
		{
			auto & parsingContext = getParserContext( context );

			if ( params.empty() )
			{
				CU_ParsingError( cuT( "Missing parameter." ) );
			}
			else
			{
				auto & plugin = parsingContext.parser->getEngine()->getPassComponentsRegister().getPlugin( SpecularFactorMapComponent::TypeName );
				plugin.fillTextureConfiguration( parsingContext.sceneImportConfig.textureRemapIt->second
					, params[0]->get< uint32_t >() );
			}
		}
		CU_EndAttribute()
	}

	//*********************************************************************************************

	void SpecularFactorMapComponent::ComponentsShader::applyComponents( PipelineFlags const * flags
		, shader::TextureConfigData const & config
		, sdw::U32Vec3 const & imgCompConfig
		, sdw::Vec4 const & sampled
		, sdw::Vec2 const & uv
		, shader::BlendComponents & components )const
	{
		applyFloatComponent( "specularFactor"
			, getTextureFlags()
			, config
			, imgCompConfig
			, sampled
			, components );
	}

	//*********************************************************************************************

	void SpecularFactorMapComponent::Plugin::createParsers( castor::AttributeParsers & parsers
		, ChannelFillers & channelFillers )const
	{
		channelFillers.emplace( "specular_factor", ChannelFiller{ getTextureFlags()
			, []( SceneFileContext & parsingContext )
			{
				auto & component = getPassComponent< SpecularFactorMapComponent >( parsingContext );
				component.fillChannel( parsingContext.texture.configuration
					, 0x00FFFFFF );
			} } );

		castor::addParserT( parsers
			, CSCNSection::eTexture
			, cuT( "specular_factor_mask" )
			, spcftcmp::parserUnitSpecularFactorMask
			, { castor::makeParameter< castor::ParameterType::eUInt32 >() } );

		castor::addParserT( parsers
			, CSCNSection::eTextureUnit
			, cuT( "specular_factor_mask" )
			, spcftcmp::parserUnitSpecularFactorMask
			, { castor::makeParameter< castor::ParameterType::eUInt32 >() } );

		castor::addParserT( parsers
			, CSCNSection::eTextureRemap
			, cuT( "specular_factor" )
			, spcftcmp::parserTexRemapSpecularFactor );

		castor::addParserT( parsers
			, CSCNSection::eTextureRemapChannel
			, cuT( "specular_factor_mask" )
			, spcftcmp::parserTexRemapSpecularFactorMask
			, { castor::makeParameter< castor::ParameterType::eUInt32 >() } );
	}

	bool SpecularFactorMapComponent::Plugin::isComponentNeeded( TextureCombine const & textures
		, ComponentModeFlags const & filter )const
	{
		return checkFlag( filter, ComponentModeFlag::eSpecularLighting )
			|| hasAny( textures, getTextureFlags() );
	}

	void SpecularFactorMapComponent::Plugin::createMapComponent( Pass & pass
		, std::vector< PassComponentUPtr > & result )const
	{
		result.push_back( castor::makeUniqueDerived< PassComponent, SpecularFactorMapComponent >( pass ) );
	}

	bool SpecularFactorMapComponent::Plugin::doWriteTextureConfig( TextureConfiguration const & configuration
		, uint32_t mask
		, castor::String const & tabs
		, castor::StringStream & file )const
	{
		return castor::TextWriter< SpecularFactorMapComponent >{ tabs, mask }( file );
	}

	//*********************************************************************************************

	castor::String const SpecularFactorMapComponent::TypeName = C3D_MakePassMapComponentName( "specular_factor" );

	SpecularFactorMapComponent::SpecularFactorMapComponent( Pass & pass )
		: PassMapComponent{ pass
			, TypeName
			, Specular
			, { SpecularFactorComponent::TypeName } }
	{
	}

	PassComponentUPtr SpecularFactorMapComponent::doClone( Pass & pass )const
	{
		return castor::makeUniqueDerived< PassComponent, SpecularFactorMapComponent >( pass );
	}

	void SpecularFactorMapComponent::doFillConfig( TextureConfiguration & configuration
		, ConfigurationVisitorBase & vis )const
	{
		vis.visit( cuT( "Specular Factor" ) );
		vis.visit( cuT( "Map" ), getTextureFlags(), getFlagConfiguration( configuration, getTextureFlags() ), 1u );
	}

	//*********************************************************************************************
}
