#include "Castor3D/Material/Pass/Component/Lighting/MetalnessComponent.hpp"

#include "Castor3D/Material/Pass/Pass.hpp"
#include "Castor3D/Miscellaneous/ConfigurationVisitor.hpp"
#include "Castor3D/Material/Pass/Component/Lighting/SpecularComponent.hpp"
#include "Castor3D/Scene/SceneFileParserData.hpp"
#include "Castor3D/Shader/ShaderBuffers/PassBuffer.hpp"
#include "Castor3D/Shader/Shaders/GlslBlendComponents.hpp"
#include "Castor3D/Shader/Shaders/GlslMaterial.hpp"

#include <CastorUtils/FileParser/FileParser.hpp>

namespace c3d
{
	template<>
	class TextWriter< MetalnessComponent >
		: public TextWriterT< MetalnessComponent >
	{
	public:
		explicit TextWriter( String const & tabs )
			: TextWriterT< MetalnessComponent >{ tabs }
		{
		}

		bool operator()( MetalnessComponent const & object
			, StringStream & file )override
		{
			return writeOpt( file, cuT( "metalness" ), object.getMetalness(), MetalnessComponent::Default );
		}
	};

	//*********************************************************************************************

	namespace mtlcmp
	{
		static CU_ImplementAttributeParserBlock( parserPassMetalness, PassContext )
		{
			if ( !blockContext->pass )
			{
				CU_ParsingError( cuT( "No Pass initialised." ) );
			}
			else if ( !params.empty() )
			{
				float value;
				params[0]->get( value );
				auto & component = getPassComponent< MetalnessComponent >( *blockContext );
				component.setMetalness( value );
			}
		}
		CU_EndAttribute()
	}

	//*********************************************************************************************

	void MetalnessComponent::ComponentsShader::fillComponents( ComponentModeFlags componentsMask
		, sdw::type::BaseStruct & components
		, shader::Materials const & materials
		, sdw::StructInstance const * surface )const
	{
		if ( ( !checkFlag( componentsMask, ComponentModeFlag::eOpacity )
				&& !checkFlag( componentsMask, ComponentModeFlag::eSpecularLighting ) )
			|| ( !checkFlag( materials.getFilter(), ComponentModeFlag::eOpacity )
				&& !checkFlag( materials.getFilter(), ComponentModeFlag::eSpecularLighting ) ) )
		{
			return;
		}

		if ( !components.hasMember( "metalness" ) )
		{
			components.declMember( "metalness", sdw::type::Kind::eFloat );
		}
	}

	void MetalnessComponent::ComponentsShader::fillComponentsInits( sdw::type::BaseStruct const & components
		, shader::Materials const & materials
		, shader::Material const * material
		, sdw::StructInstance const * surface
		, sdw::Vec4 const * clrCot
		, sdw::expr::ExprList & inits )const
	{
		if ( !components.hasMember( "metalness" ) )
		{
			return;
		}

		if ( material )
		{
			inits.emplace_back( sdw::makeExpr( material->getMember< sdw::Float >( "metalness" ) ) );
		}
		else
		{
			inits.emplace_back( sdw::makeExpr( sdw::Float{ MetalnessComponent::Default } ) );
		}
	}

	void MetalnessComponent::ComponentsShader::blendComponents( shader::Materials const & materials
		, sdw::Float const & passMultiplier
		, shader::BlendComponents & res
		, shader::BlendComponents const & src )const
	{
		if ( res.hasMember( "metalness" ) )
		{
			res.metalness += src.metalness * passMultiplier;
		}
	}

	//*********************************************************************************************

	MetalnessComponent::MaterialShader::MaterialShader()
		: shader::PassMaterialShader{ 4u }
	{
	}

	void MetalnessComponent::MaterialShader::fillMaterialType( ast::type::BaseStruct & type
		, sdw::expr::ExprList & inits )const
	{
		if ( !type.hasMember( "metalness" ) )
		{
			type.declMember( "metalness", ast::type::Kind::eFloat );
			inits.emplace_back( sdw::makeExpr( sdw::Float{ MetalnessComponent::Default } ) );
		}
	}

	//*********************************************************************************************

	void MetalnessComponent::Plugin::createParsers( AttributeParsers & parsers
		, ChannelFillers & channelFillers )const
	{
		c3d::addParserT( parsers
			, CSCNSection::ePass
			, cuT( "metalness" )
			, mtlcmp::parserPassMetalness
			, { makeParameter< ParameterType::eFloat >() } );
	}

	void MetalnessComponent::Plugin::zeroBuffer( Pass const & pass
		, shader::PassMaterialShader const & materialShader
		, PassBuffer & buffer )const
	{
		auto data = buffer.getData( pass.getId() );
		data.write( materialShader.getMaterialChunk(), MetalnessComponent::Default, 0u );
	}

	bool MetalnessComponent::Plugin::isComponentNeeded( TextureCombine const & textures
		, ComponentModeFlags const & filter )const
	{
		return checkFlag( filter, ComponentModeFlag::eOpacity )
			|| checkFlag( filter, ComponentModeFlag::eSpecularLighting );
	}

	//*********************************************************************************************

	String const MetalnessComponent::TypeName = C3D_MakePassLightingComponentName( "metalness" );

	MetalnessComponent::MetalnessComponent( Pass & pass
		, float defaultValue )
		: BaseDataPassComponentT{ pass
			, TypeName
			, { SpecularComponent::TypeName }
			, defaultValue }
	{
	}

	void MetalnessComponent::accept( ConfigurationVisitorBase & vis )
	{
		vis.visit( cuT( "Metalness" ) );
		vis.visit( cuT( "Factor" ), m_value );
	}

	PassComponentUPtr MetalnessComponent::doClone( Pass & pass )const
	{
		auto result = makeRawUnique< MetalnessComponent >( pass );
		result->setData( getData() );
		return passComponentCast( result );
	}

	bool MetalnessComponent::doWriteText( String const & tabs
		, Path const & folder
		, String const & subfolder
		, StringStream & file )const
	{
		return TextWriter< MetalnessComponent >{ tabs }( *this, file );
	}

	void MetalnessComponent::doFillBuffer( PassBuffer & buffer )const
	{
		auto data = buffer.getData( getOwner()->getId() );
		data.write( m_materialShader->getMaterialChunk(), getMetalness(), 0u );
	}

	//*********************************************************************************************
}
