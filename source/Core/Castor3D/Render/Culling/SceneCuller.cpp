#include "Castor3D/Render/Culling/SceneCuller.hpp"

#include "Castor3D/DebugDefines.hpp"
#include "Castor3D/Engine.hpp"
#include "Castor3D/Limits.hpp"
#include "Castor3D/Cache/BillboardCache.hpp"
#include "Castor3D/Cache/GeometryCache.hpp"
#include "Castor3D/Cache/ObjectCache.hpp"
#include "Castor3D/Material/Material.hpp"
#include "Castor3D/Material/Pass/Pass.hpp"
#include "Castor3D/Model/Mesh/Mesh.hpp"
#include "Castor3D/Model/Mesh/Submesh/Submesh.hpp"
#include "Castor3D/Model/Skeleton/Skeleton.hpp"
#include "Castor3D/Render/RenderNodesPass.hpp"
#include "Castor3D/Render/RenderSystem.hpp"
#include "Castor3D/Render/Culling/PipelineNodes.hpp"
#include "Castor3D/Render/Node/BillboardRenderNode.hpp"
#include "Castor3D/Render/Node/SceneRenderNodes.hpp"
#include "Castor3D/Render/Node/SubmeshRenderNode.hpp"
#include "Castor3D/Scene/BillboardList.hpp"
#include "Castor3D/Scene/Camera.hpp"
#include "Castor3D/Scene/Geometry.hpp"
#include "Castor3D/Scene/MovableObject.hpp"
#include "Castor3D/Scene/Scene.hpp"
#include "Castor3D/Scene/SceneNode.hpp"
#include "Castor3D/Scene/Animation/AnimatedMesh.hpp"
#include "Castor3D/Scene/Animation/AnimatedSkeleton.hpp"
#include "Castor3D/Scene/ParticleSystem/ParticleSystem.hpp"

#include <CastorUtils/Miscellaneous/BitSize.hpp>
#include <CastorUtils/Miscellaneous/BlockTimer.hpp>
#include <CastorUtils/Miscellaneous/Hash.hpp>

CU_ImplementSmartPtr( castor3d, SceneCuller )

namespace castor3d
{
	//*********************************************************************************************

	namespace cull
	{
		static bool isSphereVisible( Frustum const & frustum
			, SceneNode const & sceneNode
			, Geometry const & instance
			, Submesh const & data )
		{
			return frustum.isVisible( instance.getBoundingSphere( data )
				, instance.getGlobalTransform()
				, sceneNode.getDerivedScale() );
		}

		static bool isBoxVisible( Frustum const & frustum
			, SceneNode const & sceneNode
			, Geometry const & instance
			, Submesh const & data )
		{
			return frustum.isVisible( instance.getBoundingBox( data )
				, instance.getGlobalTransform() );
		}
	}

	//*********************************************************************************************

	AnimatedObjectRPtr findAnimatedObject( Scene const & scene
		, castor::String const & name )
	{
		AnimatedObjectRPtr result{};
		auto & cache = scene.getAnimatedObjectGroupCache();
		using LockType = castor::UniqueLock< AnimatedObjectGroupCache const >;
		LockType lock{ castor::makeUniqueLock( cache ) };

		for ( auto const & [_, group] : cache )
		{
			if ( !result )
			{
				if ( auto it = group->getObjects().find( name );
					it != group->getObjects().end() )
				{
					result = it->second.get();
				}
			}
		}

		return result;
	}

	//*********************************************************************************************

	size_t hash( SubmeshRenderNode const & culled )
	{
		return hash( culled.instance, culled.data, *culled.pass );
	}

	bool isVisible( Camera const & camera
		, SubmeshRenderNode const & node )
	{
		return isVisible( camera.getFrustum(), node );
	}

	bool isVisible( Frustum const & frustum
		, SubmeshRenderNode const & node )
	{
		auto sceneNode = node.instance.getParent();
		return sceneNode
			&& sceneNode->isDisplayable()
			&& sceneNode->isVisible()
			&& ( node.data.getInstantiation().isInstanced( *node.pass ) // Don't cull individual instances
				|| ( cull::isSphereVisible( frustum, *sceneNode, node.instance, node.data )
					&& cull::isBoxVisible( frustum, *sceneNode, node.instance, node.data ) ) );
	}

	//*********************************************************************************************

	size_t hash( BillboardRenderNode const & culled )
	{
		return hash( culled.data, *culled.pass );
	}

	bool isVisible( Camera const & camera
		, BillboardRenderNode const & node )
	{
		return isVisible( camera.getFrustum(), node );
	}

	bool isVisible( Frustum const &
		, BillboardRenderNode const & node )
	{
		auto sceneNode = node.instance.getNode();
		return sceneNode
			&& sceneNode->isDisplayable()
			&& sceneNode->isVisible();
	}

	//*********************************************************************************************

	SceneCuller::SceneCuller( Scene & scene
		, Camera * camera
		, std::optional< bool > isStatic )
		: m_scene{ scene }
		, m_camera{ camera }
		, m_isStatic{ isStatic }
	{
		m_scene.getRenderNodes().registerCuller( *this );
#if C3D_DebugTimers
		auto & device = m_scene.getEngine()->getRenderSystem()->getRenderDevice();
		m_timer = castor::makeUnique< FramePassTimer >( device.makeContext(), cuT( "Culling/General" ), crg::TimerScope::eUpdate );
		m_timerDirty = castor::makeUnique< FramePassTimer >( device.makeContext(), cuT( "Culling/Dirty" ), crg::TimerScope::eUpdate );
		m_timerCompute = castor::makeUnique< FramePassTimer >( device.makeContext(), cuT( "Culling/Compute" ), crg::TimerScope::eUpdate );
		m_scene.getEngine()->registerTimer( cuT( "Culling/General" ), *m_timer );
		m_scene.getEngine()->registerTimer( cuT( "Culling/Dirty" ), *m_timerDirty );
		m_scene.getEngine()->registerTimer( cuT( "Culling/Compute" ), *m_timerCompute );
#endif
	}

	SceneCuller::~SceneCuller()noexcept
	{
		m_scene.getRenderNodes().unregisterCuller( *this );
	}

	void SceneCuller::update( CpuUpdater & updater )
	{
#if C3D_DebugTimers
		auto block( m_timer->start() );
#endif
		m_anyChanged = false;
		m_culledChanged = false;
		auto sceneIt = updater.dirtyScenes.find( &m_scene );

		if ( !m_first
			&& ( sceneIt == updater.dirtyScenes.end()
				|| sceneIt->second.isEmpty() ) )
		{
			return;
		}

		if ( m_first )
		{
			doInitialiseCulled();
		}
		else
		{
			auto & sceneObjs = sceneIt->second;
			doUpdateChanged( sceneObjs );
			doUpdateCulled( sceneObjs );
		}

		if ( m_culledChanged )
		{
			onCompute( *this );
		}
	}

	void SceneCuller::removeCulled( SubmeshRenderNode const & node )
	{
		auto it = std::find_if( m_culledSubmeshes.begin()
			, m_culledSubmeshes.end()
			, [&node]( CulledNodePtrT< SubmeshRenderNode > const & lookup )
			{
				return &node == lookup->node;
			} );

		if ( m_culledSubmeshes.end() != it )
		{
			onSubmeshRemoved( *this, **it, false );
			m_culledSubmeshes.erase( it );
		}
	}

	void SceneCuller::removeCulled( BillboardRenderNode const & node )
	{
		auto it = std::find_if( m_culledBillboards.begin()
			, m_culledBillboards.end()
			, [&node]( CulledNodePtrT< BillboardRenderNode > const & lookup )
			{
				return &node == lookup->node;
			} );

		if ( m_culledBillboards.end() != it )
		{
			onBillboardRemoved( *this, **it, false );
			m_culledBillboards.erase( it );
		}
	}

	void SceneCuller::resetCamera( Camera * camera )
	{
		if ( m_camera != camera )
		{
			m_camera = camera;
			m_first = true;
			m_anyChanged = true;
			m_culledChanged = true;
			m_culledSubmeshes.clear();
			m_culledBillboards.clear();
		}
	}

	void SceneCuller::doInitialiseCulled()
	{
#if C3D_DebugTimers
		auto blockCompute( m_timerCompute->start() );
#endif
		m_anyChanged = true;
		m_culledChanged = true;

		for ( auto const & [_, node] : getScene().getRenderNodes().getSubmeshNodes() )
		{
			if ( m_isStatic == std::nullopt
				|| node->instance.getParent()->isStatic() == m_isStatic )
			{
				m_culledSubmeshes.emplace_back( castor::make_unique< CulledNodeT< SubmeshRenderNode > >( node.get()
					, node->getInstanceCount()
					, isSubmeshVisible( *node ) ) );
			}

			++m_total.objectCount;
			m_total.faceCount += node->data.getFaceCount();
			m_total.vertexCount += node->data.getPointsCount();
		}

		for ( auto & [_, node] : getScene().getRenderNodes().getBillboardNodes() )
		{
			if ( m_isStatic == std::nullopt
				|| node->instance.getNode()->isStatic() == m_isStatic )
			{
				m_culledBillboards.emplace_back( castor::make_unique< CulledNodeT< BillboardRenderNode > >( node.get()
					, node->getInstanceCount()
					, isBillboardVisible( *node ) ) );
			}

			++m_total.billboardCount;
		}

		m_first = m_culledBillboards.empty()
			&& m_culledSubmeshes.empty();
	}

	void SceneCuller::doUpdateChanged( CpuUpdater::DirtyObjects & sceneObjs )
	{
		auto itCamera = std::find( sceneObjs.dirtyCameras.begin()
			, sceneObjs.dirtyCameras.end()
			, m_camera );

		if ( itCamera != sceneObjs.dirtyCameras.end() )
		{
			m_anyChanged = true;
#if C3D_DebugTimers
			auto blockCompute( m_timerCompute->start() );
#endif
			for ( auto const & culled : m_culledSubmeshes )
			{
				auto visible = isSubmeshVisible( *culled->node );
				auto count = culled->node->getInstanceCount();

				if ( culled->visible != visible
					|| culled->instanceCount != count
					|| culled->vertexCount != culled->node->modelData.vertexCount
					|| culled->indexCount != culled->node->modelData.indexCount )
				{
					m_culledChanged = true;
					culled->visible = visible;
					culled->instanceCount = count;
					culled->indexCount = culled->node->modelData.indexCount;
					culled->vertexCount = culled->node->modelData.vertexCount;
					onSubmeshChanged( *this, *culled, visible );
				}
			}

			for ( auto const & node : m_culledBillboards )
			{
				auto visible = isBillboardVisible( *node->node );
				auto count = node->node->getInstanceCount();

				if ( node->visible != visible
					|| node->instanceCount != count )
				{
					m_culledChanged = true;
					node->visible = visible;
					node->instanceCount = count;
					onBillboardChanged( *this, *node, visible );
				}
			}
		}
	}

	void SceneCuller::doUpdateCulled( CpuUpdater::DirtyObjects const & sceneObjs )
	{
		castor::Vector< SubmeshRenderNode const * > dirtySubmeshes;
		castor::Vector< BillboardRenderNode const * > dirtyBillboards;
		doMarkDirty( sceneObjs, dirtySubmeshes, dirtyBillboards );

		if ( !dirtySubmeshes.empty()
			|| !dirtyBillboards.empty() )
		{
#if C3D_DebugTimers
			auto blockCompute( m_timerCompute->start() );
#endif
			duUpdateCulledSubmeshes( dirtySubmeshes );
			duUpdateCulledBillboards( dirtyBillboards );
			m_anyChanged = true;
		}
	}

	void SceneCuller::doMarkDirty( CpuUpdater::DirtyObjects const & sceneObjs
		, castor::Vector< SubmeshRenderNode const * > & dirtySubmeshes
		, castor::Vector< BillboardRenderNode const * > & dirtyBillboards )const
	{
#if C3D_DebugTimers
		auto blockDirty( m_timerDirty->start() );
#endif
		for ( auto geometry : sceneObjs.dirtyGeometries )
		{
			doMakeDirty( *geometry, dirtySubmeshes );
		}

		for ( auto billboard : sceneObjs.dirtyBillboards )
		{
			doMakeDirty( *billboard, dirtyBillboards );
		}
	}

	void SceneCuller::duUpdateCulledSubmeshes( castor::Vector< SubmeshRenderNode const * > const & dirtySubmeshes )
	{
		for ( auto dirty : dirtySubmeshes )
		{
			auto it = std::find_if( m_culledSubmeshes.begin()
				, m_culledSubmeshes.end()
				, [dirty]( CulledNodePtrT< SubmeshRenderNode > const & lookup )
				{
					return lookup->node == dirty;
				} );
			auto visible = isSubmeshVisible( *dirty );
			auto count = dirty->getInstanceCount();

			if ( it != m_culledSubmeshes.end() )
			{
				auto const & culled = *it;

				if ( culled->visible != visible
					|| culled->instanceCount != count
					|| culled->vertexCount != culled->node->modelData.vertexCount
					|| culled->indexCount != culled->node->modelData.indexCount )
				{
					m_culledChanged = true;
					culled->visible = visible;
					culled->instanceCount = count;
					culled->indexCount = culled->node->modelData.indexCount;
					culled->vertexCount = culled->node->modelData.vertexCount;
					onSubmeshChanged( *this, *culled, visible );
				}
			}
			else
			{
				m_culledChanged = true;
				m_culledSubmeshes.emplace_back( castor::make_unique< CulledNodeT< SubmeshRenderNode > >( dirty, 1u, visible ) );
				onSubmeshChanged( *this, *m_culledSubmeshes.back(), visible );
			}
		}
	}

	void SceneCuller::duUpdateCulledBillboards( castor::Vector< BillboardRenderNode const * > const & dirtyBillboards )
	{
		for ( auto dirty : dirtyBillboards )
		{
			auto it = std::find_if( m_culledBillboards.begin()
				, m_culledBillboards.end()
				, [dirty]( CulledNodePtrT< BillboardRenderNode > const & lookup )
				{
					return lookup->node == dirty;
				} );
			auto visible = isBillboardVisible( *dirty );
			auto count = dirty->getInstanceCount();

			if ( it != m_culledBillboards.end() )
			{
				auto const & culled = *it;

				if ( culled->visible != visible
					|| culled->instanceCount != count )
				{
					m_culledChanged = true;
					culled->visible = visible;
					culled->instanceCount = count;
					onBillboardChanged( *this, *culled, visible );
				}
			}
			else
			{
				m_culledChanged = true;
				m_culledBillboards.emplace_back( castor::make_unique< CulledNodeT< BillboardRenderNode > >( dirty, count, visible ) );
				onBillboardChanged( *this, *m_culledBillboards.back(), visible );
			}
		}
	}

	void SceneCuller::doMakeDirty( Geometry const & object
		, castor::Vector< SubmeshRenderNode const * > & dirtySubmeshes )const
	{
		if ( m_isStatic == std::nullopt
			|| object.getParent()->isStatic() == m_isStatic )
		{
			if ( auto mesh = object.getMesh() )
			{
				for ( auto & submesh : *mesh )
				{
					if ( auto material = object.getMaterial( *submesh ) )
					{
						for ( auto & pass : *material )
						{
							if ( auto node = object.getRenderNode( *pass
								, *submesh ) )
							{
								dirtySubmeshes.push_back( node );
							}
						}
					}
				}
			}
		}
	}

	void SceneCuller::doMakeDirty( BillboardBase const & object
		, castor::Vector< BillboardRenderNode const * > & dirtyBillboards )const
	{
		if ( m_isStatic == std::nullopt
			|| object.getNode()->isStatic() == m_isStatic )
		{
			if ( auto material = object.getMaterial() )
			{
				for ( auto const & pass : *material )
				{
					if ( auto node = object.getRenderNode( *pass ) )
					{
						dirtyBillboards.push_back( node );
					}
				}
			}
		}
	}

	//*********************************************************************************************
}
