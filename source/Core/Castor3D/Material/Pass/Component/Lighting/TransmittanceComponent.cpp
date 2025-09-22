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

#include <CastorUtils/FileParser/FileParser.hpp>

namespace c3d
{
	template<>
	class TextWriter< TransmittanceComponent >
		: public TextWriterT< TransmittanceComponent >
	{
	public:
		explicit TextWriter( String const & tabs )
			: TextWriterT< TransmittanceComponent >{ tabs }
		{
		}

		bool operator()( TransmittanceComponent const & object
			, StringStream & file )override
		{
			return writeOpt( file, cuT( "transmittance" ), object.getTransmittance(), TransmittanceComponent::Default );
		}
	};

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
			inits.emplace_back( sdw::makeExpr( sdw::Float{ TransmittanceComponent::Default } ) );
		}
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
			inits.emplace_back( sdw::makeExpr( sdw::Float{ TransmittanceComponent::Default } ) );
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

	void TransmittanceComponent::Plugin::createParsers( AttributeParsers & parsers
		, ChannelFillers & channelFillers )const
	{
		c3d::addParserT( parsers
			, CSCNSection::ePass
			, cuT( "transmittance" )
			, trscmp::parserTransmittance
			, { makeParameter< ParameterType::eFloat >() } );
	}

	void TransmittanceComponent::Plugin::zeroBuffer( Pass const & pass
		, shader::PassMaterialShader const & materialShader
		, PassBuffer & buffer )const
	{
		auto data = buffer.getData( pass.getId() );
		data.write( materialShader.getMaterialChunk(), TransmittanceComponent::Default, 0u );
	}

	bool TransmittanceComponent::Plugin::isComponentNeeded( TextureCombine const & textures
		, ComponentModeFlags const & filter )const
	{
		return checkFlag( filter, ComponentModeFlag::eDiffuseLighting )
			|| checkFlag( filter, ComponentModeFlag::eSpecularLighting );
	}

	//*********************************************************************************************

	String const TransmittanceComponent::TypeName = C3D_MakePassLightingComponentName( "transmittance" );

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
		auto result = makeRawUnique< TransmittanceComponent >( pass );
		result->setData( getData() );
		return passComponentCast( result );
	}

	bool TransmittanceComponent::doWriteText( String const & tabs
		, Path const & folder
		, String const & subfolder
		, StringStream & file )const
	{
		return TextWriter< TransmittanceComponent >{ tabs }( *this, file );
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
