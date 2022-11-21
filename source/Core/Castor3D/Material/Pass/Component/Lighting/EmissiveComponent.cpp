#include "Castor3D/Material/Pass/Component/Lighting/EmissiveComponent.hpp"

#include "Castor3D/Material/Pass/Pass.hpp"
#include "Castor3D/Material/Pass/PassVisitor.hpp"
#include "Castor3D/Scene/SceneFileParser.hpp"
#include "Castor3D/Shader/ShaderBuffers/PassBuffer.hpp"
#include "Castor3D/Shader/Shaders/GlslBlendComponents.hpp"
#include "Castor3D/Shader/Shaders/GlslLighting.hpp"
#include "Castor3D/Shader/Shaders/GlslMaterial.hpp"

#include <CastorUtils/FileParser/ParserParameter.hpp>
#include <CastorUtils/Data/Text/TextRgbColour.hpp>

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
			return writeNamedSubOpt( file, cuT( "emissive_colour" ), object.getEmissiveColour(), castor::RgbColour{} )
				&& writeOpt( file, cuT( "emissive_factor" ), object.getEmissiveFactor(), 1.0f );
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
				auto & component = getPassComponent< EmissiveComponent >( parsingContext );
				component.setEmissiveFactor( params[0]->get< float >() );
			}
		}
		CU_EndAttribute()

		static CU_ImplementAttributeParser( parserPassEmissiveColour )
		{
			auto & parsingContext = getParserContext( context );

			if ( !parsingContext.pass )
			{
				CU_ParsingError( cuT( "No Pass initialised." ) );
			}
			else if ( !params.empty() )
			{
				auto & component = getPassComponent< EmissiveComponent >( parsingContext );
				component.setEmissive( params[0]->get< castor::RgbColour >() );
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

		if ( !components.hasMember( "emissiveColour" ) )
		{
			components.declMember( "emissiveColour", ast::type::Kind::eVec3F );
			components.declMember( "emissiveFactor", ast::type::Kind::eFloat );
		}
	}

	void EmissiveComponent::ComponentsShader::fillComponentsInits( sdw::type::BaseStruct const & components
		, shader::Materials const & materials
		, shader::Material const * material
		, sdw::StructInstance const * surface
		, sdw::Vec4 const * clrCot
		, sdw::expr::ExprList & inits )const
	{
		if ( !components.hasMember( "emissiveColour" ) )
		{
			return;
		}

		if ( material )
		{
			inits.emplace_back( sdw::makeExpr( material->getMember< sdw::Vec3 >( "emissiveColour" ) ) );
			inits.emplace_back( sdw::makeExpr( material->getMember< sdw::Float >( "emissiveFactor" ) ) );
		}
		else
		{
			inits.emplace_back( sdw::makeExpr( vec3( 0.0_f ) ) );
			inits.emplace_back( sdw::makeExpr( 0.0_f ) );
		}
	}

	void EmissiveComponent::ComponentsShader::blendComponents( shader::Materials const & materials
		, sdw::Float const & passMultiplier
		, shader::BlendComponents & res
		, shader::BlendComponents const & src )const
	{
		res.getMember< sdw::Vec3 >( "emissiveColour", true ) += src.getMember< sdw::Vec3 >( "emissiveColour", true ) * passMultiplier;
		res.getMember< sdw::Float >( "emissiveFactor", true ) += src.getMember< sdw::Float >( "emissiveFactor", true ) * passMultiplier;
	}

	//*********************************************************************************************

	EmissiveComponent::MaterialShader::MaterialShader()
		: shader::PassMaterialShader{ 16u }
	{
	}

	void EmissiveComponent::MaterialShader::fillMaterialType( sdw::type::BaseStruct & type
		, sdw::expr::ExprList & inits )const
	{
		if ( !type.hasMember( "emissiveColour" ) )
		{
			type.declMember( "emissiveColour", ast::type::Kind::eVec3F );
			type.declMember( "emissiveFactor", ast::type::Kind::eFloat );
			inits.emplace_back( sdw::makeExpr( vec3( 0.0_f ) ) );
			inits.emplace_back( sdw::makeExpr( 0.0_f ) );
		}
	}

	//*********************************************************************************************

	void EmissiveComponent::Plugin::createParsers( castor::AttributeParsers & parsers
		, ChannelFillers & channelFillers )const
	{
		castor::addParserT( parsers
			, CSCNSection::ePass
			, cuT( "emissive" )
			, emscmp::parserPassEmissive
			, { castor::makeParameter< castor::ParameterType::eFloat >() } );
		castor::addParserT( parsers
			, CSCNSection::ePass
			, cuT( "emissive_factor" )
			, emscmp::parserPassEmissive
			, { castor::makeParameter< castor::ParameterType::eFloat >() } );
		castor::addParserT( parsers
			, CSCNSection::ePass
			, cuT( "emissive_colour" )
			, emscmp::parserPassEmissiveColour
			, { castor::makeParameter< castor::ParameterType::eRgbColour >() } );
	}

	void EmissiveComponent::Plugin::zeroBuffer( Pass const & pass
		, shader::PassMaterialShader const & materialShader
		, PassBuffer & buffer )const
	{
		auto data = buffer.getData( pass.getId() );
		VkDeviceSize offset{};
		offset += data.write( materialShader.getMaterialChunk(), castor::RgbColour{}, offset );
		data.write( materialShader.getMaterialChunk(), 0.0f, offset );
	}

	bool EmissiveComponent::Plugin::isComponentNeeded( TextureCombine const & textures
		, ComponentModeFlags const & filter )const
	{
		return checkFlag( filter, ComponentModeFlag::eDiffuseLighting );
	}

	//*********************************************************************************************

	castor::String const EmissiveComponent::TypeName = C3D_MakePassLightingComponentName( "emissive" );

	EmissiveComponent::EmissiveComponent( Pass & pass )
		: BaseDataPassComponentT{ pass, TypeName }
	{
	}

	void EmissiveComponent::accept( PassVisitorBase & vis )
	{
		vis.visit( cuT( "Emissive Colour" ), m_value.colour );
		vis.visit( cuT( "Emissive Factor" ), m_value.factor );
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
		VkDeviceSize offset{};
		offset += data.write( m_materialShader->getMaterialChunk(), getEmissiveColour(), offset );
		data.write( m_materialShader->getMaterialChunk(), getEmissiveFactor(), offset );
	}

	//*********************************************************************************************
}
