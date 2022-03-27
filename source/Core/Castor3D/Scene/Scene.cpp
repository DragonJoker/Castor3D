#include "Castor3D/Scene/Scene.hpp"

#include "Castor3D/Engine.hpp"
#include "Castor3D/Event/Frame/CpuFunctorEvent.hpp"
#include "Castor3D/Event/Frame/FrameListener.hpp"
#include "Castor3D/Event/Frame/GpuFunctorEvent.hpp"
#include "Castor3D/Material/Material.hpp"
#include "Castor3D/Material/Pass/Pass.hpp"
#include "Castor3D/Miscellaneous/makeVkType.hpp"
#include "Castor3D/Model/Mesh/Mesh.hpp"
#include "Castor3D/Model/Mesh/Submesh/Submesh.hpp"
#include "Castor3D/Overlay/Overlay.hpp"
#include "Castor3D/Render/RenderInfo.hpp"
#include "Castor3D/Render/RenderLoop.hpp"
#include "Castor3D/Render/RenderPipeline.hpp"
#include "Castor3D/Render/RenderSystem.hpp"
#include "Castor3D/Render/RenderTarget.hpp"
#include "Castor3D/Render/RenderWindow.hpp"
#include "Castor3D/Render/EnvironmentMap/EnvironmentMap.hpp"
#include "Castor3D/Render/EnvironmentMap/EnvironmentMapPass.hpp"
#include "Castor3D/Render/Node/SceneRenderNodes.hpp"
#include "Castor3D/Scene/BillboardList.hpp"
#include "Castor3D/Scene/Camera.hpp"
#include "Castor3D/Scene/Geometry.hpp"
#include "Castor3D/Scene/SceneNode.hpp"
#include "Castor3D/Scene/Animation/AnimatedObjectGroup.hpp"
#include "Castor3D/Scene/Background/BackgroundTextWriter.hpp"
#include "Castor3D/Scene/Background/Background.hpp"
#include "Castor3D/Scene/Background/Colour.hpp"
#include "Castor3D/Scene/Light/Light.hpp"
#include "Castor3D/Scene/Light/LightFactory.hpp"
#include "Castor3D/Scene/ParticleSystem/ParticleSystem.hpp"
#include "Castor3D/Shader/Shaders/SdwModule.hpp"
#include "Castor3D/Shader/ShaderBuffers/PassBuffer.hpp"

#include <CastorUtils/Design/ResourceCache.hpp>
#include <CastorUtils/Graphics/Font.hpp>
#include <CastorUtils/Graphics/FontCache.hpp>

CU_ImplementCUSmartPtr( castor3d, Scene )

namespace castor3d
{
	//*************************************************************************************************

	std::string print( castor::Point3f const & obj )
	{
		std::stringstream stream;
		stream << std::setprecision( 4 ) << obj->x
			<< ", " << std::setprecision( 4 ) << obj->y
			<< ", " << std::setprecision( 4 ) << obj->z;
		return stream.str();
	}

	std::string print( castor::BoundingBox const & obj )
	{
		std::stringstream stream;
		stream << "min: " << print( obj.getMin() ) << ", max: " << print( obj.getMax() );
		return stream.str();
	}

	//*************************************************************************************************

	template<>
	inline void CacheViewT< OverlayCache, EventType::ePreRender >::clear()
	{
		for ( auto name : m_createdElements )
		{
			auto resource = m_cache.tryRemove( name );
		}
	}

	//*************************************************************************************************

	template<>
	inline void CacheViewT< castor::FontCache, EventType::ePreRender >::clear()
	{
		for ( auto name : m_createdElements )
		{
			auto resource = m_cache.tryRemove( name );
		}
	}

	//*************************************************************************************************

	castor::String Scene::RootNode = cuT( "C3D.RootNode" );
	castor::String Scene::CameraRootNode = cuT( "C3D.CameraRootNode" );
	castor::String Scene::ObjectRootNode = cuT( "C3D.ObjectRootNode" );

	Scene::Scene( castor::String const & name, Engine & engine )
		: castor::OwnedBy< Engine >{ engine }
		, castor::Named{ name }
		, m_rootNode{ std::make_shared< SceneNode >( RootNode, *this ) }
		, m_rootCameraNode{ std::make_shared< SceneNode >( CameraRootNode, *this ) }
		, m_rootObjectNode{ std::make_shared< SceneNode >( ObjectRootNode, *this ) }
		, m_background{ std::make_shared< ColourBackground >( engine, *this ) }
		, m_lightFactory{ std::make_shared< LightFactory >() }
		, m_listener{ engine.getFrameListenerCache().add( cuT( "Scene_" ) + name + castor::string::toString( intptr_t( this ) ) ) }
		, m_renderNodes{ castor::makeUnique< SceneRenderNodes >( *this ) }
	{
		m_rootCameraNode->attachTo( *m_rootNode );
		m_rootObjectNode->attachTo( *m_rootNode );

		m_billboardCache = makeObjectCache< BillboardList, castor::String, BillboardListCacheTraits >( *this
			, m_rootNode
			, m_rootCameraNode
			, m_rootObjectNode );
		m_cameraCache = makeObjectCache< Camera, castor::String, CameraCacheTraits >( *this
			, m_rootNode
			, m_rootCameraNode
			, m_rootObjectNode
			, castor::DummyFunctorT< CameraCache >{}
			, castor::DummyFunctorT< CameraCache >{}
			, MovableMergerT< CameraCache >{ getName() }
			, MovableAttacherT< CameraCache >{}
			, MovableDetacherT< CameraCache >{} );
		m_geometryCache = makeObjectCache< Geometry, castor::String, GeometryCacheTraits >( *this
			, m_rootNode
			, m_rootCameraNode
			, m_rootObjectNode );
		m_lightCache = makeObjectCache< Light, castor::String, LightCacheTraits >( *this
			, m_rootNode
			, m_rootCameraNode
			, m_rootObjectNode );
		m_particleSystemCache = makeObjectCache< ParticleSystem, castor::String, ParticleSystemCacheTraits >( *this
			, m_rootNode
			, m_rootCameraNode
			, m_rootObjectNode
			, [this]( ParticleSystem & element )
			{
				auto & nodes = getRenderNodes();

				getListener().postEvent( makeGpuFunctorEvent( EventType::ePreRender
					, [&element, &nodes]( RenderDevice const & device
						, QueueData const & queueData )
					{
						element.initialise( device );
						auto material = element.getMaterial();

						for ( auto & pass : *material )
						{
							nodes.createNode( *pass
								, *element.getBillboards() );
						}
					} ) );
			}
			, GpuEventCleanerT< ParticleSystemCache >{ getListener() }
			, MovableMergerT< ParticleSystemCache >{ getName() }
			, MovableAttacherT< ParticleSystemCache >{}
			, MovableDetacherT< ParticleSystemCache >{} );
		m_sceneNodeCache = makeObjectCache< SceneNode, castor::String, SceneNodeCacheTraits >( *this
			, m_rootNode
			, m_rootCameraNode
			, m_rootObjectNode
			, castor::DummyFunctorT< SceneNodeCache >{}
			, castor::DummyFunctorT< SceneNodeCache >{}
			, MovableMergerT< SceneNodeCache >{ getName() }
			, SceneNodeAttacherT< SceneNodeCache >{}
			, SceneNodeDetacherT< SceneNodeCache >{} );
		m_animatedObjectGroupCache = castor::makeCache< AnimatedObjectGroup, castor::String, AnimatedObjectGroupCacheTraits >( *this );
		m_meshCache = castor::makeCache< Mesh, castor::String, MeshCacheTraits >( getLogger( engine )
			, CpuEventInitialiserT< MeshCache >{ getListener() }
			, CpuEventCleanerT< MeshCache >{ getListener() }
			, castor::ResourceMergerT< MeshCache >{ getName() } );

		m_materialCacheView = makeCacheView< EventType::ePreRender >( getName()
			, getEngine()->getMaterialCache()
			, [this]( MaterialCache::ElementT & element )
			{
				getListener().postEvent( makeCpuInitialiseEvent( element ) );
				m_materialsListeners.emplace( &element
					, element.onChanged.connect( [this]( Material const & material )
						{
							onMaterialChanged( material );
						} ) );
				m_dirtyMaterials = true;
			}
			, [this]( MaterialCache::ElementT & element )
			{
				m_dirtyMaterials = true;
				m_materialsListeners.erase( &element );
				element.cleanup();
			} );
		m_samplerCacheView = makeCacheView< EventType::ePreRender >( getName()
			, getEngine()->getSamplerCache()
			, [this]( SamplerCache::ElementT & element )
			{
				element.initialise( getOwner()->getRenderSystem()->getRenderDevice() );
			}
			, []( SamplerCache::ElementT & element )
			{
				element.cleanup();
			} );
		m_overlayCacheView = makeCacheView< EventType::ePreRender >( getName()
			, getEngine()->getOverlayCache()
			, [this]( OverlayCache::ElementT & element )
			{
				getEngine()->getOverlayCache().initialise( element );
			}
			, [this]( OverlayCache::ElementT & element )
			{
				getEngine()->getOverlayCache().cleanup( element );
			} );
		m_fontCacheView = makeCacheView< EventType::ePreRender >( getName()
			, getEngine()->getFontCache()
			, []( castor::FontCache::ElementT & element )
			{
				element.initialise();
			}
			, []( castor::FontCache::ElementT & element )
			{
				element.cleanup();
			} );

		auto node = m_rootNode;
		m_sceneNodeCache->add( RootNode, node, false );
		node = m_rootObjectNode;
		m_sceneNodeCache->add( ObjectRootNode, node, false );
		node = m_rootCameraNode;
		m_sceneNodeCache->add( CameraRootNode, node, false );
		auto setThisChanged = [this]()
		{
			setChanged();
		};
		m_onParticleSystemChanged = m_particleSystemCache->onChanged.connect( setThisChanged );
		m_onBillboardListChanged = m_billboardCache->onChanged.connect( setThisChanged );
		m_onGeometryChanged = m_geometryCache->onChanged.connect( setThisChanged );
		m_onSceneNodeChanged = m_sceneNodeCache->onChanged.connect( setThisChanged );
		m_animatedObjectGroupCache->add( cuT( "C3D_Textures" ), *this );
		auto & device = engine.getRenderSystem()->getRenderDevice();
		auto data = device.graphicsData();
		m_reflectionMap = std::make_unique< EnvironmentMap >( engine.getGraphResourceHandler()
			, device
			, *data
			, *this );
	}

	Scene::~Scene()
	{
		m_reflectionMap.reset();
		m_onSceneNodeChanged.disconnect();
		m_onGeometryChanged.disconnect();
		m_onBillboardListChanged.disconnect();
		m_onParticleSystemChanged.disconnect();

		m_meshCache->clear();

		m_reflectionMap.reset();

		m_background.reset();
		m_animatedObjectGroupCache.reset();
		m_billboardCache.reset();
		m_particleSystemCache.reset();
		m_cameraCache.reset();
		m_geometryCache.reset();
		m_lightCache.reset();

		if ( m_rootCameraNode )
		{
			m_rootCameraNode->detach();
			m_rootCameraNode.reset();
		}

		if ( m_rootObjectNode )
		{
			m_rootObjectNode->detach();
			m_rootObjectNode.reset();
		}

		m_sceneNodeCache.reset();

		m_meshCache.reset();
		m_materialCacheView.reset();
		m_samplerCacheView.reset();
		m_overlayCacheView.reset();
		m_fontCacheView.reset();

		if ( m_rootNode )
		{
			m_rootNode->detach();
		}

		m_rootNode.reset();
		getEngine()->getFrameListenerCache().remove( m_listener.lock()->getName() );
	}

	void Scene::initialise()
	{
		auto & engine = *getEngine();
		auto & device = engine.getRenderSystem()->getRenderDevice();
		m_timerSceneNodes = castor::makeUnique< crg::FramePassTimer >( device.makeContext(), getName() + "/SceneNodes" );
		engine.registerTimer( getName() + "SceneNodes", *m_timerSceneNodes );
		m_timerBoundingBox = castor::makeUnique< crg::FramePassTimer >( device.makeContext(), getName() + "/BoundingBoxes" );
		engine.registerTimer( getName() + "BoundingBoxes", *m_timerBoundingBox );
		m_timerAnimations = castor::makeUnique< crg::FramePassTimer >( device.makeContext(), getName() + "/Animations" );
		engine.registerTimer( getName() + "Animations", *m_timerAnimations );
		m_timerMaterials = castor::makeUnique< crg::FramePassTimer >( device.makeContext(), getName() + "/Materials" );
		engine.registerTimer( getName() + "Materials", *m_timerMaterials );
		m_timerLights = castor::makeUnique< crg::FramePassTimer >( device.makeContext(), getName() + "/Lights" );
		engine.registerTimer( getName() + "Lights", *m_timerLights );
		m_timerRenderNodes = castor::makeUnique< crg::FramePassTimer >( device.makeContext(), getName() + "/RenderNodes" );
		engine.registerTimer( getName() + "RenderNodes", *m_timerRenderNodes );
		m_timerAnimGroups = castor::makeUnique< crg::FramePassTimer >( device.makeContext(), getName() + "/AnimGroups" );
		engine.registerTimer( getName() + "AnimGroups", *m_timerAnimGroups );
		m_timerParticlesCpu = castor::makeUnique< crg::FramePassTimer >( device.makeContext(), getName() + "/ParticlesCPU" );
		engine.registerTimer( getName() + "ParticlesCPU", *m_timerParticlesCpu );
		m_timerParticlesGpu = castor::makeUnique< crg::FramePassTimer >( device.makeContext(), getName() + "/ParticlesGPU" );
		engine.registerTimer( getName() + "ParticlesGPU", *m_timerParticlesGpu );

		m_animatedObjectGroupCache->initialise( device );
		m_lightCache->initialise( device );
		m_background->initialise( device );
		doUpdateLightsDependent();
		doUpdateBoundingBox();
		log::info << "Initialised scene [" << getName() << "], AABB: " << print( m_boundingBox ) << std::endl;
		m_initialised = true;
	}

	void Scene::cleanup()
	{
		m_initialised = false;
		m_animatedObjectGroupCache->cleanup();
		m_cameraCache->cleanup();
		m_particleSystemCache->cleanup();
		m_billboardCache->cleanup();
		m_geometryCache->cleanup();
		m_lightCache->cleanup();
		m_reflectionMap->cleanup();
		m_sceneNodeCache->cleanup();

		m_materialCacheView->clear();
		m_samplerCacheView->clear();
		m_overlayCacheView->clear();
		m_fontCacheView->clear();

		m_renderNodes->clear();

		// These ones, being ResourceCache, need to be cleared in destructor only
		m_meshCache->cleanup();

		getListener().postEvent( makeGpuFunctorEvent( EventType::ePreRender
			, [this]( RenderDevice const & device
				, QueueData const & queueData )
			{
				m_background->cleanup( device );
			} ) );

		auto & engine = *getEngine();
		engine.unregisterTimer( getName() + "SceneNodes", *m_timerSceneNodes );
		m_timerSceneNodes.reset();
		engine.unregisterTimer( getName() + "BoundingBoxes", *m_timerBoundingBox );
		m_timerBoundingBox.reset();
		engine.unregisterTimer( getName() + "Animations", *m_timerAnimations );
		m_timerAnimations.reset();
		engine.unregisterTimer( getName() + "Materials", *m_timerMaterials );
		m_timerMaterials.reset();
		engine.unregisterTimer( getName() + "Lights", *m_timerLights );
		m_timerLights.reset();
		engine.unregisterTimer( getName() + "RenderNodes", *m_timerRenderNodes );
		m_timerRenderNodes.reset();
		engine.unregisterTimer( getName() + "AnimGroups", *m_timerAnimGroups );
		m_timerAnimGroups.reset();
		engine.unregisterTimer( getName() + "ParticlesCPU", *m_timerParticlesCpu );
		m_timerParticlesCpu.reset();
		engine.unregisterTimer( getName() + "ParticlesGPU", *m_timerParticlesGpu );
		m_timerParticlesGpu.reset();
	}

	void Scene::update( CpuUpdater & updater )
	{
		if ( m_initialised )
		{
			updater.scene = this;
			{
				auto block( m_timerSceneNodes->start() );
				m_rootNode->update();
			}
			{
				auto block( m_timerBoundingBox->start() );
				doUpdateBoundingBox();
			}
			{
				auto block( m_timerAnimations->start() );
				doUpdateAnimations( updater );
			}
			{
				auto block( m_timerMaterials->start() );
				doUpdateMaterials();
			}
			{
				auto block( m_timerLights->start() );
				m_lightCache->update( updater );
			}
			{
				auto block( m_timerRenderNodes->start() );
				m_renderNodes->update( updater );
			}
			{
				auto block( m_timerAnimGroups->start() );
				m_animatedObjectGroupCache->update( updater );
			}
			{
				auto block( m_timerParticlesCpu->start() );
				doUpdateParticles( updater );
			}

			onUpdate( *this );

			if ( doUpdateLightsDependent() )
			{
				onChanged( *this );
			}

			m_changed = false;
		}
	}

	void Scene::update( GpuUpdater & updater )
	{
		updater.scene = this;
		doUpdateParticles( updater );
		m_renderNodes->update( updater );
		m_meshCache->forEach( []( Mesh & mesh )
			{
				for ( auto & submesh : mesh )
				{
					submesh->update();
				}
			} );
	}

	void Scene::setBackground( SceneBackgroundSPtr value )
	{
		m_background = std::move( value );
		m_background->initialise( getEngine()->getRenderSystem()->getRenderDevice() );
		onSetBackground( *m_background );
	}

	void Scene::merge( Scene & scene )
	{
		scene.getAnimatedObjectGroupCache().mergeInto( *m_animatedObjectGroupCache );
		scene.getCameraCache().mergeInto( *m_cameraCache );
		scene.getBillboardListCache().mergeInto( *m_billboardCache );
		scene.getParticleSystemCache().mergeInto( *m_particleSystemCache );
		scene.getGeometryCache().mergeInto( *m_geometryCache );
		scene.getLightCache().mergeInto( *m_lightCache );
		scene.getSceneNodeCache().mergeInto( *m_sceneNodeCache );
		m_ambientLight = scene.getAmbientLight();
		setChanged();

		scene.cleanup();
	}

	uint32_t Scene::getVertexCount()const
	{
		uint32_t result = 0;
		using LockType = std::unique_lock< GeometryCache >;
		LockType lock{ castor::makeUniqueLock( *m_geometryCache ) };

		for ( auto pair : *m_geometryCache )
		{
			auto mesh = pair.second->getMesh().lock();

			if ( mesh )
			{
				result += mesh->getVertexCount();
			}
		}

		return result;
	}

	uint32_t Scene::getFaceCount()const
	{
		uint32_t result = 0;
		using LockType = std::unique_lock< GeometryCache >;
		LockType lock{ castor::makeUniqueLock( *m_geometryCache ) };

		for ( auto pair : *m_geometryCache )
		{
			auto mesh = pair.second->getMesh().lock();

			if ( mesh )
			{
				result += mesh->getFaceCount();
			}
		}

		return result;
	}

	SceneFlags Scene::getFlags()const
	{
		SceneFlags result;

		switch ( m_fog.getType() )
		{
		case FogType::eLinear:
			result |= SceneFlag::eFogLinear;
			break;

		case FogType::eExponential:
			result |= SceneFlag::eFogExponential;
			break;

		case FogType::eSquaredExponential:
			result |= SceneFlag::eFogSquaredExponential;
			break;

		default:
			break;
		}

		if ( m_hasShadows[size_t( LightType::eDirectional )] )
		{
			result |= SceneFlag::eShadowDirectional;
		}

		if ( m_hasShadows[size_t( LightType::ePoint )] )
		{
			result |= SceneFlag::eShadowPoint;
		}

		if ( m_hasShadows[size_t( LightType::eSpot )] )
		{
			result |= SceneFlag::eShadowSpot;
		}

		if ( m_voxelConfig.enabled )
		{
			result |= SceneFlag::eVoxelConeTracing;
		}
		else
		{
			if ( needsGlobalIllumination( LightType::eDirectional, GlobalIlluminationType::eLpv )
				|| needsGlobalIllumination( LightType::eDirectional, GlobalIlluminationType::eLpvG )
				|| needsGlobalIllumination( LightType::ePoint, GlobalIlluminationType::eLpv )
				|| needsGlobalIllumination( LightType::ePoint, GlobalIlluminationType::eLpvG )
				|| needsGlobalIllumination( LightType::eSpot, GlobalIlluminationType::eLpv )
				|| needsGlobalIllumination( LightType::eSpot, GlobalIlluminationType::eLpvG ) )
			{
				result |= SceneFlag::eLpvGI;
			}

			if ( needsGlobalIllumination( LightType::eDirectional, GlobalIlluminationType::eLayeredLpv )
				|| needsGlobalIllumination( LightType::eDirectional, GlobalIlluminationType::eLayeredLpvG )
				|| needsGlobalIllumination( LightType::ePoint, GlobalIlluminationType::eLayeredLpv )
				|| needsGlobalIllumination( LightType::ePoint, GlobalIlluminationType::eLayeredLpvG )
				|| needsGlobalIllumination( LightType::eSpot, GlobalIlluminationType::eLayeredLpv )
				|| needsGlobalIllumination( LightType::eSpot, GlobalIlluminationType::eLayeredLpvG ) )
			{
				if ( getEngine()->getRenderSystem()->hasLLPV() )
				{
					result |= SceneFlag::eLayeredLpvGI;
				}
				else
				{
					result |= SceneFlag::eLpvGI;
				}
			}
		}

		return result;
	}

	bool Scene::hasShadows()const
	{
		return m_hasAnyShadows;
	}

	bool Scene::hasShadows( LightType lightType )const
	{
		return m_hasShadows[size_t( lightType )];
	}

	void Scene::addEnvironmentMap( SceneNode & node )
	{
		m_reflectionMap->addNode( node );
	}

	void Scene::removeEnvironmentMap( SceneNode & node )
	{
		m_reflectionMap->removeNode( node );
	}

	bool Scene::hasEnvironmentMap( SceneNode & node )const
	{
		return m_reflectionMap->hasNode( node );
	}

	EnvironmentMap & Scene::getEnvironmentMap()const
	{
		return *m_reflectionMap;
	}

	uint32_t Scene::getEnvironmentMapIndex( SceneNode const & node )const
	{
		return m_reflectionMap->getIndex( node );
	}

	AnimatedObjectSPtr Scene::addAnimatedTexture( TextureSourceInfo const & sourceInfo
		, TextureConfiguration const & config
		, Pass & pass )
	{
		auto & cache = getAnimatedObjectGroupCache();
		auto group = cache.find( cuT( "C3D_Textures" ) ).lock();
		return group->addObject( sourceInfo, config, pass );
	}

	void Scene::registerLight( Light & light )
	{
		m_lightConnections.emplace( light.getName()
			, light.onChanged.connect( [this]( Light const & lgt )
				{
					doUpdateLightDependent( lgt.getLightType()
						, lgt.isShadowProducer()
						, lgt.getExpectedGlobalIlluminationType() );
				} ) );
	}

	void Scene::unregisterLight( Light & light )
	{
		m_lightConnections.erase( light.getName() );
	}

	PassTypeID Scene::getPassesType()const
	{
		return getEngine()->getPassesType();
	}

	castor::String Scene::getPassesName()const
	{
		return getEngine()->getPassesName();
	}

	bool Scene::needsGlobalIllumination()const
	{
		return m_needsGlobalIllumination;
	}

	bool Scene::needsGlobalIllumination( LightType ltType
		, GlobalIlluminationType giType )const
	{
		return m_giTypes[size_t( ltType )].end() != m_giTypes[size_t( ltType )].find( giType );
	}

	void Scene::setPassesType( PassTypeID value )
	{
		getEngine()->setPassesType( value );
	}

	crg::SemaphoreWaitArray Scene::getRenderTargetsSemaphores()const
	{
		crg::SemaphoreWaitArray result;
		auto lock( castor::makeUniqueLock( getEngine()->getRenderTargetCache() ) );

		for ( auto & target : getEngine()->getRenderTargetCache().getRenderTargets( TargetType::eTexture ) )
		{
			if ( target->getScene() != this )
			{
				auto toWait = target->getSemaphore();
				result.insert( result.end()
					, toWait.begin()
					, toWait.end() );
			}
		}

		return result;
	}

	uint32_t Scene::getLpvGridSize()const
	{
		return getEngine()->getLpvGridSize();
	}

	bool Scene::hasBindless()const
	{
		return getEngine()->getTextureUnitCache().hasBindless();
	}

	ashes::DescriptorSetLayout * Scene::getBindlessTexDescriptorLayout()const
	{
		return getEngine()->getTextureUnitCache().getDescriptorLayout();
	}

	ashes::DescriptorPool * Scene::getBindlessTexDescriptorPool()const
	{
		return getEngine()->getTextureUnitCache().getDescriptorPool();
	}

	ashes::DescriptorSet * Scene::getBindlessTexDescriptorSet()const
	{
		return getEngine()->getTextureUnitCache().getDescriptorSet();
	}

	ashes::Buffer< ModelBufferConfiguration > const & Scene::getModelBuffer()const
	{
		CU_Require( m_renderNodes );
		return m_renderNodes->getModelBuffer();
	}

	ashes::Buffer< BillboardUboConfiguration > const & Scene::getBillboardsBuffer()const
	{
		CU_Require( m_renderNodes );
		return m_renderNodes->getBillboardsBuffer();
	}

	void Scene::setDirectionalShadowCascades( uint32_t value )
	{
		CU_Require( value <= shader::DirectionalMaxCascadesCount );
		m_directionalShadowCascades = value;
	}

	void Scene::setLpvIndirectAttenuation( float value )
	{
		m_lpvIndirectAttenuation = value;
	}

	void Scene::doUpdateBoundingBox()
	{
		float fmin = std::numeric_limits< float >::max();
		float fmax = std::numeric_limits< float >::lowest();
		castor::Point3f min{ fmin, fmin, fmin };
		castor::Point3f max{ fmax, fmax, fmax };
		m_geometryCache->forEach( [&min, &max]( Geometry const & geometry )
			{
				auto node = geometry.getParent();
				auto mesh = geometry.getMesh().lock();

				if ( node && mesh )
				{
					auto bbox = mesh->getBoundingBox().getAxisAligned( node->getDerivedTransformationMatrix() );

					for ( auto i = 0u; i < 3u; ++i )
					{
						min[i] = std::min( min[i], bbox.getMin()[i] );
						max[i] = std::max( max[i], bbox.getMax()[i] );
					}
				}
			} );
		m_boundingBox.load( min, max );
	}

	void Scene::doUpdateAnimations( CpuUpdater & updater )
	{
		m_animatedObjectGroupCache->forEach( [&updater]( AnimatedObjectGroup & group )
			{
				group.update( updater );
			} );
	}

	void Scene::doUpdateParticles( CpuUpdater & updater )
	{
		auto & cache = getParticleSystemCache();
		auto lock( castor::makeUniqueLock( cache ) );
		updater.index = 0u;

		for ( auto & particleSystem : cache )
		{
			particleSystem.second->update( updater );
		}
	}

	void Scene::doUpdateParticles( GpuUpdater & updater )
	{
		auto & cache = getParticleSystemCache();
		auto lock( castor::makeUniqueLock( cache ) );
		auto count = 2u * cache.getObjectCountNoLock();

		if ( m_timerParticlesGpu->getCount() < count )
		{
			m_timerParticlesGpu->updateCount( count );
		}

		auto timerBlock( m_timerParticlesGpu->start() );
		updater.index = 0u;
		updater.timer = m_timerParticlesGpu.get();

		for ( auto & particleSystem : cache )
		{
			particleSystem.second->update( updater );
			updater.info.m_particlesCount += particleSystem.second->getParticlesCount();
		}
	}

	void Scene::doUpdateMaterials()
	{
		if ( m_dirtyMaterials )
		{
			auto & cache = getEngine()->getMaterialCache();
			cache.lock();
			m_needsSubsurfaceScattering = false;
			m_hasTransparentObjects = false;
			m_hasOpaqueObjects = false;

			for ( auto & matName : *m_materialCacheView )
			{
				if ( cache.hasNoLock( matName ) )
				{
					auto material = cache.findNoLock( matName ).lock();

					m_needsSubsurfaceScattering |= material->hasSubsurfaceScattering();

					for ( auto & pass : *material )
					{
						m_hasTransparentObjects |= pass->hasAlphaBlending();
						m_hasOpaqueObjects |= !pass->hasOnlyAlphaBlending();
					}
				}
			}

			cache.unlock();
			m_dirtyMaterials = false;
		}
	}

	bool Scene::doUpdateLightDependent( LightType lightType
		, bool shadowProducer
		, GlobalIlluminationType globalIllumination )
	{
		auto changed = m_hasShadows[size_t( lightType )] != shadowProducer
			|| m_giTypes[size_t( lightType )].find( globalIllumination ) == m_giTypes[size_t( lightType )].end();

		if ( changed )
		{
			m_hasShadows[size_t( lightType )] = shadowProducer;

			if ( globalIllumination != GlobalIlluminationType::eNone )
			{
				m_giTypes[size_t( lightType )].insert( globalIllumination );
			}

			onChanged( *this );
		}

		return changed;
	}

	bool Scene::doUpdateLightsDependent()
	{
		bool needsGI = false;
		bool hasAnyShadows = false;
		std::array< bool, size_t( LightType::eCount ) > hasShadows{};
		std::array< std::set< GlobalIlluminationType >, size_t( LightType::eCount ) > giTypes{};

		m_lightCache->forEach( [&giTypes, &needsGI, &hasAnyShadows, &hasShadows]( Light const & light )
			{
				if ( light.getExpectedGlobalIlluminationType() != GlobalIlluminationType::eNone )
				{
					giTypes[uint32_t( light.getLightType() )].insert( light.getExpectedGlobalIlluminationType() );
					needsGI = true;
				}

				if ( light.isExpectedShadowProducer() )
				{
					hasAnyShadows = true;
					hasShadows[size_t( light.getLightType() )] = true;
				}
			} );

		size_t i = 0u;
		bool changed = false;
		for ( auto & shadows : m_hasShadows )
		{
			changed = shadows.exchange( hasShadows[i] ) != hasShadows[i]
				|| changed;
			++i;
		}

		i = 0u;
		for ( auto & types : m_giTypes )
		{
			changed = types != giTypes[i]
				|| changed;
			types = giTypes[i];
			++i;
		}

		changed = m_hasAnyShadows.exchange( hasAnyShadows ) != hasAnyShadows
			|| changed;
		changed = m_needsGlobalIllumination.exchange( needsGI ) != needsGI
			|| changed;
		return changed;
	}

	void Scene::onMaterialChanged( Material const & material )
	{
		m_dirtyMaterials = true;
	}
}
