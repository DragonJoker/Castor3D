#include "Castor3D/Material/Pass/Component/Map/SpecularMapComponent.hpp"

#include "Castor3D/Limits.hpp"
#include "Castor3D/Engine.hpp"
#include "Castor3D/Material/Pass/Pass.hpp"
#include "Castor3D/Material/Pass/PassFactory.hpp"
#include "Castor3D/Material/Pass/PassVisitor.hpp"
#include "Castor3D/Material/Pass/PbrPass.hpp"
#include "Castor3D/Material/Pass/Component/Map/MetalnessMapComponent.hpp"
#include "Castor3D/Material/Pass/Component/Lighting/SpecularComponent.hpp"
#include "Castor3D/Material/Pass/Component/PassComponentRegister.hpp"
#include "Castor3D/Material/Texture/TextureConfiguration.hpp"
#include "Castor3D/Scene/SceneFileParser.hpp"
#include "Castor3D/Shader/Shaders/GlslBlendComponents.hpp"
#include "Castor3D/Shader/Shaders/GlslLighting.hpp"
#include "Castor3D/Shader/Shaders/GlslMaterial.hpp"
#include "Castor3D/Shader/Shaders/GlslTextureConfiguration.hpp"

#include <CastorUtils/FileParser/ParserParameter.hpp>

//*************************************************************************************************

CU_ImplementSmartPtr( castor3d, SpecularMapComponent )

namespace castor
{
	template<>
	class TextWriter< castor3d::SpecularMapComponent >
		: public TextWriterT< castor3d::SpecularMapComponent >
	{
	public:
		explicit TextWriter( String const & tabs
			, uint32_t mask )
			: TextWriterT< castor3d::SpecularMapComponent >{ tabs }
			, m_mask{ mask }
		{
		}

		bool operator()( StringStream & file )
		{
			return writeMask( file, cuT( "specular_mask" ), m_mask );
		}

		bool operator()( castor3d::SpecularMapComponent const & object
			, StringStream & file )override
		{
			return writeMask( file, cuT( "specular_mask" ), m_mask );
		}

	private:
		uint32_t m_mask;
	};
}

namespace castor3d
{
	//*********************************************************************************************

	namespace spccmp
	{
		static CU_ImplementAttributeParser( parserUnitSpecularMask )
		{
			auto & parsingContext = getParserContext( context );

			if ( params.empty() )
			{
				CU_ParsingError( cuT( "Missing parameter." ) );
			}
			else
			{
				auto & plugin = parsingContext.pass->getComponentPlugin( SpecularMapComponent::TypeName );
				plugin.fillTextureConfiguration( parsingContext.textureConfiguration
					, params[0]->get< uint32_t >() );
			}
		}
		CU_EndAttribute()

		static CU_ImplementAttributeParser( parserTexRemapSpecular )
		{
			auto & parsingContext = getParserContext( context );
			auto & plugin = parsingContext.parser->getEngine()->getPassComponentsRegister().getPlugin( SpecularMapComponent::TypeName );
			parsingContext.sceneImportConfig.textureRemapIt = parsingContext.sceneImportConfig.textureRemaps.emplace( plugin.getTextureFlags(), TextureConfiguration{} ).first;
			parsingContext.sceneImportConfig.textureRemapIt->second = TextureConfiguration{};
		}
		CU_EndAttributePush( CSCNSection::eTextureRemapChannel )

		static CU_ImplementAttributeParser( parserTexRemapSpecularMask )
		{
			auto & parsingContext = getParserContext( context );

			if ( params.empty() )
			{
				CU_ParsingError( cuT( "Missing parameter." ) );
			}
			else
			{
				auto & plugin = parsingContext.parser->getEngine()->getPassComponentsRegister().getPlugin( SpecularMapComponent::TypeName );
				plugin.fillTextureConfiguration( parsingContext.sceneImportConfig.textureRemapIt->second
					, params[0]->get< uint32_t >() );
			}
		}
		CU_EndAttribute()
	}

	//*********************************************************************************************

	void SpecularMapComponent::ComponentsShader::applyComponents( TextureCombine const & combine
		, PipelineFlags const * flags
		, shader::TextureConfigData const & config
		, sdw::U32Vec3 const & imgCompConfig
		, sdw::Vec4 const & sampled
		, shader::BlendComponents & components )const
	{
		applyVec3Component( "specular"
			, getTextureFlags()
			, config
			, imgCompConfig
			, sampled
			, components );
	}

	//*********************************************************************************************

	void SpecularMapComponent::Plugin::createParsers( castor::AttributeParsers & parsers
		, ChannelFillers & channelFillers )const
	{
		channelFillers.emplace( "specular", ChannelFiller{ getTextureFlags()
			, []( SceneFileContext & parsingContext )
			{
				auto & component = getPassComponent< SpecularMapComponent >( parsingContext );
				component.fillChannel( parsingContext.textureConfiguration
					, 0x00FFFFFF );
			} } );

		castor::addParserT( parsers
			, CSCNSection::eTextureUnit
			, cuT( "specular_mask" )
			, spccmp::parserUnitSpecularMask
			, { castor::makeParameter< castor::ParameterType::eUInt32 >() } );

		castor::addParserT( parsers
			, CSCNSection::eTextureRemap
			, cuT( "specular" )
			, spccmp::parserTexRemapSpecular );

		castor::addParserT( parsers
			, CSCNSection::eTextureRemapChannel
			, cuT( "specular_mask" )
			, spccmp::parserTexRemapSpecularMask
			, { castor::makeParameter< castor::ParameterType::eUInt32 >() } );
	}

	bool SpecularMapComponent::Plugin::isComponentNeeded( TextureCombine const & textures
		, ComponentModeFlags const & filter )const
	{
		return checkFlag( filter, ComponentModeFlag::eSpecularLighting )
			|| hasAny( textures, getTextureFlags() );
	}

	void SpecularMapComponent::Plugin::createMapComponent( Pass & pass
		, std::vector< PassComponentUPtr > & result )const
	{
		result.push_back( castor::makeUniqueDerived< PassComponent, SpecularMapComponent >( pass ) );
	}

	bool SpecularMapComponent::Plugin::doWriteTextureConfig( TextureConfiguration const & configuration
		, uint32_t mask
		, castor::String const & tabs
		, castor::StringStream & file )const
	{
		return castor::TextWriter< SpecularMapComponent >{ tabs, mask }( file );
	}

	void SpecularMapComponent::Plugin::doUpdateComponent( PassComponentRegister const & passComponents
		, TextureCombine const & combine
		, shader::BlendComponents & components )
	{
		auto & plugin = passComponents.getPlugin< SpecularMapComponent >();
		auto & mtlPlugin = passComponents.getPlugin< MetalnessMapComponent >();

		if ( hasAny( combine, mtlPlugin.getTextureFlags() )
			&& !hasAny( combine, plugin.getTextureFlags() ) )
		{
			components.getMember< sdw::Vec3 >( "specular", true ) = shader::BlendComponents::computeF0( components.getMember< sdw::Vec3 >( "colour", vec3( 0.0_f ) )
				, components.getMember< sdw::Float >( "metalness", 0.0_f ) );
		}
	}

	//*********************************************************************************************

	castor::String const SpecularMapComponent::TypeName = C3D_MakePassMapComponentName( "specular" );

	SpecularMapComponent::SpecularMapComponent( Pass & pass )
		: PassMapComponent{ pass
			, TypeName
			, Specular
			, { SpecularComponent::TypeName } }
	{
	}

	PassComponentUPtr SpecularMapComponent::doClone( Pass & pass )const
	{
		return castor::makeUniqueDerived< PassComponent, SpecularMapComponent >( pass );
	}

	void SpecularMapComponent::doFillConfig( TextureConfiguration & configuration
		, PassVisitorBase & vis )const
	{
		vis.visit( cuT( "Specular" ) );
		vis.visit( cuT( "Map" ), getTextureFlags(), getFlagConfiguration( configuration, getTextureFlags() ), 3u );
	}

	//*********************************************************************************************
}
