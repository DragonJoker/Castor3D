#include "Castor3D/Material/Pass/Component/Lighting/TransmittanceComponent.hpp"

#include "Castor3D/Engine.hpp"
#include "Castor3D/Material/Pass/Pass.hpp"
#include "Castor3D/Miscellaneous/ConfigurationVisitor.hpp"
#include "Castor3D/Material/Pass/Component/PassComponentRegister.hpp"
#include "Castor3D/Material/Texture/TextureConfiguration.hpp"
#include "Castor3D/Scene/SceneFileParserData.hpp"
#include "Castor3D/Shader/ShaderBuffers/PassBuffer.hpp"
#include "Castor3D/Shader/Shaders/GlslBlendComponents.hpp"
#include "Castor3D/Shader/Shaders/GlslLighting.hpp"
#include "Castor3D/Shader/Shaders/GlslMaterial.hpp"
#include "Castor3D/Shader/Shaders/GlslTextureConfiguration.hpp"

#include <CastorUtils/FileParser/ParserParameter.hpp>

CU_ImplementSmartPtr( castor3d, TransmittanceComponent )

namespace castor
{
	template<>
	class TextWriter< castor3d::TransmittanceComponent >
		: public TextWriterT< castor3d::TransmittanceComponent >
	{
	public:
		explicit TextWriter( String const & tabs )
			: TextWriterT< castor3d::TransmittanceComponent >{ tabs }
		{
		}

		bool operator()( castor3d::TransmittanceComponent const & object
			, StringStream & file )override
		{
			return writeOpt( file, cuT( "transmittance" ), object.getTransmittance(), 1.0f );
		}
	};
}

namespace castor3d
{
	//*********************************************************************************************

	namespace trscmp
	{
		static CU_ImplementAttributeParserBlock( parserTransmittance, PassContext )
		{
			if ( !blockContext->pass )
			{
				CU_ParsingError( cuT( "No Pass initialised." ) );
			}
			else if ( params.empty() )
			{
				CU_ParsingError( cuT( "Missing parameter." ) );
			}
			else
			{
				auto & component = getPassComponent< TransmittanceComponent >( *blockContext );
				component.setTransmittance( params[0]->get< float >() );
			}
		}
		CU_EndAttribute()
	}

	//*********************************************************************************************

	void TransmittanceComponent::ComponentsShader::fillComponents( ComponentModeFlags componentsMask
		, sdw::type::BaseStruct & components
		, shader::Materials const & materials
		, sdw::StructInstance const * surface )const
	{
		if ( ( !checkFlag( componentsMask, ComponentModeFlag::eDiffuseLighting )
				&& !checkFlag( componentsMask, ComponentModeFlag::eSpecularLighting ) )
			|| ( !checkFlag( materials.getFilter(), ComponentModeFlag::eDiffuseLighting )
				&& !checkFlag( materials.getFilter(), ComponentModeFlag::eSpecularLighting ) ) )
		{
			return;
		}

		if ( !components.hasMember( "transmittance" ) )
		{
			components.declMember( "transmittance", sdw::type::Kind::eFloat );
		}
	}

	void TransmittanceComponent::ComponentsShader::fillComponentsInits( sdw::type::BaseStruct const & components
		, shader::Materials const & materials
		, shader::Material const * material
		, sdw::StructInstance const * surface
		, sdw::Vec4 const * clrCot
		, sdw::expr::ExprList & inits )const
	{
		if ( !components.hasMember( "transmittance" ) )
		{
			return;
		}

		if ( material )
		{
			inits.emplace_back( sdw::makeExpr( material->getMember< sdw::Float >( "transmittance" ) ) );
		}
		else
		{
			inits.emplace_back( sdw::makeExpr( 1.0_f ) );
		}
	}

	void TransmittanceComponent::ComponentsShader::blendComponents( shader::Materials const & materials
		, sdw::Float const & passMultiplier
		, shader::BlendComponents & res
		, shader::BlendComponents const & src )const
	{
		if ( res.hasMember( "transmittance" ) )
		{
			res.getMember< sdw::Float >( "transmittance" ) = src.getMember< sdw::Float >( "transmittance" ) * passMultiplier;
		}
	}

	//*********************************************************************************************

	TransmittanceComponent::MaterialShader::MaterialShader()
		: shader::PassMaterialShader{ sizeof( float ) }
	{
	}

	void TransmittanceComponent::MaterialShader::fillMaterialType( sdw::type::BaseStruct & type
		, sdw::expr::ExprList & inits )const
	{
		if ( !type.hasMember( "transmittance" ) )
		{
			type.declMember( "transmittance", ast::type::Kind::eFloat );
			inits.emplace_back( sdw::makeExpr( 1.0_f ) );
		}
	}

	//*********************************************************************************************

	void TransmittanceComponent::Plugin::createParsers( castor::AttributeParsers & parsers
		, ChannelFillers & channelFillers )const
	{
		castor::addParserT( parsers
			, CSCNSection::ePass
			, cuT( "transmittance" )
			, trscmp::parserTransmittance
			, { castor::makeParameter< castor::ParameterType::eFloat >() } );
	}

	void TransmittanceComponent::Plugin::zeroBuffer( Pass const & pass
		, shader::PassMaterialShader const & materialShader
		, PassBuffer & buffer )const
	{
		auto data = buffer.getData( pass.getId() );
		data.write( materialShader.getMaterialChunk(), 1.0f, 0u );
	}

	bool TransmittanceComponent::Plugin::isComponentNeeded( TextureCombine const & textures
		, ComponentModeFlags const & filter )const
	{
		return checkFlag( filter, ComponentModeFlag::eDiffuseLighting )
			|| checkFlag( filter, ComponentModeFlag::eSpecularLighting );
	}

	//*********************************************************************************************

	castor::String const TransmittanceComponent::TypeName = C3D_MakePassLightingComponentName( "transmittance" );

	TransmittanceComponent::TransmittanceComponent( Pass & pass )
		: BaseDataPassComponentT{ pass, TypeName }
	{
	}

	void TransmittanceComponent::accept( ConfigurationVisitorBase & vis )
	{
		vis.visit( cuT( "Transmittance" ) );
		vis.visit( cuT( "Factor" ), m_value );
	}

	PassComponentUPtr TransmittanceComponent::doClone( Pass & pass )const
	{
		auto result = castor::makeUnique< TransmittanceComponent >( pass );
		result->setData( getData() );
		return castor::ptrRefCast< PassComponent >( result );
	}

	bool TransmittanceComponent::doWriteText( castor::String const & tabs
		, castor::Path const & folder
		, castor::String const & subfolder
		, castor::StringStream & file )const
	{
		return castor::TextWriter< TransmittanceComponent >{ tabs }( *this, file );
	}

	void TransmittanceComponent::doFillBuffer( PassBuffer & buffer )const
	{
		auto data = buffer.getData( getOwner()->getId() );
		data.write( m_materialShader->getMaterialChunk()
			, getTransmittance()
			, 0u );
	}

	//*********************************************************************************************
}
