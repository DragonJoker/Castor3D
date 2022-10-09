#include "Castor3D/Material/Pass/Component/Map/OpacityMapComponent.hpp"

#include "Castor3D/Engine.hpp"
#include "Castor3D/Material/Pass/Pass.hpp"
#include "Castor3D/Material/Pass/PassVisitor.hpp"
#include "Castor3D/Material/Pass/Component/Map/ColourMapComponent.hpp"
#include "Castor3D/Material/Pass/Component/Other/OpacityComponent.hpp"
#include "Castor3D/Material/Texture/TextureConfiguration.hpp"
#include "Castor3D/Scene/SceneFileParser.hpp"
#include "Castor3D/Shader/ShaderBuffers/PassBuffer.hpp"
#include "Castor3D/Shader/Shaders/GlslBlendComponents.hpp"
#include "Castor3D/Shader/Shaders/GlslLighting.hpp"
#include "Castor3D/Shader/Shaders/GlslMaterial.hpp"
#include "Castor3D/Shader/Shaders/GlslTextureConfiguration.hpp"

#include <CastorUtils/FileParser/ParserParameter.hpp>
#include <CastorUtils/Data/Text/TextRgbColour.hpp>

namespace castor
{
	template<>
	class TextWriter< castor3d::OpacityMapComponent >
		: public TextWriterT< castor3d::OpacityMapComponent >
	{
	public:
		explicit TextWriter( String const & tabs
			, uint32_t mask )
			: TextWriterT< castor3d::OpacityMapComponent >{ tabs }
			, m_mask{ mask }
		{
		}

		bool operator()( castor3d::OpacityMapComponent const & object
			, StringStream & file )override
		{
			return writeNamedSub( file, cuT( "opacity_mask" ), m_mask );
		}

		bool operator()( StringStream & file )
		{
			return writeNamedSub( file, cuT( "opacity_mask" ), m_mask );
		}

	private:
		uint32_t m_mask;
	};
}

namespace castor3d
{
	//*********************************************************************************************

	namespace opacmp
	{
		static CU_ImplementAttributeParser( parserUnitOpacityMask )
		{
			auto & parsingContext = getParserContext( context );

			if ( params.empty() )
			{
				CU_ParsingError( cuT( "Missing parameter." ) );
			}
			else
			{
				auto & component = getPassComponent< OpacityMapComponent >( parsingContext );
				component.fillChannel( parsingContext.textureConfiguration
					, params[0]->get< uint32_t >() );
			}
		}
		CU_EndAttribute()

		static CU_ImplementAttributeParser( parserTexRemapOpacity )
		{
			auto & parsingContext = getParserContext( context );
			parsingContext.sceneImportConfig.textureRemapIt = parsingContext.sceneImportConfig.textureRemaps.emplace( TextureFlag::eOpacity, TextureConfiguration{} ).first;
			parsingContext.sceneImportConfig.textureRemapIt->second = TextureConfiguration{};
		}
		CU_EndAttributePush( CSCNSection::eTextureRemapChannel )

		static CU_ImplementAttributeParser( parserTexRemapOpacityMask )
		{
			auto & parsingContext = getParserContext( context );

			if ( params.empty() )
			{
				CU_ParsingError( cuT( "Missing parameter." ) );
			}
			else
			{
				OpacityMapComponent::fillRemapMask( params[0]->get< uint32_t >()
					, parsingContext.sceneImportConfig.textureRemapIt->second );
			}
		}
		CU_EndAttribute()
	}

	//*********************************************************************************************

	void OpacityMapComponent::ComponentsShader::applyComponents( TextureFlags const & texturesFlags
		, PipelineFlags const * flags
		, shader::TextureConfigData const & config
		, sdw::Vec4 const & sampled
		, shader::BlendComponents const & components )const
	{
		if ( !components.hasMember( "opacity" )
			|| !checkFlag( texturesFlags, TextureFlag::eOpacity ) )
		{
			return;
		}

		auto & writer{ *sampled.getWriter() };

		IF( writer, config.opaEnbl() != 0.0_f )
		{
			components.getMember< sdw::Float >( "opacity" ) *= config.getFloat( sampled, config.opaMask() );
		}
		FI;
	}

	//*********************************************************************************************

	castor::String const OpacityMapComponent::TypeName = C3D_MakePassMapComponentName( "opacity" );

	OpacityMapComponent::OpacityMapComponent( Pass & pass )
		: PassMapComponent{ pass, TypeName }
	{
		if ( !pass.hasComponent< OpacityComponent >() )
		{
			pass.createComponent< OpacityComponent >();
		}
	}

	void OpacityMapComponent::createParsers( castor::AttributeParsers & parsers
		, ChannelFillers & channelFillers )
	{
		channelFillers.emplace( "opacity", ChannelFiller{ uint32_t( TextureFlag::eOpacity )
			, []( SceneFileContext & parsingContext )
			{
				auto & component = getPassComponent< OpacityMapComponent >( parsingContext );
				component.fillChannel( parsingContext.textureConfiguration
					, 0xFF000000 );
			} } );

		Pass::addParserT( parsers
			, CSCNSection::eTextureUnit
			, cuT( "opacity_mask" )
			, opacmp::parserUnitOpacityMask
			, { castor::makeParameter< castor::ParameterType::eUInt32 >() } );

		Pass::addParserT( parsers
			, CSCNSection::eTextureRemap
			, cuT( "opacity" )
			, opacmp::parserTexRemapOpacity );

		Pass::addParserT( parsers
			, CSCNSection::eTextureRemapChannel
			, cuT( "opacity_mask" )
			, opacmp::parserTexRemapOpacityMask
			, { castor::makeParameter< castor::ParameterType::eUInt32 >() } );
	}

	void OpacityMapComponent::fillRemapMask( uint32_t maskValue
		, TextureConfiguration & configuration )
	{
		configuration.opacityMask[0] = maskValue;
	}

	bool OpacityMapComponent::writeTextureConfig( TextureConfiguration const & configuration
		, castor::String const & tabs
		, castor::StringStream & file )
	{
		return castor::TextWriter< OpacityMapComponent >{ tabs, configuration.opacityMask[0] }( file );
	}

	bool OpacityMapComponent::isComponentNeeded( TextureFlags const & textures
		, ComponentModeFlags const & filter )
	{
		return checkFlag( filter, ComponentModeFlag::eOpacity )
			|| checkFlag( textures, TextureFlag::eOpacity );
	}

	bool OpacityMapComponent::needsMapComponent( TextureConfiguration const & configuration )
	{
		return configuration.opacityMask[0] != 0u;
	}

	void OpacityMapComponent::createMapComponent( Pass & pass
		, std::vector< PassComponentUPtr > & result )
	{
		result.push_back( std::make_unique< OpacityMapComponent >( pass ) );
	}

	void OpacityMapComponent::mergeImages( TextureUnitDataSet & result )
	{
		if ( !getOwner()->hasComponent< ColourMapComponent >() )
		{
			getOwner()->mergeImages( TextureFlag::eColour
				, offsetof( TextureConfiguration, colourMask )
				, 0x00FFFFFF
				, TextureFlag::eOpacity
				, offsetof( TextureConfiguration, opacityMask )
				, 0xFF000000
				, "ColOpa"
				, result );
		}
	}

	void OpacityMapComponent::fillChannel( TextureConfiguration & configuration
		, uint32_t mask )
	{
		configuration.textureSpace |= TextureSpace::eNormalised;
		configuration.opacityMask[0] = mask;
	}

	void OpacityMapComponent::fillConfig( TextureConfiguration & configuration
		, PassVisitorBase & vis )
	{
		vis.visit( cuT( "Opacity" ), TextureFlag::eOpacity, configuration.opacityMask, 1u );
	}

	PassComponentUPtr OpacityMapComponent::doClone( Pass & pass )const
	{
		return std::make_unique< OpacityMapComponent >( pass );
	}

	//*********************************************************************************************
}
