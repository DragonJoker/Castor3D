#include "Castor3D/Material/Pass/Component/Lighting/MetalnessComponent.hpp"

#include "Castor3D/Engine.hpp"
#include "Castor3D/Material/Material.hpp"
#include "Castor3D/Material/Pass/Pass.hpp"
#include "Castor3D/Miscellaneous/ConfigurationVisitor.hpp"
#include "Castor3D/Material/Pass/Component/PassComponentRegister.hpp"
#include "Castor3D/Material/Pass/Component/Lighting/SpecularComponent.hpp"
#include "Castor3D/Scene/SceneFileParser.hpp"
#include "Castor3D/Shader/ShaderBuffers/PassBuffer.hpp"
#include "Castor3D/Shader/Shaders/GlslBlendComponents.hpp"
#include "Castor3D/Shader/Shaders/GlslLighting.hpp"
#include "Castor3D/Shader/Shaders/GlslMaterial.hpp"

#include <CastorUtils/FileParser/ParserParameter.hpp>

CU_ImplementSmartPtr( castor3d, MetalnessComponent )

namespace castor
{
	template<>
	class TextWriter< castor3d::MetalnessComponent >
		: public TextWriterT< castor3d::MetalnessComponent >
	{
	public:
		explicit TextWriter( String const & tabs )
			: TextWriterT< castor3d::MetalnessComponent >{ tabs }
		{
		}

		bool operator()( castor3d::MetalnessComponent const & object
			, StringStream & file )override
		{
			return writeOpt( file, "metalness", object.getMetalness(), castor3d::MetalnessComponent::Default );
		}
	};
}

namespace castor3d
{
	//*********************************************************************************************

	namespace mtlcmp
	{
		static CU_ImplementAttributeParser( parserPassMetalness )
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
				auto & component = getPassComponent< MetalnessComponent >( parsingContext );
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
		if ( !res.hasMember( "metalness" ) )
		{
			return;
		}

		res.getMember< sdw::Float >( "metalness", true ) += src.getMember< sdw::Float >( "metalness", true ) * passMultiplier;
	}

	void MetalnessComponent::ComponentsShader::updateComponent( TextureCombine const & combine
		, sdw::Array< sdw::CombinedImage2DRgba32 > const & maps
		, shader::BlendComponents & components )const
	{
		if ( !components.hasMember( "metalness" ) )
		{
			return;
		}

		if ( components.hasMember( "transmission" ) )
		{
			components.transmission = mix( components.transmission, 0.0_f, components.metalness );
		}

		if ( components.hasMember( "refraction" ) )
		{
			components.refraction *= ( 1.0_f - components.getMember< sdw::Float >( "metalness" ) );
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

	void MetalnessComponent::Plugin::createParsers( castor::AttributeParsers & parsers
		, ChannelFillers & channelFillers )const
	{
		castor::addParserT( parsers
			, CSCNSection::ePass
			, cuT( "metalness" )
			, mtlcmp::parserPassMetalness
			, { castor::makeParameter< castor::ParameterType::eFloat >() } );
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

	castor::String const MetalnessComponent::TypeName = C3D_MakePassLightingComponentName( "metalness" );

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
		auto result = castor::makeUnique< MetalnessComponent >( pass );
		result->setData( getData() );
		return castor::ptrRefCast< PassComponent >( result );
	}

	bool MetalnessComponent::doWriteText( castor::String const & tabs
		, castor::Path const & folder
		, castor::String const & subfolder
		, castor::StringStream & file )const
	{
		return castor::TextWriter< MetalnessComponent >{ tabs }( *this, file );
	}

	void MetalnessComponent::doFillBuffer( PassBuffer & buffer )const
	{
		auto data = buffer.getData( getOwner()->getId() );
		data.write( m_materialShader->getMaterialChunk(), getMetalness(), 0u );
	}

	//*********************************************************************************************
}
