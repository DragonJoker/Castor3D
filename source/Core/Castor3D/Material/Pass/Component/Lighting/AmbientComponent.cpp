#include "Castor3D/Material/Pass/Component/Lighting/AmbientComponent.hpp"

#include "Castor3D/Material/Pass/Pass.hpp"
#include "Castor3D/Material/Pass/PassVisitor.hpp"
#include "Castor3D/Scene/SceneFileParser.hpp"
#include "Castor3D/Shader/ShaderBuffers/PassBuffer.hpp"
#include "Castor3D/Shader/Shaders/GlslBlendComponents.hpp"
#include "Castor3D/Shader/Shaders/GlslLighting.hpp"
#include "Castor3D/Shader/Shaders/GlslMaterial.hpp"

#include <CastorUtils/FileParser/ParserParameter.hpp>
#include <CastorUtils/Data/Text/TextRgbColour.hpp>

CU_ImplementSmartPtr( castor3d, AmbientComponent )

namespace castor
{
	template<>
	class TextWriter< castor3d::AmbientComponent >
		: public TextWriterT< castor3d::AmbientComponent >
	{
	public:
		explicit TextWriter( String const & tabs )
			: TextWriterT< castor3d::AmbientComponent >{ tabs }
		{
		}

		bool operator()( castor3d::AmbientComponent const & object
			, StringStream & file )override
		{
			return writeNamedSubOpt( file, cuT( "ambient_colour" ), object.getAmbientColour(), castor::RgbColour::fromComponents( 1.0f, 1.0f, 1.0f ) )
				&& writeOpt( file, cuT( "ambient_factor" ), object.getAmbientFactor(), 1.0f );
		}
	};
}

namespace castor3d
{
	//*********************************************************************************************

	namespace emscmp
	{
		static CU_ImplementAttributeParser( parserPassAmbient )
		{
			auto & parsingContext = getParserContext( context );

			if ( !parsingContext.pass )
			{
				CU_ParsingError( cuT( "No Pass initialised." ) );
			}
			else if ( !params.empty() )
			{
				auto & component = getPassComponent< AmbientComponent >( parsingContext );
				component.setAmbientFactor( params[0]->get< float >() );
			}
		}
		CU_EndAttribute()

		static CU_ImplementAttributeParser( parserPassAmbientColour )
		{
			auto & parsingContext = getParserContext( context );

			if ( !parsingContext.pass )
			{
				CU_ParsingError( cuT( "No Pass initialised." ) );
			}
			else if ( !params.empty() )
			{
				auto & component = getPassComponent< AmbientComponent >( parsingContext );
				component.setAmbient( params[0]->get< castor::RgbColour >() );
			}
		}
		CU_EndAttribute()
	}

	//*********************************************************************************************

	void AmbientComponent::ComponentsShader::fillComponents( sdw::type::BaseStruct & components
		, shader::Materials const & materials
		, sdw::StructInstance const * surface )const
	{
		if ( !checkFlag( materials.getFilter(), ComponentModeFlag::eDiffuseLighting ) )
		{
			return;
		}

		if ( !components.hasMember( "ambientColour" ) )
		{
			components.declMember( "ambientColour", ast::type::Kind::eVec3F );
			components.declMember( "ambientFactor", ast::type::Kind::eFloat );
		}
	}

	void AmbientComponent::ComponentsShader::fillComponentsInits( sdw::type::BaseStruct const & components
		, shader::Materials const & materials
		, shader::Material const * material
		, sdw::StructInstance const * surface
		, sdw::Vec4 const * clrCot
		, sdw::expr::ExprList & inits )const
	{
		if ( !components.hasMember( "ambientColour" ) )
		{
			return;
		}

		if ( material )
		{
			inits.emplace_back( sdw::makeExpr( material->getMember< sdw::Vec3 >( "ambientColour" ) ) );
			inits.emplace_back( sdw::makeExpr( material->getMember< sdw::Float >( "ambientFactor" ) ) );
		}
		else
		{
			inits.emplace_back( sdw::makeExpr( vec3( 1.0_f ) ) );
			inits.emplace_back( sdw::makeExpr( 1.0_f ) );
		}
	}

	void AmbientComponent::ComponentsShader::blendComponents( shader::Materials const & materials
		, sdw::Float const & passMultiplier
		, shader::BlendComponents & res
		, shader::BlendComponents const & src )const
	{
		res.getMember< sdw::Vec3 >( "ambientColour", true ) += src.getMember< sdw::Vec3 >( "ambientColour", true ) * passMultiplier;
		res.getMember< sdw::Float >( "ambientFactor", true ) += src.getMember< sdw::Float >( "ambientFactor", true ) * passMultiplier;
	}

	//*********************************************************************************************

	AmbientComponent::MaterialShader::MaterialShader()
		: shader::PassMaterialShader{ 16u }
	{
	}

	void AmbientComponent::MaterialShader::fillMaterialType( sdw::type::BaseStruct & type
		, sdw::expr::ExprList & inits )const
	{
		if ( !type.hasMember( "ambientColour" ) )
		{
			type.declMember( "ambientColour", ast::type::Kind::eVec3F );
			type.declMember( "ambientFactor", ast::type::Kind::eFloat );
			inits.emplace_back( sdw::makeExpr( vec3( 1.0_f ) ) );
			inits.emplace_back( sdw::makeExpr( 1.0_f ) );
		}
	}

	//*********************************************************************************************

	void AmbientComponent::Plugin::createParsers( castor::AttributeParsers & parsers
		, ChannelFillers & channelFillers )const
	{
		castor::addParserT( parsers
			, CSCNSection::ePass
			, cuT( "ambient" )
			, emscmp::parserPassAmbient
			, { castor::makeParameter< castor::ParameterType::eFloat >() } );
		castor::addParserT( parsers
			, CSCNSection::ePass
			, cuT( "ambient_factor" )
			, emscmp::parserPassAmbient
			, { castor::makeParameter< castor::ParameterType::eFloat >() } );
		castor::addParserT( parsers
			, CSCNSection::ePass
			, cuT( "ambient_colour" )
			, emscmp::parserPassAmbientColour
			, { castor::makeParameter< castor::ParameterType::eRgbColour >() } );
	}

	void AmbientComponent::Plugin::zeroBuffer( Pass const & pass
		, shader::PassMaterialShader const & materialShader
		, PassBuffer & buffer )const
	{
		auto data = buffer.getData( pass.getId() );
		VkDeviceSize offset{};
		offset += data.write( materialShader.getMaterialChunk(), castor::RgbColour::fromComponents( 1.0f, 1.0f, 1.0f ), offset );
		data.write( materialShader.getMaterialChunk(), 1.0f, offset );
	}

	bool AmbientComponent::Plugin::isComponentNeeded( TextureCombine const & textures
		, ComponentModeFlags const & filter )const
	{
		return checkFlag( filter, ComponentModeFlag::eDiffuseLighting );
	}

	//*********************************************************************************************

	castor::String const AmbientComponent::TypeName = C3D_MakePassLightingComponentName( "ambient" );

	AmbientComponent::AmbientComponent( Pass & pass )
		: BaseDataPassComponentT{ pass, TypeName }
	{
	}

	void AmbientComponent::accept( PassVisitorBase & vis )
	{
		vis.visit( cuT( "Ambient Colour" ), m_value.colour );
		vis.visit( cuT( "Ambient Factor" ), m_value.factor );
	}

	PassComponentUPtr AmbientComponent::doClone( Pass & pass )const
	{
		auto result = castor::makeUnique< AmbientComponent >( pass );
		result->setData( getData() );
		return castor::ptrRefCast< PassComponent >( result );
	}

	bool AmbientComponent::doWriteText( castor::String const & tabs
		, castor::Path const & folder
		, castor::String const & subfolder
		, castor::StringStream & file )const
	{
		return castor::TextWriter< AmbientComponent >{ tabs }( *this, file );
	}

	void AmbientComponent::doFillBuffer( PassBuffer & buffer )const
	{
		auto data = buffer.getData( getOwner()->getId() );
		VkDeviceSize offset{};
		offset += data.write( m_materialShader->getMaterialChunk(), getAmbientColour(), offset );
		data.write( m_materialShader->getMaterialChunk(), getAmbientFactor(), offset );
	}

	//*********************************************************************************************
}
