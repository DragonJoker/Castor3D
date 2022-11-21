#include "Castor3D/Material/Pass/Component/Other/ColourComponent.hpp"

#include "Castor3D/Engine.hpp"
#include "Castor3D/Material/Pass/Pass.hpp"
#include "Castor3D/Material/Pass/PassVisitor.hpp"
#include "Castor3D/Scene/SceneFileParser.hpp"
#include "Castor3D/Shader/ShaderBuffers/PassBuffer.hpp"
#include "Castor3D/Shader/Shaders/GlslBlendComponents.hpp"
#include "Castor3D/Shader/Shaders/GlslLighting.hpp"
#include "Castor3D/Shader/Shaders/GlslMaterial.hpp"
#include "Castor3D/Shader/Shaders/GlslSurface.hpp"

#include <CastorUtils/FileParser/ParserParameter.hpp>
#include <CastorUtils/Data/Text/TextRgbColour.hpp>

namespace castor
{
	template<>
	class TextWriter< castor3d::ColourComponent >
		: public TextWriterT< castor3d::ColourComponent >
	{
	public:
		explicit TextWriter( String const & tabs )
			: TextWriterT< castor3d::ColourComponent >{ tabs }
		{
		}

		bool operator()( castor3d::ColourComponent const & object
			, StringStream & file )override
		{
			return writeNamedSub( file, cuT( "colour_hdr" ), object.getColour() );
		}
	};
}

namespace castor3d
{
	//*********************************************************************************************

	namespace albcmp
	{
		static CU_ImplementAttributeParser( parserPassHdrColour )
		{
			auto & parsingContext = getParserContext( context );

			if ( !parsingContext.pass )
			{
				CU_ParsingError( cuT( "No Pass initialised." ) );
			}
			else if ( !params.empty() )
			{
				castor::HdrRgbColour value;
				params[0]->get( value );
				auto & component = getPassComponent< ColourComponent >( parsingContext );
				component.setColour( value );
			}
		}
		CU_EndAttribute()

		static CU_ImplementAttributeParser( parserPassSrgbColour )
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
				auto & component = getPassComponent< ColourComponent >( parsingContext );
				component.setColour( value );
			}
		}
		CU_EndAttribute()
	}

	//*********************************************************************************************

	void ColourComponent::ComponentsShader::fillComponents( sdw::type::BaseStruct & components
		, shader::Materials const & materials
		, sdw::StructInstance const * surface )const
	{
		if ( !checkFlag( materials.getFilter(), ComponentModeFlag::eColour ) )
		{
			return;
		}

		if ( !components.hasMember( "colour" ) )
		{
			components.declMember( "colour", sdw::type::Kind::eVec3F );
		}
	}

	void ColourComponent::ComponentsShader::fillComponentsInits( sdw::type::BaseStruct const & components
		, shader::Materials const & materials
		, shader::Material const * material
		, sdw::StructInstance const * surface
		, sdw::Vec4 const * clrCot
		, sdw::expr::ExprList & inits )const
	{
		if ( !components.hasMember( "colour" ) )
		{
			return;
		}

		if ( material )
		{
			inits.emplace_back( sdw::makeExpr( material->getMember< sdw::Vec3 >( "colour" ) ) );
		}
		else
		{
			inits.emplace_back( sdw::makeExpr( vec3( pow( 1.0_f, 2.2_f ) ) ) );
		}
	}

	void ColourComponent::ComponentsShader::blendComponents( shader::Materials const & materials
		, sdw::Float const & passMultiplier
		, shader::BlendComponents & res
		, shader::BlendComponents const & src )const
	{
		res.getMember< sdw::Vec3 >( "colour", true ) += src.getMember< sdw::Vec3 >( "colour", true ) * passMultiplier;
	}

	void ColourComponent::ComponentsShader::updateOutputs( sdw::StructInstance const & components
		, sdw::StructInstance const & surface
		, sdw::Vec4 & spcRgh
		, sdw::Vec4 & colMtl
		, sdw::Vec4 & sheen )const
	{
		colMtl.rgb() = components.getMember< sdw::Vec3 >( "colour", true );
		colMtl.rgb() *= surface.getMember< sdw::Vec3 >( "colour", true );
	}

	//*********************************************************************************************

	ColourComponent::MaterialShader::MaterialShader()
		: shader::PassMaterialShader{ 12u }
	{

	}

	void ColourComponent::MaterialShader::fillMaterialType( sdw::type::BaseStruct & type
		, sdw::expr::ExprList & inits )const
	{
		if ( !type.hasMember( "colour" ) )
		{
			type.declMember( "colour", ast::type::Kind::eVec3F );
			inits.emplace_back( sdw::makeExpr( vec3( pow( 1.0_f, 2.2_f ) ) ) );
		}
	}

	void ColourComponent::MaterialShader::updateMaterial( sdw::Vec3 const & albedo
		, sdw::Vec4 const & spcRgh
		, sdw::Vec4 const & colMtl
		, sdw::Vec4 const & crTsIr
		, sdw::Vec4 const & sheen
		, shader::Material & material )const
	{
		material.getMember< sdw::Vec3 >( "colour", true ) = colMtl.rgb();
	}

	//*********************************************************************************************

	void ColourComponent::Plugin::createParsers( castor::AttributeParsers & parsers
		, ChannelFillers & channelFillers )const
	{
		castor::addParserT( parsers
			, CSCNSection::ePass
			, cuT( "colour_hdr" )
			, albcmp::parserPassHdrColour
			, { castor::makeParameter< castor::ParameterType::eHdrRgbColour >() } );
		castor::addParserT( parsers
			, CSCNSection::ePass
			, cuT( "colour_srgb" )
			, albcmp::parserPassSrgbColour
			, { castor::makeParameter< castor::ParameterType::eRgbColour >() } );
		castor::addParserT( parsers
			, CSCNSection::ePass
			, cuT( "albedo" )
			, albcmp::parserPassHdrColour
			, { castor::makeParameter< castor::ParameterType::eHdrRgbColour >() } );
		castor::addParserT( parsers
			, CSCNSection::ePass
			, cuT( "diffuse" )
			, albcmp::parserPassSrgbColour
			, { castor::makeParameter< castor::ParameterType::eRgbColour >() } );
	}

	void ColourComponent::Plugin::zeroBuffer( Pass const & pass
		, shader::PassMaterialShader const & materialShader
		, PassBuffer & buffer )const
	{
		static castor::HdrRgbColour const dummy{ castor::RgbColour{ 1.0f, 1.0f, 1.0f }, 2.2f };
		auto data = buffer.getData( pass.getId() );
		data.write( materialShader.getMaterialChunk()
			, dummy.red()
			, dummy.green()
			, dummy.blue()
			, 0u );
	}

	bool ColourComponent::Plugin::isComponentNeeded( TextureCombine const & textures
		, ComponentModeFlags const & filter )const
	{
		return checkFlag( filter, ComponentModeFlag::eColour );
	}

	//*********************************************************************************************

	castor::String const ColourComponent::TypeName = C3D_MakePassOtherComponentName( "colour" );

	ColourComponent::ColourComponent( Pass & pass
		, castor::HdrRgbColour defaultValue )
		: BaseDataPassComponentT{ pass, TypeName, std::move( defaultValue ) }
	{
	}

	void ColourComponent::accept( PassVisitorBase & vis )
	{
		vis.visit( cuT( "Colour" ), m_value );
	}

	PassComponentUPtr ColourComponent::doClone( Pass & pass )const
	{
		auto result = std::make_unique< ColourComponent >( pass );
		result->setData( getData() );
		return result;
	}

	bool ColourComponent::doWriteText( castor::String const & tabs
		, castor::Path const & folder
		, castor::String const & subfolder
		, castor::StringStream & file )const
	{
		return castor::TextWriter< ColourComponent >{ tabs }( *this, file );
	}

	void ColourComponent::doFillBuffer( PassBuffer & buffer )const
	{
		auto data = buffer.getData( getOwner()->getId() );
		data.write( m_materialShader->getMaterialChunk(), getColour(), 0u );
	}

	//*********************************************************************************************
}
