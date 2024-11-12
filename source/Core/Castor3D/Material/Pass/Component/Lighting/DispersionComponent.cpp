#include "Castor3D/Material/Pass/Component/Lighting/DispersionComponent.hpp"

#include "Castor3D/Material/Pass/Pass.hpp"
#include "Castor3D/Miscellaneous/ConfigurationVisitor.hpp"
#include "Castor3D/Material/Pass/Component/Lighting/TransmissionComponent.hpp"
#include "Castor3D/Scene/SceneFileParserData.hpp"
#include "Castor3D/Shader/ShaderBuffers/PassBuffer.hpp"
#include "Castor3D/Shader/Shaders/GlslBlendComponents.hpp"
#include "Castor3D/Shader/Shaders/GlslLighting.hpp"
#include "Castor3D/Shader/Shaders/GlslMaterial.hpp"
#include "Castor3D/Shader/Shaders/GlslSurface.hpp"

#include <CastorUtils/FileParser/FileParser.hpp>

namespace castor
{
	template<>
	class TextWriter< castor3d::DispersionComponent >
		: public TextWriterT< castor3d::DispersionComponent >
	{
	public:
		explicit TextWriter( String const & tabs )
			: TextWriterT< castor3d::DispersionComponent >{ tabs }
		{
		}

		bool operator()( castor3d::DispersionComponent const & object
			, StringStream & file )override
		{
			return writeNamedSubOpt( file, cuT( "dispersion" ), object.getDispersion(), castor3d::DispersionComponent::Default );
		}
	};
}

namespace castor3d
{
	//*********************************************************************************************

	namespace trsatt
	{
		static CU_ImplementAttributeParserBlock( parserPassDispersion, PassContext )
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
				auto & component = getPassComponent< DispersionComponent >( *blockContext );
				component.setDispersion( params[0]->get< float >() );
			}
		}
		CU_EndAttribute()
	}

	//*********************************************************************************************

	void DispersionComponent::ComponentsShader::fillComponents( ComponentModeFlags componentsMask
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

		if ( !components.hasMember( "dispersion" ) )
		{
			components.declMember( "dispersion", sdw::type::Kind::eFloat );
		}
	}

	void DispersionComponent::ComponentsShader::fillComponentsInits( sdw::type::BaseStruct const & components
		, shader::Materials const & materials
		, shader::Material const * material
		, sdw::StructInstance const * surface
		, sdw::Vec4 const * clrCot
		, sdw::expr::ExprList & inits )const
	{
		if ( !components.hasMember( "dispersion" ) )
		{
			return;
		}

		if ( material )
		{
			inits.emplace_back( sdw::makeExpr( material->getMember< sdw::Float >( "dispersion" ) ) );
		}
		else
		{
			inits.emplace_back( sdw::makeExpr( sdw::Float{ DispersionComponent::Default } ) );
		}
	}

	void DispersionComponent::ComponentsShader::blendComponents( shader::Materials const & materials
		, sdw::Float const & passMultiplier
		, shader::BlendComponents & res
		, shader::BlendComponents const & src )const
	{
		if ( !res.hasMember( "dispersion" ) )
		{
			return;
		}

		res.getMember< sdw::Float >( "dispersion", true ) += src.getMember< sdw::Float >( "dispersion", true ) * passMultiplier;
	}

	//*********************************************************************************************

	DispersionComponent::MaterialShader::MaterialShader()
		: shader::PassMaterialShader{ 4u }
	{
	}

	void DispersionComponent::MaterialShader::fillMaterialType( ast::type::BaseStruct & type
		, sdw::expr::ExprList & inits )const
	{
		if ( !type.hasMember( "dispersion" ) )
		{
			type.declMember( "dispersion", ast::type::Kind::eFloat );
			inits.emplace_back( sdw::makeExpr( sdw::Float{ DispersionComponent::Default } ) );
		}
	}

	//*********************************************************************************************

	void DispersionComponent::Plugin::createParsers( castor::AttributeParsers & parsers
		, ChannelFillers & channelFillers )const
	{
		castor::addParserT( parsers
			, CSCNSection::ePass
			, cuT( "dispersion" )
			, trsatt::parserPassDispersion
			, { castor::makeParameter< castor::ParameterType::eFloat >() } );
	}

	void DispersionComponent::Plugin::zeroBuffer( Pass const & pass
		, shader::PassMaterialShader const & materialShader
		, PassBuffer & buffer )const
	{
		auto data = buffer.getData( pass.getId() );
		data.write( materialShader.getMaterialChunk(), DispersionComponent::Default, 0u );
	}

	bool DispersionComponent::Plugin::isComponentNeeded( TextureCombine const & textures
		, ComponentModeFlags const & filter )const
	{
		return checkFlag( filter, ComponentModeFlag::eDiffuseLighting )
			|| checkFlag( filter, ComponentModeFlag::eSpecularLighting );
	}

	//*********************************************************************************************

	castor::String const DispersionComponent::TypeName = C3D_MakePassLightingComponentName( "dispersion" );

	DispersionComponent::DispersionComponent( Pass & pass )
		: BaseDataPassComponentT{ pass, TypeName, { TransmissionComponent::TypeName }
			, DispersionComponent::Default }
	{
	}

	void DispersionComponent::accept( ConfigurationVisitorBase & vis )
	{
		vis.visit( cuT( "Dispersion" ) );
		vis.visit( cuT( "Value" ), m_value );
	}

	PassComponentUPtr DispersionComponent::doClone( Pass & pass )const
	{
		auto result = castor::make_unique< DispersionComponent >( pass );
		result->setData( getData() );
		return PassComponentUPtr{ result.release() };
	}

	bool DispersionComponent::doWriteText( castor::String const & tabs
		, castor::Path const & folder
		, castor::String const & subfolder
		, castor::StringStream & file )const
	{
		return castor::TextWriter< DispersionComponent >{ tabs }( *this, file );
	}

	void DispersionComponent::doFillBuffer( PassBuffer & buffer )const
	{
		auto data = buffer.getData( getOwner()->getId() );
		data.write( m_materialShader->getMaterialChunk(), getDispersion(), 0u );
	}

	//*********************************************************************************************
}
