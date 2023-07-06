#include "Castor3D/Render/Clustered/FrustumClusters.hpp"

#include "Castor3D/DebugDefines.hpp"
#include "Castor3D/Engine.hpp"
#include "Castor3D/Buffer/GpuBufferPool.hpp"
#include "Castor3D/Cache/LightCache.hpp"
#include "Castor3D/Render/RenderDevice.hpp"
#include "Castor3D/Render/Clustered/AssignLightsToClusters.hpp"
#include "Castor3D/Render/Clustered/BuildLightsBVH.hpp"
#include "Castor3D/Render/Clustered/ClustersMask.hpp"
#include "Castor3D/Render/Clustered/ComputeClustersAABB.hpp"
#include "Castor3D/Render/Clustered/ComputeLightsMortonCode.hpp"
#include "Castor3D/Render/Clustered/FindUniqueClusters.hpp"
#include "Castor3D/Render/Clustered/ReduceLightsAABB.hpp"
#include "Castor3D/Render/Clustered/SortLightsMortonCode.hpp"
#include "Castor3D/Scene/Camera.hpp"
#include "Castor3D/Scene/Scene.hpp"
#include "Castor3D/Scene/Light/DirectionalLight.hpp"
#include "Castor3D/Scene/Light/Light.hpp"
#include "Castor3D/Scene/Light/PointLight.hpp"
#include "Castor3D/Scene/Light/SpotLight.hpp"

#include <RenderGraph/FramePassGroup.hpp>

CU_ImplementSmartPtr( castor3d, FrustumClusters )

namespace castor3d
{
	//*********************************************************************************************

	namespace frscls
	{
		template< typename DataT >
		void updateBuffer( RenderDevice const & device
			, VkDeviceSize elementCount
			, std::string const & debugName
			, ashes::BufferBasePtr & buffer
			, std::vector< ashes::BufferBasePtr > & toDelete
			, VkBufferUsageFlags usage = VK_BUFFER_USAGE_STORAGE_BUFFER_BIT | VK_BUFFER_USAGE_TRANSFER_DST_BIT )
		{
			if ( buffer )
			{
				toDelete.emplace_back( std::move( buffer ) );
			}

			buffer = makeBufferBase( device
				, elementCount * sizeof( DataT )
				, usage
				, VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT
				, "C3D_" + debugName );
		}
	}

	//*********************************************************************************************

	FrustumClusters::FrustumClusters( RenderDevice const & device
		, Camera const & camera )
		: m_device{ device }
		, m_camera{ camera }
		, m_dimensions{ m_clustersDirty, {} }
		, m_clusterSize{ m_clustersDirty, 64u }
		, m_cameraProjection{ m_clustersDirty, {} }
		, m_cameraView{ m_clustersDirty, {} }
		, m_nearK{ m_clustersDirty, 0.0f }
		, m_clustersUbo{ m_device }
#if C3D_DebugUseDepthClusteredSamples
		, m_clustersIndirect{ makeBuffer< VkDispatchIndirectCommand >( m_device
			, getNumNodes( MaxLightsCount )
			, VK_BUFFER_USAGE_STORAGE_BUFFER_BIT | VK_BUFFER_USAGE_TRANSFER_DST_BIT | VK_BUFFER_USAGE_INDIRECT_BUFFER_BIT
			, VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT
			, "C3D_ClustersIndirect" ) }
#endif
#if C3D_DebugUseLightsBVH
		, m_lightsAABBBuffer{ makeBuffer< AABB >( m_device
			, MaxLightsCount
			, VK_BUFFER_USAGE_STORAGE_BUFFER_BIT | VK_BUFFER_USAGE_TRANSFER_DST_BIT
			, VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT
			, "C3D_LightsAABB" ) }
		, m_pointMortonCodesBuffers{ { makeBuffer< u32 >( m_device
				, MaxLightsCount
				, VK_BUFFER_USAGE_STORAGE_BUFFER_BIT | VK_BUFFER_USAGE_TRANSFER_DST_BIT
				, VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT
				, "C3D_PointLightMortonCodesA" )
			, makeBuffer< u32 >( m_device
				, MaxLightsCount
				, VK_BUFFER_USAGE_STORAGE_BUFFER_BIT | VK_BUFFER_USAGE_TRANSFER_DST_BIT
				, VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT
				, "C3D_PointLightMortonCodesB" ) } }
		, m_spotMortonCodesBuffers{ { makeBuffer< u32 >( m_device
				, MaxLightsCount
				, VK_BUFFER_USAGE_STORAGE_BUFFER_BIT | VK_BUFFER_USAGE_TRANSFER_DST_BIT
				, VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT
				, "C3D_SpotLightMortonCodesA" )
			, makeBuffer< u32 >( m_device
				, MaxLightsCount
				, VK_BUFFER_USAGE_STORAGE_BUFFER_BIT | VK_BUFFER_USAGE_TRANSFER_DST_BIT
				, VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT
				, "C3D_SpotLightMortonCodesB" ) } }
		, m_pointIndicesBuffers{ { makeBuffer< u32 >( m_device
				, MaxLightsCount
				, VK_BUFFER_USAGE_STORAGE_BUFFER_BIT | VK_BUFFER_USAGE_TRANSFER_DST_BIT
				, VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT
				, "C3D_PointLightIndicesA" )
			, makeBuffer< u32 >( m_device
				, MaxLightsCount
				, VK_BUFFER_USAGE_STORAGE_BUFFER_BIT | VK_BUFFER_USAGE_TRANSFER_DST_BIT
				, VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT
				, "C3D_PointLightIndicesB" ) } }
		, m_spotIndicesBuffers{ { makeBuffer< u32 >( m_device
				, MaxLightsCount
				, VK_BUFFER_USAGE_STORAGE_BUFFER_BIT | VK_BUFFER_USAGE_TRANSFER_DST_BIT
				, VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT
				, "C3D_SpotLightIndicesA" )
			, makeBuffer< u32 >( m_device
				, MaxLightsCount
				, VK_BUFFER_USAGE_STORAGE_BUFFER_BIT | VK_BUFFER_USAGE_TRANSFER_DST_BIT
				, VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT
				, "C3D_SpotLightIndicesB" ) } }
		, m_pointBVHBuffer{ makeBuffer< AABB >( m_device
			, getNumNodes( MaxLightsCount )
			, VK_BUFFER_USAGE_STORAGE_BUFFER_BIT | VK_BUFFER_USAGE_TRANSFER_DST_BIT
			, VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT
			, "C3D_PointLightBVH" ) }
		, m_spotBVHBuffer{ makeBuffer< AABB >( m_device
			, getNumNodes( MaxLightsCount )
			, VK_BUFFER_USAGE_STORAGE_BUFFER_BIT | VK_BUFFER_USAGE_TRANSFER_DST_BIT
			, VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT
			, "C3D_SpotLightBVH" ) }
#endif
	{
#if C3D_DebugUseLightsBVH
#	if C3D_DebugSortLightsMortonCode
		static uint32_t constexpr NumThreadsPerThreadGroup = 256u;
		static uint32_t constexpr ElementsPerThread = 8u;

		// The maximum number of elements that need to be sorted.
		uint32_t maxElements = MaxLightsCount;

		// Radix sort will sort Morton codes (keys) into chunks of SORT_NUM_THREADS_PER_THREAD_GROUP size.
		uint32_t chunkSize = NumThreadsPerThreadGroup;
		// The number of chunks that need to be merge sorted after Radix sort finishes.
		uint32_t numChunks = uint32_t( std::ceil( float( maxElements ) / float( chunkSize ) ) );
		// The number of sort groups that are needed to sort the first set of chunks.
		// Each sort group will sort 2 chunks. So the maximum number of sort groups is 1/2 of the 
		// number of chunks.
		uint32_t maxSortGroups = numChunks / 2u;
		// The number of merge path partitions per sort group is the total values
		// to be sorted per sort group (2 chunks) divided by the number of elements 
		// that can be sorted per thread group. One is added to account for the 
		// merge path partition at the END of the chunk.
		uint32_t numMergePathPartitionsPerSortGroup = uint32_t( std::ceil( float( chunkSize * 2u ) / float( ElementsPerThread * NumThreadsPerThreadGroup ) ) ) + 1u;

		// The maximum number of merge path partitions is the number of merge path partitions
		// needed by a single sort group multiplied by the maximum number of sort groups.
		uint32_t maxMergePathPartitions = numMergePathPartitionsPerSortGroup * maxSortGroups;

		m_mergePathPartitionsBuffer = makeBuffer< s32 >( m_device
			, maxMergePathPartitions
			, VK_BUFFER_USAGE_STORAGE_BUFFER_BIT | VK_BUFFER_USAGE_TRANSFER_DST_BIT
			, VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT
			, "C3D_MergePathPartitions" );
#	endif
#endif

		doUpdate();
	}

	void FrustumClusters::update( CpuUpdater & updater )
	{
		auto scene = m_camera.getScene();
		auto & lightCache = scene->getLightCache();
		m_clustersDirty = lightCache.hasClusteredLights()
			&& m_first > 0;
		doUpdate();
		m_clustersUbo.cpuUpdate( m_dimensions.value()
			, m_camera.getNear()
			, m_clusterSize.value()
			, m_nearK.value()
			, lightCache.getLightsBufferCount( LightType::ePoint )
			, lightCache.getLightsBufferCount( LightType::eSpot ) );
		auto it = updater.dirtyScenes.find( scene );
		m_lightsDirty = lightCache.hasClusteredLights()
			&& ( m_clustersDirty
				|| lightCache.isDirty()
				|| ( it != updater.dirtyScenes.end() && !it->second.isEmpty() ) );
		m_first = m_camera.getEngine()->areUpdateOptimisationsEnabled()
			? std::max( 0, m_first - 1 )
			: 5;
	}

	crg::FramePass const & FrustumClusters::createFramePasses( crg::FramePassGroup & parentGraph
		, crg::FramePass const * previousPass
		, RenderTechnique & technique
		, CameraUbo const & cameraUbo
		, RenderNodesPass *& nodesPass )
	{
		auto & graph = parentGraph.createPassGroup( "Clusters" );
		crg::FramePassArray lastPasses{ 1u, previousPass };
#if C3D_DebugUseDepthClusteredSamples
		lastPasses = { &createClustersMaskPass( graph, *lastPasses.front()
			, m_device, cameraUbo, *this
			, technique, nodesPass ) };
		lastPasses = { &createFindUniqueClustersPass( graph, *lastPasses.front()
			, m_device, cameraUbo, *this ) };
#endif
		lastPasses = { &createComputeClustersAABBPass( graph, lastPasses.front()
			, m_device, cameraUbo, *this ) };
#if C3D_DebugUseLightsBVH
		lastPasses = { &createReduceLightsAABBPass( graph, lastPasses.front()
			, m_device, cameraUbo, *this ) };
		lastPasses = { &createComputeLightsMortonCodePass( graph, lastPasses.front()
			, m_device, cameraUbo, *this ) };
#	if C3D_DebugSortLightsMortonCode
		lastPasses = createSortLightsMortonCodePass( graph, lastPasses.front()
			, m_device, cameraUbo, *this );
#	endif
		lastPasses = { &createBuildLightsBVHPass( graph, lastPasses
			, m_device, cameraUbo, *this ) };
#endif
		return createAssignLightsToClustersPass( graph, lastPasses.front()
			, m_device, cameraUbo, *this );
	}

	uint32_t FrustumClusters::getNumLevels( uint32_t numLeaves )
	{
		static const float log32f = std::log( 32.0f );
		uint32_t numLevels = 0;

		if ( numLeaves > 0 )
		{
			numLevels = uint32_t( std::ceil( std::log( numLeaves ) / log32f ) );
		}

		return numLevels;
	}

	uint32_t FrustumClusters::getNumLevelNodes( uint32_t level )
	{
		static const uint32_t numLevelNodes[] =
		{
			1,          // 1st level (32^0)
			32,         // 2nd level (32^1)
			1024,       // 3rd level (32^2)
			32768,      // 4th level (32^3)
			1048576,    // 5th level (32^4)
			33554432,   // 6th level (32^5)
		};
		return numLevelNodes[level];
	}

	uint32_t FrustumClusters::getNumNodes( uint32_t numLeaves )
	{
		static const uint32_t numBVHNodes[] =
		{
			1,          // 1 level  =32^0
			33,         // 2 levels +32^1
			1057,       // 3 levels +32^2
			33825,      // 4 levels +32^3
			1082401,    // 5 levels +32^4
			34636833,   // 6 levels +32^5
		};

		uint32_t numLevels = getNumLevels( numLeaves );
		uint32_t numNodes = 0;

		if ( numLevels > 0 && numLevels < castor::getCountOf( numBVHNodes ) )
		{
			numNodes = numBVHNodes[numLevels - 1];
		}

		return numNodes;
	}

	void FrustumClusters::doUpdate()
	{
		m_toDelete.clear();

		// The half-angle of the field of view in the Y-direction.
		auto fieldOfViewY = m_camera.getFovY() * 0.5f;
		f32 zNear = m_camera.getNear();
		f32 zFar = m_camera.getFar();
		u32 clusterSize = m_clusterSize.value();
		auto renderSize = getSafeBandedSize( m_camera.getSize() );

		// Number of clusters in the screen X direction.
		u32 clusterDimX = u32( std::ceil( f32( renderSize->x ) / f32( clusterSize ) ) );
		// Number of clusters in the screen Y direction.
		u32 clusterDimY = u32( std::ceil( f32( renderSize->y ) / f32( clusterSize ) ) );

		// The depth of the cluster grid during clustered rendering is dependent on the 
		// number of clusters subdivisions in the screen Y direction.
		// Source: Clustered Deferred and Forward Shading (2012) (Ola Olsson, Markus Billeter, Ulf Assarsson).
		f32 sD = 2.0f * fieldOfViewY.tan() / f32( clusterDimY );
		m_nearK = 1.0f + sD;
		auto logDimY = 1.0f / std::log( m_nearK.value() );

		f32 logDepth = std::log( zFar / zNear );
		u32 clusterDimZ = u32( std::floor( logDepth * logDimY ) );

		m_dimensions = { clusterDimX, clusterDimY, clusterDimZ };
		m_cameraProjection = m_camera.getProjection( true );
		m_cameraView = m_camera.getView();
		auto cellCount = clusterDimX * clusterDimY * clusterDimZ;

		if ( !m_aabbBuffer
			|| m_aabbBuffer->getSize() < cellCount * sizeof( AABB ) )
		{
			auto indexCount = cellCount * MaxLightsPerCluster;
			frscls::updateBuffer< AABB >( m_device, cellCount, "ClustersAABB", m_aabbBuffer, m_toDelete );
			frscls::updateBuffer< castor::Point2ui >( m_device, cellCount, "PointLightClusterGrid", m_pointLightClusterGridBuffer, m_toDelete );
			frscls::updateBuffer< castor::Point2ui >( m_device, cellCount, "SpotLightClusterGrid", m_spotLightClusterGridBuffer, m_toDelete );
			frscls::updateBuffer< u32 >( m_device, indexCount, "PointLightClusterIndex", m_pointLightClusterIndexBuffer, m_toDelete );
			frscls::updateBuffer< u32 >( m_device, indexCount, "SpotLightClusterIndex", m_spotLightClusterIndexBuffer, m_toDelete );
#if C3D_DebugUseDepthClusteredSamples
			frscls::updateBuffer< u32 >( m_device, cellCount, "ClusterFlags", m_clusterFlags, m_toDelete );
			frscls::updateBuffer< u32 >( m_device, cellCount, "UniqueClusters", m_uniqueClusters, m_toDelete );
#endif
			onClusterBuffersChanged( *this );
		}
	}

	//*********************************************************************************************
}
