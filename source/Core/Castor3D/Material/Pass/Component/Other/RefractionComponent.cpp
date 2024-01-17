#include "Castor3D/Material/Pass/Component/Other/RefractionComponent.hpp"

#include "Castor3D/Material/Pass/Pass.hpp"
#include "Castor3D/Miscellaneous/ConfigurationVisitor.hpp"
#include "Castor3D/Material/Pass/Component/PassComponentRegister.hpp"
#include "Castor3D/Scene/SceneFileParserData.hpp"
#include "Castor3D/Shader/ShaderBuffers/PassBuffer.hpp"
#include "Castor3D/Shader/Shaders/GlslBlendComponents.hpp"
#include "Castor3D/Shader/Shaders/GlslMaterial.hpp"

#include <CastorUtils/FileParser/FileParser.hpp>

#include <ShaderWriter/Intrinsics/IntrinsicFunctions.hpp>

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
			return writeOpt( file, cuT( "refraction_ratio" ), object.getRefractionRatio(), 0.0f )
				&& writeOpt( file, cuT( "has_refraction" ), object.hasRefraction(), false );
		}
	};
}

namespace castor3d
{
	//*********************************************************************************************

	namespace refrcmp
	{
		static CU_ImplementAttributeParserBlock( parserPassRefractionRatio, PassContext )
		{
			if ( !blockContext->pass )
			{
				CU_ParsingError( cuT( "No Pass initialised." ) );
			}
			else if ( !params.empty() )
			{
				auto & component = getPassComponent< RefractionComponent >( *blockContext );
				component.setRefractionRatio( params[0]->get< float >() );
			}
		}
		CU_EndAttribute()

		static CU_ImplementAttributeParserBlock( parserPassHasRefraction, PassContext )
		{
			if ( !blockContext->pass )
			{
				CU_ParsingError( cuT( "No Pass initialised." ) );
			}
			else if ( !params.empty() )
			{
				auto & component = getPassComponent< RefractionComponent >( *blockContext );
				component.enableRefraction( params[0]->get< bool >() );
			}
		}
		CU_EndAttribute()
	}

	//*********************************************************************************************

	void RefractionComponent::ComponentsShader::fillComponents( ComponentModeFlags componentsMask
		, sdw::type::BaseStruct & components
		, shader::Materials const & materials
		, sdw::StructInstance const * surface )const
	{
		if ( ( !checkFlag( componentsMask, ComponentModeFlag::eDiffuseLighting )
				&& !checkFlag( componentsMask, ComponentModeFlag::eSpecularLighting ) )
			|| ( !checkFlag( materials.getFilter(), ComponentModeFlag::eDiffuseLighting )
				&& !checkFlag( materials.getFilter(), ComponentModeFlag::eSpecularLighting ) ) )
		{
			return;
		}

		if ( !components.hasMember( "refractionRatio" ) )
		{
			components.declMember( "refractionRatio", sdw::type::Kind::eFloat );
			components.declMember( "hasRefraction", sdw::type::Kind::eUInt );
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
			inits.emplace_back( sdw::makeExpr( material->getMember< sdw::UInt >( "hasRefraction" ) ) );
		}
		else
		{
			inits.emplace_back( sdw::makeExpr( sdw::Float{ RefractionComponent::Default } ) );
			inits.emplace_back( sdw::makeExpr( 0_u ) );
		}
	}

	void RefractionComponent::ComponentsShader::blendComponents( shader::Materials const & materials
		, sdw::Float const & passMultiplier
		, shader::BlendComponents & res
		, shader::BlendComponents const & src )const
	{
		if ( res.hasMember( "refractionRatio" ) )
		{
			res.getMember< sdw::Float >( "refractionRatio" ) += src.getMember< sdw::Float >( "refractionRatio" ) * passMultiplier;
			res.getMember< sdw::UInt >( "hasRefraction" ) = sdw::max( res.getMember< sdw::UInt >( "hasRefraction" )
				, src.getMember< sdw::UInt >( "hasRefraction" ) );
		}
	}

	//*********************************************************************************************

	RefractionComponent::MaterialShader::MaterialShader()
		: shader::PassMaterialShader{ 8u }
	{
	}

	void RefractionComponent::MaterialShader::fillMaterialType( ast::type::BaseStruct & type
		, sdw::expr::ExprList & inits )const
	{
		if ( !type.hasMember( "refractionRatio" ) )
		{
			type.declMember( "refractionRatio", ast::type::Kind::eFloat );
			type.declMember( "hasRefraction", ast::type::Kind::eUInt );
			inits.emplace_back( sdw::makeExpr( sdw::Float{ RefractionComponent::Default } ) );
			inits.emplace_back( sdw::makeExpr( 0_u ) );
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
		castor::addParserT( parsers
			, CSCNSection::ePass
			, cuT( "has_refraction" )
			, refrcmp::parserPassHasRefraction
			, { castor::makeParameter< castor::ParameterType::eBool >() } );
	}

	void RefractionComponent::Plugin::zeroBuffer( Pass const & pass
		, shader::PassMaterialShader const & materialShader
		, PassBuffer & buffer )const
	{
		auto data = buffer.getData( pass.getId() );
		VkDeviceSize offset{};
		offset += data.write( materialShader.getMaterialChunk(), RefractionComponent::Default, offset );
		data.write( materialShader.getMaterialChunk(), 0u, offset );
	}

	bool RefractionComponent::Plugin::isComponentNeeded( TextureCombine const & textures
		, ComponentModeFlags const & filter )const
	{
		return checkFlag( filter, ComponentModeFlag::eDiffuseLighting )
			|| checkFlag( filter, ComponentModeFlag::eSpecularLighting );
	}

	//*********************************************************************************************

	castor::String const RefractionComponent::TypeName = C3D_MakePassOtherComponentName( "refraction" );

	RefractionComponent::RefractionComponent( Pass & pass )
		: BaseDataPassComponentT< RefractionData >{ pass
			, TypeName
			, {} }
	{
	}

	void RefractionComponent::accept( ConfigurationVisitorBase & vis )
	{
		vis.visit( cuT( "Refraction" ) );
		vis.visit( cuT( "IoR" ), m_value.factor );
		vis.visit( cuT( "Has Refraction" ), m_value.enabled );
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
		VkDeviceSize offset{};
		offset += data.write( m_materialShader->getMaterialChunk(), getRefractionRatio(), offset );
		data.write( m_materialShader->getMaterialChunk(), hasRefraction() ? 1u : 0u, offset );
	}

	//*********************************************************************************************
}
