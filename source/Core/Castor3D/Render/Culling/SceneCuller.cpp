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

#include <CastorUtils/Miscellaneous/BitSize.hpp>
#include <CastorUtils/Miscellaneous/Hash.hpp>

namespace castor3d
{
	//*********************************************************************************************

	namespace cullscn
	{
		static PipelineBaseHash getPipelineHash( RenderNodesPass const & renderPass
			, SubmeshRenderNode const & culled
			, bool isFrontCulled )
		{
			return getPipelineBaseHash( renderPass, culled.data, *culled.pass, isFrontCulled );
		}

		static PipelineBaseHash getPipelineHash( RenderNodesPass const & renderPass
			, BillboardRenderNode const & culled
			, bool isFrontCulled )
		{
			return getPipelineBaseHash( renderPass, culled.data, *culled.pass, isFrontCulled );
		}

		template< typename DataT >
		static PipelineBaseHash getPipelineBaseHash( SubmeshFlags submeshFlags
			, ProgramFlags programFlags
			, PassFlags passFlags
			, uint32_t maxTexcoordSet
			, uint32_t texturesCount
			, TextureFlags texturesFlags
			, uint32_t passLayerIndex
			, VkDeviceSize morphTargetsOffset
			, DataT const * dataPtr )
		{
			remFlag( programFlags, ProgramFlag::eAllOptional );
			remFlag( passFlags, PassFlag::eAllOptional );
			constexpr auto maxSubmeshSize = castor::getBitSize( uint32_t( SubmeshFlag::eAllBase ) );
			constexpr auto maxProgramSize = castor::getBitSize( uint32_t( ProgramFlag::eAllBase ) );
			constexpr auto maxPassSize = castor::getBitSize( uint32_t( PassFlag::eAllBase ) );
			constexpr auto maxTexcoorSetSize = castor::getBitSize( MaxTextureCoordinatesSets );
			constexpr auto maxTextureSize = castor::getBitSize( uint32_t( TextureFlag::eAll ) );
			constexpr auto maxTexturesSize = castor::getBitSize( uint32_t( maxTextureSize ) );
			constexpr auto maxPassLayerSize = castor::getBitSize( MaxPassLayers );
			constexpr auto maxTargetOffsetSize = 64 - maxPassLayerSize;
			static_assert( 64 >= maxSubmeshSize + maxProgramSize + maxPassSize + maxTexcoorSetSize + maxTextureSize + maxTexturesSize );
			auto offset = 0u;
			PipelineBaseHash result{};
			result.a = uint64_t( submeshFlags ) << offset;
			offset += maxSubmeshSize;
			result.a |= uint64_t( programFlags ) << offset;
			offset += maxProgramSize;
			result.a |= uint64_t( passFlags ) << offset;
			offset += maxPassSize;
			result.a |= uint64_t( maxTexcoordSet ) << offset;
			offset += maxTexcoorSetSize;
			result.a |= uint64_t( texturesCount ) << offset;
			offset += maxTexturesSize;
			result.a |= uint64_t( texturesFlags ) << offset;
			CU_Require( passLayerIndex < MaxPassLayers );
			CU_Require( ( morphTargetsOffset >> maxTargetOffsetSize ) == 0 );
			auto hash = morphTargetsOffset;

			if constexpr ( std::is_same_v< DataT, Submesh > )
			{
				castor::hashCombinePtr( hash, *dataPtr );
				hash >>= maxPassLayerSize;
			}

			result.b = hash
				| ( uint64_t( passLayerIndex ) << maxTargetOffsetSize );
			return result;
		}

		static void registerPipelineNodes( PipelineBaseHash hash
			, ashes::BufferBase const & buffer
			, std::vector< PipelineBuffer > & cont )
		{
			auto it = std::find_if( cont.begin()
				, cont.end()
				, [&hash, &buffer]( PipelineBuffer const & lookup )
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
		static void addRenderNode( SceneCuller::CulledNodeT< NodeT > const & culled
			, RenderNodesPass const & renderPass
			, bool isFrontCulled
			, SceneCuller::SidedNodePipelineMapT< NodeT > & sortedNodes
			, SceneCuller::PipelineBufferArray & nodesIds )
		{
			NodeT const & node = *culled.node;
			PipelineBaseHash hash = getPipelineHash( renderPass, node, isFrontCulled );
			SceneCuller::SidedNodeBufferMapT< NodeT > & pipelineNodes = sortedNodes.emplace( hash, SceneCuller::SidedNodeBufferMapT< NodeT >{} ).first->second;
			ashes::BufferBase const * buffer = &node.getFinalBufferOffsets().getBuffer( SubmeshFlag::ePositions );
			SceneCuller::SidedNodeArrayT< NodeT > & bufferNodes = pipelineNodes.emplace( buffer, SceneCuller::SidedNodeArrayT< NodeT >{} ).first->second;

			if ( renderPass.isMeshShading()
				&& std::is_same_v< NodeT, SubmeshRenderNode > )
			{
				auto sidedNodeIt = std::find_if( bufferNodes.begin()
					, bufferNodes.end()
					, [&culled]( SceneCuller::SidedNodeT< NodeT > const & lookup )
					{
						return culled.node == lookup.first.node;
					} );

				if ( sidedNodeIt == bufferNodes.end() )
				{
					bufferNodes.emplace_back( culled, isFrontCulled );
					sidedNodeIt = std::next( bufferNodes.begin(), ptrdiff_t( bufferNodes.size() - 1u ) );
				}

				++sidedNodeIt->first.count;
				registerPipelineNodes( hash, *buffer, nodesIds );
			}
			else
			{
				bufferNodes.emplace_back( culled, isFrontCulled );
				registerPipelineNodes( hash, *buffer, nodesIds );
			}
		}

		template< typename NodeT >
		static void addRenderNode( SceneCuller::CulledNodeT< NodeT > const & culled
			, RenderNodesPass const & renderPass
			, bool isFrontCulled
			, SceneCuller::SidedObjectNodePipelineMapT< NodeT > & sortedInstancedSubmeshes
			, SceneCuller::PipelineBufferArray & nodesIds )
		{
			NodeT const & node = *culled.node;
			PipelineBaseHash hash = getPipelineHash( renderPass, node, isFrontCulled );
			SceneCuller::SidedObjectNodeBufferMapT< NodeT > & pipelineNodes = sortedInstancedSubmeshes.emplace( hash, SceneCuller::SidedObjectNodeBufferMapT< SubmeshRenderNode >{} ).first->second;
			ashes::BufferBase const * buffer = &node.getFinalBufferOffsets().getBuffer( SubmeshFlag::ePositions );
			SceneCuller::SidedObjectNodePassMapT< NodeT > & bufferNodes = pipelineNodes.emplace( buffer, SceneCuller::SidedObjectNodePassMapT< SubmeshRenderNode >{} ).first->second;
			SceneCuller::SidedObjectNodeMapT< NodeT > & passNodes = bufferNodes.emplace( node.pass, SceneCuller::SidedObjectNodeMapT< SubmeshRenderNode >{} ).first->second;
			SceneCuller::SidedNodeArrayT< SubmeshRenderNode > & objectNodes = passNodes.emplace( &node.data, SceneCuller::SidedNodeArrayT< SubmeshRenderNode >{} ).first->second;

			if ( renderPass.isMeshShading()
				&& std::is_same_v< NodeT, SubmeshRenderNode > )
			{
				auto sidedNodeIt = std::find_if( objectNodes.begin()
					, objectNodes.end()
					, [&culled]( SceneCuller::SidedNodeT< NodeT > const & lookup )
					{
						return culled.node == lookup.first.node;
					} );

				if ( sidedNodeIt == objectNodes.end() )
				{
					objectNodes.emplace_back( culled, isFrontCulled );
					sidedNodeIt = std::next( objectNodes.begin(), ptrdiff_t( objectNodes.size() - 1u ) );
				}

				++sidedNodeIt->first.count;
				registerPipelineNodes( hash, *buffer, nodesIds );
			}
			else
			{
				objectNodes.emplace_back( culled, isFrontCulled );
				registerPipelineNodes( hash, *buffer, nodesIds );
			}
		}

		//*****************************************************************************************

		static uint32_t getPipelineNodeIndex( PipelineBaseHash hash
			, ashes::BufferBase const & buffer
			, std::vector< PipelineBuffer > const & cont )
		{
			auto it = std::find_if( cont.begin()
				, cont.end()
				, [&hash, &buffer]( PipelineBuffer const & lookup )
				{
					return lookup.first == hash
						&& lookup.second == &buffer;
				} );
			CU_Require( it != cont.end() );
			return uint32_t( std::distance( cont.begin(), it ) );
		}

		static PipelineNodes & getPipelineNodes( PipelineBaseHash hash
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
			return instantiation.getRefCount( culled.pass->getOwner() );
		}

		static uint32_t getInstanceCount( BillboardRenderNode const & culled )
		{
			return culled.data.getCount();
		}

		static void fillIndirectCommand( SubmeshRenderNode const & culled
			, VkDrawIndexedIndirectCommand *& indirectIndexedCommands
			, uint32_t instanceCount )
		{
			auto & indexOffset = culled.getSourceBufferOffsets().getBufferChunk( SubmeshFlag::eIndex );
			auto & bufferOffsets = culled.getFinalBufferOffsets();
			indirectIndexedCommands->indexCount = indexOffset.getCount< uint32_t >();
			indirectIndexedCommands->instanceCount = instanceCount;
			indirectIndexedCommands->firstIndex = indexOffset.getFirst< uint32_t >();
			indirectIndexedCommands->vertexOffset = int32_t( bufferOffsets.getFirstVertex< castor::Point4f >() );
			indirectIndexedCommands->firstInstance = 0u;
			++indirectIndexedCommands;
		}

		static void fillIndirectCommand( SubmeshRenderNode const & culled
			, VkDrawIndirectCommand *& indirectCommands
			, uint32_t instanceCount )
		{
			auto & bufferOffsets = culled.getFinalBufferOffsets();
			indirectCommands->vertexCount = bufferOffsets.getCount< castor::Point4f >( SubmeshFlag::ePositions );
			indirectCommands->instanceCount = instanceCount;
			indirectCommands->firstVertex = bufferOffsets.getFirstVertex< castor::Point3f >();
			indirectCommands->firstInstance = 0u;
			++indirectCommands;
		}

		static void fillIndirectCommand( BillboardRenderNode const & culled
			, VkDrawIndirectCommand *& indirectCommands )
		{
			auto & bufferOffsets = culled.getFinalBufferOffsets();
			indirectCommands->vertexCount = bufferOffsets.getCount< BillboardVertex >( SubmeshFlag::ePositions );
			indirectCommands->instanceCount = getInstanceCount( culled );
			indirectCommands->firstVertex = bufferOffsets.getFirstVertex< BillboardVertex >();
			indirectCommands->firstInstance = 0u;
			++indirectCommands;
		}

		static void fillNodeCommands( SubmeshRenderNode const & node
			, VkDrawIndexedIndirectCommand *& indirectIdxBuffer
			, VkDrawIndirectCommand *& indirectNIdxBuffer
			, uint32_t instanceCount )
		{
			if ( node.getSourceBufferOffsets().hasData( SubmeshFlag::eIndex ) )
			{
				fillIndirectCommand( node, indirectIdxBuffer, instanceCount );
			}
			else
			{
				fillIndirectCommand( node, indirectNIdxBuffer, instanceCount );
			}
		}

#ifndef NDEBUG

		static void checkBuffers( SubmeshRenderNode const & firstNode
			, SubmeshRenderNode const & currentNode )
		{
			auto idxBuffer = firstNode.getSourceBufferOffsets().hasData( SubmeshFlag::eIndex )
				? &firstNode.getSourceBufferOffsets().getBuffer( SubmeshFlag::eIndex )
				: nullptr;
			auto posBuffer = firstNode.getFinalBufferOffsets().hasData( SubmeshFlag::ePositions )
				? &firstNode.getFinalBufferOffsets().getBuffer( SubmeshFlag::ePositions )
				: nullptr;
			auto nmlBuffer = firstNode.getFinalBufferOffsets().hasData( SubmeshFlag::eNormals )
				? &firstNode.getFinalBufferOffsets().getBuffer( SubmeshFlag::eNormals )
				: nullptr;
			auto tanBuffer = firstNode.getFinalBufferOffsets().hasData( SubmeshFlag::eTangents )
				? &firstNode.getFinalBufferOffsets().getBuffer( SubmeshFlag::eTangents )
				: nullptr;
			auto tex0Buffer = firstNode.getFinalBufferOffsets().hasData( SubmeshFlag::eTexcoords0 )
				? &firstNode.getFinalBufferOffsets().getBuffer( SubmeshFlag::eTexcoords0 )
				: nullptr;
			auto tex1Buffer = firstNode.getFinalBufferOffsets().hasData( SubmeshFlag::eTexcoords1 )
				? &firstNode.getFinalBufferOffsets().getBuffer( SubmeshFlag::eTexcoords1 )
				: nullptr;
			auto tex2Buffer = firstNode.getFinalBufferOffsets().hasData( SubmeshFlag::eTexcoords2 )
				? &firstNode.getFinalBufferOffsets().getBuffer( SubmeshFlag::eTexcoords2 )
				: nullptr;
			auto tex3Buffer = firstNode.getFinalBufferOffsets().hasData( SubmeshFlag::eTexcoords3 )
				? &firstNode.getFinalBufferOffsets().getBuffer( SubmeshFlag::eTexcoords3 )
				: nullptr;
			auto colBuffer = firstNode.getFinalBufferOffsets().hasData( SubmeshFlag::eColours )
				? &firstNode.getFinalBufferOffsets().getBuffer( SubmeshFlag::eColours )
				: nullptr;
			auto bonBuffer = firstNode.getFinalBufferOffsets().hasData( SubmeshFlag::eSkin )
				? &firstNode.getFinalBufferOffsets().getBuffer( SubmeshFlag::eSkin )
				: nullptr;

			CU_Require( idxBuffer == ( currentNode.getSourceBufferOffsets().hasData( SubmeshFlag::eIndex )
				? &currentNode.getSourceBufferOffsets().getBuffer( SubmeshFlag::eIndex )
				: nullptr ) );
			CU_Require( posBuffer == ( currentNode.getFinalBufferOffsets().hasData( SubmeshFlag::ePositions )
				? &currentNode.getFinalBufferOffsets().getBuffer( SubmeshFlag::ePositions )
				: nullptr ) );
			CU_Require( nmlBuffer == ( currentNode.getFinalBufferOffsets().hasData( SubmeshFlag::eNormals )
				? &currentNode.getFinalBufferOffsets().getBuffer( SubmeshFlag::eNormals )
				: nullptr ) );
			CU_Require( tanBuffer == ( currentNode.getFinalBufferOffsets().hasData( SubmeshFlag::eTangents )
				? &currentNode.getFinalBufferOffsets().getBuffer( SubmeshFlag::eTangents )
				: nullptr ) );
			CU_Require( tex0Buffer == ( currentNode.getFinalBufferOffsets().hasData( SubmeshFlag::eTexcoords0 )
				? &currentNode.getFinalBufferOffsets().getBuffer( SubmeshFlag::eTexcoords0 )
				: nullptr ) );
			CU_Require( tex1Buffer == ( currentNode.getFinalBufferOffsets().hasData( SubmeshFlag::eTexcoords1 )
				? &currentNode.getFinalBufferOffsets().getBuffer( SubmeshFlag::eTexcoords1 )
				: nullptr ) );
			CU_Require( tex2Buffer == ( currentNode.getFinalBufferOffsets().hasData( SubmeshFlag::eTexcoords2 )
				? &currentNode.getFinalBufferOffsets().getBuffer( SubmeshFlag::eTexcoords2 )
				: nullptr ) );
			CU_Require( tex3Buffer == ( currentNode.getFinalBufferOffsets().hasData( SubmeshFlag::eTexcoords3 )
				? &currentNode.getFinalBufferOffsets().getBuffer( SubmeshFlag::eTexcoords3 )
				: nullptr ) );
			CU_Require( colBuffer == ( currentNode.getFinalBufferOffsets().hasData( SubmeshFlag::eColours )
				? &currentNode.getFinalBufferOffsets().getBuffer( SubmeshFlag::eColours )
				: nullptr ) );
			CU_Require( bonBuffer == ( currentNode.getFinalBufferOffsets().hasData( SubmeshFlag::eSkin )
				? &currentNode.getFinalBufferOffsets().getBuffer( SubmeshFlag::eSkin )
				: nullptr ) );
		}

#endif

#if VK_NV_mesh_shader

		static void fillIndirectCommand( SubmeshRenderNode const & culled
			, VkDrawMeshTasksIndirectCommandNV *& indirectMeshCommands
			, uint32_t taskCount )
		{
			indirectMeshCommands->taskCount = taskCount;
			indirectMeshCommands->firstTask = 0u;
			++indirectMeshCommands;
		}

		static void fillNodeCommands( SubmeshRenderNode const & node
			, bool meshShading
			, VkDrawMeshTasksIndirectCommandNV *& indirectMeshBuffer
			, VkDrawIndexedIndirectCommand *& indirectIdxBuffer
			, VkDrawIndirectCommand *& indirectNIdxBuffer
			, uint32_t instanceCount )
		{
			if ( meshShading
				&& node.data.getMeshletsCount()
				&& indirectMeshBuffer )
			{
				fillIndirectCommand( node, indirectMeshBuffer, node.data.getMeshletsCount() );
			}
			else if ( node.getSourceBufferOffsets().hasData( SubmeshFlag::eIndex ) )
			{
				fillIndirectCommand( node, indirectIdxBuffer, instanceCount );
			}
			else
			{
				fillIndirectCommand( node, indirectNIdxBuffer, instanceCount );
			}
		}

		static void fillNodeCommands( SceneCuller::SidedNodeArrayT< SubmeshRenderNode > const & nodes
			, bool meshShading
			, VkDrawMeshTasksIndirectCommandNV *& indirectMeshBuffer
			, VkDrawIndexedIndirectCommand *& indirectIdxBuffer
			, VkDrawIndirectCommand *& indirectNIdxBuffer )
		{
			uint32_t instanceCount = 0u;

			for ( auto & node : nodes )
			{
				if ( node.first.node->instance.getParent()->isVisible() )
				{
					++instanceCount;
#	ifndef NDEBUG
					checkBuffers( *nodes.front().first.node, *node.first.node );
#	endif
				}
			}

			fillNodeCommands( *nodes.front().first.node
				, meshShading
				, indirectMeshBuffer
				, indirectIdxBuffer
				, indirectNIdxBuffer
				, instanceCount );
		}

		static void fillNodeCommands( SubmeshRenderNode const & node
			, Scene const & scene
			, bool meshShading
			, VkDrawMeshTasksIndirectCommandNV *& indirectMeshBuffer
			, VkDrawIndexedIndirectCommand *& indirectIdxBuffer
			, VkDrawIndirectCommand *& indirectNIdxBuffer
			, castor::Point4ui *& pipelinesBuffer )
		{
			fillNodeCommands( node
				, meshShading
				, indirectMeshBuffer
				, indirectIdxBuffer
				, indirectNIdxBuffer
				, getInstanceCount( node ) );
			( *pipelinesBuffer )->x = node.instance.getId( *node.pass, node.data );
			++pipelinesBuffer;
		}

#else

		static void fillNodeCommands( SceneCuller::SidedNodeArrayT< SubmeshRenderNode > const & nodes
			, VkDrawIndexedIndirectCommand *& indirectIdxBuffer
			, VkDrawIndirectCommand *& indirectNIdxBuffer )
		{
			uint32_t instanceCount = 0u;

			for ( auto & node : nodes )
			{
				if ( node.first.node->instance.getParent()->isVisible() )
				{
					++instanceCount;
#ifndef NDEBUG
					checkBuffers( *nodes.front().first.node, *node.first.node );
#endif
				}
			}

			fillNodeCommands( *nodes.front().first.node
				, indirectIdxBuffer
				, indirectNIdxBuffer
				, instanceCount );
		}

		static void fillNodeCommands( SubmeshRenderNode const & node
			, Scene const & scene
			, VkDrawIndexedIndirectCommand *& indirectIdxBuffer
			, VkDrawIndirectCommand *& indirectNIdxBuffer
			, castor::Point4ui *& pipelinesBuffer )
		{
			fillNodeCommands( node
				, indirectIdxBuffer
				, indirectNIdxBuffer
				, getInstanceCount( node ) );
			( *pipelinesBuffer )->x = node.instance.getId( *node.pass, node.data );
			++pipelinesBuffer;
		}

#endif
	}

	//*********************************************************************************************

	PipelineBaseHash getPipelineBaseHash( RenderNodesPass const & renderPass
		, Submesh const & data
		, Pass const & pass
		, bool isFrontCulled )
	{
		auto submeshFlags = data.getSubmeshFlags( &pass );
		auto programFlags = data.getProgramFlags( *pass.getOwner() );

		if ( isFrontCulled )
		{
			addFlag( programFlags, ProgramFlag::eInvertNormals );
		}

		return cullscn::getPipelineBaseHash( renderPass.adjustFlags( submeshFlags )
			, renderPass.adjustFlags( programFlags )
			, renderPass.adjustFlags( pass.getPassFlags() )
			, pass.getMaxTexCoordSet()
			, pass.getTextureUnitsCount()
			, pass.getTextures()
			, pass.getIndex()
			, data.getMorphTargets().getOffset()
			, &data );
	}

	PipelineBaseHash getPipelineBaseHash( RenderNodesPass const & renderPass
		, BillboardBase const & data
		, Pass const & pass
		, bool isFrontCulled )
	{
		auto submeshFlags = data.getSubmeshFlags();
		auto programFlags = data.getProgramFlags();

		if ( isFrontCulled )
		{
			addFlag( programFlags, ProgramFlag::eInvertNormals );
		}

		return cullscn::getPipelineBaseHash( renderPass.adjustFlags( submeshFlags )
			, renderPass.adjustFlags( programFlags )
			, renderPass.adjustFlags( pass.getPassFlags() )
			, pass.getMaxTexCoordSet()
			, pass.getTextureUnitsCount()
			, pass.getTextures()
			, pass.getIndex()
			, 0u
			, &data );
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
			&& ( node.data.getInstantiation().isInstanced( node.pass->getOwner() )		// Don't cull individual instances
				|| ( frustum.isVisible( node.instance.getBoundingSphere( node.data )	// First test against bounding sphere
						, sceneNode->getDerivedTransformationMatrix()
						, sceneNode->getDerivedScale() )
					&& frustum.isVisible( node.instance.getBoundingBox( node.data )		// Then against bounding box
						, sceneNode->getDerivedTransformationMatrix() ) ) );
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
		m_scene.getRenderNodes().registerCuller( *this );
#if C3D_DebugTimers
		auto & device = m_scene.getEngine()->getRenderSystem()->getRenderDevice();
		m_timer = castor::makeUnique< FramePassTimer >( device.makeContext(), cuT( "Culling/General" ) );
		m_timerDirty = castor::makeUnique< FramePassTimer >( device.makeContext(), cuT( "Culling/Dirty" ) );
		m_timerCompute = castor::makeUnique< FramePassTimer >( device.makeContext(), cuT( "Culling/Compute" ) );
		m_timerIndirect = castor::makeUnique< FramePassTimer >( device.makeContext(), cuT( "Culling/Indirect" ) );
		m_scene.getEngine()->registerTimer( cuT( "Culling/General" ), *m_timer );
		m_scene.getEngine()->registerTimer( cuT( "Culling/Dirty" ), *m_timerDirty );
		m_scene.getEngine()->registerTimer( cuT( "Culling/Compute" ), *m_timerCompute );
		m_scene.getEngine()->registerTimer( cuT( "Culling/Indirect" ), *m_timerIndirect );
#endif
	}

	SceneCuller::~SceneCuller()
	{
		m_scene.getRenderNodes().unregisterCuller( *this );
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
			doSortNodes();
			doInitGpuBuffers();
			doFillIndirect();
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

	void SceneCuller::removeCulled( SubmeshRenderNode const & node )
	{
		auto it = std::find_if( m_culledSubmeshes.begin()
			, m_culledSubmeshes.end()
			, [&node]( CulledNodeT< SubmeshRenderNode > const & lookup )
			{
				return &node == lookup.node;
			} );

		if ( m_culledSubmeshes.end() != it )
		{
			m_culledSubmeshes.erase( it );
		}
	}

	void SceneCuller::removeCulled( BillboardRenderNode const & node )
	{
		auto it = std::find_if( m_culledBillboards.begin()
			, m_culledBillboards.end()
			, [&node]( CulledNodeT< BillboardRenderNode > const & lookup )
			{
				return &node == lookup.node;
			} );

		if ( m_culledBillboards.end() != it )
		{
			m_culledBillboards.erase( it );
		}
	}

	void SceneCuller::doInitialiseCulled()
	{
#if C3D_DebugTimers
		auto blockCompute( m_timerCompute->start() );
#endif
		m_first = false;
		m_anyChanged = true;
		m_culledChanged = true;
		auto & submeshNodes = getScene().getRenderNodes().getSubmeshNodes();

		for ( auto & texturesIt : submeshNodes )
		{
			for ( auto & programIt : texturesIt.second )
			{
				m_culledSubmeshes.push_back( { programIt.second.get()
					, isSubmeshVisible( *programIt.second )
					, 0u } );
			}
		}

		auto & billboardNodes = getScene().getRenderNodes().getBillboardNodes();

		for ( auto & texturesIt : billboardNodes )
		{
			for ( auto & programIt : texturesIt.second )
			{
				m_culledBillboards.push_back( { programIt.second.get()
					, isBillboardVisible( *programIt.second )
					, 0u } );
			}
		}
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
			for ( auto & node : m_culledSubmeshes )
			{
				auto visible = isSubmeshVisible( *node.node );
				m_culledChanged = m_culledChanged || node.visible != visible;
				node.visible = visible;
			}

			for ( auto & node : m_culledBillboards )
			{
				auto visible = isBillboardVisible( *node.node );
				m_culledChanged = m_culledChanged || node.visible != visible;
				node.visible = visible;
			}
		}
		else
		{
			m_anyChanged = !sceneObjs.dirtyGeometries.empty()
				|| !sceneObjs.dirtyBillboards.empty();
		}
	}

	void SceneCuller::doUpdateCulled( CpuUpdater::DirtyObjects & sceneObjs )
	{
		std::vector< SubmeshRenderNode const * > dirtySubmeshes;
		std::vector< BillboardRenderNode const * > dirtyBillboards;
		doMarkDirty( sceneObjs, dirtySubmeshes, dirtyBillboards );

		if ( !dirtySubmeshes.empty()
			|| !dirtyBillboards.empty() )
		{
#if C3D_DebugTimers
			auto blockCompute( m_timerCompute->start() );
#endif
			duUpdateCulledSubmeshes( dirtySubmeshes );
			duUpdateCulledBillboards( dirtyBillboards );
		}
	}

	void SceneCuller::doSortNodes()
	{
		for ( auto & renderPassIt : m_renderPasses )
		{
			auto & renderPass = *renderPassIt.first;
			auto & nodesIds = renderPassIt.second.nodesIds;
			auto & sortedSubmeshes = renderPassIt.second.sortedSubmeshes;
			auto & sortedInstancedSubmeshes = renderPassIt.second.sortedInstancedSubmeshes;
			nodesIds.clear();
			sortedSubmeshes.clear();
			sortedInstancedSubmeshes.clear();

			for ( auto toAdd : m_culledSubmeshes )
			{
				doAddSubmesh( toAdd
					, renderPass
					, sortedSubmeshes
					, sortedInstancedSubmeshes
					, nodesIds );
			}

			auto & sortedBillboards = renderPassIt.second.sortedBillboards;
			sortedBillboards.clear();

			for ( auto toAdd : m_culledBillboards )
			{
				doAddBillboard( toAdd
					, renderPass
					, sortedBillboards
					, nodesIds );
			}
		}
	}

	void SceneCuller::doInitGpuBuffers()
	{
		auto & device = getScene().getEngine()->getRenderSystem()->getRenderDevice();

		for ( auto & renderPassIt : m_renderPasses )
		{
			if ( !renderPassIt.second.pipelinesNodes
				&& ( !renderPassIt.second.sortedBillboards.empty()
					|| !renderPassIt.second.sortedSubmeshes.empty()
					|| !renderPassIt.second.sortedInstancedSubmeshes.empty() ) )
			{
				renderPassIt.second.pipelinesNodes = makeBuffer< PipelineNodes >( device
					, MaxPipelines
					, VK_BUFFER_USAGE_STORAGE_BUFFER_BIT
					, VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT
					, renderPassIt.first->getTypeName() + "/NodesIDs" );
			}

			if ( !renderPassIt.second.submeshIdxIndirectCommands
				&& ( !renderPassIt.second.sortedSubmeshes.empty()
					|| !renderPassIt.second.sortedInstancedSubmeshes.empty() ) )
			{
#if VK_NV_mesh_shader
				renderPassIt.second.submeshMeshletIndirectCommands = makeBuffer< VkDrawMeshTasksIndirectCommandNV >( device
					, MaxSubmeshMeshletDrawIndirectCommand
					, VK_BUFFER_USAGE_INDIRECT_BUFFER_BIT
					, VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT
					, renderPassIt.first->getTypeName() + "/SubmeshMeshletIndirectBuffer" );
#endif
				renderPassIt.second.submeshIdxIndirectCommands = makeBuffer< VkDrawIndexedIndirectCommand >( device
					, MaxSubmeshIdxDrawIndirectCommand
					, VK_BUFFER_USAGE_INDIRECT_BUFFER_BIT
					, VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT
					, renderPassIt.first->getTypeName() + "/SubmeshIndexedIndirectBuffer" );
				renderPassIt.second.submeshNIdxIndirectCommands = makeBuffer< VkDrawIndirectCommand >( device
					, MaxSubmeshNIdxDrawIndirectCommand
					, VK_BUFFER_USAGE_INDIRECT_BUFFER_BIT
					, VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT
					, renderPassIt.first->getTypeName() + "/SubmeshIndirectBuffer" );
			}

			if ( !renderPassIt.second.billboardIndirectCommands
				&& !renderPassIt.second.sortedBillboards.empty() )
			{
				renderPassIt.second.billboardIndirectCommands = makeBuffer< VkDrawIndirectCommand >( device
					, MaxBillboardDrawIndirectCommand
					, VK_BUFFER_USAGE_INDIRECT_BUFFER_BIT
					, VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT
					, renderPassIt.first->getTypeName() + "/BillboardIndirectBuffer" );
			}
		}
	}

	void SceneCuller::doFillIndirect()
	{
#if C3D_DebugTimers
		auto block( m_timerIndirect->start() );
#endif
		for ( auto & renderPassIt : m_renderPasses )
		{
			auto & renderPass = *renderPassIt.first;
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
#if VK_NV_mesh_shader
					auto & indirectMshCommands = renderPassIt.second.submeshMeshletIndirectCommands;
					auto origIndirectMshBuffer = indirectMshCommands
						? indirectMshCommands->lock( 0u, ashes::WholeSize, 0u )
						: nullptr;
					auto indirectMshBuffer = origIndirectMshBuffer;
#endif
					auto & indirectIdxCommands = renderPassIt.second.submeshIdxIndirectCommands;
					auto origIndirectIdxBuffer = indirectIdxCommands->lock( 0u, ashes::WholeSize, 0u );
					auto indirectIdxBuffer = origIndirectIdxBuffer;

					auto & indirectNIdxCommands = renderPassIt.second.submeshNIdxIndirectCommands;
					auto origIndirectNIdxBuffer = indirectNIdxCommands->lock( 0u, ashes::WholeSize, 0u );
					auto indirectNIdxBuffer = origIndirectNIdxBuffer;

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
								if ( sidedCulled.first.visible )
								{
#if VK_NV_mesh_shader
									cullscn::fillNodeCommands( *sidedCulled.first.node
										, m_scene
										, renderPass.isMeshShading()
										, indirectMshBuffer
										, indirectIdxBuffer
										, indirectNIdxBuffer
										, pipelinesBuffer );
									CU_Require( size_t( std::distance( origIndirectMshBuffer, indirectMshBuffer ) ) <= indirectMshCommands->getCount() );
#else
									cullscn::fillNodeCommands( *sidedCulled.first.node
										, m_scene
										, indirectIdxBuffer
										, indirectNIdxBuffer
										, pipelinesBuffer );
#endif
									CU_Require( size_t( std::distance( origIndirectIdxBuffer, indirectIdxBuffer ) ) <= indirectIdxCommands->getCount() );
									CU_Require( size_t( std::distance( origIndirectNIdxBuffer, indirectNIdxBuffer ) ) <= indirectNIdxCommands->getCount() );
									CU_Require( size_t( std::distance( pipelineNodes.data(), pipelinesBuffer ) ) <= pipelineNodes.size() );
								}
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
#if VK_NV_mesh_shader
									cullscn::fillNodeCommands( submeshIt.second
										, renderPass.isMeshShading()
										, indirectMshBuffer
										, indirectIdxBuffer
										, indirectNIdxBuffer );
									CU_Require( size_t( std::distance( origIndirectMshBuffer, indirectMshBuffer ) ) <= indirectMshCommands->getCount() );
#else
									cullscn::fillNodeCommands( submeshIt.second
										, indirectIdxBuffer
										, indirectNIdxBuffer );
#endif
									CU_Require( size_t( std::distance( origIndirectIdxBuffer, indirectIdxBuffer ) ) <= indirectIdxCommands->getCount() );
									CU_Require( size_t( std::distance( origIndirectNIdxBuffer, indirectNIdxBuffer ) ) <= indirectNIdxCommands->getCount() );
								}
							}
						}
					}

#if VK_NV_mesh_shader

					if ( indirectMshCommands )
					{
						indirectMshCommands->flush( 0u, ashes::WholeSize );
						indirectMshCommands->unlock();
					}

#endif
					indirectIdxCommands->flush( 0u, ashes::WholeSize );
					indirectIdxCommands->unlock();
					indirectNIdxCommands->flush( 0u, ashes::WholeSize );
					indirectNIdxCommands->unlock();
				}

				auto & sortedBillboards = renderPassIt.second.sortedBillboards;

				if ( !sortedBillboards.empty() )
				{
					auto & indirectCommands = renderPassIt.second.billboardIndirectCommands;
					auto origIndirectBuffer = indirectCommands->lock( 0u, ashes::WholeSize, 0u );
					auto indirectBuffer = origIndirectBuffer;

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

								if ( culled.visible )
								{
									cullscn::fillIndirectCommand( *culled.node, indirectBuffer );
									( *pipelinesBuffer )->x = culled.node->instance.getId( *culled.node->pass );
									++pipelinesBuffer;
									CU_Require( size_t( std::distance( origIndirectBuffer, indirectBuffer ) ) <= indirectCommands->getCount() );
									CU_Require( size_t( std::distance( pipelineNodes.data(), pipelinesBuffer ) ) <= pipelineNodes.size() );
								}
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

	void SceneCuller::doMarkDirty( CpuUpdater::DirtyObjects & sceneObjs
		, std::vector< SubmeshRenderNode const * > & dirtySubmeshes
		, std::vector< BillboardRenderNode const * > & dirtyBillboards )
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

	void SceneCuller::duUpdateCulledSubmeshes( std::vector< SubmeshRenderNode const * > const & dirtySubmeshes )
	{
		for ( auto dirty : dirtySubmeshes )
		{
			auto it = std::find_if( m_culledSubmeshes.begin()
				, m_culledSubmeshes.end()
				, [dirty]( CulledNodeT< SubmeshRenderNode > const & lookup )
				{
					return lookup.node == dirty;
				} );
			auto visible = isSubmeshVisible( *dirty );

			if ( it != m_culledSubmeshes.end() )
			{
				m_culledChanged = m_culledChanged || it->visible != visible;
				it->visible = visible;
			}
			else
			{
				m_culledChanged = true;
				m_culledSubmeshes.push_back( { dirty, visible, 1u } );
			}
		}
	}

	void SceneCuller::duUpdateCulledBillboards( std::vector< BillboardRenderNode const * > const & dirtyBillboards )
	{
		for ( auto dirty : dirtyBillboards )
		{
			auto it = std::find_if( m_culledBillboards.begin()
				, m_culledBillboards.end()
				, [dirty]( CulledNodeT< BillboardRenderNode > const & lookup )
				{
					return lookup.node == dirty;
				} );
			auto visible = isBillboardVisible( *dirty );
			auto count = dirty->getInstanceCount();

			if ( it != m_culledBillboards.end() )
			{
				m_culledChanged = m_culledChanged
					|| it->visible != visible
					|| it->count != count;
				it->visible = visible;
				it->count = count;
			}
			else
			{
				m_culledChanged = true;
				m_culledBillboards.push_back( { dirty, visible, count } );
			}
		}
	}

	void SceneCuller::doMakeDirty( Geometry const & object
		, std::vector< SubmeshRenderNode const * > & dirtySubmeshes )const
	{
		if ( auto mesh = object.getMesh().lock() )
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

	void SceneCuller::doMakeDirty( BillboardBase const & object
		, std::vector< BillboardRenderNode const * > & dirtyBillboards )const
	{
		if ( auto material = object.getMaterial() )
		{
			for ( auto & pass : *material )
			{
				if ( auto node = object.getRenderNode( *pass ) )
				{
					dirtyBillboards.push_back( node );
				}
			}
		}
	}

	void SceneCuller::doAddSubmesh( CulledNodeT< SubmeshRenderNode > const & culled
		, RenderNodesPass const & renderPass
		, SceneCuller::SidedNodePipelineMapT< SubmeshRenderNode > & sorted
		, SceneCuller::SidedObjectNodePipelineMapT< SubmeshRenderNode > & sortedInstanced
		, SceneCuller::PipelineBufferArray & nodesIds )
	{
		auto & node = *culled.node;

		if ( renderPass.isValidPass( *node.pass )
			&& renderPass.isValidRenderable( node.instance )
			&& node.instance.getParent() != renderPass.getIgnoredNode() )
		{
			bool needsFront = renderPass.getRenderMode() == RenderMode::eTransparentOnly
				|| renderPass.forceTwoSided()
				|| node.pass->isTwoSided()
				|| node.pass->hasAlphaBlending();
			auto & instantiation = node.data.getInstantiation();

			if ( !renderPass.isMeshShading()
				&& instantiation.isInstanced( node.instance.getMaterial( node.data ) ) )
			{
				if ( node.instance.getParent()->isVisible() )
				{
					cullscn::addRenderNode( culled
						, renderPass
						, false
						, sortedInstanced
						, nodesIds );

					if ( needsFront )
					{
						cullscn::addRenderNode( culled
							, renderPass
							, true
							, sortedInstanced
							, nodesIds );
					}
				}
			}
			else
			{
				cullscn::addRenderNode( culled
					, renderPass
					, false
					, sorted
					, nodesIds );

				if ( needsFront )
				{
					cullscn::addRenderNode( culled
						, renderPass
						, true
						, sorted
						, nodesIds );
				}
			}
		}
	}

	void SceneCuller::doAddBillboard( CulledNodeT< BillboardRenderNode > const & culled
		, RenderNodesPass const & renderPass
		, SceneCuller::SidedNodePipelineMapT< BillboardRenderNode > & sorted
		, SceneCuller::PipelineBufferArray & nodesIds )
	{
		auto & node = *culled.node;

		if ( renderPass.isValidPass( *node.pass )
			&& renderPass.isValidRenderable( node.instance )
			&& node.instance.getNode() != renderPass.getIgnoredNode() )
		{
			cullscn::addRenderNode( culled
				, renderPass
				, false
				, sorted
				, nodesIds );
		}
	}

	//*********************************************************************************************
}
