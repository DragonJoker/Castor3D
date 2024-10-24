#include "Castor3D/Material/Pass/Component/Lighting/RoughnessComponent.hpp"

#include "Castor3D/Material/Pass/Pass.hpp"
#include "Castor3D/Miscellaneous/ConfigurationVisitor.hpp"
#include "Castor3D/Material/Pass/Component/Lighting/MetalnessComponent.hpp"
#include "Castor3D/Material/Pass/Component/Lighting/SpecularComponent.hpp"
#include "Castor3D/Miscellaneous/Logger.hpp"
#include "Castor3D/Scene/SceneFileParserData.hpp"
#include "Castor3D/Shader/ShaderBuffers/PassBuffer.hpp"
#include "Castor3D/Shader/Shaders/GlslBlendComponents.hpp"
#include "Castor3D/Shader/Shaders/GlslLighting.hpp"
#include "Castor3D/Shader/Shaders/GlslMaterial.hpp"

#include <CastorUtils/FileParser/FileParser.hpp>

CU_ImplementSmartPtr( castor3d, RoughnessComponent )

namespace castor
{
	template<>
	class TextWriter< castor3d::RoughnessComponent >
		: public TextWriterT< castor3d::RoughnessComponent >
	{
	public:
		explicit TextWriter( String const & tabs )
			: TextWriterT< castor3d::RoughnessComponent >{ tabs }
		{
		}

		bool operator()( castor3d::RoughnessComponent const & object
			, StringStream & file )override
		{
			if ( object.isShininess() )
			{
				return write( file, cuT( "shininess" ), object.getShininess() );
			}

			if ( object.isGlossiness() )
			{
				return write( file, cuT( "glossiness" ), object.getGlossiness() );
			}

			return write( file, cuT( "roughness" ), object.getRoughness() );
		}
	};
}

namespace castor3d
{
	//*********************************************************************************************

	namespace rghcmp
	{
		static CU_ImplementAttributeParserBlock( parserPassGlossiness, PassContext )
		{
			if ( !blockContext->pass )
			{
				CU_ParsingError( cuT( "No Pass initialised." ) );
			}
			else if ( !params.empty() )
			{
				float value;
				params[0]->get( value );
				getPassComponent< SpecularComponent >( *blockContext );
				getPassComponent< MetalnessComponent >( *blockContext );
				auto & component = getPassComponent< RoughnessComponent >( *blockContext );
				component.setGlossiness( value );
			}
		}
		CU_EndAttribute()

		static CU_ImplementAttributeParserBlock( parserPassShininess, PassContext )
		{
			if ( !blockContext->pass )
			{
				CU_ParsingError( cuT( "No Pass initialised." ) );
			}
			else if ( !params.empty() )
			{
				float value;
				params[0]->get( value );
				getPassComponent< SpecularComponent >( *blockContext );
				auto & component = getPassComponent< RoughnessComponent >( *blockContext );
				component.setShininess( value );
			}
		}
		CU_EndAttribute()

		static CU_ImplementAttributeParserBlock( parserPassRoughness, PassContext )
		{
			if ( !blockContext->pass )
			{
				CU_ParsingError( cuT( "No Pass initialised." ) );
			}
			else if ( !params.empty() )
			{
				float value;
				params[0]->get( value );
				auto & component = getPassComponent< RoughnessComponent >( *blockContext );
				component.setRoughness( value );
			}
		}
		CU_EndAttribute()
	}

	//*********************************************************************************************

	void RoughnessComponent::ComponentsShader::fillComponents( ComponentModeFlags componentsMask
		, sdw::type::BaseStruct & components
		, shader::Materials const & materials
		, sdw::StructInstance const * surface )const
	{
		if ( ( !checkFlag( componentsMask, ComponentModeFlag::eDiffuseLighting )
				&& !checkFlag( materials.getFilter(), ComponentModeFlag::eSpecularLighting ) )
			|| ( !checkFlag( materials.getFilter(), ComponentModeFlag::eDiffuseLighting )
				&& !checkFlag( materials.getFilter(), ComponentModeFlag::eSpecularLighting ) ) )
		{
			return;
		}

		if ( !components.hasMember( "roughness" ) )
		{
			components.declMember( "roughness", sdw::type::Kind::eFloat );
			components.declMember( "roughnessMode", sdw::type::Kind::eUInt32 );
		}
	}

	void RoughnessComponent::ComponentsShader::fillComponentsInits( sdw::type::BaseStruct const & components
		, shader::Materials const & materials
		, shader::Material const * material
		, sdw::StructInstance const * surface
		, sdw::Vec4 const * clrCot
		, sdw::expr::ExprList & inits )const
	{
		if ( !components.hasMember( "roughness" ) )
		{
			return;
		}

		if ( material )
		{
			inits.emplace_back( sdw::makeExpr( material->getMember< sdw::Float >( "roughness" ) ) );
			inits.emplace_back( sdw::makeExpr( material->getMember< sdw::UInt32 >( "roughnessMode" ) ) );
		}
		else
		{
			inits.emplace_back( sdw::makeExpr( 1.0_f ) );
			inits.emplace_back( sdw::makeExpr( 0_u ) );
		}
	}

	void RoughnessComponent::ComponentsShader::blendComponents( shader::Materials const & materials
		, sdw::Float const & passMultiplier
		, shader::BlendComponents & res
		, shader::BlendComponents const & src )const
	{
		if ( res.hasMember( "roughness" ) )
		{
			res.getMember< sdw::Float >( "roughness" ) += src.getMember< sdw::Float >( "roughness" ) * passMultiplier;
			res.getMember< sdw::UInt32 >( "roughnessMode" ) = sdw::max( res.getMember< sdw::UInt32 >( "roughnessMode" )
				, src.getMember< sdw::UInt32 >( "roughnessMode" ) );
		}
	}

	//*********************************************************************************************

	RoughnessComponent::MaterialShader::MaterialShader()
		: shader::PassMaterialShader{ 8u }
	{
	}

	void RoughnessComponent::MaterialShader::fillMaterialType( ast::type::BaseStruct & type
		, sdw::expr::ExprList & inits )const
	{
		if ( !type.hasMember( "roughness" ) )
		{
			type.declMember( "roughness", ast::type::Kind::eFloat );
			type.declMember( "roughnessMode", ast::type::Kind::eUInt32 );
			inits.emplace_back( sdw::makeExpr( 1.0_f ) );
			inits.emplace_back( sdw::makeExpr( 0_u ) );
		}
	}

	//*********************************************************************************************

	void RoughnessComponent::Plugin::createParsers( castor::AttributeParsers & parsers
		, ChannelFillers & channelFillers )const
	{
		castor::addParserT( parsers
			, CSCNSection::ePass
			, cuT( "roughness" )
			, rghcmp::parserPassRoughness
			, { castor::makeParameter< castor::ParameterType::eFloat >() } );
		castor::addParserT( parsers
			, CSCNSection::ePass
			, cuT( "glossiness" )
			, rghcmp::parserPassGlossiness
			, { castor::makeParameter< castor::ParameterType::eFloat >() } );
		castor::addParserT( parsers
			, CSCNSection::ePass
			, cuT( "shininess" )
			, rghcmp::parserPassShininess
			, { castor::makeParameter< castor::ParameterType::eFloat >() } );
	}

	void RoughnessComponent::Plugin::zeroBuffer( Pass const & pass
		, shader::PassMaterialShader const & materialShader
		, PassBuffer & buffer )const
	{
		auto data = buffer.getData( pass.getId() );
		auto offset = data.write( materialShader.getMaterialChunk(), 1.0f, 0u );
		data.write( materialShader.getMaterialChunk(), 0u, offset );
	}

	bool RoughnessComponent::Plugin::isComponentNeeded( TextureCombine const & textures
		, ComponentModeFlags const & filter )const
	{
		return checkFlag( filter, ComponentModeFlag::eDiffuseLighting )
			|| checkFlag( filter, ComponentModeFlag::eSpecularLighting );
	}

	//*********************************************************************************************

	castor::String const RoughnessComponent::TypeName = C3D_MakePassLightingComponentName( "roughness" );

	RoughnessComponent::RoughnessComponent( Pass & pass
		, float defaultValue )
		: BaseDataPassComponentT< RoughnessData >{ pass, TypeName, {}, defaultValue }
	{
	}

	void RoughnessComponent::accept( ConfigurationVisitorBase & vis )
	{
		static castor::StringArray const Names{ cuT( "Roughness" ), cuT( "Glossiness" ), cuT( "Shininess" ) };
		vis.visit( cuT( "Roughness" ) );
		vis.visit( cuT( "Factor" ), m_value.factor );
		vis.visit( cuT( "Mode" ), m_value.mode, Names
			, ConfigurationVisitorBase::OnEnumValueChangeT< RoughnessMode >( []( RoughnessMode, RoughnessMode ){} ) );
	}

	PassComponentUPtr RoughnessComponent::doClone( Pass & pass )const
	{
		auto result = castor::makeUnique< RoughnessComponent >( pass );
		result->setData( getData() );
		return castor::ptrRefCast< PassComponent >( result );
	}

	bool RoughnessComponent::doWriteText( castor::String const & tabs
		, castor::Path const & folder
		, castor::String const & subfolder
		, castor::StringStream & file )const
	{
		return castor::TextWriter< RoughnessComponent >{ tabs }( *this, file );
	}

	void RoughnessComponent::doFillBuffer( PassBuffer & buffer )const
	{
		auto data = buffer.getData( getOwner()->getId() );
		auto offset = data.write( m_materialShader->getMaterialChunk(), getRoughness(), 0u );
		data.write( m_materialShader->getMaterialChunk(), uint32_t( m_value.mode.value() ), offset );
	}

	//*********************************************************************************************
}
