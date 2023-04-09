#include "Castor3D/Material/Pass/Component/Other/RefractionComponent.hpp"

#include "Castor3D/Material/Pass/Pass.hpp"
#include "Castor3D/Material/Pass/PassVisitor.hpp"
#include "Castor3D/Material/Pass/Component/PassComponentRegister.hpp"
#include "Castor3D/Scene/SceneFileParser.hpp"
#include "Castor3D/Shader/ShaderBuffers/PassBuffer.hpp"
#include "Castor3D/Shader/Shaders/GlslBlendComponents.hpp"
#include "Castor3D/Shader/Shaders/GlslMaterial.hpp"

#include <CastorUtils/FileParser/ParserParameter.hpp>

CU_ImplementSmartPtr( castor3d, RefractionComponent )

namespace castor
{
	template<>
	class TextWriter< castor3d::RefractionComponent >
		: public TextWriterT< castor3d::RefractionComponent >
	{
	public:
		explicit TextWriter( String const & tabs )
			: TextWriterT< castor3d::RefractionComponent >{ tabs }
		{
		}

		bool operator()( castor3d::RefractionComponent const & object
			, StringStream & file )override
		{
			return writeOpt( file, cuT( "refraction_ratio" ), object.getRefractionRatio(), 0.0f );
		}
	};
}

namespace castor3d
{
	//*********************************************************************************************

	namespace refrcmp
	{
		static CU_ImplementAttributeParser( parserPassRefractionRatio )
		{
			auto & parsingContext = getParserContext( context );

			if ( !parsingContext.pass )
			{
				CU_ParsingError( cuT( "No Pass initialised." ) );
			}
			else if ( !params.empty() )
			{
				float value = 0;
				params[0]->get( value );
				auto & component = getPassComponent< RefractionComponent >( parsingContext );
				component.setRefractionRatio( value );
			}
		}
		CU_EndAttribute()
	}

	//*********************************************************************************************

	void RefractionComponent::ComponentsShader::fillComponents( sdw::type::BaseStruct & components
		, shader::Materials const & materials
		, sdw::StructInstance const * surface )const
	{
		if ( !checkFlag( materials.getFilter(), ComponentModeFlag::eDiffuseLighting )
			&& !checkFlag( materials.getFilter(), ComponentModeFlag::eSpecularLighting ) )
		{
			return;
		}

		if ( !components.hasMember( "refractionRatio" ) )
		{
			components.declMember( "refractionRatio", sdw::type::Kind::eFloat );
		}
	}

	void RefractionComponent::ComponentsShader::fillComponentsInits( sdw::type::BaseStruct const & components
		, shader::Materials const & materials
		, shader::Material const * material
		, sdw::StructInstance const * surface
		, sdw::Vec4 const * clrCot
		, sdw::expr::ExprList & inits )const
	{
		if ( !components.hasMember( "refractionRatio" ) )
		{
			return;
		}

		if ( material )
		{
			inits.emplace_back( sdw::makeExpr( material->getMember< sdw::Float >( "refractionRatio" ) ) );
		}
		else
		{
			inits.emplace_back( sdw::makeExpr( 0.0_f ) );
		}
	}

	void RefractionComponent::ComponentsShader::blendComponents( shader::Materials const & materials
		, sdw::Float const & passMultiplier
		, shader::BlendComponents & res
		, shader::BlendComponents const & src )const
	{
		if ( src.hasMember( "refractionRatio" ) )
		{
			res.getMember< sdw::Float >( "refractionRatio" ) += src.getMember< sdw::Float >( "refractionRatio" ) * passMultiplier;
		}
	}

	//*********************************************************************************************

	RefractionComponent::MaterialShader::MaterialShader()
		: shader::PassMaterialShader{ 4u }
	{
	}

	void RefractionComponent::MaterialShader::fillMaterialType( ast::type::BaseStruct & type
		, sdw::expr::ExprList & inits )const
	{
		if ( !type.hasMember( "refractionRatio" ) )
		{
			type.declMember( "refractionRatio", ast::type::Kind::eFloat );
			inits.emplace_back( sdw::makeExpr( 0.0_f ) );
		}
	}

	//*********************************************************************************************

	void RefractionComponent::Plugin::createParsers( castor::AttributeParsers & parsers
		, ChannelFillers & channelFillers )const
	{
		castor::addParserT( parsers
			, CSCNSection::ePass
			, cuT( "refraction_ratio" )
			, refrcmp::parserPassRefractionRatio
			, { castor::makeParameter< castor::ParameterType::eFloat >() } );
	}

	void RefractionComponent::Plugin::zeroBuffer( Pass const & pass
		, shader::PassMaterialShader const & materialShader
		, PassBuffer & buffer )const
	{
		auto data = buffer.getData( pass.getId() );
		data.write( materialShader.getMaterialChunk(), 0.0f, 0u );
	}

	bool RefractionComponent::Plugin::isComponentNeeded( TextureCombine const & textures
		, ComponentModeFlags const & filter )const
	{
		return checkFlag( filter, ComponentModeFlag::eDiffuseLighting )
			|| checkFlag( filter, ComponentModeFlag::eSpecularLighting );
	}

	//*********************************************************************************************

	castor::String const RefractionComponent::TypeName = C3D_MakePassOtherComponentName( "refraction" );

	RefractionComponent::RefractionComponent( Pass & pass
		, float defaultValue )
		: BaseDataPassComponentT< castor::AtomicGroupChangeTracked< float > >{ pass, TypeName, defaultValue }
	{
	}

	void RefractionComponent::accept( PassVisitorBase & vis )
	{
		vis.visit( cuT( "Refraction ratio" ), m_value );
	}

	PassComponentUPtr RefractionComponent::doClone( Pass & pass )const
	{
		auto result = castor::makeUnique< RefractionComponent >( pass );
		result->setData( getData() );
		return castor::ptrRefCast< PassComponent >( result );
	}

	bool RefractionComponent::doWriteText( castor::String const & tabs
		, castor::Path const & folder
		, castor::String const & subfolder
		, castor::StringStream & file )const
	{
		return castor::TextWriter< RefractionComponent >{ tabs }( *this, file );
	}

	void RefractionComponent::doFillBuffer( PassBuffer & buffer )const
	{
		auto data = buffer.getData( getOwner()->getId() );
		data.write( m_materialShader->getMaterialChunk(), getRefractionRatio(), 0u );
	}

	//*********************************************************************************************
}
