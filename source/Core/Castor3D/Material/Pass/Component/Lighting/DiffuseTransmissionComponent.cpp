#include "Castor3D/Material/Pass/Component/Lighting/DiffuseTransmissionComponent.hpp"

#include "Castor3D/Material/Pass/Pass.hpp"
#include "Castor3D/Miscellaneous/ConfigurationVisitor.hpp"
#include "Castor3D/Material/Pass/Component/Other/RefractionComponent.hpp"
#include "Castor3D/Scene/SceneFileParserData.hpp"
#include "Castor3D/Shader/ShaderBuffers/PassBuffer.hpp"
#include "Castor3D/Shader/Shaders/GlslBlendComponents.hpp"
#include "Castor3D/Shader/Shaders/GlslLighting.hpp"
#include "Castor3D/Shader/Shaders/GlslMaterial.hpp"
#include "Castor3D/Shader/Shaders/GlslSurface.hpp"
#include "Castor3D/Shader/Ubos/ModelDataUbo.hpp"

#include <CastorUtils/FileParser/FileParser.hpp>
#include <CastorUtils/Data/Text/TextRgbColour.hpp>

namespace castor
{
	template<>
	class TextWriter< castor3d::DiffuseTransmissionComponent >
		: public TextWriterT< castor3d::DiffuseTransmissionComponent >
	{
	public:
		explicit TextWriter( String const & tabs )
			: TextWriterT< castor3d::DiffuseTransmissionComponent >{ tabs }
		{
		}

		bool operator()( castor3d::DiffuseTransmissionComponent const & object
			, StringStream & file )override
		{
			return writeNamedSubOpt( file, cuT( "diffuse_transmission_factor" ), object.getTransmissionFactor(), castor3d::DiffuseTransmissionComponent::DefaultFactor )
				&& writeNamedSubOpt( file, cuT( "diffuse_transmission_colour" ), object.getTransmissionColour(), castor3d::DiffuseTransmissionComponent::DefaultColour );
		}
	};
}

namespace castor3d
{
	//*********************************************************************************************

	namespace trs
	{
		static CU_ImplementAttributeParserBlock( parserPassTransmissionFactor, PassContext )
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
				auto & component = getPassComponent< DiffuseTransmissionComponent >( *blockContext );
				component.setTransmissionFactor( params[0]->get< float >() );
			}
		}
		CU_EndAttribute()

		static CU_ImplementAttributeParserBlock( parserPassTransmissionColour, PassContext )
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
				auto & component = getPassComponent< DiffuseTransmissionComponent >( *blockContext );
				component.setTransmissionColour( params[0]->get< castor::RgbColour >() );
			}
		}
		CU_EndAttribute()
	}

	//*********************************************************************************************

	DiffuseTransmissionComponent::MaterialShader::MaterialShader()
		: shader::PassMaterialShader{ 16u }
	{
	}

	void DiffuseTransmissionComponent::MaterialShader::fillMaterialType( ast::type::BaseStruct & type
		, sdw::expr::ExprList & inits )const
	{
		if ( !type.hasMember( "diffuseTransmissionFactor" ) )
		{
			type.declMember( "diffuseTransmissionColour", ast::type::Kind::eVec3F );
			type.declMember( "diffuseTransmissionFactor", ast::type::Kind::eFloat );
			inits.emplace_back( sdw::makeExpr( vec3( sdw::Float{ DiffuseTransmissionComponent::DefaultComponent } ) ) );
			inits.emplace_back( sdw::makeExpr( sdw::Float{ DiffuseTransmissionComponent::DefaultFactor } ) );
		}
	}

	//*********************************************************************************************

	void DiffuseTransmissionComponent::ComponentsShader::fillComponents( ComponentModeFlags componentsMask
		, sdw::type::BaseStruct & components
		, shader::Materials const & materials
		, sdw::StructInstance const * surface )const
	{
		if ( !checkFlag( componentsMask, ComponentModeFlag::eOpacity )
			|| !checkFlag( materials.getFilter(), ComponentModeFlag::eOpacity )
			|| !checkFlag( componentsMask, ComponentModeFlag::eDiffuseLighting )
			|| !checkFlag( materials.getFilter(), ComponentModeFlag::eDiffuseLighting ) )
		{
			return;
		}

		if ( !components.hasMember( "diffuseTransmissionFactor" ) )
		{
			components.declMember( "diffuseTransmissionColour", sdw::type::Kind::eVec3F );
			components.declMember( "diffuseTransmissionFactor", sdw::type::Kind::eFloat );
		}
	}

	void DiffuseTransmissionComponent::ComponentsShader::fillComponentsInits( sdw::type::BaseStruct const & components
		, shader::Materials const & materials
		, shader::Material const * material
		, sdw::StructInstance const * surface
		, sdw::Vec4 const * clrCot
		, sdw::expr::ExprList & inits )const
	{
		if ( !components.hasMember( "diffuseTransmissionFactor" ) )
		{
			return;
		}

		if ( material )
		{
			inits.emplace_back( sdw::makeExpr( material->getMember< sdw::Vec3 >( "diffuseTransmissionColour" ) ) );
			inits.emplace_back( sdw::makeExpr( material->getMember< sdw::Float >( "diffuseTransmissionFactor" ) ) );
		}
		else
		{
			inits.emplace_back( sdw::makeExpr( vec3( sdw::Float{ DiffuseTransmissionComponent::DefaultComponent } ) ) );
			inits.emplace_back( sdw::makeExpr( sdw::Float{ DiffuseTransmissionComponent::DefaultFactor } ) );
		}
	}

	void DiffuseTransmissionComponent::ComponentsShader::blendComponents( shader::Materials const & materials
		, sdw::Float const & passMultiplier
		, shader::BlendComponents & res
		, shader::BlendComponents const & src )const
	{
		if ( res.hasMember( "diffuseTransmissionFactor" ) )
		{
			res.diffuseTransmissionColour += src.diffuseTransmissionColour * passMultiplier;
			res.diffuseTransmissionFactor += src.diffuseTransmissionFactor * passMultiplier;
		}
	}

	//*********************************************************************************************

	void DiffuseTransmissionComponent::Plugin::createParsers( castor::AttributeParsers & parsers
		, ChannelFillers & channelFillers )const
	{
		castor::addParserT( parsers
			, CSCNSection::ePass
			, cuT( "diffuse_transmission_factor" )
			, trs::parserPassTransmissionFactor
			, { castor::makeParameter< castor::ParameterType::eFloat >() } );
		castor::addParserT( parsers
			, CSCNSection::ePass
			, cuT( "diffuse_transmission_colour" )
			, trs::parserPassTransmissionColour
			, { castor::makeParameter< castor::ParameterType::eRgbColour >() } );
	}

	void DiffuseTransmissionComponent::Plugin::zeroBuffer( Pass const & pass
		, shader::PassMaterialShader const & materialShader
		, PassBuffer & buffer )const
	{
		auto data = buffer.getData( pass.getId() );
		VkDeviceSize offset{};
		offset += data.write( materialShader.getMaterialChunk(), DiffuseTransmissionComponent::DefaultColour, offset );
		data.write( materialShader.getMaterialChunk(), DiffuseTransmissionComponent::DefaultFactor, offset );
	}

	bool DiffuseTransmissionComponent::Plugin::isComponentNeeded( TextureCombine const & textures
		, ComponentModeFlags const & filter )const
	{
		return checkFlag( filter, ComponentModeFlag::eOpacity );
	}

	//*********************************************************************************************

	castor::String const DiffuseTransmissionComponent::TypeName = C3D_MakePassLightingComponentName( "diffuse_transmission" );

	DiffuseTransmissionComponent::DiffuseTransmissionComponent( Pass & pass )
		: BaseDataPassComponentT{ pass, TypeName, { RefractionComponent::TypeName }
			, DiffuseTransmissionComponent::DefaultColour, DiffuseTransmissionComponent::DefaultFactor }
	{
	}

	void DiffuseTransmissionComponent::accept( ConfigurationVisitorBase & vis )
	{
		vis.visit( cuT( "DiffuseTransmission" ) );
		vis.visit( cuT( "Colour" ), m_value.colour );
		vis.visit( cuT( "Factor" ), m_value.factor );
	}

	PassComponentUPtr DiffuseTransmissionComponent::doClone( Pass & pass )const
	{
		auto result = std::make_unique< DiffuseTransmissionComponent >( pass );
		result->setData( getData() );
		return PassComponentUPtr{ result.release() };
	}

	bool DiffuseTransmissionComponent::doWriteText( castor::String const & tabs
		, castor::Path const & folder
		, castor::String const & subfolder
		, castor::StringStream & file )const
	{
		return castor::TextWriter< DiffuseTransmissionComponent >{ tabs }( *this, file );
	}

	void DiffuseTransmissionComponent::doFillBuffer( PassBuffer & buffer )const
	{
		auto data = buffer.getData( getOwner()->getId() );
		VkDeviceSize offset{};
		offset += data.write( m_materialShader->getMaterialChunk(), getTransmissionColour(), offset );
		data.write( m_materialShader->getMaterialChunk(), getTransmissionFactor(), offset );
	}

	//*********************************************************************************************
}
