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
#include "Castor3D/Render/Node/SubmeshRenderNode.hpp"
#include "Castor3D/Scene/BillboardList.hpp"
#include "Castor3D/Scene/Camera.hpp"
#include "Castor3D/Scene/Geometry.hpp"
#include "Castor3D/Scene/Scene.hpp"
#include "Castor3D/Scene/SceneNode.hpp"
#include "Castor3D/Scene/Animation/AnimatedMesh.hpp"
#include "Castor3D/Scene/Animation/AnimatedSkeleton.hpp"
#include "Castor3D/Scene/ParticleSystem/ParticleSystem.hpp"

#include <CastorUtils/Miscellaneous/Hash.hpp>

namespace castor3d
{
	//*********************************************************************************************

	namespace
	{
		static constexpr VkDeviceSize MaxPipelineNodes = 1000ull;
		static constexpr VkDeviceSize MaxSubmeshIdxDrawIndirectCommand = 1000ull;
		static constexpr VkDeviceSize MaxSubmeshNIdxDrawIndirectCommand = 1000ull;
		static constexpr VkDeviceSize MaxBillboardDrawIndirectCommand = 1000ull;

		uint64_t getPipelineHash( RenderNodesPass const & renderPass
			, SubmeshRenderNode const & culled
			, bool isFrontCulled )
		{
			return getPipelineBaseHash( renderPass, culled.data, culled.pass, isFrontCulled );
		}

		uint64_t getPipelineHash( RenderNodesPass const & renderPass
			, BillboardRenderNode const & culled
			, bool isFrontCulled )
		{
			return getPipelineBaseHash( renderPass, culled.data, culled.pass, isFrontCulled );
		}

		void registerPipelineNodes( size_t hash
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

		template< typename NodeT >
		void addRenderNode( NodeT const & node
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
		void addRenderNode( NodeT const & node
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

		//*****************************************************************************************

		uint32_t getPipelineNodeIndex( uint64_t hash
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

		PipelineNodes & getPipelineNodes( uint64_t hash
			, ashes::BufferBase const & buffer
			, std::vector< PipelineBuffer > const & cont
			, PipelineNodes * nodes )
		{
			auto index = getPipelineNodeIndex( hash, buffer, cont );
			return nodes[index];
		}

		//*****************************************************************************************

		uint32_t getInstanceCount( SubmeshRenderNode const & culled )
		{
			auto & instantiation = culled.data.getInstantiation();
			return instantiation.getRefCount( culled.pass.getOwner() );
		}

		uint32_t getInstanceCount( BillboardRenderNode const & culled )
		{
			return culled.data.getCount();
		}

		void fillIndirectCommand( SubmeshRenderNode const & culled
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

		void fillIndirectCommand( SubmeshRenderNode const & culled
			, VkDrawIndirectCommand *& indirectCommands )
		{
			auto & bufferOffsets = culled.data.getBufferOffsets();
			indirectCommands->vertexCount = bufferOffsets.getVertexCount< InterleavedVertex >();
			indirectCommands->instanceCount = getInstanceCount( culled );
			indirectCommands->firstVertex = bufferOffsets.getFirstVertex< InterleavedVertex >();
			indirectCommands->firstInstance = 0u;
			++indirectCommands;
		}

		void fillIndirectCommand( BillboardRenderNode const & culled
			, VkDrawIndirectCommand *& indirectCommands )
		{
			auto & bufferOffsets = culled.data.getGeometryBuffers().bufferOffset;
			indirectCommands->vertexCount = bufferOffsets.getVertexCount< BillboardVertex >();
			indirectCommands->instanceCount = getInstanceCount( culled );
			indirectCommands->firstVertex = bufferOffsets.getFirstVertex< BillboardVertex >();
			indirectCommands->firstInstance = 0u;
			++indirectCommands;
		}

		void fillNodeCommands( SubmeshRenderNode const & node
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

		void fillNodeCommands( SubmeshRenderNode const & node
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

		if ( m_allChanged || m_culledChanged )
		{
			onCompute( *this );
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
		return getPipelineNodeIndex( getPipelineBaseHash( renderPass, submesh, pass, isFrontCulled )
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
		return getPipelineNodeIndex( getPipelineBaseHash( renderPass, billboard, pass, isFrontCulled )
			, buffer
			, it->second.nodesIds );
	}

	void SceneCuller::onSceneChanged( Scene const & scene )
	{
		m_sceneDirty = true;
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

				if ( renderPass.isValidPass( culled->pass )
					&& renderPass.isValidRenderable( culled->instance )
					&& culled->instance.getParent() != renderPass.getIgnoredNode() )
				{
					bool needsFront = renderPass.getRenderMode() == RenderMode::eTransparentOnly
						|| renderPass.forceTwoSided()
						|| culled->pass.isTwoSided()
						|| culled->pass.hasAlphaBlending();
					auto & instantiation = culled->data.getInstantiation();

					if ( instantiation.isInstanced( culled->instance.getMaterial( culled->data ) ) )
					{
						addRenderNode( *culled
							, renderPass
							, false
							, sortedInstancedSubmeshes
							, renderPassIt.second.nodesIds );

						if ( needsFront )
						{
							addRenderNode( *culled
								, renderPass
								, true
								, sortedInstancedSubmeshes
								, renderPassIt.second.nodesIds );
						}
					}
					else
					{
						addRenderNode( *culled
							, renderPass
							, false
							, sortedSubmeshes
							, renderPassIt.second.nodesIds );

						if ( needsFront )
						{
							addRenderNode( *culled
								, renderPass
								, true
								, sortedSubmeshes
								, renderPassIt.second.nodesIds );
						}
					}
				}
			}

			auto & sortedBillboards = renderPassIt.second.sortedBillboards;

			for ( auto & culled : m_culledBillboards )
			{
				culled->pass.prepareTextures();

				if ( renderPass.isValidPass( culled->pass )
					&& renderPass.isValidRenderable( culled->instance )
					&& culled->instance.getNode() != renderPass.getIgnoredNode() )
				{
					addRenderNode( *culled
						, renderPass
						, false
						, sortedBillboards
						, renderPassIt.second.nodesIds );
				}
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
					, MaxPipelineNodes
					, VK_BUFFER_USAGE_STORAGE_BUFFER_BIT | VK_BUFFER_USAGE_TRANSFER_DST_BIT
					, VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT
					, renderPassIt.first->getTypeName() + "/NodesIDs" );
			}

			if ( !renderPassIt.second.submeshIdxIndirectCommands
				&& ( !renderPassIt.second.sortedSubmeshes.empty()
					|| !renderPassIt.second.sortedInstancedSubmeshes.empty() ) )
			{
				renderPassIt.second.submeshIdxIndirectCommands = makeBuffer< VkDrawIndexedIndirectCommand >( device
					, MaxSubmeshIdxDrawIndirectCommand
					, VK_BUFFER_USAGE_INDIRECT_BUFFER_BIT | VK_BUFFER_USAGE_TRANSFER_DST_BIT
					, VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT
					, renderPassIt.first->getTypeName() + "/SubmeshIndexedIndirectBuffer" );
				renderPassIt.second.submeshNIdxIndirectCommands = makeBuffer< VkDrawIndirectCommand >( device
					, MaxSubmeshNIdxDrawIndirectCommand
					, VK_BUFFER_USAGE_INDIRECT_BUFFER_BIT | VK_BUFFER_USAGE_TRANSFER_DST_BIT
					, VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT
					, renderPassIt.first->getTypeName() + "/SubmeshIndirectBuffer" );
			}

			if ( !renderPassIt.second.billboardIndirectCommands
				&& !renderPassIt.second.sortedBillboards.empty() )
			{
				renderPassIt.second.billboardIndirectCommands = makeBuffer< VkDrawIndirectCommand >( device
					, MaxBillboardDrawIndirectCommand
					, VK_BUFFER_USAGE_INDIRECT_BUFFER_BIT | VK_BUFFER_USAGE_TRANSFER_DST_BIT
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
							auto & pipelineNodes = getPipelineNodes( pipelineIt.first
								, *bufferIt.first
								, pipelinesNodes
								, nodesIdsBuffer );
							auto pipelinesBuffer = pipelineNodes.data();

							for ( auto & sidedCulled : bufferIt.second )
							{
								fillNodeCommands( *sidedCulled.first
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
									fillNodeCommands( *submeshIt.second.front().first
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
							auto & pipelineNodes = getPipelineNodes( perPipeline.first
								, *perBuffer.first
								, pipelinesNodes
								, nodesIdsBuffer );
							auto pipelinesBuffer = pipelineNodes.data();

							for ( auto & sidedCulled : perBuffer.second )
							{
								auto culled = sidedCulled.first;
								fillIndirectCommand( *culled, indirectBuffer );
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

	//*********************************************************************************************
}
