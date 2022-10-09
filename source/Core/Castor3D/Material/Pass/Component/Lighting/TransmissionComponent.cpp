#include "Castor3D/Material/Pass/Component/Lighting/TransmissionComponent.hpp"

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
	class TextWriter< castor3d::TransmissionComponent >
		: public TextWriterT< castor3d::TransmissionComponent >
	{
	public:
		explicit TextWriter( String const & tabs )
			: TextWriterT< castor3d::TransmissionComponent >{ tabs }
		{
		}

		bool operator()( castor3d::TransmissionComponent const & object
			, StringStream & file )override
		{
			return writeNamedSubOpt( file, cuT( "transmission" ), object.getTransmission(), castor::RgbColour{ 1.0f, 1.0f, 1.0f } );
		}
	};
}

namespace castor3d
{
	//*********************************************************************************************

	namespace trs
	{
		static CU_ImplementAttributeParser( parserPassTransmission )
		{
			auto & parsingContext = getParserContext( context );

			if ( !parsingContext.pass )
			{
				CU_ParsingError( cuT( "No Pass initialised." ) );
			}
			else if ( params.empty() )
			{
				CU_ParsingError( cuT( "Missing parameter." ) );
			}
			else
			{
				castor::RgbColour value{ 1.0f, 1.0f, 1.0f };
				params[0]->get( value );
				auto & component = getPassComponent< TransmissionComponent >( parsingContext );
				component.setTransmission( value );
			}
		}
		CU_EndAttribute()
	}

	//*********************************************************************************************

	void TransmissionComponent::ComponentsShader::fillComponents( sdw::type::BaseStruct & components
		, shader::Materials const & materials
		, shader::Material const * material
		, sdw::StructInstance const * surface )const
	{
		if ( !checkFlag( materials.getFilter(), ComponentModeFlag::eDiffuseLighting )
			&& !checkFlag( materials.getFilter(), ComponentModeFlag::eSpecularLighting ) )
		{
			return;
		}

		if ( !components.hasMember( "transmission" ) )
		{
			components.declMember( "transmission", sdw::type::Kind::eVec3F );
		}
	}

	void TransmissionComponent::ComponentsShader::fillComponentsInits( sdw::type::BaseStruct & components
		, shader::Materials const & materials
		, shader::Material const * material
		, sdw::StructInstance const * surface
		, sdw::expr::ExprList & inits )const
	{
		if ( !components.hasMember( "transmission" ) )
		{
			return;
		}

		if ( material )
		{
			inits.emplace_back( sdw::makeExpr( material->getMember< sdw::Vec3 >( "transmission" ) ) );
		}
		else
		{
			inits.emplace_back( sdw::makeExpr( vec3( 1.0_f ) ) );
		}
	}

	void TransmissionComponent::ComponentsShader::blendComponents( shader::Materials const & materials
		, sdw::Float const & passMultiplier
		, shader::BlendComponents const & res
		, shader::BlendComponents const & src )const
	{
		res.getMember< sdw::Vec3 >( "transmission", true ) = src.getMember< sdw::Vec3 >( "transmission", true ) * passMultiplier;
	}

	//*********************************************************************************************

	TransmissionComponent::MaterialShader::MaterialShader()
		: shader::PassMaterialShader{ MemChunk{ 0u, 12u, 12u } }
	{
	}

	void TransmissionComponent::MaterialShader::fillMaterialType( ast::type::BaseStruct & type
		, sdw::expr::ExprList & inits )const
	{
		if ( !type.hasMember( "transmission" ) )
		{
			type.declMember( "transmission", ast::type::Kind::eVec3F );
			inits.emplace_back( sdw::makeExpr( vec3( 1.0_f ) ) );
		}
	}

	//*********************************************************************************************

	castor::String const TransmissionComponent::TypeName = C3D_MakePassComponentName( "transmission" );

	TransmissionComponent::TransmissionComponent( Pass & pass
		, castor::RgbColour defaultValue )
		: BaseDataPassComponentT{ pass, TypeName, std::move( defaultValue ) }
	{
	}

	void TransmissionComponent::createParsers( castor::AttributeParsers & parsers
		, ChannelFillers & channelFillers )
	{
		Pass::addParserT( parsers
			, CSCNSection::ePass
			, cuT( "transmission" )
			, trs::parserPassTransmission
			, { castor::makeParameter< castor::ParameterType::eRgbColour >() } );
	}

	void TransmissionComponent::zeroBuffer( Pass const & pass
		, shader::PassMaterialShader const & materialShader
		, PassBuffer & buffer )
	{
		auto data = buffer.getData( pass.getId() );
		data.write( materialShader.getMaterialChunk(), 1.0f, 1.0f, 1.0f, 0u );
	}

	bool TransmissionComponent::isComponentNeeded( TextureFlags const & textures
		, ComponentModeFlags const & filter )
	{
		return checkFlag( filter, ComponentModeFlag::eDiffuseLighting )
			|| checkFlag( filter, ComponentModeFlag::eSpecularLighting );
	}

	void TransmissionComponent::accept( PassVisitorBase & vis )
	{
		vis.visit( cuT( "Transmission" ), m_value );
	}

	PassComponentUPtr TransmissionComponent::doClone( Pass & pass )const
	{
		auto result = std::make_unique< TransmissionComponent >( pass );
		result->setData( getData() );
		return result;
	}

	bool TransmissionComponent::doWriteText( castor::String const & tabs
		, castor::Path const & folder
		, castor::String const & subfolder
		, castor::StringStream & file )const
	{
		return castor::TextWriter< TransmissionComponent >{ tabs }( *this, file );
	}

	void TransmissionComponent::doFillBuffer( PassBuffer & buffer )const
	{
		auto data = buffer.getData( getOwner()->getId() );
		data.write( m_materialShader->getMaterialChunk()
			, powf( getTransmission().red(), 2.2f )
			, powf( getTransmission().green(), 2.2f )
			, powf( getTransmission().blue(), 2.2f )
			, 0u );
	}

	//*********************************************************************************************
}
