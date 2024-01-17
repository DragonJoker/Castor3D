#include "Castor3D/Material/Pass/Component/Lighting/SpecularComponent.hpp"

#include "Castor3D/Limits.hpp"
#include "Castor3D/Engine.hpp"
#include "Castor3D/Material/Material.hpp"
#include "Castor3D/Material/Pass/Pass.hpp"
#include "Castor3D/Material/Pass/PassFactory.hpp"
#include "Castor3D/Miscellaneous/ConfigurationVisitor.hpp"
#include "Castor3D/Material/Pass/Component/Lighting/MetalnessComponent.hpp"
#include "Castor3D/Material/Pass/Component/Map/SpecularMapComponent.hpp"
#include "Castor3D/Material/Pass/Component/PassComponent.hpp"
#include "Castor3D/Material/Pass/Component/PassComponentRegister.hpp"
#include "Castor3D/Scene/SceneFileParserData.hpp"
#include "Castor3D/Shader/Shaders/GlslBlendComponents.hpp"
#include "Castor3D/Shader/Shaders/GlslLighting.hpp"
#include "Castor3D/Shader/Shaders/GlslMaterial.hpp"

#include <CastorUtils/FileParser/FileParser.hpp>
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
			return writeNamedSubOpt( file, "specular_colour", object.getSpecular(), castor3d::SpecularComponent::Default );
		}
	};
}

namespace castor3d
{
	//*********************************************************************************************

	namespace spccmp
	{
		static CU_ImplementAttributeParserBlock( parserPassSpecular, PassContext )
		{
			if ( !blockContext->pass )
			{
				CU_ParsingError( cuT( "No Pass initialised." ) );
			}
			else if ( !params.empty() )
			{
				auto & component = getPassComponent< SpecularComponent >( *blockContext );
				component.setSpecular( params[0]->get< castor::RgbColour >() );
			}
		}
		CU_EndAttribute()
	}

	//*********************************************************************************************

	void SpecularComponent::ComponentsShader::fillComponents( ComponentModeFlags componentsMask
		, sdw::type::BaseStruct & components
		, shader::Materials const & materials
		, sdw::StructInstance const * surface )const
	{
		if ( !checkFlag( componentsMask, ComponentModeFlag::eSpecularLighting )
			|| !checkFlag( materials.getFilter(), ComponentModeFlag::eSpecularLighting ) )
		{
			return;
		}

		if ( !components.hasMember( "specular" ) )
		{
			components.declMember( "specular", sdw::type::Kind::eVec3F );
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
		}
		else
		{
			inits.emplace_back( sdw::makeExpr( sdw::vec3( SpecularComponent::DefaultComponent ) ) );
		}
	}

	void SpecularComponent::ComponentsShader::blendComponents( shader::Materials const & materials
		, sdw::Float const & passMultiplier
		, shader::BlendComponents & res
		, shader::BlendComponents const & src )const
	{
		if ( !res.hasMember( "specular" ) )
		{
			return;
		}

		res.getMember< sdw::Vec3 >( "specular", true ) += src.getMember< sdw::Vec3 >( "specular", true ) * passMultiplier;
	}

	//*********************************************************************************************

	SpecularComponent::MaterialShader::MaterialShader()
		: shader::PassMaterialShader{ 12u }
	{
	}

	void SpecularComponent::MaterialShader::fillMaterialType( ast::type::BaseStruct & type
		, sdw::expr::ExprList & inits )const
	{
		if ( !type.hasMember( "specular" ) )
		{
			type.declMember( "specular", ast::type::Kind::eVec3F );
			inits.emplace_back( sdw::makeExpr( sdw::vec3( SpecularComponent::DefaultComponent ) ) );
		}
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
			, cuT( "specular_colour" )
			, spccmp::parserPassSpecular
			, { castor::makeParameter< castor::ParameterType::eRgbColour >() } );
	}

	void SpecularComponent::Plugin::zeroBuffer( Pass const & pass
		, shader::PassMaterialShader const & materialShader
		, PassBuffer & buffer )const
	{
		auto data = buffer.getData( pass.getId() );
		data.write( materialShader.getMaterialChunk(), SpecularComponent::Default, 0u );
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

	void SpecularComponent::accept( ConfigurationVisitorBase & vis )
	{
		vis.visit( cuT( "Specular" ) );
		vis.visit( cuT( "Colour" ), m_value );
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
	}

	//*********************************************************************************************
}
