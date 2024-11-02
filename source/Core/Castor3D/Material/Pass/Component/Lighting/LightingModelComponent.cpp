#include "Castor3D/Material/Pass/Component/Lighting/LightingModelComponent.hpp"

#include "Castor3D/Engine.hpp"
#include "Castor3D/Material/Pass/Pass.hpp"
#include "Castor3D/Material/Pass/PassFactory.hpp"
#include "Castor3D/Miscellaneous/ConfigurationVisitor.hpp"
#include "Castor3D/Material/Pass/PbrPass.hpp"
#include "Castor3D/Material/Pass/PhongPass.hpp"
#include "Castor3D/Material/Pass/Component/PassComponentRegister.hpp"
#include "Castor3D/Miscellaneous/Logger.hpp"
#include "Castor3D/Scene/SceneFileParserData.hpp"
#include "Castor3D/Shader/LightingModelFactory.hpp"
#include "Castor3D/Shader/ShaderBuffers/PassBuffer.hpp"
#include "Castor3D/Shader/Shaders/GlslBlendComponents.hpp"
#include "Castor3D/Shader/Shaders/GlslLighting.hpp"
#include "Castor3D/Shader/Shaders/GlslMaterial.hpp"

#include <CastorUtils/FileParser/FileParser.hpp>

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
			bool result{};
			auto baseName = object.getLightingModelName();

			if ( auto block = beginBlock( file, cuT( "lighting_model" ), baseName ) )
			{
				castor3d::Engine const & engine = *object.getOwner()->getOwner()->getEngine();
				auto & model = engine.getLightingModelFactory().getModel( baseName );
				result = block->writeNameOpt( file, cuT( "diffuse_brdf" ), object.getDiffuseBrdfName(), model.defaultDiffuseBrdf.name )
					&& block->writeNameOpt( file, cuT( "specular_brdf" ), object.getSpecularBrdfName(), model.defaultSpecularBrdf.name )
					&& block->writeNameOpt( file, cuT( "sheen_brdf" ), object.getSheenBrdfName(), model.defaultSheenBrdf.name )
					&& block->writeNameOpt( file, cuT( "clearcoat_brdf" ), object.getClearcoatBrdfName(), model.defaultClearcoatBrdf.name );
			}

			return result;
		}
	};
}

namespace castor3d
{
	//*********************************************************************************************

	namespace lgtmdl
	{
		struct ModelContext
		{
			RootContext * root{};
			PassContext * pass{};
			castor::String lightingModel{};
			castor::String diffuseBrdf{};
			castor::String specularBrdf{};
			castor::String sheenBrdf{};
			castor::String clearcoatBrdf{};
			bool defaultModel{};
		};

		static CU_ImplementAttributeParserNewBlock( parserRootDefaultLightingModel, RootContext, ModelContext )
		{
			if ( params.empty() )
			{
				CU_ParsingError( cuT( "Missing parameter." ) );
			}
			else if ( !params.empty() )
			{
				newBlockContext->root = blockContext;
				newBlockContext->defaultModel = true;
				newBlockContext->lightingModel = LightingModelFactory::normaliseName( params[0]->get< castor::String >() );
			}
		}
		CU_EndAttributePushNewBlock( CSCNSection::eDefaultLightingModel )

		static CU_ImplementAttributeParserNewBlock( parserPassLightingModel, PassContext, ModelContext )
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
				newBlockContext->pass = blockContext;
				newBlockContext->lightingModel = LightingModelFactory::normaliseName( params[0]->get< castor::String >() );
			}
		}
		CU_EndAttributePushNewBlock( CSCNSection::eLightingModel )

		static CU_ImplementAttributeParserBlock( parserPassDiffuseBRDF, ModelContext )
		{
			if ( !blockContext->pass && !blockContext->root )
			{
				CU_ParsingError( cuT( "No Pass initialised." ) );
			}
			else if ( params.empty() )
			{
				CU_ParsingError( cuT( "Missing parameter." ) );
			}
			else
			{
				params[0]->get( blockContext->diffuseBrdf );
			}
		}
		CU_EndAttribute()

		static CU_ImplementAttributeParserBlock( parserPassSpecularBRDF, ModelContext )
		{
			if ( !blockContext->pass && !blockContext->root )
			{
				CU_ParsingError( cuT( "No Pass initialised." ) );
			}
			else if ( params.empty() )
			{
				CU_ParsingError( cuT( "Missing parameter." ) );
			}
			else
			{
				params[0]->get( blockContext->specularBrdf );
			}
		}
		CU_EndAttribute()

		static CU_ImplementAttributeParserBlock( parserPassSheenBRDF, ModelContext )
		{
			if ( !blockContext->pass && !blockContext->root )
			{
				CU_ParsingError( cuT( "No Pass initialised." ) );
			}
			else if ( params.empty() )
			{
				CU_ParsingError( cuT( "Missing parameter." ) );
			}
			else
			{
				params[0]->get( blockContext->sheenBrdf );
			}
		}
		CU_EndAttribute()

		static CU_ImplementAttributeParserBlock( parserPassClearcoatBRDF, ModelContext )
		{
			if ( !blockContext->pass && !blockContext->root )
			{
				CU_ParsingError( cuT( "No Pass initialised." ) );
			}
			else if ( params.empty() )
			{
				CU_ParsingError( cuT( "Missing parameter." ) );
			}
			else
			{
				params[0]->get( blockContext->clearcoatBrdf );
			}
		}
		CU_EndAttribute()

		static CU_ImplementAttributeParserBlock( parserPassDefaultLightingModelEnd, ModelContext )
		{
			auto & engine = *getEngine( *blockContext->root );
			auto lightingModelId = engine.getLightingModelFactory().getNameId( blockContext->lightingModel
				, blockContext->diffuseBrdf
				, blockContext->specularBrdf
				, blockContext->sheenBrdf
				, blockContext->clearcoatBrdf );

			if ( lightingModelId == 0 )
			{
				CU_ParsingError( cuT( "Lighting model is unregistered." ) );
			}
			else
			{
				engine.setDefaultLightingModel( lightingModelId );
			}
		}
		CU_EndAttributePop()

		static CU_ImplementAttributeParserBlock( parserPassLightingModelEnd, ModelContext )
		{
			auto const & engine = *getEngine( *blockContext->pass );
			auto & component = getPassComponent< LightingModelComponent >( *blockContext->pass );
			auto lightingModelId = engine.getLightingModelFactory().getNameId( blockContext->lightingModel
				, blockContext->diffuseBrdf
				, blockContext->specularBrdf
				, blockContext->sheenBrdf
				, blockContext->clearcoatBrdf );

			if ( lightingModelId == 0 )
			{
				CU_ParsingError( cuT( "Lighting model is unregistered." ) );
			}
			else
			{
				component.setLightingModelId( lightingModelId );
			}
		}
		CU_EndAttributePop()
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
			, uint32_t( CSCNSection::eDefaultLightingModel )
			, cuT( "materials" )
			, lgtmdl::parserRootDefaultLightingModel
			, { castor::makeParameter< castor::ParameterType::eText >() } );
		castor::addParserT( parsers
			, uint32_t( CSCNSection::eRoot )
			, uint32_t( CSCNSection::eDefaultLightingModel )
			, cuT( "default_lighting_model" )
			, lgtmdl::parserRootDefaultLightingModel
			, { castor::makeParameter< castor::ParameterType::eText >() } );
		castor::addParserT( parsers
			, uint32_t( CSCNSection::eDefaultLightingModel )
			, cuT( "diffuse_brdf" )
			, lgtmdl::parserPassDiffuseBRDF
			, { castor::makeParameter< castor::ParameterType::eText >() } );
		castor::addParserT( parsers
			, uint32_t( CSCNSection::eDefaultLightingModel )
			, cuT( "specular_brdf" )
			, lgtmdl::parserPassSpecularBRDF
			, { castor::makeParameter< castor::ParameterType::eText >() } );
		castor::addParserT( parsers
			, uint32_t( CSCNSection::eDefaultLightingModel )
			, cuT( "sheen_brdf" )
			, lgtmdl::parserPassSheenBRDF
			, { castor::makeParameter< castor::ParameterType::eText >() } );
		castor::addParserT( parsers
			, uint32_t( CSCNSection::eDefaultLightingModel )
			, cuT( "clearcoat_brdf" )
			, lgtmdl::parserPassClearcoatBRDF
			, { castor::makeParameter< castor::ParameterType::eText >() } );
		castor::addParserT( parsers
			, uint32_t( CSCNSection::eDefaultLightingModel )
			, uint32_t( CSCNSection::eRoot )
			, cuT( "}" )
			, lgtmdl::parserPassDefaultLightingModelEnd );

		castor::addParserT( parsers
			, uint32_t( CSCNSection::ePass )
			, uint32_t( CSCNSection::eLightingModel )
			, cuT( "lighting_model" )
			, lgtmdl::parserPassLightingModel
			, { castor::makeParameter< castor::ParameterType::eText >() } );
		castor::addParserT( parsers
			, uint32_t( CSCNSection::eLightingModel )
			, cuT( "diffuse_brdf" )
			, lgtmdl::parserPassDiffuseBRDF
			, { castor::makeParameter< castor::ParameterType::eText >() } );
		castor::addParserT( parsers
			, uint32_t( CSCNSection::eLightingModel )
			, cuT( "specular_brdf" )
			, lgtmdl::parserPassSpecularBRDF
			, { castor::makeParameter< castor::ParameterType::eText >() } );
		castor::addParserT( parsers
			, uint32_t( CSCNSection::eLightingModel )
			, cuT( "sheen_brdf" )
			, lgtmdl::parserPassSheenBRDF
			, { castor::makeParameter< castor::ParameterType::eText >() } );
		castor::addParserT( parsers
			, uint32_t( CSCNSection::eLightingModel )
			, cuT( "clearcoat_brdf" )
			, lgtmdl::parserPassClearcoatBRDF
			, { castor::makeParameter< castor::ParameterType::eText >() } );
		castor::addParserT( parsers
			, uint32_t( CSCNSection::eLightingModel )
			, uint32_t( CSCNSection::ePass )
			, cuT( "}" )
			, lgtmdl::parserPassLightingModelEnd );
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
		LightingModelFactory const & factory = getOwner()->getOwner()->getEngine()->getLightingModelFactory();
		castor::StringArray values = factory.listRegisteredTypes();
		vis.visit( cuT( "Lighting Model" )
			, m_zeroBasedValue
			, values
			, [this]( uint32_t, uint32_t newV )
			{
				m_zeroBasedValue = newV;
				setData( m_zeroBasedValue + 1u );
			}
			, ConfigurationVisitorBase::makeControlsList< bool >( nullptr ) );
	}

	castor::String LightingModelComponent::getLightingModelName()const
	{
		return getOwner()->getOwner()->getEngine()->getLightingModelFactory().getBaseName( getLightingModelId() );
	}

	castor::String LightingModelComponent::getDiffuseBrdfName()const
	{
		return getOwner()->getOwner()->getEngine()->getLightingModelFactory().getDiffuseBrdfName( getLightingModelId() );
	}

	castor::String LightingModelComponent::getSpecularBrdfName()const
	{
		return getOwner()->getOwner()->getEngine()->getLightingModelFactory().getSpecularBrdfName( getLightingModelId() );
	}

	castor::String LightingModelComponent::getSheenBrdfName()const
	{
		return getOwner()->getOwner()->getEngine()->getLightingModelFactory().getSheenBrdfName( getLightingModelId() );
	}

	castor::String LightingModelComponent::getClearcoatBrdfName()const
	{
		return getOwner()->getOwner()->getEngine()->getLightingModelFactory().getClearcoatBrdfName( getLightingModelId() );
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
