#include "Castor3D/Material/Material.hpp"

#include "Castor3D/Engine.hpp"
#include "Castor3D/Cache/MaterialCache.hpp"
#include "Castor3D/Miscellaneous/Logger.hpp"
#include "Castor3D/Material/Pass/Pass.hpp"
#include "Castor3D/Material/Pass/PassFactory.hpp"
#include "Castor3D/Render/RenderSystem.hpp"
#include "Castor3D/Scene/Scene.hpp"
#include "Castor3D/Scene/SceneFileParserData.hpp"

#include <CastorUtils/FileParser/FileParser.hpp>

CU_ImplementSmartPtr( castor3d, Material )

namespace castor3d
{
	namespace mat
	{
		static CU_ImplementAttributeParserNewBlock( parserRootMaterial, RootContext, MaterialContext )
		{
			if ( params.empty() )
			{
				CU_ParsingError( cuT( "Missing parameter." ) );
			}
			else
			{
				auto & engine = *getEngine( *blockContext );
				castor::String name;
				newBlockContext->root = blockContext;
				newBlockContext->material = engine.tryFindMaterial( params[0]->get( name ) );
				newBlockContext->passIndex = 0u;
				newBlockContext->createMaterial = newBlockContext->material == nullptr;

				if ( newBlockContext->createMaterial )
				{
					newBlockContext->ownMaterial = engine.createMaterial( name
						, engine
						, engine.getDefaultLightingModel() );
					newBlockContext->material = newBlockContext->ownMaterial.get();
				}
			}
		}
		CU_EndAttributePushNewBlock( CSCNSection::eMaterial )

		static CU_ImplementAttributeParserNewBlock( parserSceneMaterial, SceneContext, MaterialContext )
		{
			if ( params.empty() )
			{
				CU_ParsingError( cuT( "Missing parameter." ) );
			}
			else
			{
				auto & engine = *getEngine( *blockContext );
				auto name = params[0]->get< castor::String >();
				newBlockContext->root = blockContext->root;
				newBlockContext->scene = blockContext;
				newBlockContext->material = engine.tryFindMaterial( name );
				newBlockContext->passIndex = 0u;
				newBlockContext->createMaterial = newBlockContext->material == nullptr;

				if ( newBlockContext->createMaterial )
				{
					newBlockContext->ownMaterial = engine.createMaterial( name
						, engine
						, engine.getDefaultLightingModel() );
					newBlockContext->material = newBlockContext->ownMaterial.get();
				}
			}
		}
		CU_EndAttributePushNewBlock( CSCNSection::eMaterial )

		static CU_ImplementAttributeParserBlock( parserRenderPass, MaterialContext )
		{
			if ( params.empty() )
			{
				CU_ParsingError( cuT( "Missing parameters" ) );
			}
			else
			{
				castor::String typeName;
				params[0]->get( typeName );
				blockContext->material->setRenderPassInfo( getEngine( *blockContext )->getRenderPassInfo( typeName ) );
			}
		}
		CU_EndAttribute()

		static CU_ImplementAttributeParserBlock( parserEnd, MaterialContext )
		{
			if ( !blockContext->ownMaterial
				&& !blockContext->material )
			{
				CU_ParsingError( cuT( "Material not initialised" ) );
			}
			else if ( blockContext->ownMaterial )
			{
				log::info << "Loaded material [" << blockContext->material->getName() << "]" << std::endl;

				if ( blockContext->scene )
				{
					blockContext->scene->scene->addMaterial( blockContext->material->getName()
						, blockContext->ownMaterial
						, true );
				}
				else
				{
					getEngine( *blockContext )->addMaterial( blockContext->material->getName()
						, blockContext->ownMaterial
						, true );
				}
			}
		}
		CU_EndAttributePop()
	}

	const castor::String Material::DefaultMaterialName = cuT( "C3D_DefaultMaterial" );

	Material::Material( castor::String const & name
		, Engine & engine
		, LightingModelID lightingModelId )
		: castor::Named{ name }
		, castor::OwnedBy< Engine >{ engine }
		, m_lightingModelId{ lightingModelId }
	{
	}

	void Material::initialise()
	{
		if ( m_initialised )
		{
			return;
		}

		log::debug << cuT( "Initialising material [" ) << getName() << cuT( "]" ) << std::endl;

		for ( auto const & pass : m_passes )
		{
			pass->initialise();
		}

		m_initialised = true;
	}

	void Material::cleanup()
	{
		m_initialised = false;

		for ( auto const & pass : m_passes )
		{
			pass->cleanup();
		}
	}

	PassRPtr Material::createPass( LightingModelID lightingModelId )
	{
		if ( m_passes.size() == MaxPassLayers )
		{
			log::error << cuT( "Can't create a new pass, max pass count reached" ) << std::endl;
			CU_Failure( "Can't create a new pass, max pass count reached" );
			return nullptr;
		}

		auto result = getEngine()->getPassFactory().create( *this
			, lightingModelId );
		CU_Require( result );
		auto ret = result.get();
		m_passListeners.try_emplace( ret
			, result->onChanged.connect( [this]( Pass const & p
				, CU_UnusedParam( PassComponentCombineID, oldCombineID )
				, CU_UnusedParam( PassComponentCombineID, newCombineID ) )
			{
				onPassChanged( p );
			} ) );
		m_passes.emplace_back( castor::move( result ) );
		onChanged( *this );
		return ret;
	}

	PassRPtr Material::createPass()
	{
		return createPass( m_lightingModelId );
	}

	void Material::addPass( Pass const & pass )
	{
		if ( m_passes.size() == MaxPassLayers )
		{
			log::error << cuT( "Can't create a new pass, max pass count reached" ) << std::endl;
			CU_Failure( "Can't create a new pass, max pass count reached" );
			return;
		}

		auto newPass = getEngine()->getPassFactory().create( *this
			, pass );
		CU_Require( newPass );
		m_passListeners.try_emplace( newPass.get()
			, newPass->onChanged.connect( [this]( Pass const & p
				, CU_UnusedParam( PassComponentCombineID, oldCombineID )
				, CU_UnusedParam( PassComponentCombineID, newCombineID ) )
			{
				onPassChanged( p );
			} ) );
		m_passes.emplace_back( castor::move( newPass ) );
		onChanged( *this );
	}

	void Material::removePass( Pass const & pass )
	{
		auto it = std::find_if( m_passes.begin()
			, m_passes.end()
			, [&pass]( PassUPtr const & lookup )
			{
				return lookup.get() == &pass;
			} );

		if ( it != m_passes.end() )
		{
			m_passListeners.erase( it->get() );
			m_passes.erase( it );
			onChanged( *this );
		}
	}

	PassRPtr Material::getPass( uint32_t index )const
	{
		CU_Require( index < m_passes.size() );
		return m_passes[index].get();
	}

	void Material::destroyPass( uint32_t index )
	{
		CU_Require( index < m_passes.size() );
		m_passListeners.erase( ( m_passes.begin() + index )->get() );
		m_passes.erase( m_passes.begin() + index );
		onChanged( *this );
	}

	bool Material::hasAlphaBlending()const
	{
		return m_passes.end() == std::find_if( m_passes.begin()
			, m_passes.end()
			, []( PassUPtr const & pass )
			{
				return !pass->hasAlphaBlending();
			} );
	}

	bool Material::hasEnvironmentMapping()const
	{
		return m_passes.end() != std::find_if( m_passes.begin()
			, m_passes.end()
			, []( PassUPtr const & pass )
			{
				return pass->hasEnvironmentMapping();
			} );
	}

	bool Material::hasSubsurfaceScattering()const
	{
		return m_passes.end() != std::find_if( m_passes.begin()
			, m_passes.end()
			, []( PassUPtr const & pass )
			{
				return pass->hasSubsurfaceScattering();
			} );
	}

	void Material::onPassChanged( Pass const & )
	{
		onChanged( *this );
	}

	void Material::addParsers( castor::AttributeParsers & result
			, castor::UInt32StrMap const & textureChannels )
	{
		using namespace castor;
		BlockParserContextT< RootContext > rootContext{ result, CSCNSection::eRoot };
		BlockParserContextT< SceneContext > sceneContext{ result, CSCNSection::eScene, CSCNSection::eRoot };
		BlockParserContextT< MaterialContext > materialContext{ result, CSCNSection::eMaterial };

		rootContext.addPushParser( cuT( "material" ), CSCNSection::eMaterial, mat::parserRootMaterial, { makeParameter< ParameterType::eName >() } );

		sceneContext.addPushParser( cuT( "material" ), CSCNSection::eMaterial, mat::parserSceneMaterial, { makeParameter< ParameterType::eName >() } );

		materialContext.addParser( cuT( "render_pass" ), mat::parserRenderPass, { makeParameter< ParameterType::eText >() } );
		materialContext.addPopParser( cuT( "}" ), mat::parserEnd );

		Pass::addParsers( result, textureChannels );
	}

	Engine * getEngine( MaterialContext const & context )
	{
		return getEngine( *context.root );
	}
}
