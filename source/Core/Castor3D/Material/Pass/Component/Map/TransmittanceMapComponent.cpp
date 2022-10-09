#include "Castor3D/Material/Pass/Component/Map/TransmittanceMapComponent.hpp"

#include "Castor3D/Material/Pass/Pass.hpp"
#include "Castor3D/Material/Pass/PassVisitor.hpp"
#include "Castor3D/Material/Texture/TextureConfiguration.hpp"
#include "Castor3D/Scene/SceneFileParser.hpp"
#include "Castor3D/Shader/ShaderBuffers/PassBuffer.hpp"
#include "Castor3D/Shader/Shaders/GlslBlendComponents.hpp"
#include "Castor3D/Shader/Shaders/GlslLighting.hpp"
#include "Castor3D/Shader/Shaders/GlslMaterial.hpp"
#include "Castor3D/Shader/Shaders/GlslTextureConfiguration.hpp"

#include <CastorUtils/FileParser/ParserParameter.hpp>

namespace castor
{
	template<>
	class TextWriter< castor3d::TransmittanceMapComponent >
		: public TextWriterT< castor3d::TransmittanceMapComponent >
	{
	public:
		explicit TextWriter( String const & tabs
			, uint32_t mask = 0u )
			: TextWriterT< castor3d::TransmittanceMapComponent >{ tabs }
			, m_mask{ mask }
		{
		}

		bool operator()( StringStream & file )
		{
			return writeNamedSub( file, cuT( "transmittance_mask" ), m_mask );
		}

		bool operator()( castor3d::TransmittanceMapComponent const & object
			, StringStream & file )override
		{
			return writeNamedSub( file, cuT( "transmittance" ), object.getTransmittance() );
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
		static CU_ImplementAttributeParser( parserUnitTransmittanceMask )
		{
			auto & parsingContext = getParserContext( context );

			if ( params.empty() )
			{
				CU_ParsingError( cuT( "Missing parameter." ) );
			}
			else
			{
				auto & component = getPassComponent< TransmittanceMapComponent >( parsingContext );
				component.fillChannel( parsingContext.textureConfiguration
					, params[0]->get< uint32_t >() );
			}
		}
		CU_EndAttribute()

		static CU_ImplementAttributeParser( parserTexRemapTransmittance )
		{
			auto & parsingContext = getParserContext( context );
			parsingContext.sceneImportConfig.textureRemapIt = parsingContext.sceneImportConfig.textureRemaps.emplace( TextureFlag::eTransmittance, TextureConfiguration{} ).first;
			parsingContext.sceneImportConfig.textureRemapIt->second = TextureConfiguration{};
		}
		CU_EndAttributePush( CSCNSection::eTextureRemapChannel )

		static CU_ImplementAttributeParser( parserTexRemapTransmittanceMask )
		{
			auto & parsingContext = getParserContext( context );

			if ( params.empty() )
			{
				CU_ParsingError( cuT( "Missing parameter." ) );
			}
			else
			{
				TransmittanceMapComponent::fillRemapMask( params[0]->get< uint32_t >()
					, parsingContext.sceneImportConfig.textureRemapIt->second );
			}
		}
		CU_EndAttribute()
	}

	//*********************************************************************************************

	void TransmittanceMapComponent::ComponentsShader::fillComponents( sdw::type::BaseStruct & components
		, shader::Materials const & materials
		, shader::Material const * material
		, sdw::StructInstance const * surface )const
	{
		if ( !checkFlag( materials.getFilter(), ComponentModeFlag::eDiffuseLighting )
			&& !checkFlag( materials.getFilter(), ComponentModeFlag::eSpecularLighting ) )
		{
			return;
		}

		if ( !components.hasMember( "transmittance" ) )
		{
			components.declMember( "transmittance", sdw::type::Kind::eFloat );
		}
	}

	void TransmittanceMapComponent::ComponentsShader::fillComponentsInits( sdw::type::BaseStruct & components
		, shader::Materials const & materials
		, shader::Material const * material
		, sdw::StructInstance const * surface
		, sdw::expr::ExprList & inits )const
	{
		if ( !components.hasMember( "transmittance" ) )
		{
			return;
		}

		if ( material )
		{
			inits.emplace_back( sdw::makeExpr( material->getMember< sdw::Float >( "transmittance" ) ) );
		}
		else
		{
			inits.emplace_back( sdw::makeExpr( 1.0_f ) );
		}
	}

	void TransmittanceMapComponent::ComponentsShader::blendComponents( shader::Materials const & materials
		, sdw::Float const & passMultiplier
		, shader::BlendComponents const & res
		, shader::BlendComponents const & src )const
	{
		if ( src.hasMember( "transmittance" ) )
		{
			res.getMember< sdw::Float >( "transmittance" ) = src.getMember< sdw::Float >( "transmittance" ) * passMultiplier;
		}
	}

	void TransmittanceMapComponent::ComponentsShader::applyComponents( TextureFlags const & texturesFlags
		, PipelineFlags const * flags
		, shader::TextureConfigData const & config
		, sdw::Vec4 const & sampled
		, shader::BlendComponents const & components )const
	{
		if ( !components.hasMember( "transmittance" )
			|| !checkFlag( texturesFlags, TextureFlag::eTransmittance ) )
		{
			return;
		}

		auto & writer{ *sampled.getWriter() };

		IF( writer, config.trsEnbl() != 0.0_f )
		{
			components.getMember< sdw::Float >( "transmittance" ) *= config.getFloat( sampled, config.trsMask() );
		}
		FI;
	}

	//*********************************************************************************************

	TransmittanceMapComponent::MaterialShader::MaterialShader()
		: shader::PassMaterialShader{ MemChunk{ 0u, 4u, 4u } }
	{
	}

	void TransmittanceMapComponent::MaterialShader::fillMaterialType( ast::type::BaseStruct & type
		, sdw::expr::ExprList & inits )const
	{
		if ( !type.hasMember( "transmittance" ) )
		{
			type.declMember( "transmittance", ast::type::Kind::eFloat );
			inits.emplace_back( sdw::makeExpr( 1.0_f ) );
		}
	}

	//*********************************************************************************************

	castor::String const TransmittanceMapComponent::TypeName = C3D_MakePassMapComponentName( "transmittance" );

	TransmittanceMapComponent::TransmittanceMapComponent( Pass & pass )
		: PassMapComponent{ pass, TypeName }
		, m_transmittance{ m_dirty, 1.0f }
	{
	}

	void TransmittanceMapComponent::createParsers( castor::AttributeParsers & parsers
		, ChannelFillers & channelFillers )
	{
		channelFillers.emplace( "transmittance", ChannelFiller{ uint32_t( TextureFlag::eTransmittance )
			, []( SceneFileContext & parsingContext )
			{
				auto & component = getPassComponent< TransmittanceMapComponent >( parsingContext );
				component.fillChannel( parsingContext.textureConfiguration
					, 0xFF000000 );
			} } );

		Pass::addParserT( parsers
			, CSCNSection::eTextureUnit
			, cuT( "transmittance_mask" )
			, trscmp::parserUnitTransmittanceMask
			, { castor::makeParameter< castor::ParameterType::eUInt32 >() } );

		Pass::addParserT( parsers
			, CSCNSection::eTextureRemap
			, cuT( "transmittance" )
			, trscmp::parserTexRemapTransmittance );

		Pass::addParserT( parsers
			, CSCNSection::eTextureRemapChannel
			, cuT( "transmittance_mask" )
			, trscmp::parserTexRemapTransmittanceMask
			, { castor::makeParameter< castor::ParameterType::eUInt32 >() } );
	}

	bool TransmittanceMapComponent::isComponentNeeded( TextureFlags const & textures
		, ComponentModeFlags const & filter )
	{
		return checkFlag( filter, ComponentModeFlag::eDiffuseLighting )
			|| checkFlag( filter, ComponentModeFlag::eSpecularLighting )
			|| checkFlag( textures, TextureFlag::eTransmittance );
	}

	void TransmittanceMapComponent::fillRemapMask( uint32_t maskValue
		, TextureConfiguration & configuration )
	{
		configuration.transmittanceMask[0] = maskValue;
	}

	bool TransmittanceMapComponent::writeTextureConfig( TextureConfiguration const & configuration
		, castor::String const & tabs
		, castor::StringStream & file )
	{
		return castor::TextWriter< TransmittanceMapComponent >{ tabs, configuration.transmittanceMask[0] }( file );
	}

	bool TransmittanceMapComponent::needsMapComponent( TextureConfiguration const & configuration )
	{
		return configuration.transmittanceMask[0] != 0;
	}

	void TransmittanceMapComponent::createMapComponent( Pass & pass
		, std::vector< PassComponentUPtr > & result )
	{
		result.push_back( std::make_unique< TransmittanceMapComponent >( pass ) );
	}

	void TransmittanceMapComponent::zeroBuffer( Pass const & pass
		, shader::PassMaterialShader const & materialShader
		, PassBuffer & buffer )
	{
		auto data = buffer.getData( pass.getId() );
		data.write( materialShader.getMaterialChunk(), 1.0f, 0u );
	}

	void TransmittanceMapComponent::mergeImages( TextureUnitDataSet & result )
	{
		getOwner()->prepareImage( TextureFlag::eTransmittance
			, offsetof( TextureConfiguration, transmittanceMask )
			, 0x00FFFFFF
			, "Trs"
			, result );
	}

	void TransmittanceMapComponent::fillChannel( TextureConfiguration & configuration
		, uint32_t mask )
	{
		configuration.textureSpace |= TextureSpace::eNormalised;
		configuration.transmittanceMask[0] = mask;
	}

	void TransmittanceMapComponent::fillConfig( TextureConfiguration & configuration
		, PassVisitorBase & vis )
	{
		vis.visit( cuT( "Transmittance" ), TextureFlag::eTransmittance, configuration.transmittanceMask, 1u );
	}

	PassComponentUPtr TransmittanceMapComponent::doClone( Pass & pass )const
	{
		return std::make_unique< TransmittanceMapComponent >( pass );
	}

	bool TransmittanceMapComponent::doWriteText( castor::String const & tabs
		, castor::Path const & folder
		, castor::String const & subfolder
		, castor::StringStream & file )const
	{
		return castor::TextWriter< TransmittanceMapComponent >{ tabs }( *this, file );
	}

	void TransmittanceMapComponent::doFillBuffer( PassBuffer & buffer )const
	{
		auto data = buffer.getData( getOwner()->getId() );
		data.write( m_materialShader->getMaterialChunk(), getTransmittance(), 0u );
	}

	//*********************************************************************************************
}
