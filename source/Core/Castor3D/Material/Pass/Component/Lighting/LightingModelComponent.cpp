#include "Castor3D/Material/Pass/Component/Lighting/LightingModelComponent.hpp"

#include "Castor3D/Engine.hpp"
#include "Castor3D/Material/Pass/Pass.hpp"
#include "Castor3D/Material/Pass/PassFactory.hpp"
#include "Castor3D/Miscellaneous/ConfigurationVisitor.hpp"
#include "Castor3D/Material/Pass/PbrPass.hpp"
#include "Castor3D/Material/Pass/PhongPass.hpp"
#include "Castor3D/Material/Pass/Component/PassComponentRegister.hpp"
#include "Castor3D/Miscellaneous/Logger.hpp"
#include "Castor3D/Scene/SceneFileParser.hpp"
#include "Castor3D/Shader/LightingModelFactory.hpp"
#include "Castor3D/Shader/ShaderBuffers/PassBuffer.hpp"
#include "Castor3D/Shader/Shaders/GlslBlendComponents.hpp"
#include "Castor3D/Shader/Shaders/GlslLighting.hpp"
#include "Castor3D/Shader/Shaders/GlslMaterial.hpp"

#include <CastorUtils/FileParser/ParserParameter.hpp>

CU_ImplementSmartPtr( castor3d, LightingModelComponent )

namespace castor
{
	template<>
	class TextWriter< castor3d::LightingModelComponent >
		: public TextWriterT< castor3d::LightingModelComponent >
	{
	public:
		explicit TextWriter( String const & tabs )
			: TextWriterT< castor3d::LightingModelComponent >{ tabs }
		{
		}

		bool operator()( castor3d::LightingModelComponent const & object
			, StringStream & file )override
		{
			return write( file, cuT( "lighting_model" ), object.getLightingModelName() );
		}
	};
}

namespace castor3d
{
	//*********************************************************************************************

	namespace lgtmdl
	{
		static CU_ImplementAttributeParserBlock( parserRootMaterials, RootContext )
		{
			if ( params.empty() )
			{
				CU_ParsingError( cuT( "Missing parameter." ) );
			}
			else if ( !params.empty() )
			{
				auto name = LightingModelFactory::normaliseName( params[0]->get< castor::String >() );
				auto & engine = *blockContext->engine;
				engine.setDefaultLightingModel( engine.getLightingModelFactory().getNameId( name ) );
			}
		}
		CU_EndAttribute()

		static CU_ImplementAttributeParserBlock( parserPassLightingModel, PassContext )
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
				auto name = LightingModelFactory::normaliseName( params[0]->get< castor::String >() );
				auto & engine = *blockContext->root->engine;
				auto & component = getPassComponent< LightingModelComponent >( *blockContext );
				component.setLightingModelId( engine.getLightingModelFactory().getNameId( name ) );
			}
		}
		CU_EndAttribute()
	}

	//*********************************************************************************************

	LightingModelComponent::MaterialShader::MaterialShader()
		: shader::PassMaterialShader{ 4u }
	{
	}

	void LightingModelComponent::MaterialShader::fillMaterialType( ast::type::BaseStruct & type
		, sdw::expr::ExprList & inits )const
	{
		if ( !type.hasMember( "lightingModel" ) )
		{
			type.declMember( "lightingModel", ast::type::Kind::eUInt32 );
			inits.emplace_back( sdw::makeExpr( 0_u32 ) );
		}
	}

	//*********************************************************************************************

	void LightingModelComponent::Plugin::createParsers( castor::AttributeParsers & parsers
		, ChannelFillers & channelFillers )const
	{
		castor::addParserT( parsers
			, uint32_t( CSCNSection::eRoot )
			, cuT( "materials" )
			, lgtmdl::parserRootMaterials
			, { castor::makeParameter< castor::ParameterType::eText >() } );
		castor::addParserT( parsers
			, uint32_t( CSCNSection::ePass )
			, cuT( "lighting_model" )
			, lgtmdl::parserPassLightingModel
			, { castor::makeParameter< castor::ParameterType::eText >() } );
	}

	void LightingModelComponent::Plugin::zeroBuffer( Pass const & pass
		, shader::PassMaterialShader const & materialShader
		, PassBuffer & buffer )const
	{
		auto data = buffer.getData( pass.getId() );
		data.write( materialShader.getMaterialChunk(), pass.getLightingModelId(), 0u );
	}

	bool LightingModelComponent::Plugin::isComponentNeeded( TextureCombine const & textures
		, ComponentModeFlags const & filter )const
	{
		return checkFlag( filter, ComponentModeFlag::eDiffuseLighting )
			|| checkFlag( filter, ComponentModeFlag::eSpecularLighting );
	}

	//*********************************************************************************************

	castor::String const LightingModelComponent::TypeName = C3D_MakePassLightingComponentName( "model" );

	LightingModelComponent::LightingModelComponent( Pass & pass )
		: BaseDataPassComponentT{ pass, TypeName, {}, pass.getLightingModelId() }
	{
	}

	void LightingModelComponent::accept( ConfigurationVisitorBase & vis )
	{
		auto types = getOwner()->getOwner()->getEngine()->getPassFactory().listRegisteredTypes();
		castor::StringArray values;

		for ( auto & entry : types )
		{
			values.push_back( entry.name );
		}

		vis.visit( cuT( "Lighting Model" )
			, m_zeroBasedValue
			, values
			, [this]( uint32_t oldV, uint32_t newV )
			{
				m_zeroBasedValue = newV;
				m_value = newV + 1u;
			}
			, ConfigurationVisitorBase::makeControlsList< bool >( nullptr ) );
	}

	castor::String LightingModelComponent::getLightingModelName()const
	{
		return getOwner()->getOwner()->getEngine()->getPassFactory().getIdName( getLightingModelId() );
	}

	PassComponentUPtr LightingModelComponent::doClone( Pass & pass )const
	{
		auto result = castor::makeUnique< LightingModelComponent >( pass );
		result->setData( getData() );
		return castor::ptrRefCast< PassComponent >( result );
	}

	bool LightingModelComponent::doWriteText( castor::String const & tabs
		, castor::Path const & folder
		, castor::String const & subfolder
		, castor::StringStream & file )const
	{
		return castor::TextWriter< LightingModelComponent >{ tabs }( *this, file );
	}

	void LightingModelComponent::doFillBuffer( PassBuffer & buffer )const
	{
		auto data = buffer.getData( getOwner()->getId() );
		data.write( m_materialShader->getMaterialChunk(), uint32_t( getLightingModelId() ), 0u );
	}

	//*********************************************************************************************
}
