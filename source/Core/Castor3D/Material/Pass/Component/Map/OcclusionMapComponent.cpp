#include "Castor3D/Material/Pass/Component/Map/OcclusionMapComponent.hpp"

#include "Castor3D/Engine.hpp"
#include "Castor3D/Material/Pass/Pass.hpp"
#include "Castor3D/Miscellaneous/ConfigurationVisitor.hpp"
#include "Castor3D/Material/Pass/Component/PassComponentRegister.hpp"
#include "Castor3D/Material/Pass/Component/Map/EmissiveMapComponent.hpp"
#include "Castor3D/Material/Texture/TextureConfiguration.hpp"
#include "Castor3D/Scene/SceneFileParser.hpp"
#include "Castor3D/Shader/Shaders/GlslBlendComponents.hpp"
#include "Castor3D/Shader/Shaders/GlslLighting.hpp"
#include "Castor3D/Shader/Shaders/GlslMaterial.hpp"
#include "Castor3D/Shader/Shaders/GlslTextureConfiguration.hpp"

#include <CastorUtils/FileParser/ParserParameter.hpp>

CU_ImplementSmartPtr( castor3d, OcclusionMapComponent )

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
			return writeMask( file, cuT( "occlusion_mask" ), m_mask );
		}

		bool operator()( castor3d::OcclusionMapComponent const & object
			, StringStream & file )override
		{
			return writeMask( file, cuT( "occlusion_mask" ), m_mask );
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
		static CU_ImplementAttributeParserBlock( parserUnitOcclusionMask, TextureContext )
		{
			if ( params.empty() )
			{
				CU_ParsingError( cuT( "Missing parameter." ) );
			}
			else if ( !blockContext->pass )
			{
				auto & plugin = blockContext->root->engine->getPassComponentsRegister().getPlugin( OcclusionMapComponent::TypeName );
				plugin.fillTextureConfiguration( blockContext->configuration
					, params[0]->get< uint32_t >() );
			}
			else
			{
				auto & plugin = blockContext->pass->pass->getComponentPlugin( OcclusionMapComponent::TypeName );
				plugin.fillTextureConfiguration( blockContext->configuration
					, params[0]->get< uint32_t >() );
			}
		}
		CU_EndAttribute()

		static CU_ImplementAttributeParserBlock( parserTexRemapOcclusion, SceneImportContext )
		{
			auto & plugin = blockContext->root->engine->getPassComponentsRegister().getPlugin( OcclusionMapComponent::TypeName );
			blockContext->textureRemapIt = blockContext->textureRemaps.emplace( plugin.getTextureFlags(), TextureConfiguration{} ).first;
			blockContext->textureRemapIt->second = TextureConfiguration{};
		}
		CU_EndAttributePushBlock( CSCNSection::eTextureRemapChannel, blockContext )

		static CU_ImplementAttributeParserBlock( parserTexRemapOcclusionMask, SceneImportContext )
		{
			if ( params.empty() )
			{
				CU_ParsingError( cuT( "Missing parameter." ) );
			}
			else
			{
				auto & plugin = blockContext->root->engine->getPassComponentsRegister().getPlugin( OcclusionMapComponent::TypeName );
				plugin.fillTextureConfiguration( blockContext->textureRemapIt->second
					, params[0]->get< uint32_t >() );
			}
		}
		CU_EndAttribute()
	}

	//*********************************************************************************************

	void OcclusionMapComponent::ComponentsShader::fillComponents( ComponentModeFlags componentsMask
		, sdw::type::BaseStruct & components
		, shader::Materials const & materials
		, sdw::StructInstance const * surface )const
	{
		if ( !checkFlag( componentsMask, ComponentModeFlag::eOcclusion )
			|| !checkFlag( materials.getFilter(), ComponentModeFlag::eOcclusion ) )
		{
			return;
		}

		if ( !components.hasMember( "occlusion" ) )
		{
			components.declMember( "occlusion", sdw::type::Kind::eFloat );
		}
	}

	void OcclusionMapComponent::ComponentsShader::fillComponentsInits( sdw::type::BaseStruct const & components
		, shader::Materials const & materials
		, shader::Material const * material
		, sdw::StructInstance const * surface
		, sdw::Vec4 const * clrCot
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
		, shader::BlendComponents & res
		, shader::BlendComponents const & src )const
	{
		if ( res.hasMember( "occlusion" ) )
		{
			res.getMember< sdw::Float >( "occlusion" ) += src.getMember< sdw::Float >( "occlusion" ) * passMultiplier;
		}
	}

	void OcclusionMapComponent::ComponentsShader::applyTexture( shader::PassShaders const & passShaders
		, shader::TextureConfigurations const & textureConfigs
		, shader::TextureAnimations const & textureAnims
		, sdw::Array< sdw::CombinedImage2DRgba32 > const & maps
		, shader::Material const & material
		, shader::BlendComponents & components
		, shader::SampleTexture const & sampleTexture )const
	{
		applyFloatComponent( "occlusion"
			, passShaders
			, textureConfigs
			, textureAnims
			, material
			, components
			, sampleTexture );
	}

	//*********************************************************************************************

	void OcclusionMapComponent::Plugin::createParsers( castor::AttributeParsers & parsers
		, ChannelFillers & channelFillers )const
	{
		channelFillers.emplace( "occlusion", ChannelFiller{ getTextureFlags()
			, []( TextureContext & blockContext )
			{
				auto & component = getPassComponent< OcclusionMapComponent >( blockContext );
				component.fillChannel( blockContext.configuration
					, 0x00FF0000 );
			} } );

		castor::addParserT( parsers
			, CSCNSection::eTexture
			, cuT( "occlusion_mask" )
			, occcmp::parserUnitOcclusionMask
			, { castor::makeParameter< castor::ParameterType::eUInt32 >() } );

		castor::addParserT( parsers
			, CSCNSection::eTextureUnit
			, cuT( "occlusion_mask" )
			, occcmp::parserUnitOcclusionMask
			, { castor::makeParameter< castor::ParameterType::eUInt32 >() } );

		castor::addParserT( parsers
			, CSCNSection::eTextureRemap
			, cuT( "occlusion" )
			, occcmp::parserTexRemapOcclusion );

		castor::addParserT( parsers
			, CSCNSection::eTextureRemapChannel
			, cuT( "occlusion_mask" )
			, occcmp::parserTexRemapOcclusionMask
			, { castor::makeParameter< castor::ParameterType::eUInt32 >() } );
	}

	bool OcclusionMapComponent::Plugin::isComponentNeeded( TextureCombine const & textures
		, ComponentModeFlags const & filter )const
	{
		return checkFlag( filter, ComponentModeFlag::eOcclusion )
			|| hasAny( textures, getTextureFlags() );
	}

	void OcclusionMapComponent::Plugin::createMapComponent( Pass & pass
		, std::vector< PassComponentUPtr > & result )const
	{
		result.push_back( castor::makeUniqueDerived< PassComponent, OcclusionMapComponent >( pass ) );
	}

	bool OcclusionMapComponent::Plugin::doWriteTextureConfig( TextureConfiguration const & configuration
		, uint32_t mask
		, castor::String const & tabs
		, castor::StringStream & file )const
	{
		return castor::TextWriter< OcclusionMapComponent >{ tabs, mask }( file );
	}

	//*********************************************************************************************

	castor::String const OcclusionMapComponent::TypeName = C3D_MakePassMapComponentName( "occlusion" );

	OcclusionMapComponent::OcclusionMapComponent( Pass & pass )
		: PassMapComponent{ pass
			, TypeName
			, Occlusion }
	{
	}

	PassComponentUPtr OcclusionMapComponent::doClone( Pass & pass )const
	{
		return castor::makeUniqueDerived< PassComponent, OcclusionMapComponent >( pass );
	}

	void OcclusionMapComponent::doFillConfig( TextureConfiguration & configuration
		, ConfigurationVisitorBase & vis )const
	{
		vis.visit( cuT( "Occlusion" ) );
		vis.visit( cuT( "Map" ), getTextureFlags(), getFlagConfiguration( configuration, getTextureFlags() ), 1u );
	}

	//*********************************************************************************************
}
