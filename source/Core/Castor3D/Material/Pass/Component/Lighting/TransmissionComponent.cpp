#include "Castor3D/Material/Pass/Component/Lighting/TransmissionComponent.hpp"

#include "Castor3D/Material/Pass/Pass.hpp"
#include "Castor3D/Material/Pass/PassVisitor.hpp"
#include "Castor3D/Material/Pass/Component/Other/RefractionComponent.hpp"
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
			return writeNamedSubOpt( file, cuT( "transmission" ), object.getTransmission(), 0.0f );
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
				auto & component = getPassComponent< TransmissionComponent >( parsingContext );
				component.setTransmission( params[0]->get< float >() );
			}
		}
		CU_EndAttribute()
	}

	//*********************************************************************************************

	void TransmissionComponent::ComponentsShader::fillComponents( sdw::type::BaseStruct & components
		, shader::Materials const & materials
		, sdw::StructInstance const * surface )const
	{
		if ( !checkFlag( materials.getFilter(), ComponentModeFlag::eOpacity ) )
		{
			return;
		}

		if ( !components.hasMember( "transmission" ) )
		{
			components.declMember( "transmission", sdw::type::Kind::eFloat );
			components.declMember( "hasTransmission", sdw::type::Kind::eUInt );
		}
	}

	void TransmissionComponent::ComponentsShader::fillComponentsInits( sdw::type::BaseStruct const & components
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
			inits.emplace_back( sdw::makeExpr( material->getMember< sdw::Float >( "transmission" ) ) );
			inits.emplace_back( sdw::makeExpr( material->getMember< sdw::UInt >( "hasTransmission" ) ) );
		}
		else
		{
			inits.emplace_back( sdw::makeExpr( 1.0_f ) );
			inits.emplace_back( sdw::makeExpr( 0_u ) );
		}
	}

	void TransmissionComponent::ComponentsShader::blendComponents( shader::Materials const & materials
		, sdw::Float const & passMultiplier
		, shader::BlendComponents & res
		, shader::BlendComponents const & src )const
	{
		res.getMember< sdw::Float >( "transmission", true ) = src.getMember< sdw::Float >( "transmission", true ) * passMultiplier;
		res.getMember< sdw::Float >( "hasTransmission", true ) = max( res.getMember< sdw::Float >( "hasTransmission", true )
			, src.getMember< sdw::Float >( "hasTransmission", true ) );
	}

	//*********************************************************************************************

	TransmissionComponent::MaterialShader::MaterialShader()
		: shader::PassMaterialShader{ 8u }
	{
	}

	void TransmissionComponent::MaterialShader::fillMaterialType( ast::type::BaseStruct & type
		, sdw::expr::ExprList & inits )const
	{
		if ( !type.hasMember( "transmission" ) )
		{
			type.declMember( "transmission", ast::type::Kind::eFloat );
			type.declMember( "hasTransmission", ast::type::Kind::eUInt );
			inits.emplace_back( sdw::makeExpr( 0.0_f ) );
			inits.emplace_back( sdw::makeExpr( 0_u ) );
		}
	}

	//*********************************************************************************************

	void TransmissionComponent::Plugin::createParsers( castor::AttributeParsers & parsers
		, ChannelFillers & channelFillers )const
	{
		Pass::addParserT( parsers
			, CSCNSection::ePass
			, cuT( "transmission" )
			, trs::parserPassTransmission
			, { castor::makeParameter< castor::ParameterType::eFloat >() } );
	}

	void TransmissionComponent::Plugin::zeroBuffer( Pass const & pass
		, shader::PassMaterialShader const & materialShader
		, PassBuffer & buffer )const
	{
		auto data = buffer.getData( pass.getId() );
		VkDeviceSize offset{};
		offset += data.write( materialShader.getMaterialChunk(), 0.0f, offset );
		data.write( materialShader.getMaterialChunk(), 0u, offset );
	}

	bool TransmissionComponent::Plugin::isComponentNeeded( TextureCombine const & textures
		, ComponentModeFlags const & filter )const
	{
		return checkFlag( filter, ComponentModeFlag::eOpacity );
	}

	//*********************************************************************************************

	castor::String const TransmissionComponent::TypeName = C3D_MakePassLightingComponentName( "transmission" );

	TransmissionComponent::TransmissionComponent( Pass & pass
		, float defaultValue )
		: BaseDataPassComponentT{ pass, TypeName, defaultValue }
	{
		if ( !pass.hasComponent< RefractionComponent >() )
		{
			pass.createComponent< RefractionComponent >( 1.0f );
		}
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
		VkDeviceSize offset{};
		offset += data.write( m_materialShader->getMaterialChunk()
			, getTransmission()
			, offset );
		data.write( m_materialShader->getMaterialChunk()
			, 1u
			, offset );
	}

	//*********************************************************************************************
}
