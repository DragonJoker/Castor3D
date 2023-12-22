#include "Castor3D/Material/Pass/Component/Lighting/SpecularFactorComponent.hpp"

#include "Castor3D/Limits.hpp"
#include "Castor3D/Engine.hpp"
#include "Castor3D/Material/Material.hpp"
#include "Castor3D/Material/Pass/Pass.hpp"
#include "Castor3D/Material/Pass/PassFactory.hpp"
#include "Castor3D/Miscellaneous/ConfigurationVisitor.hpp"
#include "Castor3D/Material/Pass/Component/Lighting/MetalnessComponent.hpp"
#include "Castor3D/Material/Pass/Component/Lighting/SpecularComponent.hpp"
#include "Castor3D/Material/Pass/Component/PassComponent.hpp"
#include "Castor3D/Material/Pass/Component/PassComponentRegister.hpp"
#include "Castor3D/Scene/SceneFileParserData.hpp"
#include "Castor3D/Shader/Shaders/GlslBlendComponents.hpp"
#include "Castor3D/Shader/Shaders/GlslLighting.hpp"
#include "Castor3D/Shader/Shaders/GlslMaterial.hpp"

#include <CastorUtils/FileParser/ParserParameter.hpp>
#include <CastorUtils/Data/Text/TextRgbColour.hpp>

//*************************************************************************************************

CU_ImplementSmartPtr( castor3d, SpecularFactorComponent )

namespace castor
{
	template<>
	class TextWriter< castor3d::SpecularFactorComponent >
		: public TextWriterT< castor3d::SpecularFactorComponent >
	{
	public:
		explicit TextWriter( String const & tabs )
			: TextWriterT< castor3d::SpecularFactorComponent >{ tabs }
		{
		}

		bool operator()( castor3d::SpecularFactorComponent const & object
			, StringStream & file )override
		{
			return writeOpt( file, "specular_factor", object.getFactor(), castor3d::SpecularFactorComponent::Default );
		}
	};
}

namespace castor3d
{
	//*********************************************************************************************

	namespace spccmp
	{
		static CU_ImplementAttributeParserBlock( parserPassSpecularFactor, PassContext )
		{
			if ( !blockContext->pass )
			{
				CU_ParsingError( cuT( "No Pass initialised." ) );
			}
			else if ( !params.empty() )
			{
				auto & component = getPassComponent< SpecularFactorComponent >( *blockContext );
				component.setFactor( params[0]->get< float >() );
			}
		}
		CU_EndAttribute()
	}

	//*********************************************************************************************

	void SpecularFactorComponent::ComponentsShader::fillComponents( ComponentModeFlags componentsMask
		, sdw::type::BaseStruct & components
		, shader::Materials const & materials
		, sdw::StructInstance const * surface )const
	{
		if ( !checkFlag( componentsMask, ComponentModeFlag::eSpecularLighting )
			|| !checkFlag( materials.getFilter(), ComponentModeFlag::eSpecularLighting ) )
		{
			return;
		}

		if ( !components.hasMember( "specularFactor" ) )
		{
			components.declMember( "specularFactor", sdw::type::Kind::eFloat );
		}
	}

	void SpecularFactorComponent::ComponentsShader::fillComponentsInits( sdw::type::BaseStruct const & components
		, shader::Materials const & materials
		, shader::Material const * material
		, sdw::StructInstance const * surface
		, sdw::Vec4 const * clrCot
		, sdw::expr::ExprList & inits )const
	{
		if ( !components.hasMember( "specularFactor" ) )
		{
			return;
		}

		if ( material )
		{
			inits.emplace_back( sdw::makeExpr( material->getMember< sdw::Float >( "specularFactor" ) ) );
		}
		else
		{
			inits.emplace_back( sdw::makeExpr( sdw::Float{ Default } ) );
		}
	}

	void SpecularFactorComponent::ComponentsShader::blendComponents( shader::Materials const & materials
		, sdw::Float const & passMultiplier
		, shader::BlendComponents & res
		, shader::BlendComponents const & src )const
	{
		if ( !res.hasMember( "specularFactor" ) )
		{
			return;
		}

		res.getMember< sdw::Float >( "specularFactor", true ) += src.getMember< sdw::Float >( "specularFactor", true ) * passMultiplier;
	}

	//*********************************************************************************************

	SpecularFactorComponent::MaterialShader::MaterialShader()
		: shader::PassMaterialShader{ 4u }
	{
	}

	void SpecularFactorComponent::MaterialShader::fillMaterialType( ast::type::BaseStruct & type
		, sdw::expr::ExprList & inits )const
	{
		if ( !type.hasMember( "specularFactor" ) )
		{
			type.declMember( "specularFactor", ast::type::Kind::eFloat );
			inits.emplace_back( sdw::makeExpr( sdw::Float{ Default } ) );
		}
	}

	//*********************************************************************************************

	void SpecularFactorComponent::Plugin::createParsers( castor::AttributeParsers & parsers
		, ChannelFillers & channelFillers )const
	{
		castor::addParserT( parsers
			, CSCNSection::ePass
			, cuT( "specular_factor" )
			, spccmp::parserPassSpecularFactor
			, { castor::makeParameter< castor::ParameterType::eFloat >() } );
	}

	void SpecularFactorComponent::Plugin::zeroBuffer( Pass const & pass
		, shader::PassMaterialShader const & materialShader
		, PassBuffer & buffer )const
	{
		auto data = buffer.getData( pass.getId() );
		data.write( materialShader.getMaterialChunk(), SpecularFactorComponent::Default, 0u );
	}

	bool SpecularFactorComponent::Plugin::isComponentNeeded( TextureCombine const & textures
		, ComponentModeFlags const & filter )const
	{
		return checkFlag( filter, ComponentModeFlag::eSpecularLighting );
	}

	//*********************************************************************************************

	castor::String const SpecularFactorComponent::TypeName = C3D_MakePassLightingComponentName( "specular_factor" );

	SpecularFactorComponent::SpecularFactorComponent( Pass & pass
		, float defaultValue )
		: BaseDataPassComponentT{ pass
			, TypeName
			, { SpecularComponent::TypeName }
			, defaultValue }
	{
	}

	void SpecularFactorComponent::accept( ConfigurationVisitorBase & vis )
	{
		vis.visit( cuT( "Specular Factor" ) );
		vis.visit( cuT( "Factor" ), m_value );
	}

	PassComponentUPtr SpecularFactorComponent::doClone( Pass & pass )const
	{
		auto result = castor::makeUnique< SpecularFactorComponent >( pass );
		result->setData( getData() );
		return castor::ptrRefCast< PassComponent >( result );
	}

	bool SpecularFactorComponent::doWriteText( castor::String const & tabs
		, castor::Path const & folder
		, castor::String const & subfolder
		, castor::StringStream & file )const
	{
		return castor::TextWriter< SpecularFactorComponent >{ tabs }( *this, file );
	}

	void SpecularFactorComponent::doFillBuffer( PassBuffer & buffer )const
	{
		auto data = buffer.getData( getOwner()->getId() );
		data.write( m_materialShader->getMaterialChunk(), getFactor(), 0u );
	}

	//*********************************************************************************************
}
