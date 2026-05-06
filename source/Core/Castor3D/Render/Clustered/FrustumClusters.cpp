#include "Castor3D/Render/Clustered/FrustumClusters.hpp"

#include "Castor3D/Engine.hpp"
#include "Castor3D/Cache/LightCache.hpp"
#include "Castor3D/Render/RenderDevice.hpp"
#include "Castor3D/Render/Clustered/AssignLightsToClusters.hpp"
#include "Castor3D/Render/Clustered/BuildLightsBVH.hpp"
#include "Castor3D/Render/Clustered/ComputeClustersAABB.hpp"
#include "Castor3D/Render/Clustered/ComputeLightsAABB.hpp"
#include "Castor3D/Render/Clustered/ComputeLightsMortonCode.hpp"
#include "Castor3D/Render/Clustered/MergeSortLights.hpp"
#include "Castor3D/Render/Clustered/BucketSortLights.hpp"
#include "Castor3D/Render/Clustered/ReduceLightsAABB.hpp"
#include "Castor3D/Render/Clustered/SortAssignedLights.hpp"
#include "Castor3D/Render/Debug/DebugModule.hpp"
#include "Castor3D/Scene/Camera.hpp"
#include "Castor3D/Scene/Scene.hpp"
#include "Castor3D/Shader/Shaders/GlslRadixSort.hpp"
#include "Castor3D/Shader/Ubos/CameraUbo.hpp"

#include <RenderGraph/FramePassGroup.hpp>

CU_ImplementSmartPtr( c3d, FrustumClusters )

namespace c3d
{
	//*********************************************************************************************

	namespace frscls
	{
		inline const Array< uint32_t, 6u > NumLevelNodes
		{
			1,          // 1st level (32^0)
			32,         // 2nd level (32^1)
			1024,       // 3rd level (32^2)
			32768,      // 4th level (32^3)
			1048576,    // 5th level (32^4)
			33554432,   // 6th level (32^5)
		};

		inline const Array< uint32_t, 6u > NumBVHNodes
		{
			1,          // 1 level  =32^0
			33,         // 2 levels +32^1
			1057,       // 3 levels +32^2
			33825,      // 4 levels +32^3
			1082401,    // 5 levels +32^4
			34636833,   // 6 levels +32^5
		};

		static uint32_t getMaxMergePathPartitionsCount()
		{
			static uint32_t constexpr NumThreadsPerThreadGroup = 256u;
			static uint32_t constexpr ElementsPerThread = 8u;

			// The maximum number of elements that need to be sorted.
			uint32_t maxElements = MaxLightsCount;

			// Radix sort will sort Morton codes (keys) into chunks of BucketSortBucketSize size.
			uint32_t chunkSize = FrustumClusters::getBucketSortBucketSize();
			// The number of chunks that need to be merge sorted after Radix sort finishes.
			uint32_t numChunks = divRoundUp( maxElements, chunkSize );
			// The number of sort groups that are needed to sort the first set of chunks.
			// Each sort group will sort 2 chunks. So the maximum number of sort groups is 1/2 of the 
			// number of chunks.
			uint32_t maxSortGroups = numChunks / 2u;
			// The number of merge path partitions per sort group is the total values
			// to be sorted per sort group (2 chunks) divided by the number of elements 
			// that can be sorted per thread group. One is added to account for the 
			// merge path partition at the END of the chunk.
			uint32_t numMergePathPartitionsPerSortGroup = divRoundUp( chunkSize * 2u, ElementsPerThread * NumThreadsPerThreadGroup ) + 1u;

			// The maximum number of merge path partitions is the number of merge path partitions
			// needed by a single sort group multiplied by the maximum number of sort groups.
			return numMergePathPartitionsPerSortGroup * maxSortGroups;

		}
	}

	//*********************************************************************************************

	FrustumClusters::Buffers::Buffers( RenderDevice const & device
		, crg::ResourcesCache & resources
		, String const & name )
		: mortonCodes{ device, resources
			, cuT( "C3D_" ) + name + cuT( "LightMortonCodes" )
			, BufferCreateFlags::eNone
			, MaxLightsCount
			, BufferUsageFlags::eStorageBuffer | BufferUsageFlags::eTransferDst | BufferUsageFlags::eTransferSrc }
		, tmpMortonCodes{ device, resources
			, cuT( "C3D_" ) + name + cuT( "LightTmpMortonCodes" )
			, BufferCreateFlags::eNone
			, MaxLightsCount
			, BufferUsageFlags::eStorageBuffer | BufferUsageFlags::eTransferDst | BufferUsageFlags::eTransferSrc }
		, indices{ device, resources
			, cuT( "C3D_" ) + name + cuT( "LightIndices" )
			, BufferCreateFlags::eNone
			, MaxLightsCount
			, BufferUsageFlags::eStorageBuffer | BufferUsageFlags::eTransferDst | BufferUsageFlags::eTransferSrc }
		, tmpIndices{ device, resources
			, cuT( "C3D_" ) + name + cuT( "LightTmpIndices" )
			, BufferCreateFlags::eNone
			, MaxLightsCount
			, BufferUsageFlags::eStorageBuffer | BufferUsageFlags::eTransferDst | BufferUsageFlags::eTransferSrc }
		, bvh{ device, resources
			, cuT( "C3D_" ) + name + cuT( "LightBVH" )
			, BufferCreateFlags::eNone
			, getNumNodes( MaxLightsCount )
			, BufferUsageFlags::eStorageBuffer | BufferUsageFlags::eTransferDst }
		, clusterGrid{ device, resources
			, cuT( "C3D_" ) + name + cuT( "LightClusterGrid" )
			, BufferCreateFlags::eNone
			, MaxClusterCount
			, BufferUsageFlags::eStorageBuffer | BufferUsageFlags::eTransferDst }
		, clusterIndex{ device, resources
			, cuT( "C3D_" ) + name + cuT( "LightClusterIndex" )
			, BufferCreateFlags::eNone
			, ClusterLightsIndicesPageSize
			, MaxClusterLightsIndicesPageCount
			, BufferUsageFlags::eStorageBuffer | BufferUsageFlags::eTransferDst }
		, toTmpIndices{ indices.bufferViewId, tmpIndices.bufferViewId }
		, fromTmpIndices{ tmpIndices.bufferViewId, indices.bufferViewId }
		, toTmpMortonCodes{ mortonCodes.bufferViewId, tmpMortonCodes.bufferViewId }
		, fromTmpMortonCodes{ tmpMortonCodes.bufferViewId, mortonCodes.bufferViewId }
	{
		mortonCodes.create();
		tmpMortonCodes.create();
		indices.create();
		tmpIndices.create();
		bvh.create();
		clusterGrid.create();
		clusterIndex.create();
	}

	FrustumClusters::Buffers::~Buffers()noexcept
	{
		mortonCodes.destroy();
		tmpMortonCodes.destroy();
		indices.destroy();
		tmpIndices.destroy();
		bvh.destroy();
		clusterGrid.destroy();
		clusterIndex.destroy();
	}

	//*********************************************************************************************

	FrustumClusters::FrustumClusters( RenderDevice const & device
		, crg::ResourcesCache & resources
		, Camera const & camera
		, ClustersConfig const & config )
		: m_device{ device }
		, m_camera{ camera }
		, m_config{ config }
		, m_clusterSize{ m_clustersDirty, Point2f{} }
		, m_cameraProjection{ m_clustersDirty, Matrix4x4f{} }
		, m_cameraView{ m_clustersDirty, Matrix4x4f{} }
		, m_clustersUbo{ m_device }
		, m_clustersCameraUbo{ m_device }
		, m_clustersIndirect{ m_device, resources
			, cuT( "C3D_ClustersIndirect" )
			, BufferCreateFlags::eNone
			, getNumNodes( LightsAABBPageSize * MaxLightsAABBPageCount )
			, BufferUsageFlags::eStorageBuffer | BufferUsageFlags::eTransferDst | BufferUsageFlags::eIndirectBuffer }
		, m_mergePathPartitions{ device, resources
			, cuT( "C3D_MergePathPartitions" )
			, BufferCreateFlags::eNone
			, frscls::getMaxMergePathPartitionsCount()
			, BufferUsageFlags::eStorageBuffer | BufferUsageFlags::eTransferDst }
		, m_allLightsAABBBuffer{ m_device, resources
			, cuT( "C3D_AllLightsAABB" )
			, BufferCreateFlags::eNone
			, LightsAABBPageSize
			, MaxLightsAABBPageCount
			, BufferUsageFlags::eStorageBuffer | BufferUsageFlags::eTransferDst }
		, m_reducedLightsAABBBuffer{ m_device, resources
			, cuT( "C3D_ReducedLightsAABB" )
			, BufferCreateFlags::eNone
			, 513u
			, BufferUsageFlags::eStorageBuffer | BufferUsageFlags::eTransferDst }
		, m_pointBuffers{ m_device, resources, cuT( "Point" ) }
		, m_spotBuffers{ m_device, resources, cuT( "Spot" ) }
		, m_tmpCounters{ m_device, resources
			, cuT( "C3D_TmpClustersCounters" )
			, BufferCreateFlags::eNone
			, 2u
			, BufferUsageFlags::eStorageBuffer | BufferUsageFlags::eTransferDst | BufferUsageFlags::eTransferSrc }
		, m_counters{ BufferT< uint32_t >{ m_device, resources
				, cuT( "C3D_ClustersCounters0" )
				, BufferCreateFlags::eNone
				, 2u
				, BufferUsageFlags::eStorageBuffer | BufferUsageFlags::eTransferDst
				, MemoryPropertyFlags::eHostVisible }
			, BufferT< uint32_t >{ m_device, resources
				, cuT( "C3D_ClustersCounters1" )
				, BufferCreateFlags::eNone
				, 2u
				, BufferUsageFlags::eStorageBuffer | BufferUsageFlags::eTransferDst
				, MemoryPropertyFlags::eHostVisible }
			, BufferT< uint32_t >{ m_device, resources
				, cuT( "C3D_ClustersCounters2" )
				, BufferCreateFlags::eNone
				, 2u
				, BufferUsageFlags::eStorageBuffer | BufferUsageFlags::eTransferDst
				, MemoryPropertyFlags::eHostVisible } }
		, m_aabbBuffer{ m_device, resources
			, cuT( "C3D_ClustersAABB" )
			, BufferCreateFlags::eNone
			, MaxClusterCount
			, BufferUsageFlags::eStorageBuffer | BufferUsageFlags::eTransferDst }
	{
		m_tmpCounters.create();
		m_counters[0].create();
		m_counters[1].create();
		m_counters[2].create();
		m_clustersIndirect.create();
		m_mergePathPartitions.create();
		m_allLightsAABBBuffer.create();
		m_reducedLightsAABBBuffer.create();
		m_aabbBuffer.create();
		doUpdate( {}, {}, nullptr );
	}

	FrustumClusters::~FrustumClusters()noexcept
	{
		m_tmpCounters.destroy();
		m_counters[0].destroy();
		m_counters[1].destroy();
		m_counters[2].destroy();
		m_clustersIndirect.destroy();
		m_mergePathPartitions.destroy();
		m_allLightsAABBBuffer.destroy();
		m_reducedLightsAABBBuffer.destroy();
		m_aabbBuffer.destroy();
	}

	void FrustumClusters::update( CpuUpdater & updater )
	{
		auto scene = m_camera.getScene();
		auto const & lightCache = scene->getLightCache();
		m_clustersDirty = scene->hasClusteredLights()
			&& ( m_first > 0 || m_config.dirty );
		doUpdate( updater.renderSize, updater.finalRenderSize, updater.viewport );
		m_clustersUbo.cpuUpdate( m_dimensions
			, m_clusterSize.value()
			, m_camera.getNear()
			, m_camera.getFar()
			, lightCache.getLightsBufferCount( LightType::ePoint )
			, lightCache.getLightsBufferCount( LightType::eSpot )
			, m_config.splitScheme
			, m_config.minDistance
			, m_config.enableWaveIntrinsics );
		auto it = updater.dirtyScenes.find( scene );
		m_lightsDirty = scene->hasClusteredLights()
			&& ( m_clustersDirty
				|| lightCache.isDirty()
				|| ( it != updater.dirtyScenes.end() && !it->second.isEmpty() )
				|| m_config.dirty );
		m_first = m_camera.getEngine()->areUpdateOptimisationsEnabled()
			? std::max( 0, m_first - 1 )
			: 5;

		if ( !m_config.lockClustersFrustum.value() )
		{
			m_clustersCameraUbo.cpuUpdate( m_camera
				, updater.jitter );
		}
	}

	void FrustumClusters::updateDebug( DebugDrawer & drawer )const
	{
		if ( m_config.debugDisplay.value() == ClusterDebugDisplay::eClustersAABB )
		{
			addDebugAabbs( drawer
				, m_displayClustersAABBBindings
				, m_displayClustersAABBWrites
				, getDimensions()->x * getDimensions()->y * getDimensions()->z
				, m_displayClustersAABBProgram
				, true );
		}
		else if ( m_config.debugDisplay.value() == ClusterDebugDisplay::eLightsAABB )
		{
			auto scene = m_camera.getScene();
			auto const & lightCache = scene->getLightCache();
			addDebugAabbs( drawer
				, m_displayLightsAABBBindings
				, m_displayLightsAABBWrites
				, lightCache.getLightsBufferCount( LightType::ePoint )
					+ lightCache.getLightsBufferCount( LightType::eSpot )
				, m_displayLightsAABBProgram
				, true );
		}
		else if ( m_config.debugDisplay.value() == ClusterDebugDisplay::eLightsBVH )
		{
			auto scene = m_camera.getScene();
			auto const & lightCache = scene->getLightCache();

			if ( auto count = lightCache.getLightsBufferCount( LightType::ePoint ) )
			{
				addDebugAabbs( drawer
					, m_displayPointLightsBVHBindings
					, m_displayPointLightsBVHWrites
					, FrustumClusters::getNumNodes( count )
					, m_displayPointLightsBVHProgram
					, false );
			}

			if ( auto count = lightCache.getLightsBufferCount( LightType::eSpot ) )
			{
				addDebugAabbs( drawer
					, m_displaySpotLightsBVHBindings
					, m_displaySpotLightsBVHWrites
					, FrustumClusters::getNumNodes( count )
					, m_displaySpotLightsBVHProgram
					, false );
			}
		}
	}

	void FrustumClusters::createFramePasses( crg::FramePassGroup & parentGraph
		, RenderUbo const & renderUbo )
	{
		auto & graph = parentGraph.createPassGroup( "Clusters" );
		createComputeLightsAABBPass( graph, m_device, *this
			, m_clustersCameraUbo, m_allLightsAABBBuffer );
		createReduceLightsAABBPass( graph, m_device, *this
			, m_clustersCameraUbo, m_allLightsAABBBuffer, m_reducedLightsAABBBuffer );
		createComputeClustersAABBPass( graph, m_device, *this
			, m_clustersCameraUbo, renderUbo, m_reducedLightsAABBBuffer, m_aabbBuffer );
		// Compute morton codes, store in src buffers.
		// Use array attaches to make sure we can link passes.
		m_sortAttachsToTmp = createComputeLightsMortonCodePass( graph, m_device, *this
			, m_allLightsAABBBuffer, m_reducedLightsAABBBuffer
			, m_pointBuffers.toTmpMortonCodes, m_spotBuffers.toTmpMortonCodes
			, m_pointBuffers.toTmpIndices, m_spotBuffers.toTmpIndices );
		// Bucket sort src buffers, and store in tmp buffers
		m_sortAttachsfromTmp = createBucketSortLightsPass( graph, m_device, *this
			, m_sortAttachsToTmp
			, m_pointBuffers.fromTmpMortonCodes, m_spotBuffers.fromTmpMortonCodes
			, m_pointBuffers.fromTmpIndices, m_spotBuffers.fromTmpIndices );
		// Merge sort tmp buffers, and store back in src buffers (copying if needed).
		createMergeSortLightsPass( graph, m_device, *this
			, m_sortAttachsfromTmp, m_sortAttachsToTmp, m_mergePathPartitions );
		// From now on, we can only use src buffers, which contain sorted morton codes and indices.
		// Note that we still need to use the array attaches to link passes.
		createBuildLightsBVHPass( graph, m_device, *this
			, m_allLightsAABBBuffer, m_sortAttachsToTmp, m_mergePathPartitions, m_pointBuffers.bvh, m_spotBuffers.bvh );
		createAssignLightsToClustersPass( graph, m_device, *this
			, m_clustersCameraUbo, m_allLightsAABBBuffer, m_aabbBuffer, m_pointBuffers.bvh, m_spotBuffers.bvh, m_sortAttachsToTmp, m_counters
			, m_pointBuffers.clusterIndex, m_spotBuffers.clusterIndex, m_pointBuffers.clusterGrid, m_spotBuffers.clusterGrid
			, m_tmpCounters );
		createSortAssignedLightsPass( graph, m_device, *this
			, m_pointBuffers.clusterIndex, m_spotBuffers.clusterIndex, m_pointBuffers.clusterGrid, m_spotBuffers.clusterGrid );
	}

	void FrustumClusters::createDebugDisplayPrograms( CameraUbo const & cameraUbo )
	{
		if ( m_displayClustersAABBProgram.empty() )
		{
			createDisplayClustersAABBProgram( m_device, *this, cameraUbo, m_clustersCameraUbo
				, m_displayClustersAABBProgram
				, m_displayClustersAABBBindings
				, m_displayClustersAABBWrites
				, m_aabbBuffer );
		}

		if ( m_displayLightsAABBProgram.empty() )
		{
			createDisplayLightsAABBProgram( m_device, cameraUbo, m_clustersCameraUbo
				, m_displayLightsAABBProgram
				, m_displayLightsAABBBindings
				, m_displayLightsAABBWrites
				, m_allLightsAABBBuffer );
		}

		if ( m_displayPointLightsBVHProgram.empty() )
		{
			createDisplayPointLightsBVHProgram( m_device, cameraUbo, m_clustersCameraUbo
				, m_displayPointLightsBVHProgram
				, m_displayPointLightsBVHBindings
				, m_displayPointLightsBVHWrites
				, m_pointBuffers.bvh );
		}

		if ( m_displaySpotLightsBVHProgram.empty() )
		{
			createDisplaySpotLightsBVHProgram( m_device, cameraUbo, m_clustersCameraUbo
				, m_displaySpotLightsBVHProgram
				, m_displaySpotLightsBVHBindings
				, m_displaySpotLightsBVHWrites
				, m_spotBuffers.bvh );
		}
	}

	Scene const & FrustumClusters::getScene()const noexcept
	{
		return *m_camera.getScene();
	}

	uint32_t FrustumClusters::getNumLevels( uint32_t numLeaves )
	{
		static const float log32f = std::log( 32.0f );
		uint32_t numLevels = 0;

		if ( numLeaves > 0 )
		{
			numLevels = std::max( 1u, uint32_t( std::ceil( std::log( numLeaves ) / log32f ) ) );
		}

		return numLevels;
	}

	uint32_t FrustumClusters::getNumLevelNodes( uint32_t level )
	{
		return frscls::NumLevelNodes[level];
	}

	uint32_t FrustumClusters::getNumNodes( uint32_t numLeaves )
	{
		uint32_t numLevels = getNumLevels( numLeaves );
		uint32_t numNodes = 0;

		if ( numLevels > 0 && numLevels < frscls::NumBVHNodes.size() )
		{
			numNodes = frscls::NumBVHNodes[size_t( numLevels ) - 1];
		}

		return numNodes;
	}

	uint32_t FrustumClusters::getBucketSortBucketSize()
	{
		return shader::RadixSortT< 4u >::bucketSize;
	}

	void FrustumClusters::doUpdate( Size const & rawRenderSize
		, Point2f const & finalRenderSize
		, Viewport const * viewport )
	{
		m_rawRenderSize = rawRenderSize;
		m_finalRenderSize = finalRenderSize;

		f32 fAspectRatio = f32( m_rawRenderSize->x ) / f32( std::max( 1u, m_rawRenderSize->y ) );
		if ( m_rawRenderSize->x > m_rawRenderSize->y )
		{
			m_dimensions->x = MaxClusterGridWidthHeight;
			m_dimensions->y = u32( MaxClusterGridWidthHeight / fAspectRatio );
		}
		else if ( m_rawRenderSize->x < m_rawRenderSize->y )
		{
			m_dimensions->x = u32( MaxClusterGridWidthHeight * fAspectRatio );
			m_dimensions->y = MaxClusterGridWidthHeight;
		}
		else
		{
			m_dimensions->x = MaxClusterGridWidthHeight;
			m_dimensions->y = MaxClusterGridWidthHeight;
		}

		m_clusterCount = m_dimensions->x * m_dimensions->y * m_dimensions->z;
		m_clusterSize = { m_finalRenderSize->x / f32( m_dimensions->x )
			, m_finalRenderSize->y / f32( m_dimensions->y ) };

		m_cameraView = m_camera.getView();
		if ( viewport )
			m_cameraProjection = viewport->getProjection();
		else
			m_cameraProjection = m_camera.getProjection( rawRenderSize, true );
	}

	//*********************************************************************************************
}
