#include "Castor3D/Material/Pass/Component/Other/ReflectionComponent.hpp"

#include "Castor3D/Material/Pass/Pass.hpp"
#include "Castor3D/Material/Pass/PassVisitor.hpp"
#include "Castor3D/Scene/SceneFileParser.hpp"
#include "Castor3D/Shader/ShaderBuffers/PassBuffer.hpp"
#include "Castor3D/Shader/Shaders/GlslBlendComponents.hpp"
#include "Castor3D/Shader/Shaders/GlslMaterial.hpp"

#include <CastorUtils/FileParser/ParserParameter.hpp>

namespace castor
{
	template<>
	class TextWriter< castor3d::ReflectionComponent >
		: public TextWriterT< castor3d::ReflectionComponent >
	{
	public:
		explicit TextWriter( String const & tabs )
			: TextWriterT< castor3d::ReflectionComponent >{ tabs }
		{
		}

		bool operator()( castor3d::ReflectionComponent const & object
			, StringStream & file )override
		{
			return writeOpt( file, cuT( "reflections" ), object.hasReflections() );
		}
	};
}

namespace castor3d
{
	//*********************************************************************************************

	namespace reflcmp
	{
		static CU_ImplementAttributeParser( parserPassReflections )
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
				auto & component = getPassComponent< ReflectionComponent >( parsingContext );
				component.enableReflections( params[0]->get< bool >() );
			}
		}
		CU_EndAttribute()
	}

	//*********************************************************************************************

	void ReflectionComponent::ComponentsShader::fillComponents( sdw::type::BaseStruct & components
		, shader::Materials const & materials
		, sdw::StructInstance const * surface )const
	{
		if ( !checkFlag( materials.getFilter(), ComponentModeFlag::eSpecularLighting ) )
		{
			return;
		}

		if ( !components.hasMember( "hasReflection" ) )
		{
			components.declMember( "hasReflection", sdw::type::Kind::eUInt );
		}
	}

	void ReflectionComponent::ComponentsShader::fillComponentsInits( sdw::type::BaseStruct const & components
		, shader::Materials const & materials
		, shader::Material const * material
		, sdw::StructInstance const * surface
		, sdw::expr::ExprList & inits )const
	{
		if ( !components.hasMember( "hasReflection" ) )
		{
			return;
		}

		if ( material )
		{
			inits.emplace_back( sdw::makeExpr( material->getMember< sdw::UInt >( "hasReflection" ) ) );
		}
		else
		{
			inits.emplace_back( sdw::makeExpr( 0_u ) );
		}
	}

	void ReflectionComponent::ComponentsShader::blendComponents( shader::Materials const & materials
		, sdw::Float const & passMultiplier
		, shader::BlendComponents & res
		, shader::BlendComponents const & src )const
	{
		if ( src.hasMember( "hasReflection" ) )
		{
			res.getMember< sdw::UInt >( "hasReflection" ) += src.getMember< sdw::UInt >( "hasReflection" );
		}
	}

	//*********************************************************************************************

	ReflectionComponent::MaterialShader::MaterialShader()
		: shader::PassMaterialShader{ 4u }
	{
	}

	void ReflectionComponent::MaterialShader::fillMaterialType( ast::type::BaseStruct & type
		, sdw::expr::ExprList & inits )const
	{
		if ( !type.hasMember( "hasReflection" ) )
		{
			type.declMember( "hasReflection", ast::type::Kind::eUInt );
			inits.emplace_back( sdw::makeExpr( 0_u ) );
		}
	}

	//*********************************************************************************************

	void ReflectionComponent::Plugin::createParsers( castor::AttributeParsers & parsers
		, ChannelFillers & channelFillers )const
	{
		Pass::addParserT( parsers
			, CSCNSection::ePass
			, cuT( "reflections" )
			, reflcmp::parserPassReflections
			, { castor::makeParameter< castor::ParameterType::eBool >() } );
	}

	void ReflectionComponent::Plugin::zeroBuffer( Pass const & pass
		, shader::PassMaterialShader const & materialShader
		, PassBuffer & buffer )const
	{
		auto data = buffer.getData( pass.getId() );
		data.write( materialShader.getMaterialChunk(), 0u, 0u );
	}

	bool ReflectionComponent::Plugin::isComponentNeeded( TextureCombine const & textures
		, ComponentModeFlags const & filter )const
	{
		return checkFlag( filter, ComponentModeFlag::eSpecularLighting );
	}

	//*********************************************************************************************

	castor::String const ReflectionComponent::TypeName = C3D_MakePassOtherComponentName( "reflection" );

	ReflectionComponent::ReflectionComponent( Pass & pass )
		: BaseDataPassComponentT< castor::AtomicGroupChangeTracked< bool > >{ pass, TypeName }
	{
	}

	void ReflectionComponent::accept( PassVisitorBase & vis )
	{
		vis.visit( cuT( "Enable Reflection" ), m_value );
	}

	PassComponentUPtr ReflectionComponent::doClone( Pass & pass )const
	{
		auto result = std::make_unique< ReflectionComponent >( pass );
		result->setData( getData() );
		return result;
	}

	bool ReflectionComponent::doWriteText( castor::String const & tabs
		, castor::Path const & folder
		, castor::String const & subfolder
		, castor::StringStream & file )const
	{
		return castor::TextWriter< ReflectionComponent >{ tabs }( *this, file );
	}

	void ReflectionComponent::doFillBuffer( PassBuffer & buffer )const
	{
		auto data = buffer.getData( getOwner()->getId() );
		data.write( m_materialShader->getMaterialChunk(), hasReflections() ? 1u : 0u, 0u );
	}

	//*********************************************************************************************
}
