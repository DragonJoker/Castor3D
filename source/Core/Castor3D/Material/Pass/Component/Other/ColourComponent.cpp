#include "Castor3D/Material/Pass/Component/Other/ColourComponent.hpp"

#include "Castor3D/Engine.hpp"
#include "Castor3D/Material/Pass/Pass.hpp"
#include "Castor3D/Miscellaneous/ConfigurationVisitor.hpp"
#include "Castor3D/Scene/SceneFileParserData.hpp"
#include "Castor3D/Shader/ShaderBuffers/PassBuffer.hpp"
#include "Castor3D/Shader/Shaders/GlslBlendComponents.hpp"
#include "Castor3D/Shader/Shaders/GlslLighting.hpp"
#include "Castor3D/Shader/Shaders/GlslMaterial.hpp"
#include "Castor3D/Shader/Shaders/GlslSurface.hpp"

#include <CastorUtils/FileParser/FileParser.hpp>
#include <CastorUtils/Data/Text/TextRgbColour.hpp>

namespace c3d
{
	template<>
	class TextWriter< ColourComponent >
		: public TextWriterT< ColourComponent >
	{
	public:
		explicit TextWriter( String const & tabs )
			: TextWriterT< ColourComponent >{ tabs }
		{
		}

		bool operator()( ColourComponent const & object
			, StringStream & file )override
		{
			return writeNamedSubOpt( file, cuT( "colour_hdr" ), object.getColour(), ColourComponent::DefaultColour );
		}
	};
}

namespace c3d
{
	//*********************************************************************************************

	namespace albcmp
	{
		static CU_ImplementAttributeParserBlock( parserPassHdrColour, PassContext )
		{
			if ( !blockContext->pass )
			{
				CU_ParsingError( cuT( "No Pass initialised." ) );
			}
			else if ( !params.empty() )
			{
				HdrRgbColour value;
				params[0]->get( value );
				auto & component = getPassComponent< ColourComponent >( *blockContext );
				component.setColour( value );
			}
		}
		CU_EndAttribute()

		static CU_ImplementAttributeParserBlock( parserPassSrgbColour, PassContext )
		{
			if ( !blockContext->pass )
			{
				CU_ParsingError( cuT( "No Pass initialised." ) );
			}
			else if ( !params.empty() )
			{
				RgbColour value;
				params[0]->get( value );
				auto & component = getPassComponent< ColourComponent >( *blockContext );
				component.setColour( value );
			}
		}
		CU_EndAttribute()
	}

	//*********************************************************************************************

	ColourComponent::MaterialShader::MaterialShader()
		: shader::PassMaterialShader{ 12u }
	{

	}

	void ColourComponent::MaterialShader::fillMaterialType( sdw::type::BaseStruct & type
		, sdw::expr::ExprList & inits )const
	{
		if ( !type.hasMember( "baseColour" ) )
		{
			type.declMember( "baseColour", ast::type::Kind::eVec3F );
			inits.emplace_back( sdw::makeExpr( vec3( sdw::Float{ ColourComponent::DefaultColour.red() } ) ) );
		}
	}

	//*********************************************************************************************

	void ColourComponent::ComponentsShader::fillComponents( ComponentModeFlags componentsMask
		, sdw::type::BaseStruct & components
		, shader::Materials const & materials
		, sdw::StructInstance const * surface )const
	{
		if ( !checkFlag( componentsMask, ComponentModeFlag::eColour )
			|| !checkFlag( materials.getFilter(), ComponentModeFlag::eColour ) )
		{
			return;
		}

		if ( !components.hasMember( "baseColour" ) )
		{
			components.declMember( "baseColour", sdw::type::Kind::eVec3F );
		}
	}

	void ColourComponent::ComponentsShader::fillComponentsInits( sdw::type::BaseStruct const & components
		, shader::Materials const & materials
		, shader::Material const * material
		, sdw::StructInstance const * surface
		, sdw::Vec4 const * clrCot
		, sdw::expr::ExprList & inits )const
	{
		if ( !components.hasMember( "baseColour" ) )
		{
			return;
		}

		if ( surface
			&& surface->hasMember( "colour" ) )
		{
			if ( material )
			{
				inits.emplace_back( sdw::makeExpr( material->getMember< sdw::Vec3 >( "baseColour" )
					* surface->getMember< sdw::Vec3 >( "colour", vec3( sdw::Float{ ColourComponent::DefaultColour.red() } ) ) ) );
			}
			else
			{
				inits.emplace_back( sdw::makeExpr( pow( surface->getMember< sdw::Vec3 >( "colour", vec3( 1.0_f ) ), vec3( 2.2_f ) ) ) );
			}
		}
		else if ( material )
		{
			inits.emplace_back( sdw::makeExpr( material->getMember< sdw::Vec3 >( "baseColour" ) ) );
		}
		else
		{
			inits.emplace_back( sdw::makeExpr( vec3( sdw::Float{ ColourComponent::DefaultColour.red() } ) ) );
		}
	}

	void ColourComponent::ComponentsShader::blendComponents( shader::Materials const & materials
		, sdw::Float const & passMultiplier
		, shader::BlendComponents & res
		, shader::BlendComponents const & src )const
	{
		if ( res.hasMember( "baseColour" ) )
		{
			res.getMember< sdw::Vec3 >( "baseColour" ) += src.getMember< sdw::Vec3 >( "baseColour", true ) * passMultiplier;
		}
	}

	//*********************************************************************************************

	void ColourComponent::Plugin::createParsers( AttributeParsers & parsers
		, ChannelFillers & channelFillers )const
	{
		c3d::addParserT( parsers
			, CSCNSection::ePass
			, cuT( "colour_hdr" )
			, albcmp::parserPassHdrColour
			, { makeParameter< ParameterType::eHdrRgbColour >() } );
		c3d::addParserT( parsers
			, CSCNSection::ePass
			, cuT( "colour_srgb" )
			, albcmp::parserPassSrgbColour
			, { makeParameter< ParameterType::eRgbColour >() } );
		c3d::addParserT( parsers
			, CSCNSection::ePass
			, cuT( "albedo" )
			, albcmp::parserPassHdrColour
			, { makeParameter< ParameterType::eHdrRgbColour >() } );
		c3d::addParserT( parsers
			, CSCNSection::ePass
			, cuT( "diffuse" )
			, albcmp::parserPassSrgbColour
			, { makeParameter< ParameterType::eRgbColour >() } );
	}

	void ColourComponent::Plugin::zeroBuffer( Pass const & pass
		, shader::PassMaterialShader const & materialShader
		, PassBuffer & buffer )const
	{
		auto data = buffer.getData( pass.getId() );
		data.write( materialShader.getMaterialChunk(), ColourComponent::DefaultColour, 0u );
	}

	bool ColourComponent::Plugin::isComponentNeeded( TextureCombine const & textures
		, ComponentModeFlags const & filter )const
	{
		return checkFlag( filter, ComponentModeFlag::eColour );
	}

	//*********************************************************************************************

	String const ColourComponent::TypeName = C3D_MakePassOtherComponentName( "colour" );
	HdrRgbColour const ColourComponent::DefaultColour{ RgbColour{ 1.0f, 1.0f, 1.0f }, 2.2f };

	ColourComponent::ColourComponent( Pass & pass
		, HdrRgbColour defaultValue )
		: BaseDataPassComponentT{ pass, TypeName, {}
			, c3d::move( defaultValue ) }
	{
	}

	void ColourComponent::accept( ConfigurationVisitorBase & vis )
	{
		vis.visit( cuT( "Colour" ), m_value );
	}

	PassComponentUPtr ColourComponent::doClone( Pass & pass )const
	{
		auto result = makeRawUnique< ColourComponent >( pass );
		result->setData( getData() );
		return passComponentCast( result );
	}

	bool ColourComponent::doWriteText( String const & tabs
		, Path const & folder
		, String const & subfolder
		, StringStream & file )const
	{
		return TextWriter< ColourComponent >{ tabs }( *this, file );
	}

	void ColourComponent::doFillBuffer( PassBuffer & buffer )const
	{
		auto data = buffer.getData( getOwner()->getId() );
		data.write( m_materialShader->getMaterialChunk(), getColour(), 0u );
	}

	//*********************************************************************************************
}
