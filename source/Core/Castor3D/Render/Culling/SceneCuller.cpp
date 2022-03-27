#include "Castor3D/Render/Culling/SceneCuller.hpp"

#include "Castor3D/Engine.hpp"
#include "Castor3D/Cache/BillboardCache.hpp"
#include "Castor3D/Cache/GeometryCache.hpp"
#include "Castor3D/Cache/ObjectCache.hpp"
#include "Castor3D/Material/Material.hpp"
#include "Castor3D/Material/Pass/Pass.hpp"
#include "Castor3D/Model/Mesh/Mesh.hpp"
#include "Castor3D/Model/Mesh/Submesh/Submesh.hpp"
#include "Castor3D/Render/RenderNodesPass.hpp"
#include "Castor3D/Render/RenderSystem.hpp"
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

#include <CastorUtils/Miscellaneous/Hash.hpp>

namespace castor3d
{
	//*********************************************************************************************

	namespace cullscn
	{
		static constexpr VkDeviceSize MaxPipelineNodes = 1000ull;
		static constexpr VkDeviceSize MaxSubmeshIdxDrawIndirectCommand = 1000ull;
		static constexpr VkDeviceSize MaxSubmeshNIdxDrawIndirectCommand = 1000ull;
		static constexpr VkDeviceSize MaxBillboardDrawIndirectCommand = 1000ull;

		static uint64_t getPipelineHash( RenderNodesPass const & renderPass
			, SubmeshRenderNode const & culled
			, bool isFrontCulled )
		{
			return getPipelineBaseHash( renderPass, culled.data, culled.pass, isFrontCulled );
		}

		static uint64_t getPipelineHash( RenderNodesPass const & renderPass
			, BillboardRenderNode const & culled
			, bool isFrontCulled )
		{
			return getPipelineBaseHash( renderPass, culled.data, culled.pass, isFrontCulled );
		}

		static void registerPipelineNodes( size_t hash
			, ashes::BufferBase const & buffer
			, std::vector< PipelineBuffer > & cont )
		{
			auto it = std::find_if( cont.begin()
				, cont.end()
				, [hash, &buffer]( PipelineBuffer const & lookup )
				{
					return lookup.first == hash
						&& lookup.second == &buffer;
				} );

			if ( it == cont.end() )
			{
				cont.emplace_back( hash, &buffer );
			}
		}

		static void unregisterPipelineNodes( size_t hash
			, ashes::BufferBase const & buffer
			, std::vector< PipelineBuffer > & cont )
		{
			auto it = std::find_if( cont.begin()
				, cont.end()
				, [hash, &buffer]( PipelineBuffer const & lookup )
				{
					return lookup.first == hash
						&& lookup.second == &buffer;
				} );

			if ( it != cont.end() )
			{
				cont.erase( it );
			}
		}

		template< typename NodeT >
		static void addRenderNode( NodeT const & node
			, RenderNodesPass const & renderPass
			, bool isFrontCulled
			, SceneCuller::SidedNodePipelineMapT< NodeT > & sortedNodes
			, SceneCuller::PipelineBufferArray & nodesIds )
		{
			auto hash = getPipelineHash( renderPass, node, isFrontCulled );
			auto & pipelineNodes = sortedNodes.emplace( hash, SceneCuller::SidedNodeBufferMapT< NodeT >{} ).first->second;
			auto buffer = &node.data.getBufferOffsets().vtxBuffer->getBuffer().getBuffer();
			auto bufferIres = pipelineNodes.emplace( buffer, SceneCuller::SidedNodeArrayT< NodeT >{} );
			bufferIres.first->second.emplace_back( &node, isFrontCulled );
			registerPipelineNodes( hash, *buffer, nodesIds );
		}

		template< typename NodeT >
		static void addRenderNode( NodeT const & node
			, RenderNodesPass const & renderPass
			, bool isFrontCulled
			, SceneCuller::SidedObjectNodePipelineMapT< NodeT > & sortedInstancedSubmeshes
			, SceneCuller::PipelineBufferArray & nodesIds )
		{
			auto hash = getPipelineHash( renderPass, node, isFrontCulled );
			auto & pipelineNodes = sortedInstancedSubmeshes.emplace( hash, SceneCuller::SidedObjectNodeBufferMapT< SubmeshRenderNode >{} ).first->second;
			auto buffer = &node.data.getBufferOffsets().vtxBuffer->getBuffer().getBuffer();
			auto bufferIres = pipelineNodes.emplace( buffer, SceneCuller::SidedObjectNodePassMapT< SubmeshRenderNode >{} );
			auto & passNodes = bufferIres.first->second.emplace( &node.pass, SceneCuller::SidedObjectNodeMapT< SubmeshRenderNode >{} ).first->second;
			auto & objectNodes = passNodes.emplace( &node.data, SceneCuller::SidedNodeArrayT< SubmeshRenderNode >{} ).first->second;
			objectNodes.emplace_back( &node, isFrontCulled );
			registerPipelineNodes( hash, *buffer, nodesIds );
		}

		template< typename NodeT >
		static void removeRenderNode( NodeT const & node
			, RenderNodesPass const & renderPass
			, bool isFrontCulled
			, SceneCuller::SidedNodePipelineMapT< NodeT > & sortedNodes
			, SceneCuller::PipelineBufferArray & nodesIds )
		{
			auto hash = getPipelineHash( renderPass, node, isFrontCulled );
			auto pipelineNodesIt = sortedNodes.find( hash );

			if ( pipelineNodesIt != sortedNodes.end() )
			{
				auto & pipelineNodes = pipelineNodesIt->second;
				auto buffer = &node.data.getBufferOffsets().vtxBuffer->getBuffer().getBuffer();
				auto bufferIt = pipelineNodes.find( buffer );

				if ( bufferIt != pipelineNodes.end() )
				{
					auto nodeIt = std::find_if( bufferIt->second.begin()
						, bufferIt->second.end()
						, [&node, isFrontCulled]( auto & lookup )
						{
							return lookup.first == &node
								&& lookup.second == isFrontCulled;
						} );

					if ( nodeIt != bufferIt->second.end() )
					{
						bufferIt->second.erase( nodeIt );
						unregisterPipelineNodes( hash, *buffer, nodesIds );
					}
				}
			}
		}

		template< typename NodeT >
		static void removeRenderNode( NodeT const & node
			, RenderNodesPass const & renderPass
			, bool isFrontCulled
			, SceneCuller::SidedObjectNodePipelineMapT< NodeT > & sortedInstancedSubmeshes
			, SceneCuller::PipelineBufferArray & nodesIds )
		{
			auto hash = getPipelineHash( renderPass, node, isFrontCulled );
			auto pipelineNodesIt = sortedInstancedSubmeshes.find( hash );

			if ( pipelineNodesIt != sortedInstancedSubmeshes.end() )
			{
				auto & pipelineNodes = pipelineNodesIt->second;
				auto buffer = &node.data.getBufferOffsets().vtxBuffer->getBuffer().getBuffer();
				auto bufferIt = pipelineNodes.find( buffer );

				if ( bufferIt != pipelineNodes.end() )
				{
					auto passNodesIt = bufferIt->second.find( &node.pass );

					if ( passNodesIt != bufferIt->second.end() )
					{
						auto objectNodesIt = passNodesIt->second.find( &node.data );

						if ( objectNodesIt != passNodesIt->second.end() )
						{
							auto nodeIt = std::find_if( objectNodesIt->second.begin()
								, objectNodesIt->second.end()
								, [&node, isFrontCulled]( auto & lookup )
								{
									return lookup.first == &node
										&& lookup.second == isFrontCulled;
								} );

							if ( nodeIt != objectNodesIt->second.end() )
							{
								objectNodesIt->second.erase( nodeIt );
								unregisterPipelineNodes( hash, *buffer, nodesIds );
							}
						}
					}
				}
			}
		}

		//*****************************************************************************************

		static uint32_t getPipelineNodeIndex( uint64_t hash
			, ashes::BufferBase const & buffer
			, std::vector< PipelineBuffer > const & cont )
		{
			auto it = std::find_if( cont.begin()
				, cont.end()
				, [hash, &buffer]( PipelineBuffer const & lookup )
				{
					return lookup.first == hash
						&& lookup.second == &buffer;
				} );
			CU_Require( it != cont.end() );
			return uint32_t( std::distance( cont.begin(), it ) );
		}

		static PipelineNodes & getPipelineNodes( uint64_t hash
			, ashes::BufferBase const & buffer
			, std::vector< PipelineBuffer > const & cont
			, PipelineNodes * nodes )
		{
			auto index = getPipelineNodeIndex( hash, buffer, cont );
			return nodes[index];
		}

		//*****************************************************************************************

		static uint32_t getInstanceCount( SubmeshRenderNode const & culled )
		{
			auto & instantiation = culled.data.getInstantiation();
			return instantiation.getRefCount( culled.pass.getOwner() );
		}

		static uint32_t getInstanceCount( BillboardRenderNode const & culled )
		{
			return culled.data.getCount();
		}

		static void fillIndirectCommand( SubmeshRenderNode const & culled
			, VkDrawIndexedIndirectCommand *& indirectIndexedCommands )
		{
			auto & bufferOffsets = culled.data.getBufferOffsets();
			indirectIndexedCommands->indexCount = bufferOffsets.getIndexCount();
			indirectIndexedCommands->instanceCount = getInstanceCount( culled );
			indirectIndexedCommands->firstIndex = bufferOffsets.getFirstIndex();
			indirectIndexedCommands->vertexOffset = int32_t( bufferOffsets.getFirstVertex< InterleavedVertex >() );
			indirectIndexedCommands->firstInstance = 0u;
			++indirectIndexedCommands;
		}

		static void fillIndirectCommand( SubmeshRenderNode const & culled
			, VkDrawIndirectCommand *& indirectCommands )
		{
			auto & bufferOffsets = culled.data.getBufferOffsets();
			indirectCommands->vertexCount = bufferOffsets.getVertexCount< InterleavedVertex >();
			indirectCommands->instanceCount = getInstanceCount( culled );
			indirectCommands->firstVertex = bufferOffsets.getFirstVertex< InterleavedVertex >();
			indirectCommands->firstInstance = 0u;
			++indirectCommands;
		}

		static void fillIndirectCommand( BillboardRenderNode const & culled
			, VkDrawIndirectCommand *& indirectCommands )
		{
			auto & bufferOffsets = culled.data.getGeometryBuffers().bufferOffset;
			indirectCommands->vertexCount = bufferOffsets.getVertexCount< BillboardVertex >();
			indirectCommands->instanceCount = getInstanceCount( culled );
			indirectCommands->firstVertex = bufferOffsets.getFirstVertex< BillboardVertex >();
			indirectCommands->firstInstance = 0u;
			++indirectCommands;
		}

		static void fillNodeCommands( SubmeshRenderNode const & node
			, VkDrawIndexedIndirectCommand *& indirectIdxBuffer
			, VkDrawIndirectCommand *& indirectNIdxBuffer )
		{
			if ( node.data.getBufferOffsets().hasIndices() )
			{
				fillIndirectCommand( node, indirectIdxBuffer );
			}
			else
			{
				fillIndirectCommand( node, indirectNIdxBuffer );
			}
		}

		static void fillNodeCommands( SubmeshRenderNode const & node
			, Scene const & scene
			, VkDrawIndexedIndirectCommand *& indirectIdxBuffer
			, VkDrawIndirectCommand *& indirectNIdxBuffer
			, castor::Point4ui *& pipelinesBuffer )
		{
			fillNodeCommands( node
				, indirectIdxBuffer
				, indirectNIdxBuffer );
			auto submeshFlags = node.data.getProgramFlags( node.pass.getOwner() );

			auto mesh = checkFlag( submeshFlags, ProgramFlag::eMorphing )
				? std::static_pointer_cast< AnimatedMesh >( findAnimatedObject( scene, node.instance.getName() + cuT( "_Mesh" ) ) )
				: nullptr;
			auto skeleton = checkFlag( submeshFlags, ProgramFlag::eSkinning )
				? std::static_pointer_cast< AnimatedSkeleton >( findAnimatedObject( scene, node.instance.getName() + cuT( "_Skeleton" ) ) )
				: nullptr;

			( *pipelinesBuffer )->x = node.instance.getId( node.pass, node.data );

			if ( mesh )
			{
				( *pipelinesBuffer )->y = mesh->getId() - 1u;
			}

			if ( skeleton )
			{
				( *pipelinesBuffer )->z = skeleton->getId() - 1u;
			}

			++pipelinesBuffer;
		}
	}

	//*********************************************************************************************

	uint64_t getPipelineBaseHash( ProgramFlags programFlags
		, PassFlags passFlags
		, uint32_t texturesCount
		, TextureFlags texturesFlags
		, uint32_t layerIndex )
	{
		remFlag( programFlags, ProgramFlag::eLighting );
		remFlag( programFlags, ProgramFlag::eShadowMapDirectional );
		remFlag( programFlags, ProgramFlag::eShadowMapSpot );
		remFlag( programFlags, ProgramFlag::eShadowMapPoint );
		remFlag( programFlags, ProgramFlag::eEnvironmentMapping );
		remFlag( programFlags, ProgramFlag::eDepthPass );
		remFlag( programFlags, ProgramFlag::eHasGeometry );
		remFlag( programFlags, ProgramFlag::eHasTessellation );
		remFlag( programFlags, ProgramFlag::eConservativeRasterization );
		remFlag( passFlags, PassFlag::eAlphaBlending );
		remFlag( passFlags, PassFlag::eDrawEdge );
		uint32_t result{ programFlags };
		castor::hashCombine32( result, uint32_t( passFlags ) );
		castor::hashCombine32( result, texturesCount );
		castor::hashCombine32( result, uint32_t( texturesFlags ) );
		castor::hashCombine32( result, checkFlag( programFlags, ProgramFlag::eInvertNormals ) );
		return uint64_t( result ) | ( uint64_t( layerIndex ) << 32ull );
	}

	uint64_t getPipelineBaseHash( RenderNodesPass const & renderPass
		, Submesh const & data
		, Pass const & pass
		, bool isFrontCulled )
	{
		auto programFlags = data.getProgramFlags( pass.getOwner() );

		if ( isFrontCulled )
		{
			addFlag( programFlags, ProgramFlag::eInvertNormals );
		}

		return getPipelineBaseHash( renderPass.adjustFlags( programFlags )
			, renderPass.adjustFlags( pass.getPassFlags() )
			, pass.getTextureUnitsCount()
			, pass.getTextures()
			, pass.getIndex() );
	}

	uint64_t getPipelineBaseHash( RenderNodesPass const & renderPass
		, BillboardBase const & data
		, Pass const & pass
		, bool isFrontCulled )
	{
		auto programFlags = data.getProgramFlags();

		if ( isFrontCulled )
		{
			addFlag( programFlags, ProgramFlag::eInvertNormals );
		}

		return getPipelineBaseHash( renderPass.adjustFlags( programFlags )
			, renderPass.adjustFlags( pass.getPassFlags() )
			, pass.getTextureUnitsCount()
			, pass.getTextures()
			, pass.getIndex() );
	}

	AnimatedObjectSPtr findAnimatedObject( Scene const & scene
		, castor::String const & name )
	{
		AnimatedObjectSPtr result;
		auto & cache = scene.getAnimatedObjectGroupCache();
		using LockType = std::unique_lock< AnimatedObjectGroupCache const >;
		LockType lock{ castor::makeUniqueLock( cache ) };

		for ( auto group : cache )
		{
			if ( !result )
			{
				auto it = group.second->getObjects().find( name );

				if ( it != group.second->getObjects().end() )
				{
					result = it->second;
				}
			}
		}

		return result;
	}

	//*********************************************************************************************

	size_t hash( SubmeshRenderNode const & culled )
	{
		return hash( culled.instance, culled.data, culled.pass );
	}

	bool isCulled( SubmeshRenderNode const & node )
	{
		return node.instance.isCulled();
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
			&& ( node.data.getInstantiation().isInstanced( node.pass.getOwner() )
				|| ( frustum.isVisible( node.instance.getBoundingSphere( node.data )
					, sceneNode->getDerivedTransformationMatrix()
					, sceneNode->getDerivedScale() )
					&& frustum.isVisible( node.instance.getBoundingBox( node.data )
						, sceneNode->getDerivedTransformationMatrix() ) ) );
	}

	//*********************************************************************************************

	size_t hash( BillboardRenderNode const & culled )
	{
		return hash( culled.data, culled.pass );
	}

	bool isCulled( BillboardRenderNode const & node )
	{
		return node.instance.isCulled();
	}

	bool isVisible( Camera const & camera
		, BillboardRenderNode const & node )
	{
		return isVisible( camera.getFrustum(), node );
	}

	bool isVisible( Frustum const & frustum
		, BillboardRenderNode const & node )
	{
		auto sceneNode = node.instance.getNode();
		return sceneNode
			&& sceneNode->isDisplayable()
			&& sceneNode->isVisible();
	}

	//*********************************************************************************************

	SceneCuller::SceneCuller( Scene & scene
		, Camera * camera )
		: m_scene{ scene }
		, m_camera{ camera }
	{
		m_sceneChanged = m_scene.onChanged.connect( [this]( Scene const & )
			{
				onSceneChanged( m_scene );
			} );
		m_sceneNodeChanged = m_scene.onNodeChanged.connect( [this]( SceneNode const & node )
			{
				onSceneNodeChanged( node );
			} );

		if ( m_camera )
		{
			m_cameraChanged = m_camera->onChanged.connect( [this]( Camera const & cam )
				{
					onCameraChanged( cam );
				} );
		}
	}

	void SceneCuller::registerRenderPass( RenderNodesPass const & renderPass )
	{
		m_renderPasses.emplace( &renderPass, RenderPassBuffers{} );
	}

	void SceneCuller::unregisterRenderPass( RenderNodesPass const & renderPass )
	{
		auto it = m_renderPasses.find( &renderPass );
		CU_Require( it != m_renderPasses.end() );
		m_renderPasses.erase( it );
	}

	void SceneCuller::compute()
	{
		if ( m_sceneDirty || m_cameraDirty )
		{
			m_allChanged = m_sceneDirty;

			if ( m_allChanged )
			{
				doClearAll();
				doUpdateMinCuller();
				m_cameraDirty = true;
			}

			m_culledChanged = m_cameraDirty;

			if ( m_culledChanged )
			{
				doClearCulled();
				doCullGeometries();
				doCullBillboards();
				doSortNodes();
				doFillIndirect();
			}

			m_sceneDirty = false;
			m_cameraDirty = false;
			m_dirtySubmeshes.clear();
			m_dirtyBillboards.clear();

			if ( m_allChanged || m_culledChanged )
			{
				onCompute( *this );
			}
		}
		else if ( !m_dirtySubmeshes.empty()
			|| !m_dirtyBillboards.empty() )
		{
			NodeArrayT< SubmeshRenderNode > submeshToRemove;
			NodeArrayT< SubmeshRenderNode > submeshToAdd;
			NodeArrayT< BillboardRenderNode > billboardToRemove;
			NodeArrayT< BillboardRenderNode > billboardToAdd;

			for ( auto node : m_dirtySubmeshes )
			{
				auto it = std::find_if( m_culledSubmeshes.begin()
					, m_culledSubmeshes.end()
					, [&node]( SubmeshRenderNode const * lookup )
					{
						return lookup == node;
					} );

				if ( isSubmeshCulled( *node ) )
				{
					if ( it != m_culledSubmeshes.end() )
					{
						m_culledSubmeshes.erase( it );
						submeshToRemove.push_back( node );
					}
				}
				else if ( it == m_culledSubmeshes.end() )
				{
					m_culledSubmeshes.push_back( node );
					submeshToAdd.push_back( node );
				}
			}

			for ( auto node : m_dirtyBillboards )
			{
				auto it = std::find_if( m_culledBillboards.begin()
					, m_culledBillboards.end()
					, [&node]( BillboardRenderNode const * lookup )
					{
						return lookup == node;
					} );

				if ( !isBillboardCulled( *node ) )
				{
					if ( it != m_culledBillboards.end() )
					{
						m_culledBillboards.erase( it );
						billboardToRemove.push_back( node );
					}
				}
				else if ( it == m_culledBillboards.end() )
				{
					m_culledBillboards.push_back( node );
					billboardToAdd.push_back( node );
				}
			}

			if ( !submeshToRemove.empty()
				|| !billboardToRemove.empty()
				|| !submeshToAdd.empty()
				|| !billboardToAdd.empty() )
			{
				for ( auto & renderPassIt : m_renderPasses )
				{
					auto & renderPass = *renderPassIt.first;
					auto & sortedSubmeshes = renderPassIt.second.sortedSubmeshes;
					auto & sortedInstancedSubmeshes = renderPassIt.second.sortedInstancedSubmeshes;

					for ( auto toRemove : submeshToRemove )
					{
						doRemoveSubmesh( *toRemove
							, renderPass
							, sortedSubmeshes
							, sortedInstancedSubmeshes
							, renderPassIt.second.nodesIds );
					}

					for ( auto toAdd : submeshToAdd )
					{
						doAddSubmesh( *toAdd
							, renderPass
							, sortedSubmeshes
							, sortedInstancedSubmeshes
							, renderPassIt.second.nodesIds );
					}

					auto & sortedBillboards = renderPassIt.second.sortedBillboards;

					for ( auto toRemove : billboardToRemove )
					{
						doRemoveBillboard( *toRemove
							, renderPass
							, sortedBillboards
							, renderPassIt.second.nodesIds );
					}

					for ( auto toAdd : billboardToAdd )
					{
						doAddBillboard( *toAdd
							, renderPass
							, sortedBillboards
							, renderPassIt.second.nodesIds );
					}
				}

				m_dirtySubmeshes.clear();
				m_dirtyBillboards.clear();
				m_culledChanged = true;
				doFillIndirect();
				onCompute( *this );
			}
		}
	}

	uint32_t SceneCuller::getPipelineNodesIndex( RenderNodesPass const & renderPass
		, Submesh const & submesh
		, Pass const & pass
		, ashes::BufferBase const & buffer
		, bool isFrontCulled )const
	{
		auto it = m_renderPasses.find( &renderPass );
		CU_Require( it != m_renderPasses.end() );
		return cullscn::getPipelineNodeIndex( getPipelineBaseHash( renderPass, submesh, pass, isFrontCulled )
			, buffer
			, it->second.nodesIds );
	}

	uint32_t SceneCuller::getPipelineNodesIndex( RenderNodesPass const & renderPass
		, BillboardBase const & billboard
		, Pass const & pass
		, ashes::BufferBase const & buffer
		, bool isFrontCulled )const
	{
		auto it = m_renderPasses.find( &renderPass );
		CU_Require( it != m_renderPasses.end() );
		return cullscn::getPipelineNodeIndex( getPipelineBaseHash( renderPass, billboard, pass, isFrontCulled )
			, buffer
			, it->second.nodesIds );
	}

	void SceneCuller::onSceneChanged( Scene const & scene )
	{
		m_sceneDirty = true;
	}

	void SceneCuller::onSceneNodeChanged( SceneNode const & node )
	{
		std::vector< SceneNode const * > work;
		work.push_back( &node );

		while ( !work.empty() )
		{
			auto & cur = *work.back();
			work.pop_back();

			for ( auto & movable : cur.getObjects() )
			{
				switch ( movable.get().getType() )
				{
				case MovableType::eGeometry:
					doMakeDirty( static_cast< Geometry const & >( movable.get() ) );
					break;
				case MovableType::eBillboard:
					doMakeDirty( static_cast< BillboardBase const & >( static_cast< BillboardList const & >( movable.get() ) ) );
					break;
				case MovableType::eParticleEmitter:
					doMakeDirty( *static_cast< ParticleSystem const & >( movable.get() ).getBillboards() );
					break;
				default:
					break;
				}
			}

			for ( auto & child : cur.getChildren() )
			{
				work.push_back( child.second.lock().get() );
			}
		}
	}

	void SceneCuller::onCameraChanged( Camera const & camera )
	{
		m_cameraDirty = true;
	}

	void SceneCuller::doClearAll()
	{
		m_index = 0u;
		m_minCullersZ = std::numeric_limits< float >::max();
	}

	void SceneCuller::doClearCulled()
	{
		m_culledSubmeshes.clear();
		m_culledBillboards.clear();

		for ( auto & renderPassIt : m_renderPasses )
		{
			renderPassIt.second.sortedSubmeshes.clear();
			renderPassIt.second.sortedInstancedSubmeshes.clear();
			renderPassIt.second.sortedBillboards.clear();
			renderPassIt.second.nodesIds.clear();
		}
	}

	void SceneCuller::doUpdateMinCuller()
	{
		auto & scene = getScene();
		auto lock( castor::makeUniqueLock( scene.getGeometryCache() ) );

		for ( auto primitive : scene.getGeometryCache() )
		{
			if ( primitive.second
				&& primitive.second->getParent() )
			{
				auto & geometry = *primitive.second;
				auto & node = *geometry.getParent();

				if ( auto mesh = primitive.second->getMesh().lock() )
				{
					if ( m_camera )
					{
						auto aabbMin = mesh->getBoundingBox().getMin();
						auto aabbMax = mesh->getBoundingBox().getMax();
						auto & camera = getCamera();
						castor::Point3f corners[8]
						{
							castor::Point3f{ aabbMin[0], aabbMin[1], aabbMin[2] },
							castor::Point3f{ aabbMin[0], aabbMin[1], aabbMax[2] },
							castor::Point3f{ aabbMin[0], aabbMax[1], aabbMin[2] },
							castor::Point3f{ aabbMin[0], aabbMax[1], aabbMax[2] },
							castor::Point3f{ aabbMax[0], aabbMin[1], aabbMin[2] },
							castor::Point3f{ aabbMax[0], aabbMin[1], aabbMax[2] },
							castor::Point3f{ aabbMax[0], aabbMax[1], aabbMin[2] },
							castor::Point3f{ aabbMax[0], aabbMax[1], aabbMax[2] },
						};

						for ( auto & corner : corners )
						{
							m_minCullersZ = std::min( m_minCullersZ
								, ( camera.getView() * node.getDerivedTransformationMatrix() * corner )[2] );
						}
					}
					else
					{
						m_minCullersZ = std::min( m_minCullersZ
							, node.getDerivedPosition()[2] - mesh->getBoundingSphere().getRadius() );
					}
				}
			}
		}
	}

	void SceneCuller::doAddSubmesh( SubmeshRenderNode const & node
		, RenderNodesPass const & renderPass
		, SceneCuller::SidedNodePipelineMapT< SubmeshRenderNode > & sorted
		, SceneCuller::SidedObjectNodePipelineMapT< SubmeshRenderNode > & sortedInstanced
		, SceneCuller::PipelineBufferArray & nodesIds )
	{
		if ( renderPass.isValidPass( node.pass )
			&& renderPass.isValidRenderable( node.instance )
			&& node.instance.getParent() != renderPass.getIgnoredNode() )
		{
			bool needsFront = renderPass.getRenderMode() == RenderMode::eTransparentOnly
				|| renderPass.forceTwoSided()
				|| node.pass.isTwoSided()
				|| node.pass.hasAlphaBlending();
			auto & instantiation = node.data.getInstantiation();

			if ( instantiation.isInstanced( node.instance.getMaterial( node.data ) ) )
			{
				cullscn::addRenderNode( node
					, renderPass
					, false
					, sortedInstanced
					, nodesIds );

				if ( needsFront )
				{
					cullscn::addRenderNode( node
						, renderPass
						, true
						, sortedInstanced
						, nodesIds );
				}
			}
			else
			{
				cullscn::addRenderNode( node
					, renderPass
					, false
					, sorted
					, nodesIds );

				if ( needsFront )
				{
					cullscn::addRenderNode( node
						, renderPass
						, true
						, sorted
						, nodesIds );
				}
			}
		}
	}

	void SceneCuller::doAddBillboard( BillboardRenderNode const & node
		, RenderNodesPass const & renderPass
		, SceneCuller::SidedNodePipelineMapT< BillboardRenderNode > & sorted
		, SceneCuller::PipelineBufferArray & nodesIds )
	{
		if ( renderPass.isValidPass( node.pass )
			&& renderPass.isValidRenderable( node.instance )
			&& node.instance.getNode() != renderPass.getIgnoredNode() )
		{
			cullscn::addRenderNode( node
				, renderPass
				, false
				, sorted
				, nodesIds );
		}
	}

	void SceneCuller::doRemoveSubmesh( SubmeshRenderNode const & node
		, RenderNodesPass const & renderPass
		, SceneCuller::SidedNodePipelineMapT< SubmeshRenderNode > & sorted
		, SceneCuller::SidedObjectNodePipelineMapT< SubmeshRenderNode > & sortedInstanced
		, SceneCuller::PipelineBufferArray & nodesIds )
	{
		if ( renderPass.isValidPass( node.pass )
			&& renderPass.isValidRenderable( node.instance )
			&& node.instance.getParent() != renderPass.getIgnoredNode() )
		{
			bool needsFront = renderPass.getRenderMode() == RenderMode::eTransparentOnly
				|| renderPass.forceTwoSided()
				|| node.pass.isTwoSided()
				|| node.pass.hasAlphaBlending();
			auto & instantiation = node.data.getInstantiation();

			if ( instantiation.isInstanced( node.instance.getMaterial( node.data ) ) )
			{
				cullscn::removeRenderNode( node
					, renderPass
					, false
					, sortedInstanced
					, nodesIds );

				if ( needsFront )
				{
					cullscn::removeRenderNode( node
						, renderPass
						, true
						, sortedInstanced
						, nodesIds );
				}
			}
			else
			{
				cullscn::removeRenderNode( node
					, renderPass
					, false
					, sorted
					, nodesIds );

				if ( needsFront )
				{
					cullscn::removeRenderNode( node
						, renderPass
						, true
						, sorted
						, nodesIds );
				}
			}
		}
	}

	void SceneCuller::doRemoveBillboard( BillboardRenderNode const & node
		, RenderNodesPass const & renderPass
		, SceneCuller::SidedNodePipelineMapT< BillboardRenderNode > & sorted
		, SceneCuller::PipelineBufferArray & nodesIds )
	{
		if ( renderPass.isValidPass( node.pass )
			&& renderPass.isValidRenderable( node.instance )
			&& node.instance.getNode() != renderPass.getIgnoredNode() )
		{
			cullscn::removeRenderNode( node
				, renderPass
				, false
				, sorted
				, nodesIds );
		}
	}

	void SceneCuller::doSortNodes()
	{
		for ( auto & renderPassIt : m_renderPasses )
		{
			auto & renderPass = *renderPassIt.first;
			auto & sortedSubmeshes = renderPassIt.second.sortedSubmeshes;
			auto & sortedInstancedSubmeshes = renderPassIt.second.sortedInstancedSubmeshes;

			for ( auto & culled : m_culledSubmeshes )
			{
				culled->pass.prepareTextures();
				doAddSubmesh( *culled
					, renderPass
					, sortedSubmeshes
					, sortedInstancedSubmeshes
					, renderPassIt.second.nodesIds );
			}

			auto & sortedBillboards = renderPassIt.second.sortedBillboards;

			for ( auto & culled : m_culledBillboards )
			{
				culled->pass.prepareTextures();
				doAddBillboard( *culled
					, renderPass
					, sortedBillboards
					, renderPassIt.second.nodesIds );
			}
		}

		auto & device = getScene().getEngine()->getRenderSystem()->getRenderDevice();

		for ( auto & renderPassIt : m_renderPasses )
		{
			if ( !renderPassIt.second.pipelinesNodes
				&& ( !renderPassIt.second.sortedBillboards.empty()
					|| !renderPassIt.second.sortedSubmeshes.empty()
					|| !renderPassIt.second.sortedInstancedSubmeshes.empty() ) )
			{
				renderPassIt.second.pipelinesNodes = makeBuffer< PipelineNodes >( device
					, cullscn::MaxPipelineNodes
					, VK_BUFFER_USAGE_STORAGE_BUFFER_BIT
					, VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT
					, renderPassIt.first->getTypeName() + "/NodesIDs" );
			}

			if ( !renderPassIt.second.submeshIdxIndirectCommands
				&& ( !renderPassIt.second.sortedSubmeshes.empty()
					|| !renderPassIt.second.sortedInstancedSubmeshes.empty() ) )
			{
				renderPassIt.second.submeshIdxIndirectCommands = makeBuffer< VkDrawIndexedIndirectCommand >( device
					, cullscn::MaxSubmeshIdxDrawIndirectCommand
					, VK_BUFFER_USAGE_INDIRECT_BUFFER_BIT
					, VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT
					, renderPassIt.first->getTypeName() + "/SubmeshIndexedIndirectBuffer" );
				renderPassIt.second.submeshNIdxIndirectCommands = makeBuffer< VkDrawIndirectCommand >( device
					, cullscn::MaxSubmeshNIdxDrawIndirectCommand
					, VK_BUFFER_USAGE_INDIRECT_BUFFER_BIT
					, VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT
					, renderPassIt.first->getTypeName() + "/SubmeshIndirectBuffer" );
			}

			if ( !renderPassIt.second.billboardIndirectCommands
				&& !renderPassIt.second.sortedBillboards.empty() )
			{
				renderPassIt.second.billboardIndirectCommands = makeBuffer< VkDrawIndirectCommand >( device
					, cullscn::MaxBillboardDrawIndirectCommand
					, VK_BUFFER_USAGE_INDIRECT_BUFFER_BIT
					, VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT
					, renderPassIt.first->getTypeName() + "/BillboardIndirectBuffer" );
			}
		}
	}

	void SceneCuller::doFillIndirect()
	{
		for ( auto & renderPassIt : m_renderPasses )
		{
			auto & pipelinesNodes = renderPassIt.second.nodesIds;

			if ( !pipelinesNodes.empty() )
			{
				auto & nodesIds = renderPassIt.second.pipelinesNodes;
				auto nodesIdsBuffer = nodesIds->lock( 0u, ashes::WholeSize, 0u );
				auto & sortedSubmeshes = renderPassIt.second.sortedSubmeshes;
				auto & sortedInstancedSubmeshes = renderPassIt.second.sortedInstancedSubmeshes;

				if ( !sortedSubmeshes.empty()
					|| !sortedInstancedSubmeshes.empty() )
				{
					auto & indirectIdxCommands = renderPassIt.second.submeshIdxIndirectCommands;
					auto & indirectNIdxCommands = renderPassIt.second.submeshNIdxIndirectCommands;
					auto indirectIdxBuffer = indirectIdxCommands->lock( 0u, ashes::WholeSize, 0u );
					auto indirectNIdxBuffer = indirectNIdxCommands->lock( 0u, ashes::WholeSize, 0u );

					for ( auto & pipelineIt : sortedSubmeshes )
					{
						for ( auto & bufferIt : pipelineIt.second )
						{
							auto & pipelineNodes = cullscn::getPipelineNodes( pipelineIt.first
								, *bufferIt.first
								, pipelinesNodes
								, nodesIdsBuffer );
							auto pipelinesBuffer = pipelineNodes.data();

							for ( auto & sidedCulled : bufferIt.second )
							{
								cullscn::fillNodeCommands( *sidedCulled.first
									, m_scene
									, indirectIdxBuffer
									, indirectNIdxBuffer
									, pipelinesBuffer );
							}
						}
					}

					for ( auto & pipelineIt : sortedInstancedSubmeshes )
					{
						for ( auto & bufferIt : pipelineIt.second )
						{
							for ( auto & passIt : bufferIt.second )
							{
								for ( auto & submeshIt : passIt.second )
								{
									cullscn::fillNodeCommands( *submeshIt.second.front().first
										, indirectIdxBuffer
										, indirectNIdxBuffer );
								}
							}
						}
					}

					indirectIdxCommands->flush( 0u, ashes::WholeSize );
					indirectIdxCommands->unlock();
					indirectNIdxCommands->flush( 0u, ashes::WholeSize );
					indirectNIdxCommands->unlock();
				}

				auto & sortedBillboards = renderPassIt.second.sortedBillboards;

				if ( !sortedBillboards.empty() )
				{
					auto & indirectCommands = renderPassIt.second.billboardIndirectCommands;
					auto indirectBuffer = indirectCommands->lock( 0u, ashes::WholeSize, 0u );

					for ( auto & perPipeline : sortedBillboards )
					{
						for ( auto & perBuffer : perPipeline.second )
						{
							auto & pipelineNodes = cullscn::getPipelineNodes( perPipeline.first
								, *perBuffer.first
								, pipelinesNodes
								, nodesIdsBuffer );
							auto pipelinesBuffer = pipelineNodes.data();

							for ( auto & sidedCulled : perBuffer.second )
							{
								auto culled = sidedCulled.first;
								cullscn::fillIndirectCommand( *culled, indirectBuffer );
								( *pipelinesBuffer )->x = culled->instance.getId( culled->pass );
								++pipelinesBuffer;
							}
						}
					}

					indirectCommands->flush( 0u, ashes::WholeSize );
					indirectCommands->unlock();
				}

				nodesIds->flush( 0u, ashes::WholeSize );
				nodesIds->unlock();
			}
		}
	}

	void SceneCuller::doMakeDirty( Geometry const & object )
	{
		if ( auto mesh = object.getMesh().lock() )
		{
			auto & nodes = getScene().getRenderNodes();

			for ( auto & submesh : *mesh )
			{
				if ( auto material = object.getMaterial( *submesh ) )
				{
					for ( auto & pass : *material )
					{
						m_dirtySubmeshes.push_back( nodes.getSubmeshNode( object.getId( *pass
							, *submesh ) ) );
					}
				}
			}
		}
	}

	void SceneCuller::doMakeDirty( BillboardBase const & object )
	{
		auto & nodes = getScene().getRenderNodes();

		if ( auto material = object.getMaterial() )
		{
			for ( auto & pass : *material )
			{
				m_dirtyBillboards.push_back( nodes.getBillboardNode( object.getId( *pass ) ) );
			}
		}
	}

	void SceneCuller::doCullGeometries()
	{
		auto & nodes = getScene().getRenderNodes().getSubmeshNodes();
		auto & culled = m_culledSubmeshes;

		for ( auto & itPass : nodes )
		{
			for ( auto & itNode : itPass.second )
			{
				if ( !isSubmeshCulled( *itNode.second ) )
				{
					culled.push_back( itNode.second.get() );
				}
			}
		}
	}

	void SceneCuller::doCullBillboards()
	{
		auto & nodes = getScene().getRenderNodes().getBillboardNodes();
		auto & culled = m_culledBillboards;

		for ( auto & itPass : nodes )
		{
			for ( auto & itNode : itPass.second )
			{
				if ( !isBillboardCulled( *itNode.second ) )
				{
					culled.push_back( itNode.second.get() );
				}
			}
		}
	}

	//*********************************************************************************************
}
