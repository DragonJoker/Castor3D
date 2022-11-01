#include "Castor3D/Material/Pass/Component/Lighting/ClearcoatComponent.hpp"

#include "Castor3D/Material/Pass/Pass.hpp"
#include "Castor3D/Material/Pass/PassVisitor.hpp"
#include "Castor3D/Material/Pass/Component/PassComponentRegister.hpp"
#include "Castor3D/Scene/SceneFileParser.hpp"
#include "Castor3D/Shader/ShaderBuffers/PassBuffer.hpp"
#include "Castor3D/Shader/Shaders/GlslBlendComponents.hpp"
#include "Castor3D/Shader/Shaders/GlslLighting.hpp"
#include "Castor3D/Shader/Shaders/GlslMaterial.hpp"
#include "Castor3D/Shader/Shaders/GlslPassShaders.hpp"
#include "Castor3D/Shader/Shaders/GlslSurface.hpp"
#include "Castor3D/Shader/Shaders/GlslUtils.hpp"

#include <CastorUtils/FileParser/ParserParameter.hpp>
#include <CastorUtils/Data/Text/TextRgbColour.hpp>

namespace castor
{
	template<>
	class TextWriter< castor3d::ClearcoatComponent >
		: public TextWriterT< castor3d::ClearcoatComponent >
	{
	public:
		explicit TextWriter( String const & tabs )
			: TextWriterT< castor3d::ClearcoatComponent >{ tabs }
		{
		}

		bool operator()( castor3d::ClearcoatComponent const & object
			, StringStream & file )override
		{
			return writeOpt( file, cuT( "clearcoat_factor" ), object.getClearcoatFactor(), 0.0f )
				&& writeOpt( file, cuT( "clearcoat_roughness_factor" ), object.getRoughnessFactor(), 0.0f );
		}
	};
}

namespace castor3d
{
	//*********************************************************************************************

	namespace coating
	{
		static CU_ImplementAttributeParser( parserPassClearcoatFactor )
		{
			auto & parsingContext = getParserContext( context );

			if ( !parsingContext.pass )
			{
				CU_ParsingError( cuT( "No Pass initialised." ) );
			}
			else if ( params.empty() )
			{
				CU_ParsingError( cuT( "Missing parameter." ) );
			}
			else
			{
				auto & component = getPassComponent< ClearcoatComponent >( parsingContext );
				component.setClearcoatFactor( params[0]->get< float >() );
			}
		}
		CU_EndAttribute()

		static CU_ImplementAttributeParser( parserPassRoughnessFactor )
		{
			auto & parsingContext = getParserContext( context );

			if ( !parsingContext.pass )
			{
				CU_ParsingError( cuT( "No Pass initialised." ) );
			}
			else if ( params.empty() )
			{
				CU_ParsingError( cuT( "Missing parameter." ) );
			}
			else
			{
				auto & component = getPassComponent< ClearcoatComponent >( parsingContext );
				component.setRoughnessFactor( params[0]->get< float >() );
			}
		}
		CU_EndAttribute()
	}

	//*********************************************************************************************

	void ClearcoatComponent::ComponentsShader::fillComponents( sdw::type::BaseStruct & components
		, shader::Materials const & materials
		, sdw::StructInstance const * surface )const
	{
		if ( !checkFlag( materials.getFilter(), ComponentModeFlag::eSpecularLighting ) )
		{
			return;
		}

		if ( !components.hasMember( "clearcoatFactor" ) )
		{
			components.declMember( "clearcoatNormal", sdw::type::Kind::eVec3F );
			components.declMember( "clearcoatFactor", sdw::type::Kind::eFloat );
			components.declMember( "clearcoatRoughness", sdw::type::Kind::eFloat );
		}
	}

	void ClearcoatComponent::ComponentsShader::fillComponentsInits( sdw::type::BaseStruct const & components
		, shader::Materials const & materials
		, shader::Material const * material
		, sdw::StructInstance const * surface
		, sdw::Vec4 const * clrCot
		, sdw::expr::ExprList & inits )const
	{
		if ( !components.hasMember( "clearcoatFactor" ) )
		{
			return;
		}

		if ( clrCot )
		{
			inits.emplace_back( sdw::makeExpr( clrCot->xyz() ) );
			inits.emplace_back( sdw::makeExpr( clrCot->a() ) );
		}
		else
		{
			if ( surface )
			{
				inits.emplace_back( sdw::makeExpr( surface->getMember< sdw::Vec3 >( "normal" ) ) );
			}
			else
			{
				inits.emplace_back( sdw::makeExpr( vec3( 0.0_f ) ) );
			}

			if ( material )
			{
				inits.emplace_back( sdw::makeExpr( material->getMember< sdw::Float >( "clearcoatFactor" ) ) );
			}
			else
			{
				inits.emplace_back( sdw::makeExpr( 0.0_f ) );
			}
		}

		if ( material )
		{
			inits.emplace_back( sdw::makeExpr( material->getMember< sdw::Float >( "clearcoatRoughness" ) ) );
		}
		else
		{
			inits.emplace_back( sdw::makeExpr( 0.0_f ) );
		}
	}

	void ClearcoatComponent::ComponentsShader::blendComponents( shader::Materials const & materials
		, sdw::Float const & passMultiplier
		, shader::BlendComponents & res
		, shader::BlendComponents const & src )const
	{
		res.getMember< sdw::Float >( "clearcoatFactor", true ) = src.getMember< sdw::Float >( "clearcoatFactor", true ) * passMultiplier;
		res.getMember< sdw::Float >( "clearcoatRoughness", true ) = src.getMember< sdw::Float >( "clearcoatRoughness", true ) * passMultiplier;
		res.getMember< sdw::Vec3 >( "clearcoatNormal", true ) += src.getMember< sdw::Vec3 >( "clearcoatNormal", true ) * passMultiplier;
	}

	//*********************************************************************************************

	ClearcoatComponent::MaterialShader::MaterialShader()
		: shader::PassMaterialShader{ 8u }
	{
	}

	void ClearcoatComponent::MaterialShader::fillMaterialType( ast::type::BaseStruct & type
		, sdw::expr::ExprList & inits )const
	{
		if ( !type.hasMember( "clearcoatFactor" ) )
		{
			type.declMember( "clearcoatFactor", ast::type::Kind::eFloat );
			type.declMember( "clearcoatRoughness", ast::type::Kind::eFloat );
			inits.emplace_back( sdw::makeExpr( 0.0_f ) );
			inits.emplace_back( sdw::makeExpr( 0.0_f ) );
		}
	}

	void ClearcoatComponent::MaterialShader::updateMaterial( sdw::Vec3 const & albedo
		, sdw::Vec4 const & spcRgh
		, sdw::Vec4 const & colMtl
		, sdw::Vec4 const & crTsIr
		, sdw::Vec4 const & sheen
		, shader::Material & material )const
	{
		material.getMember< sdw::Float >( "clearcoatRoughness", true ) = crTsIr.x();
	}

	//*********************************************************************************************

	void ClearcoatComponent::Plugin::createParsers( castor::AttributeParsers & parsers
		, ChannelFillers & channelFillers )const
	{
		Pass::addParserT( parsers
			, CSCNSection::ePass
			, cuT( "clearcoat_factor" )
			, coating::parserPassClearcoatFactor
			, { castor::makeParameter< castor::ParameterType::eFloat >() } );
		Pass::addParserT( parsers
			, CSCNSection::ePass
			, cuT( "clearcoat_roughness_factor" )
			, coating::parserPassRoughnessFactor
			, { castor::makeParameter< castor::ParameterType::eFloat >() } );
	}

	void ClearcoatComponent::Plugin::zeroBuffer( Pass const & pass
		, shader::PassMaterialShader const & materialShader
		, PassBuffer & buffer )const
	{
		auto data = buffer.getData( pass.getId() );
		VkDeviceSize offset{};
		offset += data.write( materialShader.getMaterialChunk(), 0.0f, offset );
		data.write( materialShader.getMaterialChunk(), 0.0f, offset );
	}

	bool ClearcoatComponent::Plugin::isComponentNeeded( TextureCombine const & textures
		, ComponentModeFlags const & filter )const
	{
		return checkFlag( filter, ComponentModeFlag::eSpecularLighting );
	}

	//*********************************************************************************************

	castor::String const ClearcoatComponent::TypeName = C3D_MakePassLightingComponentName( "clearcoat" );

	ClearcoatComponent::ClearcoatComponent( Pass & pass )
		: BaseDataPassComponentT{ pass, TypeName }
	{
	}

	void ClearcoatComponent::accept( PassVisitorBase & vis )
	{
		vis.visit( cuT( "Clearcoat Factor" ), m_value.factor );
		vis.visit( cuT( "Clearcoat Roughness Factor" ), m_value.roughness );
	}

	PassComponentUPtr ClearcoatComponent::doClone( Pass & pass )const
	{
		auto result = std::make_unique< ClearcoatComponent >( pass );
		result->setData( getData() );
		return result;
	}

	bool ClearcoatComponent::doWriteText( castor::String const & tabs
		, castor::Path const & folder
		, castor::String const & subfolder
		, castor::StringStream & file )const
	{
		return castor::TextWriter< ClearcoatComponent >{ tabs }( *this, file );
	}

	void ClearcoatComponent::doFillBuffer( PassBuffer & buffer )const
	{
		auto data = buffer.getData( getOwner()->getId() );
		VkDeviceSize offset{};
		offset += data.write( m_materialShader->getMaterialChunk()
			, getClearcoatFactor()
			, offset );
		offset += data.write( m_materialShader->getMaterialChunk()
			, getRoughnessFactor()
			, offset );
	}

	//*********************************************************************************************
}
