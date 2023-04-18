#include "Castor3D/Material/Pass/Component/Lighting/SpecularComponent.hpp"

#include "Castor3D/Limits.hpp"
#include "Castor3D/Engine.hpp"
#include "Castor3D/Material/Material.hpp"
#include "Castor3D/Material/Pass/Pass.hpp"
#include "Castor3D/Material/Pass/PassFactory.hpp"
#include "Castor3D/Material/Pass/PassVisitor.hpp"
#include "Castor3D/Material/Pass/Component/Lighting/MetalnessComponent.hpp"
#include "Castor3D/Material/Pass/Component/Map/SpecularMapComponent.hpp"
#include "Castor3D/Material/Pass/Component/PassComponent.hpp"
#include "Castor3D/Material/Pass/Component/PassComponentRegister.hpp"
#include "Castor3D/Scene/SceneFileParser.hpp"
#include "Castor3D/Shader/Shaders/GlslBlendComponents.hpp"
#include "Castor3D/Shader/Shaders/GlslLighting.hpp"
#include "Castor3D/Shader/Shaders/GlslMaterial.hpp"

#include <CastorUtils/FileParser/ParserParameter.hpp>
#include <CastorUtils/Data/Text/TextRgbColour.hpp>

//*************************************************************************************************

CU_ImplementSmartPtr( castor3d, SpecularComponent )

namespace castor
{
	template<>
	class TextWriter< castor3d::SpecularComponent >
		: public TextWriterT< castor3d::SpecularComponent >
	{
	public:
		explicit TextWriter( String const & tabs )
			: TextWriterT< castor3d::SpecularComponent >{ tabs }
		{
		}

		bool operator()( castor3d::SpecularComponent const & object
			, StringStream & file )override
		{
			return writeNamedSubOpt( file, "specular", object.getSpecular(), castor3d::SpecularComponent::Default )
				&& writeOpt( file, "specular_factor", object.getFactor(), castor3d::SpecularComponent::DefaultFactor );
		}
	};
}

namespace castor3d
{
	//*********************************************************************************************

	namespace spccmp
	{
		static CU_ImplementAttributeParser( parserPassSpecular )
		{
			auto & parsingContext = getParserContext( context );

			if ( !parsingContext.pass )
			{
				CU_ParsingError( cuT( "No Pass initialised." ) );
			}
			else if ( !params.empty() )
			{
				auto & component = getPassComponent< SpecularComponent >( parsingContext );
				component.setSpecular( params[0]->get< castor::RgbColour >() );
			}
		}
		CU_EndAttribute()

		static CU_ImplementAttributeParser( parserPassSpecularFactor )
		{
			auto & parsingContext = getParserContext( context );

			if ( !parsingContext.pass )
			{
				CU_ParsingError( cuT( "No Pass initialised." ) );
			}
			else if ( !params.empty() )
			{
				auto & component = getPassComponent< SpecularComponent >( parsingContext );
				component.setFactor( params[0]->get< float >() );
			}
		}
		CU_EndAttribute()
	}

	//*********************************************************************************************

	void SpecularComponent::ComponentsShader::fillComponents( sdw::type::BaseStruct & components
		, shader::Materials const & materials
		, sdw::StructInstance const * surface )const
	{
		if ( !checkFlag( materials.getFilter(), ComponentModeFlag::eSpecularLighting ) )
		{
			return;
		}

		if ( !components.hasMember( "specular" ) )
		{
			components.declMember( "specular", sdw::type::Kind::eVec3F );
			components.declMember( "specularFactor", sdw::type::Kind::eFloat );
		}
	}

	void SpecularComponent::ComponentsShader::fillComponentsInits( sdw::type::BaseStruct const & components
		, shader::Materials const & materials
		, shader::Material const * material
		, sdw::StructInstance const * surface
		, sdw::Vec4 const * clrCot
		, sdw::expr::ExprList & inits )const
	{
		if ( !components.hasMember( "specular" ) )
		{
			return;
		}

		if ( material )
		{
			inits.emplace_back( sdw::makeExpr( material->getMember< sdw::Vec3 >( "specular" ) ) );
			inits.emplace_back( sdw::makeExpr( material->getMember< sdw::Float >( "specularFactor" ) ) );
		}
		else
		{
			inits.emplace_back( sdw::makeExpr( sdw::vec3( SpecularComponent::DefaultComponent ) ) );
			inits.emplace_back( sdw::makeExpr( sdw::Float{ DefaultFactor } ) );
		}
	}

	void SpecularComponent::ComponentsShader::blendComponents( shader::Materials const & materials
		, sdw::Float const & passMultiplier
		, shader::BlendComponents & res
		, shader::BlendComponents const & src )const
	{
		res.getMember< sdw::Vec3 >( "specular", true ) += src.getMember< sdw::Vec3 >( "specular", true ) * passMultiplier;
		res.getMember< sdw::Float >( "specularFactor", true ) += src.getMember< sdw::Float >( "specularFactor", true ) * passMultiplier;
	}

	void SpecularComponent::ComponentsShader::updateOutputs( sdw::StructInstance const & components
		, sdw::StructInstance const & surface
		, sdw::Vec4 & spcRgh
		, sdw::Vec4 & colMtl
		, sdw::Vec4 & sheen )const
	{
		spcRgh.rgb() = components.getMember< sdw::Vec3 >( "specular", true )
			* components.getMember< sdw::Float >( "specularFactor", true );
	}

	//*********************************************************************************************

	SpecularComponent::MaterialShader::MaterialShader()
		: shader::PassMaterialShader{ 16u }
	{
	}

	void SpecularComponent::MaterialShader::fillMaterialType( ast::type::BaseStruct & type
		, sdw::expr::ExprList & inits )const
	{
		if ( !type.hasMember( "specular" ) )
		{
			type.declMember( "specular", ast::type::Kind::eVec3F );
			type.declMember( "specularFactor", ast::type::Kind::eFloat );
			inits.emplace_back( sdw::makeExpr( sdw::vec3( SpecularComponent::DefaultComponent ) ) );
			inits.emplace_back( sdw::makeExpr( sdw::Float{ DefaultFactor } ) );
		}
	}

	void SpecularComponent::MaterialShader::updateMaterial( sdw::Vec3 const & albedo
		, sdw::Vec4 const & spcRgh
		, sdw::Vec4 const & colMtl
		, sdw::Vec4 const & crTsIr
		, sdw::Vec4 const & sheen
		, shader::Material & material )const
	{
		material.getMember< sdw::Vec3 >( "specular", true ) = spcRgh.rgb();
	}

	//*********************************************************************************************

	void SpecularComponent::Plugin::createParsers( castor::AttributeParsers & parsers
		, ChannelFillers & channelFillers )const
	{
		castor::addParserT( parsers
			, CSCNSection::ePass
			, cuT( "specular" )
			, spccmp::parserPassSpecular
			, { castor::makeParameter< castor::ParameterType::eRgbColour >() } );
		castor::addParserT( parsers
			, CSCNSection::ePass
			, cuT( "specular_factor" )
			, spccmp::parserPassSpecularFactor
			, { castor::makeParameter< castor::ParameterType::eFloat >() } );
	}

	void SpecularComponent::Plugin::zeroBuffer( Pass const & pass
		, shader::PassMaterialShader const & materialShader
		, PassBuffer & buffer )const
	{
		auto data = buffer.getData( pass.getId() );
		data.write( materialShader.getMaterialChunk(), SpecularComponent::Default, 0u );
		data.write( materialShader.getMaterialChunk(), SpecularComponent::DefaultFactor, 12u );
	}

	bool SpecularComponent::Plugin::isComponentNeeded( TextureCombine const & textures
		, ComponentModeFlags const & filter )const
	{
		return checkFlag( filter, ComponentModeFlag::eSpecularLighting );
	}

	//*********************************************************************************************

	castor::String const SpecularComponent::TypeName = C3D_MakePassLightingComponentName( "specular" );
	castor::RgbColour const SpecularComponent::Default = { DefaultComponent, DefaultComponent, DefaultComponent };

	SpecularComponent::SpecularComponent( Pass & pass
		, castor::RgbColour defaultValue )
		: BaseDataPassComponentT{ pass, TypeName, {}, defaultValue }
	{
	}

	void SpecularComponent::accept( PassVisitorBase & vis )
	{
		vis.visit( cuT( "Specular" ) );
		vis.visit( cuT( "Factor" ), m_value.factor );
		vis.visit( cuT( "Colour" ), m_value.colour );
	}

	PassComponentUPtr SpecularComponent::doClone( Pass & pass )const
	{
		auto result = castor::makeUnique< SpecularComponent >( pass );
		result->setData( getData() );
		return castor::ptrRefCast< PassComponent >( result );
	}

	bool SpecularComponent::doWriteText( castor::String const & tabs
		, castor::Path const & folder
		, castor::String const & subfolder
		, castor::StringStream & file )const
	{
		return castor::TextWriter< SpecularComponent >{ tabs }( *this, file );
	}

	void SpecularComponent::doFillBuffer( PassBuffer & buffer )const
	{
		auto data = buffer.getData( getOwner()->getId() );
		data.write( m_materialShader->getMaterialChunk(), getSpecular(), 0u );
		data.write( m_materialShader->getMaterialChunk(), getFactor(), 12u );
	}

	//*********************************************************************************************
}
