#include "Castor3D/Material/Pass/Component/Lighting/TransmissionComponent.hpp"

#include "Castor3D/Material/Pass/Pass.hpp"
#include "Castor3D/Miscellaneous/ConfigurationVisitor.hpp"
#include "Castor3D/Material/Pass/Component/Other/RefractionComponent.hpp"
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
	class TextWriter< TransmissionComponent >
		: public TextWriterT< TransmissionComponent >
	{
	public:
		explicit TextWriter( String const & tabs )
			: TextWriterT< TransmissionComponent >{ tabs }
		{
		}

		bool operator()( TransmissionComponent const & object
			, StringStream & file )override
		{
			return writeNamedSubOpt( file, cuT( "transmission" ), object.getTransmission(), TransmissionComponent::Default );
		}
	};

	//*********************************************************************************************

	namespace trs
	{
		static CU_ImplementAttributeParserBlock( parserPassTransmission, PassContext )
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
				auto & component = getPassComponent< TransmissionComponent >( *blockContext );
				component.setTransmission( params[0]->get< float >() );
			}
		}
		CU_EndAttribute()
	}

	//*********************************************************************************************

	TransmissionComponent::MaterialShader::MaterialShader()
		: shader::PassMaterialShader{ 4u }
	{
	}

	void TransmissionComponent::MaterialShader::fillMaterialType( ast::type::BaseStruct & type
		, sdw::expr::ExprList & inits )const
	{
		if ( !type.hasMember( "transmissionFactor" ) )
		{
			type.declMember( "transmissionFactor", ast::type::Kind::eFloat );
			inits.emplace_back( sdw::makeExpr( sdw::Float{ TransmissionComponent::Default } ) );
		}
	}

	//*********************************************************************************************

	void TransmissionComponent::ComponentsShader::fillComponents( ComponentModeFlags componentsMask
		, sdw::type::BaseStruct & components
		, shader::Materials const & materials
		, sdw::StructInstance const * surface )const
	{
		if ( !checkFlag( componentsMask, ComponentModeFlag::eOpacity )
			|| !checkFlag( materials.getFilter(), ComponentModeFlag::eOpacity ) )
		{
			return;
		}

		if ( !components.hasMember( "transmissionFactor" ) )
		{
			components.declMember( "transmissionFactor", sdw::type::Kind::eFloat );
		}
	}

	void TransmissionComponent::ComponentsShader::fillComponentsInits( sdw::type::BaseStruct const & components
		, shader::Materials const & materials
		, shader::Material const * material
		, sdw::StructInstance const * surface
		, sdw::Vec4 const * clrCot
		, sdw::expr::ExprList & inits )const
	{
		if ( !components.hasMember( "transmissionFactor" ) )
		{
			return;
		}

		if ( material )
		{
			inits.emplace_back( sdw::makeExpr( material->getMember< sdw::Float >( "transmissionFactor" ) ) );
		}
		else
		{
			inits.emplace_back( sdw::makeExpr( sdw::Float{ TransmissionComponent::Default } ) );
		}
	}

	void TransmissionComponent::ComponentsShader::blendComponents( shader::Materials const & materials
		, sdw::Float const & passMultiplier
		, shader::BlendComponents & res
		, shader::BlendComponents const & src )const
	{
		if ( res.hasMember( "transmissionFactor" ) )
		{
			res.getMember< sdw::Float >( "transmissionFactor" ) += src.getMember< sdw::Float >( "transmissionFactor", true ) * passMultiplier;
		}
	}

	//*********************************************************************************************

	void TransmissionComponent::Plugin::createParsers( AttributeParsers & parsers
		, ChannelFillers & channelFillers )const
	{
		c3d::addParserT( parsers
			, CSCNSection::ePass
			, cuT( "transmission" )
			, trs::parserPassTransmission
			, { makeParameter< ParameterType::eFloat >() } );
	}

	void TransmissionComponent::Plugin::zeroBuffer( Pass const & pass
		, shader::PassMaterialShader const & materialShader
		, PassBuffer & buffer )const
	{
		auto data = buffer.getData( pass.getId() );
		data.write( materialShader.getMaterialChunk(), TransmissionComponent::Default, 0u );
	}

	bool TransmissionComponent::Plugin::isComponentNeeded( TextureCombine const & textures
		, ComponentModeFlags const & filter )const
	{
		return checkFlag( filter, ComponentModeFlag::eOpacity );
	}

	//*********************************************************************************************

	String const TransmissionComponent::TypeName = C3D_MakePassLightingComponentName( "transmission" );

	TransmissionComponent::TransmissionComponent( Pass & pass
		, float defaultValue )
		: BaseDataPassComponentT{ pass, TypeName, { RefractionComponent::TypeName }
			, defaultValue }
	{
	}

	void TransmissionComponent::accept( ConfigurationVisitorBase & vis )
	{
		vis.visit( cuT( "Transmission" ) );
		vis.visit( cuT( "Factor" ), m_value );
	}

	PassComponentUPtr TransmissionComponent::doClone( Pass & pass )const
	{
		auto result = makeRawUnique< TransmissionComponent >( pass );
		result->setData( getData() );
		return PassComponentUPtr{ result.release() };
	}

	bool TransmissionComponent::doWriteText( String const & tabs
		, Path const & folder
		, String const & subfolder
		, StringStream & file )const
	{
		return TextWriter< TransmissionComponent >{ tabs }( *this, file );
	}

	void TransmissionComponent::doFillBuffer( PassBuffer & buffer )const
	{
		auto data = buffer.getData( getOwner()->getId() );
		data.write( m_materialShader->getMaterialChunk(), getTransmission(), 0u );
	}

	//*********************************************************************************************
}
