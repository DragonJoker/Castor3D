#include "Castor3D/Material/Pass/Component/Lighting/LightingModelComponent.hpp"

#include "Castor3D/Engine.hpp"
#include "Castor3D/Material/Pass/Pass.hpp"
#include "Castor3D/Miscellaneous/ConfigurationVisitor.hpp"
#include "Castor3D/Scene/SceneFileParserData.hpp"
#include "Castor3D/Shader/LightingModelFactory.hpp"
#include "Castor3D/Shader/ShaderBuffers/PassBuffer.hpp"

#include <CastorUtils/FileParser/FileParser.hpp>

namespace c3d
{
	template<>
	class TextWriter< LightingModelComponent >
		: public TextWriterT< LightingModelComponent >
	{
	public:
		explicit TextWriter( String const & tabs )
			: TextWriterT< LightingModelComponent >{ tabs }
		{
		}

		bool operator()( LightingModelComponent const & object
			, StringStream & file )override
		{
			bool result{};
			Engine const & engine = *object.getOwner()->getOwner()->getEngine();
			auto baseName = object.getLightingModelName();
			auto diffuseBrdf = object.getDiffuseBrdfName();
			auto specularBrdf = object.getSpecularBrdfName();
			auto sheenBrdf = object.getSheenBrdfName();
			auto clearcoatBrdf = object.getClearcoatBrdfName();
			auto scatteringModel = object.getScatteringModelName();

			if ( auto & model = engine.getLightingModelFactory().getModel( baseName );
				diffuseBrdf == model.defaultDesc.diffuse.name
					&& specularBrdf == model.defaultDesc.specular.name
					&& sheenBrdf == model.defaultDesc.sheen.name
					&& clearcoatBrdf == model.defaultDesc.clearcoat.name
					&& scatteringModel == model.defaultDesc.scattering.name )
			{
				result = writeNameOpt( file, cuT( "lighting_model" ), baseName, engine.getDefaultLightingModelName() );
			}
			else if ( auto block = beginBlock( file, cuT( "lighting_model" ), baseName ) )
			{
				result = block->writeNameOpt( file, cuT( "diffuse_brdf" ), diffuseBrdf, model.defaultDesc.diffuse.name )
					&& block->writeNameOpt( file, cuT( "specular_brdf" ), specularBrdf, model.defaultDesc.specular.name )
					&& block->writeNameOpt( file, cuT( "sheen_brdf" ), sheenBrdf, model.defaultDesc.sheen.name )
					&& block->writeNameOpt( file, cuT( "clearcoat_brdf" ), clearcoatBrdf, model.defaultDesc.clearcoat.name )
					&& block->writeNameOpt( file, cuT( "scattering_model" ), scatteringModel, model.defaultDesc.scattering.name );
			}

			return result;
		}
	};
}

namespace c3d
{
	//*********************************************************************************************

	namespace lgtmdl
	{
		struct ModelContext
		{
			RootContext * root{};
			PassContext * pass{};
			String lightingModel{};
			shader::LightingModelNames descNames;
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
				newBlockContext->lightingModel = LightingModelFactory::normaliseName( params[0]->get< String >() );
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
				newBlockContext->lightingModel = LightingModelFactory::normaliseName( params[0]->get< String >() );
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
				params[0]->get( blockContext->descNames.diffuse );
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
				params[0]->get( blockContext->descNames.specular );
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
				params[0]->get( blockContext->descNames.sheen );
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
				params[0]->get( blockContext->descNames.clearcoat );
			}
		}
		CU_EndAttribute()

		static CU_ImplementAttributeParserBlock( parserPassScatteringModel, ModelContext )
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
				params[0]->get( blockContext->descNames.scattering );
			}
		}
		CU_EndAttribute()

		static CU_ImplementAttributeParserBlock( parserPassDefaultLightingModelEnd, ModelContext )
		{
			auto & engine = *getEngine( *blockContext->root );
			auto lightingModelId = engine.getLightingModelFactory().getLightingModelId( blockContext->lightingModel
				, blockContext->descNames );

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
			auto lightingModelId = engine.getLightingModelFactory().getLightingModelId( blockContext->lightingModel
				, blockContext->descNames );

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

	void LightingModelComponent::Plugin::createParsers( AttributeParsers & parsers
		, ChannelFillers & channelFillers )const
	{
		c3d::addParserT( parsers
			, uint32_t( CSCNSection::eRoot )
			, uint32_t( CSCNSection::eDefaultLightingModel )
			, cuT( "materials" )
			, lgtmdl::parserRootDefaultLightingModel
			, { makeParameter< ParameterType::eText >() } );
		c3d::addParserT( parsers
			, uint32_t( CSCNSection::eRoot )
			, uint32_t( CSCNSection::eDefaultLightingModel )
			, cuT( "default_lighting_model" )
			, lgtmdl::parserRootDefaultLightingModel
			, { makeParameter< ParameterType::eText >() } );
		c3d::addParserT( parsers
			, uint32_t( CSCNSection::eDefaultLightingModel )
			, cuT( "diffuse_brdf" )
			, lgtmdl::parserPassDiffuseBRDF
			, { makeParameter< ParameterType::eText >() } );
		c3d::addParserT( parsers
			, uint32_t( CSCNSection::eDefaultLightingModel )
			, cuT( "specular_brdf" )
			, lgtmdl::parserPassSpecularBRDF
			, { makeParameter< ParameterType::eText >() } );
		c3d::addParserT( parsers
			, uint32_t( CSCNSection::eDefaultLightingModel )
			, cuT( "sheen_brdf" )
			, lgtmdl::parserPassSheenBRDF
			, { makeParameter< ParameterType::eText >() } );
		c3d::addParserT( parsers
			, uint32_t( CSCNSection::eDefaultLightingModel )
			, cuT( "clearcoat_brdf" )
			, lgtmdl::parserPassClearcoatBRDF
			, { makeParameter< ParameterType::eText >() } );
		c3d::addParserT( parsers
			, uint32_t( CSCNSection::eDefaultLightingModel )
			, cuT( "scattering_model" )
			, lgtmdl::parserPassScatteringModel
			, { makeParameter< ParameterType::eText >() } );
		c3d::addParserT( parsers
			, uint32_t( CSCNSection::eDefaultLightingModel )
			, uint32_t( CSCNSection::eRoot )
			, cuT( "}" )
			, lgtmdl::parserPassDefaultLightingModelEnd );

		c3d::addParserT( parsers
			, uint32_t( CSCNSection::ePass )
			, uint32_t( CSCNSection::eLightingModel )
			, cuT( "lighting_model" )
			, lgtmdl::parserPassLightingModel
			, { makeParameter< ParameterType::eText >() } );
		c3d::addParserT( parsers
			, uint32_t( CSCNSection::eLightingModel )
			, cuT( "diffuse_brdf" )
			, lgtmdl::parserPassDiffuseBRDF
			, { makeParameter< ParameterType::eText >() } );
		c3d::addParserT( parsers
			, uint32_t( CSCNSection::eLightingModel )
			, cuT( "specular_brdf" )
			, lgtmdl::parserPassSpecularBRDF
			, { makeParameter< ParameterType::eText >() } );
		c3d::addParserT( parsers
			, uint32_t( CSCNSection::eLightingModel )
			, cuT( "sheen_brdf" )
			, lgtmdl::parserPassSheenBRDF
			, { makeParameter< ParameterType::eText >() } );
		c3d::addParserT( parsers
			, uint32_t( CSCNSection::eLightingModel )
			, cuT( "clearcoat_brdf" )
			, lgtmdl::parserPassClearcoatBRDF
			, { makeParameter< ParameterType::eText >() } );
		c3d::addParserT( parsers
			, uint32_t( CSCNSection::eLightingModel )
			, cuT( "scattering_model" )
			, lgtmdl::parserPassScatteringModel
			, { makeParameter< ParameterType::eText >() } );
		c3d::addParserT( parsers
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

	String const LightingModelComponent::TypeName = C3D_MakePassLightingComponentName( "model" );

	LightingModelComponent::LightingModelComponent( Pass & pass )
		: BaseDataPassComponentT{ pass, TypeName, {}, pass.getLightingModelId() }
	{
	}

	void LightingModelComponent::accept( ConfigurationVisitorBase & vis )
	{
		LightingModelFactory const & factory = getOwner()->getOwner()->getEngine()->getLightingModelFactory();
		StringArray values = factory.listRegisteredTypes();
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

	String LightingModelComponent::getLightingModelName()const
	{
		return getOwner()->getOwner()->getEngine()->getLightingModelFactory().getBaseName( getLightingModelId() );
	}

	String LightingModelComponent::getDiffuseBrdfName()const
	{
		return getOwner()->getOwner()->getEngine()->getLightingModelFactory().getDiffuseBrdfName( getLightingModelId() );
	}

	String LightingModelComponent::getSpecularBrdfName()const
	{
		return getOwner()->getOwner()->getEngine()->getLightingModelFactory().getSpecularBrdfName( getLightingModelId() );
	}

	String LightingModelComponent::getSheenBrdfName()const
	{
		return getOwner()->getOwner()->getEngine()->getLightingModelFactory().getSheenBrdfName( getLightingModelId() );
	}

	String LightingModelComponent::getClearcoatBrdfName()const
	{
		return getOwner()->getOwner()->getEngine()->getLightingModelFactory().getClearcoatBrdfName( getLightingModelId() );
	}

	String LightingModelComponent::getScatteringModelName()const
	{
		return getOwner()->getOwner()->getEngine()->getLightingModelFactory().getScatteringModelName( getLightingModelId() );
	}

	PassComponentUPtr LightingModelComponent::doClone( Pass & pass )const
	{
		auto result = makeRawUnique< LightingModelComponent >( pass );
		result->setData( getData() );
		return passComponentCast( result );
	}

	bool LightingModelComponent::doWriteText( String const & tabs
		, Path const & folder
		, String const & subfolder
		, StringStream & file )const
	{
		return TextWriter< LightingModelComponent >{ tabs }( *this, file );
	}

	void LightingModelComponent::doFillBuffer( PassBuffer & buffer )const
	{
		auto data = buffer.getData( getOwner()->getId() );
		data.write( m_materialShader->getMaterialChunk(), uint32_t( getLightingModelId() ), 0u );
	}

	//*********************************************************************************************
}
