#include "Castor3D/Material/Pass/Component/Lighting/LightingModelComponent.hpp"

#include "Castor3D/Engine.hpp"
#include "Castor3D/Material/Pass/Pass.hpp"
#include "Castor3D/Material/Pass/PassFactory.hpp"
#include "Castor3D/Material/Pass/PassVisitor.hpp"
#include "Castor3D/Material/Pass/Component/PassComponentRegister.hpp"
#include "Castor3D/Miscellaneous/Logger.hpp"
#include "Castor3D/Scene/SceneFileParser.hpp"
#include "Castor3D/Shader/LightingModelFactory.hpp"
#include "Castor3D/Shader/ShaderBuffers/PassBuffer.hpp"
#include "Castor3D/Shader/Shaders/GlslBlendComponents.hpp"
#include "Castor3D/Shader/Shaders/GlslLighting.hpp"
#include "Castor3D/Shader/Shaders/GlslMaterial.hpp"

#include <CastorUtils/FileParser/ParserParameter.hpp>

namespace castor3d
{
	//*********************************************************************************************

	namespace lgtmdl
	{
		static CU_ImplementAttributeParser( parserRootMaterials )
		{
			auto & parsingContext = getParserContext( context );

			if ( params.empty() )
			{
				CU_ParsingError( cuT( "Missing parameter." ) );
			}
			else if ( !params.empty() )
			{
				auto name = LightingModelFactory::normaliseName( params[0]->get< castor::String >() );
				auto & engine = *parsingContext.parser->getEngine();
				engine.setDefaultLightingModel( engine.getLightingModelFactory().getNameId( name ) );
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

	castor::String const LightingModelComponent::TypeName = C3D_MakePassLightingComponentName( "lighting_model" );

	LightingModelComponent::LightingModelComponent( Pass & pass )
		: BaseDataPassComponentT{ pass, TypeName, pass.getLightingModelId() }
	{
	}

	void LightingModelComponent::accept( PassVisitorBase & vis )
	{
		vis.visit( cuT( "Lighting Model" ), m_value );
	}

	castor::String LightingModelComponent::getLightingModelName()const
	{
		return getOwner()->getOwner()->getEngine()->getPassFactory().getIdName( getLightingModelId() );
	}

	PassComponentUPtr LightingModelComponent::doClone( Pass & pass )const
	{
		auto result = std::make_unique< LightingModelComponent >( pass );
		result->setData( getData() );
		return result;
	}

	void LightingModelComponent::doFillBuffer( PassBuffer & buffer )const
	{
		auto data = buffer.getData( getOwner()->getId() );
		data.write( m_materialShader->getMaterialChunk(), uint32_t( getLightingModelId() ), 0u );
	}

	//*********************************************************************************************
}
