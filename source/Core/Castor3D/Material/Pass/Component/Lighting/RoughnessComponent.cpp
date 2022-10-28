#include "Castor3D/Material/Pass/Component/Lighting/RoughnessComponent.hpp"

#include "Castor3D/Material/Pass/Pass.hpp"
#include "Castor3D/Material/Pass/PassVisitor.hpp"
#include "Castor3D/Material/Pass/Component/Lighting/MetalnessComponent.hpp"
#include "Castor3D/Material/Pass/Component/Lighting/SpecularComponent.hpp"
#include "Castor3D/Miscellaneous/Logger.hpp"
#include "Castor3D/Scene/SceneFileParser.hpp"
#include "Castor3D/Shader/ShaderBuffers/PassBuffer.hpp"
#include "Castor3D/Shader/Shaders/GlslBlendComponents.hpp"
#include "Castor3D/Shader/Shaders/GlslLighting.hpp"
#include "Castor3D/Shader/Shaders/GlslMaterial.hpp"

#include <CastorUtils/FileParser/ParserParameter.hpp>

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
			return write( file, "roughness", object.getRoughness() );
		}
	};
}

namespace castor3d
{
	//*********************************************************************************************

	namespace rghcmp
	{
		static CU_ImplementAttributeParser( parserPassGlossiness )
		{
			auto & parsingContext = getParserContext( context );

			if ( !parsingContext.pass )
			{
				CU_ParsingError( cuT( "No Pass initialised." ) );
			}
			else if ( !params.empty() )
			{
				float value;
				params[0]->get( value );
				getPassComponent< SpecularComponent >( parsingContext );
				getPassComponent< MetalnessComponent >( parsingContext );
				auto & component = getPassComponent< RoughnessComponent >( parsingContext );
				component.setGlossiness( value );
			}
		}
		CU_EndAttribute()

		static CU_ImplementAttributeParser( parserPassShininess )
		{
			auto & parsingContext = getParserContext( context );

			if ( !parsingContext.pass )
			{
				CU_ParsingError( cuT( "No Pass initialised." ) );
			}
			else if ( !params.empty() )
			{
				float value;
				params[0]->get( value );
				getPassComponent< SpecularComponent >( parsingContext );
				auto & component = getPassComponent< RoughnessComponent >( parsingContext );
				component.setShininess( value );
			}
		}
		CU_EndAttribute()

		static CU_ImplementAttributeParser( parserPassRoughness )
		{
			auto & parsingContext = getParserContext( context );

			if ( !parsingContext.pass )
			{
				CU_ParsingError( cuT( "No Pass initialised." ) );
			}
			else if ( !params.empty() )
			{
				float value;
				params[0]->get( value );
				auto & component = getPassComponent< RoughnessComponent >( parsingContext );
				component.setRoughness( value );
			}
		}
		CU_EndAttribute()
	}

	//*********************************************************************************************

	void RoughnessComponent::ComponentsShader::fillComponents( sdw::type::BaseStruct & components
		, shader::Materials const & materials
		, sdw::StructInstance const * surface )const
	{
		if ( !checkFlag( materials.getFilter(), ComponentModeFlag::eDiffuseLighting )
			&& !checkFlag( materials.getFilter(), ComponentModeFlag::eSpecularLighting ) )
		{
			return;
		}

		if ( !components.hasMember( "roughness" ) )
		{
			components.declMember( "roughness", sdw::type::Kind::eFloat );
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
		}
		else
		{
			inits.emplace_back( sdw::makeExpr( 1.0_f ) );
		}
	}

	void RoughnessComponent::ComponentsShader::blendComponents( shader::Materials const & materials
		, sdw::Float const & passMultiplier
		, shader::BlendComponents & res
		, shader::BlendComponents const & src )const
	{
		if ( src.hasMember( "roughness" ) )
		{
			res.getMember< sdw::Float >( "roughness" ) += src.getMember< sdw::Float >( "roughness" ) * passMultiplier;
		}
	}

	void RoughnessComponent::ComponentsShader::updateOutputs( sdw::StructInstance const & components
		, sdw::StructInstance const & surface
		, sdw::Vec4 & spcRgh
		, sdw::Vec4 & colMtl )const
	{
		spcRgh.a() = components.getMember< sdw::Float >( "roughness", true );
	}

	//*********************************************************************************************

	RoughnessComponent::MaterialShader::MaterialShader()
		: shader::PassMaterialShader{ 4u }
	{
	}

	void RoughnessComponent::MaterialShader::fillMaterialType( ast::type::BaseStruct & type
		, sdw::expr::ExprList & inits )const
	{
		if ( !type.hasMember( "roughness" ) )
		{
			type.declMember( "roughness", ast::type::Kind::eFloat );
			inits.emplace_back( sdw::makeExpr( 1.0_f ) );
		}
	}

	void RoughnessComponent::MaterialShader::updateMaterial( sdw::Vec3 const & albedo
		, sdw::Vec4 const & spcRgh
		, sdw::Vec4 const & colMtl
		, sdw::Float const & transm
		, shader::Material & material )const
	{
		material.getMember< sdw::Float >( "roughness", true ) = spcRgh.a();
	}

	//*********************************************************************************************

	void RoughnessComponent::Plugin::createParsers( castor::AttributeParsers & parsers
		, ChannelFillers & channelFillers )const
	{
		Pass::addParserT( parsers
			, CSCNSection::ePass
			, cuT( "roughness" )
			, rghcmp::parserPassRoughness
			, { castor::makeParameter< castor::ParameterType::eFloat >() }
		, "The microfacet roughness factor for the pass" );
		Pass::addParserT( parsers
			, CSCNSection::ePass
			, cuT( "glossiness" )
			, rghcmp::parserPassGlossiness
			, { castor::makeParameter< castor::ParameterType::eFloat >() }
		, "The specular glossiness (or shininess exponent) for the pass" );
		Pass::addParserT( parsers
			, CSCNSection::ePass
			, cuT( "shininess" )
			, rghcmp::parserPassShininess
			, { castor::makeParameter< castor::ParameterType::eFloat >() }
		, "The specular shininess exponent (or glossiness) for the pass" );
	}

	void RoughnessComponent::Plugin::zeroBuffer( Pass const & pass
		, shader::PassMaterialShader const & materialShader
		, PassBuffer & buffer )const
	{
		auto data = buffer.getData( pass.getId() );
		data.write( materialShader.getMaterialChunk(), 1.0f, 0u );
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
		: BaseDataPassComponentT{ pass, TypeName, defaultValue }
	{
	}

	float RoughnessComponent::getGlossiness()const
	{
		return 1.0f - getRoughness();
	}

	void RoughnessComponent::setGlossiness( float v )
	{
		m_value = 1.0f - v;
	}

	float RoughnessComponent::getShininess()const
	{
		return getGlossiness() * MaxPhongShininess;
	}

	void RoughnessComponent::setShininess( float v )
	{
		setGlossiness( v / MaxPhongShininess );
	}

	void RoughnessComponent::accept( PassVisitorBase & vis )
	{
		vis.visit( cuT( "Roughness" ), m_value );
	}

	PassComponentUPtr RoughnessComponent::doClone( Pass & pass )const
	{
		auto result = std::make_unique< RoughnessComponent >( pass );
		result->setData( getData() );
		return result;
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
		data.write( m_materialShader->getMaterialChunk(), getRoughness(), 0u );
	}

	//*********************************************************************************************
}
