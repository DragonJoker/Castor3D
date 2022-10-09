#include "Castor3D/Material/Pass/Component/Map/HeightMapComponent.hpp"

#include "Castor3D/Material/Pass/Pass.hpp"
#include "Castor3D/Material/Pass/PassVisitor.hpp"
#include "Castor3D/Material/Pass/Component/Map/NormalMapComponent.hpp"
#include "Castor3D/Material/Pass/Component/Other/HeightComponent.hpp"
#include "Castor3D/Material/Texture/TextureConfiguration.hpp"
#include "Castor3D/Scene/SceneFileParser.hpp"
#include "Castor3D/Shader/ShaderBuffers/PassBuffer.hpp"
#include "Castor3D/Shader/Shaders/GlslBlendComponents.hpp"
#include "Castor3D/Shader/Shaders/GlslLighting.hpp"
#include "Castor3D/Shader/Shaders/GlslMaterial.hpp"
#include "Castor3D/Shader/Shaders/GlslSurface.hpp"
#include "Castor3D/Shader/Shaders/GlslTextureConfiguration.hpp"

#include <CastorUtils/FileParser/ParserParameter.hpp>

namespace castor
{
	template<>
	class TextWriter< castor3d::HeightMapComponent >
		: public TextWriterT< castor3d::HeightMapComponent >
	{
	public:
		explicit TextWriter( String const & tabs
			, castor3d::TextureConfiguration const & configuration )
			: TextWriterT< castor3d::HeightMapComponent >{ tabs }
			, m_configuration{ configuration }
		{
		}

		bool operator()( castor3d::HeightMapComponent const & object
			, StringStream & file )override
		{
			return writeNamedSub( file, cuT( "height_mask" ), m_configuration.heightMask[0] )
				&& writeNamedSub( file, cuT( "height_factor" ), m_configuration.heightFactor );
		}

		bool operator()( StringStream & file )
		{
			return writeNamedSub( file, cuT( "height_mask" ), m_configuration.heightMask[0] )
				&& writeNamedSub( file, cuT( "height_factor" ), m_configuration.heightFactor );
		}

	private:
		castor3d::TextureConfiguration const & m_configuration;
	};
}

namespace castor3d
{
	//*********************************************************************************************

	namespace hgtcmp
	{
		static CU_ImplementAttributeParser( parserUnitHeightMask )
		{
			auto & parsingContext = getParserContext( context );

			if ( params.empty() )
			{
				CU_ParsingError( cuT( "Missing parameter." ) );
			}
			else
			{
				auto & component = getPassComponent< HeightMapComponent >( parsingContext );
				component.fillChannel( parsingContext.textureConfiguration
					, params[0]->get< uint32_t >() );
			}
		}
		CU_EndAttribute()

		static CU_ImplementAttributeParser( parserUnitHeightFactor )
		{
			auto & parsingContext = getParserContext( context );

			if ( params.empty() )
			{
				CU_ParsingError( cuT( "Missing parameter." ) );
			}
			else
			{
				getPassComponent< HeightMapComponent >( parsingContext );
				params[0]->get( parsingContext.textureConfiguration.heightFactor );
			}
		}
		CU_EndAttribute()

		static CU_ImplementAttributeParser( parserTexRemapHeight )
		{
			auto & parsingContext = getParserContext( context );
			parsingContext.sceneImportConfig.textureRemapIt = parsingContext.sceneImportConfig.textureRemaps.emplace( TextureFlag::eHeight, TextureConfiguration{} ).first;
			parsingContext.sceneImportConfig.textureRemapIt->second = TextureConfiguration{};
		}
		CU_EndAttributePush( CSCNSection::eTextureRemapChannel )

		static CU_ImplementAttributeParser( parserTexRemapHeightMask )
		{
			auto & parsingContext = getParserContext( context );

			if ( params.empty() )
			{
				CU_ParsingError( cuT( "Missing parameter." ) );
			}
			else
			{
				HeightMapComponent::fillRemapMask( params[0]->get< uint32_t >()
					, parsingContext.sceneImportConfig.textureRemapIt->second );
			}
		}
		CU_EndAttribute()
	}

	//*********************************************************************************************

	void HeightMapComponent::ComponentsShader::applyComponents( TextureFlags const & texturesFlags
		, PipelineFlags const * flags
		, shader::TextureConfigData const & config
		, sdw::Vec4 const & sampled
		, shader::BlendComponents const & components )const
	{
	}

	//*********************************************************************************************

	castor::String const HeightMapComponent::TypeName = C3D_MakePassMapComponentName( "height" );

	HeightMapComponent::HeightMapComponent( Pass & pass )
		: PassMapComponent{ pass, TypeName }
	{
		if ( !pass.hasComponent< HeightComponent >() )
		{
			pass.createComponent< HeightComponent >();
		}
	}

	void HeightMapComponent::createParsers( castor::AttributeParsers & parsers
		, ChannelFillers & channelFillers )
	{
		static UInt32StrMap const parallaxOcclusionModes{ getEnumMapT< ParallaxOcclusionMode >() };

		channelFillers.emplace( "height", ChannelFiller{ uint32_t( TextureFlag::eHeight )
			, []( SceneFileContext & parsingContext )
			{
				auto & component = getPassComponent< HeightMapComponent >( parsingContext );
				component.fillChannel( parsingContext.textureConfiguration
					, 0x00FF0000 );
			} } );

		Pass::addParserT( parsers
			, CSCNSection::eTextureUnit
			, cuT( "height_mask" )
			, hgtcmp::parserUnitHeightMask
			, { castor::makeParameter< castor::ParameterType::eUInt32 >() } );
		Pass::addParserT( parsers
			, CSCNSection::eTextureUnit
			, cuT( "height_factor" )
			, hgtcmp::parserUnitHeightFactor
			, { castor::makeParameter< castor::ParameterType::eFloat >() } );

		Pass::addParserT( parsers
			, CSCNSection::eTextureRemap
			, cuT( "height" )
			, hgtcmp::parserTexRemapHeight );

		Pass::addParserT( parsers
			, CSCNSection::eTextureRemapChannel
			, cuT( "height_mask" )
			, hgtcmp::parserTexRemapHeightMask
			, { castor::makeParameter< castor::ParameterType::eUInt32 >() } );
	}

	void HeightMapComponent::fillRemapMask( uint32_t maskValue
		, TextureConfiguration & configuration )
	{
		configuration.heightMask[0] = maskValue;
	}

	bool HeightMapComponent::writeTextureConfig( TextureConfiguration const & configuration
		, castor::String const & tabs
		, castor::StringStream & file )
	{
		return castor::TextWriter< HeightMapComponent >{ tabs, configuration }( file );
	}

	bool HeightMapComponent::isComponentNeeded( TextureFlags const & textures
		, ComponentModeFlags const & filter )
	{
		return checkFlag( filter, ComponentModeFlag::eGeometry )
			|| checkFlag( textures, TextureFlag::eHeight );
	}

	bool HeightMapComponent::needsMapComponent( TextureConfiguration const & configuration )
	{
		return configuration.heightMask[0] != 0u;
	}

	void HeightMapComponent::createMapComponent( Pass & pass
		, std::vector< PassComponentUPtr > & result )
	{
		result.push_back( std::make_unique< HeightMapComponent >( pass ) );
	}

	void HeightMapComponent::mergeImages( TextureUnitDataSet & result )
	{
		if ( !getOwner()->hasComponent< NormalMapComponent >() )
		{
			getOwner()->mergeImages( TextureFlag::eNormal
				, offsetof( TextureConfiguration, normalMask )
				, 0x00FFFFFF
				, TextureFlag::eHeight
				, offsetof( TextureConfiguration, heightMask )
				, 0xFF000000
				, "NmlHgt"
				, result );
		}
	}

	void HeightMapComponent::fillChannel( TextureConfiguration & configuration
		, uint32_t mask )
	{
		configuration.textureSpace |= TextureSpace::eNormalised;
		configuration.heightMask[0] = mask;
	}

	void HeightMapComponent::fillConfig( TextureConfiguration & configuration
		, PassVisitorBase & vis )
	{
		vis.visit( cuT( "Height" ), TextureFlag::eHeight, configuration.heightMask, 1u );
		vis.visit( cuT( "Height factor" ), configuration.heightFactor );
	}

	PassComponentUPtr HeightMapComponent::doClone( Pass & pass )const
	{
		return std::make_unique< HeightMapComponent >( pass );
	}

	//*********************************************************************************************
}
