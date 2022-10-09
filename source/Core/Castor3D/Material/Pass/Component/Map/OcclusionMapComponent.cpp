#include "Castor3D/Material/Pass/Component/Map/OcclusionMapComponent.hpp"

#include "Castor3D/Material/Pass/Pass.hpp"
#include "Castor3D/Material/Pass/PassVisitor.hpp"
#include "Castor3D/Material/Pass/Component/Map/EmissiveMapComponent.hpp"
#include "Castor3D/Material/Texture/TextureConfiguration.hpp"
#include "Castor3D/Scene/SceneFileParser.hpp"
#include "Castor3D/Shader/Shaders/GlslBlendComponents.hpp"
#include "Castor3D/Shader/Shaders/GlslLighting.hpp"
#include "Castor3D/Shader/Shaders/GlslMaterial.hpp"
#include "Castor3D/Shader/Shaders/GlslTextureConfiguration.hpp"

#include <CastorUtils/FileParser/ParserParameter.hpp>

namespace castor
{
	template<>
	class TextWriter< castor3d::OcclusionMapComponent >
		: public TextWriterT< castor3d::OcclusionMapComponent >
	{
	public:
		explicit TextWriter( String const & tabs
			, uint32_t mask )
			: TextWriterT< castor3d::OcclusionMapComponent >{ tabs }
			, m_mask{ mask }
		{
		}

		bool operator()( StringStream & file )
		{
			return writeNamedSub( file, cuT( "occlusion_mask" ), m_mask );
		}

		bool operator()( castor3d::OcclusionMapComponent const & object
			, StringStream & file )override
		{
			return writeNamedSub( file, cuT( "occlusion_mask" ), m_mask );
		}

	private:
		uint32_t m_mask;
	};
}

namespace castor3d
{
	//*********************************************************************************************

	namespace occcmp
	{
		static CU_ImplementAttributeParser( parserUnitOcclusionMask )
		{
			auto & parsingContext = getParserContext( context );

			if ( params.empty() )
			{
				CU_ParsingError( cuT( "Missing parameter." ) );
			}
			else
			{
				auto & component = getPassComponent< OcclusionMapComponent >( parsingContext );
				component.fillChannel( parsingContext.textureConfiguration
					, params[0]->get< uint32_t >() );
			}
		}
		CU_EndAttribute()

		static CU_ImplementAttributeParser( parserTexRemapOcclusion )
		{
			auto & parsingContext = getParserContext( context );
			parsingContext.sceneImportConfig.textureRemapIt = parsingContext.sceneImportConfig.textureRemaps.emplace( TextureFlag::eOcclusion, TextureConfiguration{} ).first;
			parsingContext.sceneImportConfig.textureRemapIt->second = TextureConfiguration{};
		}
		CU_EndAttributePush( CSCNSection::eTextureRemapChannel )

		static CU_ImplementAttributeParser( parserTexRemapOcclusionMask )
		{
			auto & parsingContext = getParserContext( context );

			if ( params.empty() )
			{
				CU_ParsingError( cuT( "Missing parameter." ) );
			}
			else
			{
				OcclusionMapComponent::fillRemapMask( params[0]->get< uint32_t >()
					, parsingContext.sceneImportConfig.textureRemapIt->second );
			}
		}
		CU_EndAttribute()
	}

	//*********************************************************************************************

	void OcclusionMapComponent::ComponentsShader::fillComponents( sdw::type::BaseStruct & components
		, shader::Materials const & materials
		, shader::Material const * material
		, sdw::StructInstance const * surface )const
	{
		if ( !checkFlag( materials.getFilter(), ComponentModeFlag::eOcclusion ) )
		{
			return;
		}

		if ( !components.hasMember( "occlusion" ) )
		{
			components.declMember( "occlusion", sdw::type::Kind::eFloat );
		}
	}

	void OcclusionMapComponent::ComponentsShader::fillComponentsInits( sdw::type::BaseStruct & components
		, shader::Materials const & materials
		, shader::Material const * material
		, sdw::StructInstance const * surface
		, sdw::expr::ExprList & inits )const
	{
		if ( !components.hasMember( "occlusion" ) )
		{
			return;
		}

		inits.emplace_back( sdw::makeExpr( 1.0_f ) );
	}

	void OcclusionMapComponent::ComponentsShader::blendComponents( shader::Materials const & materials
		, sdw::Float const & passMultiplier
		, shader::BlendComponents const & res
		, shader::BlendComponents const & src )const
	{
		if ( src.hasMember( "occlusion" ) )
		{
			res.getMember< sdw::Float >( "occlusion" ) += src.getMember< sdw::Float >( "occlusion" ) * passMultiplier;
		}
	}

	void OcclusionMapComponent::ComponentsShader::applyComponents( TextureFlags const & texturesFlags
		, PipelineFlags const * flags
		, shader::TextureConfigData const & config
		, sdw::Vec4 const & sampled
		, shader::BlendComponents const & components )const
	{
		if ( !components.hasMember( "occlusion" )
			|| !checkFlag( texturesFlags, TextureFlag::eOcclusion ) )
		{
			return;
		}

		auto & writer{ *sampled.getWriter() };

		IF( writer, config.occEnbl() != 0.0_f )
		{
			components.getMember< sdw::Float >( "occlusion" ) *= config.getFloat( sampled, config.occMask() );
		}
		FI;
	}

	//*********************************************************************************************

	castor::String const OcclusionMapComponent::TypeName = C3D_MakePassMapComponentName( "occ" );

	OcclusionMapComponent::OcclusionMapComponent( Pass & pass )
		: PassMapComponent{ pass, TypeName }
	{
	}

	void OcclusionMapComponent::createParsers( castor::AttributeParsers & parsers
		, ChannelFillers & channelFillers )
	{
		channelFillers.emplace( "occlusion", ChannelFiller{ uint32_t( TextureFlag::eOcclusion )
			, []( SceneFileContext & parsingContext )
			{
				auto & component = getPassComponent< OcclusionMapComponent >( parsingContext );
				component.fillChannel( parsingContext.textureConfiguration
					, 0x00FF0000 );
			} } );

		Pass::addParserT( parsers
			, CSCNSection::eTextureUnit
			, cuT( "occlusion_mask" )
			, occcmp::parserUnitOcclusionMask
			, { castor::makeParameter< castor::ParameterType::eUInt32 >() } );

		Pass::addParserT( parsers
			, CSCNSection::eTextureRemap
			, cuT( "occlusion" )
			, occcmp::parserTexRemapOcclusion );

		Pass::addParserT( parsers
			, CSCNSection::eTextureRemapChannel
			, cuT( "occlusion_mask" )
			, occcmp::parserTexRemapOcclusionMask
			, { castor::makeParameter< castor::ParameterType::eUInt32 >() } );
	}

	void OcclusionMapComponent::fillRemapMask( uint32_t maskValue
		, TextureConfiguration & configuration )
	{
		configuration.occlusionMask[0] = maskValue;
	}

	bool OcclusionMapComponent::writeTextureConfig( TextureConfiguration const & configuration
		, castor::String const & tabs
		, castor::StringStream & file )
	{
		return castor::TextWriter< OcclusionMapComponent >{ tabs, configuration.occlusionMask[0] }( file );
	}

	bool OcclusionMapComponent::isComponentNeeded( TextureFlags const & textures
		, ComponentModeFlags const & filter )
	{
		return checkFlag( filter, ComponentModeFlag::eOcclusion )
			|| checkFlag( textures, TextureFlag::eOcclusion );
	}

	bool OcclusionMapComponent::needsMapComponent( TextureConfiguration const & configuration )
	{
		return configuration.occlusionMask[0] != 0u;
	}

	void OcclusionMapComponent::createMapComponent( Pass & pass
		, std::vector< PassComponentUPtr > & result )
	{
		result.push_back( std::make_unique< OcclusionMapComponent >( pass ) );
	}

	void OcclusionMapComponent::mergeImages( TextureUnitDataSet & result )
	{
		if ( !getOwner()->hasComponent< EmissiveMapComponent >() )
		{
			getOwner()->mergeImages( TextureFlag::eEmissive
				, offsetof( TextureConfiguration, emissiveMask )
				, 0x00FFFFFF
				, TextureFlag::eOcclusion
				, offsetof( TextureConfiguration, occlusionMask )
				, 0xFF000000
				, "EmsOcc"
				, result );
		}
	}

	void OcclusionMapComponent::fillChannel( TextureConfiguration & configuration
		, uint32_t mask )
	{
		configuration.textureSpace |= TextureSpace::eNormalised;
		configuration.occlusionMask[0] = mask;
	}

	void OcclusionMapComponent::fillConfig( TextureConfiguration & configuration
		, PassVisitorBase & vis )
	{
		vis.visit( cuT( "Occlusion" ), TextureFlag::eOcclusion, configuration.occlusionMask, 1u );
	}

	PassComponentUPtr OcclusionMapComponent::doClone( Pass & pass )const
	{
		return std::make_unique< OcclusionMapComponent >( pass );
	}

	//*********************************************************************************************
}
