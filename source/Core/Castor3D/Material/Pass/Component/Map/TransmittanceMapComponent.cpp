#include "Castor3D/Material/Pass/Component/Map/TransmittanceMapComponent.hpp"

#include "Castor3D/Engine.hpp"
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
			return writeMask( file, cuT( "transmittance_mask" ), m_mask );
		}

		bool operator()( castor3d::TransmittanceMapComponent const & object
			, StringStream & file )override
		{
			return writeOpt( file, cuT( "transmittance" ), object.getTransmittance(), 1.0f );
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
				auto & plugin = parsingContext.pass->getComponentPlugin( TransmittanceMapComponent::TypeName );
				plugin.fillTextureConfiguration( parsingContext.textureConfiguration
					, params[0]->get< uint32_t >() );
			}
		}
		CU_EndAttribute()

		static CU_ImplementAttributeParser( parserTexRemapTransmittance )
		{
			auto & parsingContext = getParserContext( context );
			auto & plugin = parsingContext.parser->getEngine()->getPassComponentsRegister().getPlugin( TransmittanceMapComponent::TypeName );
			parsingContext.sceneImportConfig.textureRemapIt = parsingContext.sceneImportConfig.textureRemaps.emplace( plugin.getTextureFlags(), TextureConfiguration{} ).first;
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
				auto & plugin = parsingContext.parser->getEngine()->getPassComponentsRegister().getPlugin( TransmittanceMapComponent::TypeName );
				plugin.fillTextureConfiguration( parsingContext.sceneImportConfig.textureRemapIt->second
					, params[0]->get< uint32_t >() );
			}
		}
		CU_EndAttribute()
	}

	//*********************************************************************************************

	void TransmittanceMapComponent::ComponentsShader::fillComponents( sdw::type::BaseStruct & components
		, shader::Materials const & materials
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

	void TransmittanceMapComponent::ComponentsShader::fillComponentsInits( sdw::type::BaseStruct const & components
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
		, shader::BlendComponents & res
		, shader::BlendComponents const & src )const
	{
		if ( src.hasMember( "transmittance" ) )
		{
			res.getMember< sdw::Float >( "transmittance" ) = src.getMember< sdw::Float >( "transmittance" ) * passMultiplier;
		}
	}

	void TransmittanceMapComponent::ComponentsShader::applyComponents( TextureFlagsArray const & texturesFlags
		, PipelineFlags const * flags
		, shader::TextureConfigData const & config
		, sdw::U32Vec3 const & imgCompConfig
		, sdw::Vec4 const & sampled
		, shader::BlendComponents & components )const
	{
		if ( !components.hasMember( "transmittance" )
			|| texturesFlags.end() == checkFlags( texturesFlags, getTextureFlags() ) )
		{
			return;
		}

		auto & writer{ *sampled.getWriter() };

		IF( writer, imgCompConfig.y() != 0_u )
		{
			components.getMember< sdw::Float >( "transmittance" ) *= config.getFloat( sampled, imgCompConfig.z() );
		}
		FI;
	}

	//*********************************************************************************************

	TransmittanceMapComponent::MaterialShader::MaterialShader()
		: shader::PassMaterialShader{ 4u }
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

	void TransmittanceMapComponent::Plugin::createParsers( castor::AttributeParsers & parsers
		, ChannelFillers & channelFillers )const
	{
		channelFillers.emplace( "transmittance", ChannelFiller{ getTextureFlags()
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

	bool TransmittanceMapComponent::Plugin::isComponentNeeded( TextureFlagsArray const & textures
		, ComponentModeFlags const & filter )const
	{
		return checkFlag( filter, ComponentModeFlag::eDiffuseLighting )
			|| checkFlag( filter, ComponentModeFlag::eSpecularLighting )
			|| textures.end( ) != checkFlags( textures, getTextureFlags() );
	}

	bool TransmittanceMapComponent::Plugin::writeTextureConfig( TextureConfiguration const & configuration
		, castor::String const & tabs
		, castor::StringStream & file )const
	{
		auto it = checkFlags( configuration.components, getTextureFlags() );

		if ( it == configuration.components.end() )
		{
			return true;
		}

		return castor::TextWriter< TransmittanceMapComponent >{ tabs, it->componentsMask }( file );
	}

	void TransmittanceMapComponent::Plugin::createMapComponent( Pass & pass
		, std::vector< PassComponentUPtr > & result )const
	{
		result.push_back( std::make_unique< TransmittanceMapComponent >( pass ) );
	}

	void TransmittanceMapComponent::Plugin::zeroBuffer( Pass const & pass
		, shader::PassMaterialShader const & materialShader
		, PassBuffer & buffer )const
	{
		auto data = buffer.getData( pass.getId() );
		data.write( materialShader.getMaterialChunk(), 1.0f, 0u );
	}

	//*********************************************************************************************

	castor::String const TransmittanceMapComponent::TypeName = C3D_MakePassMapComponentName( "transmittance" );

	TransmittanceMapComponent::TransmittanceMapComponent( Pass & pass )
		: PassMapComponent{ pass, TypeName }
		, m_transmittance{ m_dirty, 1.0f }
	{
	}

	void TransmittanceMapComponent::fillConfig( TextureConfiguration & configuration
		, PassVisitorBase & vis )const
	{
		vis.visit( cuT( "Transmittance" ), getTextureFlags(), getFlagConfiguration( configuration, getTextureFlags() ), 1u );
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
