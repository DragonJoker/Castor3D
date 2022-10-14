#include "Castor3D/Material/Pass/Component/Lighting/SpecularComponent.hpp"

#include "Castor3D/Limits.hpp"
#include "Castor3D/Engine.hpp"
#include "Castor3D/Material/Material.hpp"
#include "Castor3D/Material/Pass/Pass.hpp"
#include "Castor3D/Material/Pass/PassFactory.hpp"
#include "Castor3D/Material/Pass/Component/Lighting/MetalnessComponent.hpp"
#include "Castor3D/Material/Pass/Component/Map/SpecularMapComponent.hpp"
#include "Castor3D/Material/Pass/Component/PassComponent.hpp"
#include "Castor3D/Material/Pass/Component/PassComponentRegister.hpp"
#include "Castor3D/Material/Pass/PBR/PbrPass.hpp"
#include "Castor3D/Material/Pass/PassVisitor.hpp"
#include "Castor3D/Scene/SceneFileParser.hpp"
#include "Castor3D/Shader/Shaders/GlslBlendComponents.hpp"
#include "Castor3D/Shader/Shaders/GlslLighting.hpp"
#include "Castor3D/Shader/Shaders/GlslMaterial.hpp"

#include <CastorUtils/FileParser/ParserParameter.hpp>
#include <CastorUtils/Data/Text/TextRgbColour.hpp>

//*************************************************************************************************

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
			return writeNamedSubOpt( file, "specular", object.getSpecular(), castor3d::SpecularComponent::Default );
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
				castor::RgbColour value;
				params[0]->get( value );
				auto & component = getPassComponent< SpecularComponent >( parsingContext );
				component.setSpecular( value );
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
		}
	}

	void SpecularComponent::ComponentsShader::fillComponentsInits( sdw::type::BaseStruct const & components
		, shader::Materials const & materials
		, shader::Material const * material
		, sdw::StructInstance const * surface
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
		res.getMember< sdw::Vec3 >( "specular", true ) += src.getMember< sdw::Vec3 >( "specular", true ) * passMultiplier;
	}

	void SpecularComponent::ComponentsShader::updateOutputs( sdw::StructInstance const & components
		, sdw::StructInstance const & surface
		, sdw::Vec4 & spcRgh
		, sdw::Vec4 & colMtl )const
	{
		spcRgh.rgb() = components.getMember< sdw::Vec3 >( "specular", true );
		//spcRgh.rgb() *= shader::BlendComponents::computeF0( surface.getMember< sdw::Vec3 >( "colour", true )
		//	, components.getMember< sdw::Vec3 >( "metalness", 0.0_f ) );
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

	void SpecularComponent::MaterialShader::updateMaterial( sdw::Vec3 const & albedo
		, sdw::Vec4 const & spcRgh
		, sdw::Vec4 const & colMtl
		, shader::Material & material )const
	{
		material.getMember< sdw::Vec3 >( "specular", true ) = spcRgh.rgb();
	}

	//*********************************************************************************************

	void SpecularComponent::Plugin::createParsers( castor::AttributeParsers & parsers
		, ChannelFillers & channelFillers )const
	{
		Pass::addParserT( parsers
			, CSCNSection::ePass
			, cuT( "specular" )
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
	castor::RgbColour const SpecularComponent::DefaultPhong = { DefaultPhongComponent, DefaultPhongComponent, DefaultPhongComponent };
	castor::RgbColour const SpecularComponent::DefaultPbr = { DefaultPbrComponent, DefaultPbrComponent, DefaultPbrComponent };

	SpecularComponent::SpecularComponent( Pass & pass
		, castor::RgbColour defaultValue )
		: BaseDataPassComponentT{ pass, TypeName, castor::makeChangeTrackedT< std::atomic_bool >( defaultValue ) }
	{
		if ( m_value.value().value() == SpecularComponent::Default )
		{
			m_value->reset();
		}
	}

	void SpecularComponent::accept( PassVisitorBase & vis )
	{
		vis.visit( cuT( "Specular" ), m_value );
	}

	PassComponentUPtr SpecularComponent::doClone( Pass & pass )const
	{
		auto result = std::make_unique< SpecularComponent >( pass );
		result->m_value = m_value;
		return result;
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
		auto mtl = getOwner()->getComponent< MetalnessComponent >();
		auto & spcPlugin = getOwner()->getComponentPlugin( SpecularMapComponent::TypeName );
		auto textures = getOwner()->getTexturesMask();
		auto specular = ( isValueSet() || textures.flags.end() != checkFlags( textures, spcPlugin.getTextureFlags() ) )
			? getSpecular()
			: Pass::computeF0( getOwner()->getColour()
				, ( ( mtl && mtl->isValueSet() )
					? mtl->getMetalness()
					: ( isValueSet()
						? float( castor::point::length( castor::Point3f{ getSpecular().constPtr() } ) )
						: MetalnessComponent::Default ) ) );

		auto data = buffer.getData( getOwner()->getId() );
		data.write( m_materialShader->getMaterialChunk(), specular, 0u );
	}

	//*********************************************************************************************
}
