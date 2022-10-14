#include "Castor3D/Material/Pass/Component/Lighting/EmissiveComponent.hpp"

#include "Castor3D/Material/Pass/Pass.hpp"
#include "Castor3D/Material/Pass/PassVisitor.hpp"
#include "Castor3D/Scene/SceneFileParser.hpp"
#include "Castor3D/Shader/ShaderBuffers/PassBuffer.hpp"
#include "Castor3D/Shader/Shaders/GlslBlendComponents.hpp"
#include "Castor3D/Shader/Shaders/GlslLighting.hpp"
#include "Castor3D/Shader/Shaders/GlslMaterial.hpp"

#include <CastorUtils/FileParser/ParserParameter.hpp>

namespace castor
{
	template<>
	class TextWriter< castor3d::EmissiveComponent >
		: public TextWriterT< castor3d::EmissiveComponent >
	{
	public:
		explicit TextWriter( String const & tabs )
			: TextWriterT< castor3d::EmissiveComponent >{ tabs }
		{
		}

		bool operator()( castor3d::EmissiveComponent const & object
			, StringStream & file )override
		{
			return writeOpt( file, cuT( "emissive" ), object.getEmissiveFactor(), 0.0f );
		}
	};
}

namespace castor3d
{
	//*********************************************************************************************

	namespace emscmp
	{
		static CU_ImplementAttributeParser( parserPassEmissive )
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
				auto & component = getPassComponent< EmissiveComponent >( parsingContext );
				component.setEmissiveFactor( value );
			}
		}
		CU_EndAttribute()
	}

	//*********************************************************************************************

	void EmissiveComponent::ComponentsShader::fillComponents( sdw::type::BaseStruct & components
		, shader::Materials const & materials
		, sdw::StructInstance const * surface )const
	{
		if ( !checkFlag( materials.getFilter(), ComponentModeFlag::eDiffuseLighting ) )
		{
			return;
		}

		if ( !components.hasMember( "emissive" ) )
		{
			components.declMember( "emissive", sdw::type::Kind::eVec3F );
		}
	}

	void EmissiveComponent::ComponentsShader::fillComponentsInits( sdw::type::BaseStruct const & components
		, shader::Materials const & materials
		, shader::Material const * material
		, sdw::StructInstance const * surface
		, sdw::expr::ExprList & inits )const
	{
		if ( !components.hasMember( "emissive" ) )
		{
			return;
		}

		if ( material )
		{
			inits.emplace_back( sdw::makeExpr( vec3( material->getMember< sdw::Float >( "emissive" ) ) ) );
		}
		else
		{
			inits.emplace_back( sdw::makeExpr( vec3( 0.0_f ) ) );
		}
	}

	void EmissiveComponent::ComponentsShader::blendComponents( shader::Materials const & materials
		, sdw::Float const & passMultiplier
		, shader::BlendComponents & res
		, shader::BlendComponents const & src )const
	{
		res.getMember< sdw::Vec3 >( "emissive", true ) += src.getMember< sdw::Vec3 >( "emissive", true ) * passMultiplier;
	}

	//*********************************************************************************************

	EmissiveComponent::MaterialShader::MaterialShader()
		: shader::PassMaterialShader{ 4u }
	{
	}

	void EmissiveComponent::MaterialShader::fillMaterialType( sdw::type::BaseStruct & type
		, sdw::expr::ExprList & inits )const
	{
		if ( !type.hasMember( "emissive" ) )
		{
			type.declMember( "emissive", ast::type::Kind::eFloat );
			inits.emplace_back( sdw::makeExpr( 0.0_f ) );
		}
	}

	//*********************************************************************************************

	void EmissiveComponent::Plugin::createParsers( castor::AttributeParsers & parsers
		, ChannelFillers & channelFillers )const
	{
		Pass::addParserT( parsers
			, CSCNSection::ePass
			, cuT( "emissive" )
			, emscmp::parserPassEmissive
			, { castor::makeParameter< castor::ParameterType::eFloat >() } );
	}

	void EmissiveComponent::Plugin::zeroBuffer( Pass const & pass
		, shader::PassMaterialShader const & materialShader
		, PassBuffer & buffer )const
	{
		auto data = buffer.getData( pass.getId() );
		data.write( materialShader.getMaterialChunk(), 0.0f, 0u );
	}

	bool EmissiveComponent::Plugin::isComponentNeeded( TextureCombine const & textures
		, ComponentModeFlags const & filter )const
	{
		return checkFlag( filter, ComponentModeFlag::eDiffuseLighting );
	}

	//*********************************************************************************************

	castor::String const EmissiveComponent::TypeName = C3D_MakePassLightingComponentName( "emissive" );

	EmissiveComponent::EmissiveComponent( Pass & pass
		, float defaultValue )
		: BaseDataPassComponentT{ pass, TypeName, defaultValue }
	{
	}

	void EmissiveComponent::accept( PassVisitorBase & vis )
	{
		vis.visit( cuT( "Emissive" ), m_value );
	}

	PassComponentUPtr EmissiveComponent::doClone( Pass & pass )const
	{
		auto result = std::make_unique< EmissiveComponent >( pass );
		result->setData( getData() );
		return result;
	}

	bool EmissiveComponent::doWriteText( castor::String const & tabs
		, castor::Path const & folder
		, castor::String const & subfolder
		, castor::StringStream & file )const
	{
		return castor::TextWriter< EmissiveComponent >{ tabs }( *this, file );
	}

	void EmissiveComponent::doFillBuffer( PassBuffer & buffer )const
	{
		auto data = buffer.getData( getOwner()->getId() );
		data.write( m_materialShader->getMaterialChunk(), getEmissiveFactor(), 0u );
	}

	//*********************************************************************************************
}
