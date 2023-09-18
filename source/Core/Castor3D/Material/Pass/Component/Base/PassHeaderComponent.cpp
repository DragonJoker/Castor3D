#include "Castor3D/Material/Pass/Component/Base/PassHeaderComponent.hpp"

#include "Castor3D/Engine.hpp"
#include "Castor3D/Material/Pass/Pass.hpp"
#include "Castor3D/Material/Pass/PassFactory.hpp"
#include "Castor3D/Miscellaneous/ConfigurationVisitor.hpp"
#include "Castor3D/Scene/SceneFileParser.hpp"
#include "Castor3D/Shader/ShaderBuffers/PassBuffer.hpp"

#include <CastorUtils/FileParser/ParserParameter.hpp>
#include <CastorUtils/Data/Text/TextRgbColour.hpp>

CU_ImplementSmartPtr( castor3d, PassHeaderComponent )

namespace castor
{
	template<>
	class TextWriter< castor3d::PassHeaderComponent >
		: public TextWriterT< castor3d::PassHeaderComponent >
	{
	public:
		explicit TextWriter( String const & tabs )
			: TextWriterT< castor3d::PassHeaderComponent >{ tabs }
		{
		}

		bool operator()( castor3d::PassHeaderComponent const & object
			, StringStream & file )override
		{
			return writeOpt( file, cuT( "lighting" ), object.isLightingEnabled(), true );
		}
	};
}

namespace castor3d
{
	//*********************************************************************************************

	namespace phcmp
	{
		static CU_ImplementAttributeParser( parserPassLighting )
		{
			auto & parsingContext = getParserContext( context );

			if ( !parsingContext.pass )
			{
				CU_ParsingError( cuT( "No Pass initialised." ) );
			}
			else if ( !params.empty() )
			{
				bool value{ true };
				params[0]->get( value );
				auto & component = getPassComponent< PassHeaderComponent >( parsingContext );
				component.enableLighting( value );
			}
		}
		CU_EndAttribute()
	}

	//*********************************************************************************************

	PassHeaderComponent::MaterialShader::MaterialShader()
		: shader::PassMaterialShader{ 16u }
	{
	}

	void PassHeaderComponent::MaterialShader::fillMaterialType( sdw::type::BaseStruct & type
		, sdw::expr::ExprList & inits )const
	{
		if ( !type.hasMember( "passId" ) )
		{
			type.declMember( "passId", ast::type::Kind::eUInt );
			type.declMember( "index", ast::type::Kind::eUInt );
			type.declMember( "passCount", ast::type::Kind::eUInt );
			type.declMember( "lighting", ast::type::Kind::eUInt );
			inits.emplace_back( makeExpr( 0_u ) );
			inits.emplace_back( makeExpr( 0_u ) );
			inits.emplace_back( makeExpr( 0_u ) );
			inits.emplace_back( makeExpr( 0_u ) );
		}
	}

	//*********************************************************************************************

	void PassHeaderComponent::Plugin::createParsers( castor::AttributeParsers & parsers
		, ChannelFillers & channelFillers )const
	{
		castor::addParserT( parsers
			, CSCNSection::ePass
			, cuT( "lighting" )
			, phcmp::parserPassLighting
			, { castor::makeParameter< castor::ParameterType::eBool >() } );
	}

	void PassHeaderComponent::Plugin::zeroBuffer( Pass const & pass
		, shader::PassMaterialShader const & materialShader
		, PassBuffer & buffer )const
	{
		auto data = buffer.getData( pass.getId() );
		data.write( materialShader.getMaterialChunk()
			, 0u, 0u, 0u, 0u
			, 0u );
	}

	//*********************************************************************************************

	castor::String const PassHeaderComponent::TypeName = C3D_MakePassBaseComponentName( "header" );

	PassHeaderComponent::PassHeaderComponent( Pass & pass )
		: BaseDataPassComponentT< castor::AtomicGroupChangeTracked< uint32_t > >{ pass, TypeName }
	{
		m_value = 1u;
	}

	void PassHeaderComponent::accept( ConfigurationVisitorBase & vis )
	{
		vis.visit( cuT( "Lighting" ), m_value );
	}

	PassComponentUPtr PassHeaderComponent::doClone( Pass & pass )const
	{
		auto result = castor::makeUnique< PassHeaderComponent >( pass );
		result->setData( getData() );
		return castor::ptrRefCast< PassComponent >( result );
	}

	bool PassHeaderComponent::doWriteText( castor::String const & tabs
		, castor::Path const & folder
		, castor::String const & subfolder
		, castor::StringStream & file )const
	{
		return castor::TextWriter< PassHeaderComponent >{ tabs }( *this, file );
	}

	void PassHeaderComponent::doFillBuffer( PassBuffer & buffer )const
	{
		auto data = buffer.getData( getOwner()->getId() );
		data.write( m_materialShader->getMaterialChunk()
			, getOwner()->getId()
			, getOwner()->getIndex()
			, getOwner()->getOwner()->getPassCount()
			, getData()
			, 0u );
	}

	//*********************************************************************************************
}
