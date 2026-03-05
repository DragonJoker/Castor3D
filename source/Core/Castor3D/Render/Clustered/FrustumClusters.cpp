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
		static uint32_t constexpr MaxClusterGridWidthHeight = 32u;
		static uint32_t constexpr MaxClusterGridDepth = 64u;
		static uint32_t constexpr MaxClusterCount = MaxClusterGridWidthHeight * MaxClusterGridWidthHeight * MaxClusterGridDepth;

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
		: mortonCodes{ BufferT< u32 >{ device, resources
				, cuT( "C3D_" ) + name + cuT( "LightMortonCodesA" )
				, BufferCreateFlags::eNone
				, MaxLightsCount
				, BufferUsageFlags::eStorageBuffer | BufferUsageFlags::eTransferDst | BufferUsageFlags::eTransferSrc }
			, BufferT< u32 >{ device, resources
				, cuT( "C3D_" ) + name + cuT( "LightMortonCodesB" )
				, BufferCreateFlags::eNone
				, MaxLightsCount
				, BufferUsageFlags::eStorageBuffer | BufferUsageFlags::eTransferDst | BufferUsageFlags::eTransferSrc } }
		, indices{ BufferT< u32 >{ device, resources
				, cuT( "C3D_" ) + name + cuT( "LightIndicesA" )
				, BufferCreateFlags::eNone
				, MaxLightsCount
				, BufferUsageFlags::eStorageBuffer | BufferUsageFlags::eTransferDst | BufferUsageFlags::eTransferSrc }
			, BufferT< u32 >{ device, resources
				, cuT( "C3D_" ) + name + cuT( "LightIndicesB" )
				, BufferCreateFlags::eNone
				, MaxLightsCount
				, BufferUsageFlags::eStorageBuffer | BufferUsageFlags::eTransferDst | BufferUsageFlags::eTransferSrc } }
		, bvh{ device, resources
			, cuT( "C3D_" ) + name + cuT( "LightBVH" )
			, BufferCreateFlags::eNone
			, getNumNodes( MaxLightsCount )
			, BufferUsageFlags::eStorageBuffer | BufferUsageFlags::eTransferDst }
		, clusterGrid{ device, resources
			, cuT( "C3D_" ) + name + cuT( "LightClusterGrid" )
			, BufferCreateFlags::eNone
			, frscls::MaxClusterCount
			, BufferUsageFlags::eStorageBuffer | BufferUsageFlags::eTransferDst }
		, clusterIndex{ device, resources
			, cuT( "C3D_" ) + name + cuT( "LightClusterIndex" )
			, BufferCreateFlags::eNone
			, frscls::MaxClusterCount * MaxLightsPerCluster
			, BufferUsageFlags::eStorageBuffer | BufferUsageFlags::eTransferDst }
		, inputIndices{ indices[MortonIndicesInput].bufferViewId, indices[MortonIndicesOutput].bufferViewId }
		, outputIndices{ indices[MortonIndicesOutput].bufferViewId, indices[MortonIndicesInput].bufferViewId }
		, inputMortonCodes{ mortonCodes[MortonIndicesInput].bufferViewId, mortonCodes[MortonIndicesOutput].bufferViewId }
		, outputMortonCodes{ mortonCodes[MortonIndicesOutput].bufferViewId, mortonCodes[MortonIndicesInput].bufferViewId }
	{
		mortonCodes[0].create();
		mortonCodes[1].create();
		indices[0].create();
		indices[1].create();
		bvh.create();
		clusterGrid.create();
		clusterIndex.create();
	}

	FrustumClusters::Buffers::~Buffers()noexcept
	{
		mortonCodes[0].destroy();
		mortonCodes[1].destroy();
		indices[0].destroy();
		indices[1].destroy();
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
			, getNumNodes( MaxLightsCount )
			, BufferUsageFlags::eStorageBuffer | BufferUsageFlags::eTransferDst | BufferUsageFlags::eIndirectBuffer }
		, m_mergePathPartitions{ device, resources
			, cuT( "C3D_MergePathPartitions" )
			, BufferCreateFlags::eNone
			, frscls::getMaxMergePathPartitionsCount()
			, BufferUsageFlags::eStorageBuffer | BufferUsageFlags::eTransferDst }
		, m_allLightsAABBBuffer{ m_device, resources
			, cuT( "C3D_AllLightsAABB" )
			, BufferCreateFlags::eNone
			, MaxLightsCount
			, BufferUsageFlags::eStorageBuffer | BufferUsageFlags::eTransferDst }
		, m_reducedLightsAABBBuffer{ m_device, resources
			, cuT( "C3D_ReducedLightsAABB" )
			, BufferCreateFlags::eNone
			, 513u
			, BufferUsageFlags::eStorageBuffer | BufferUsageFlags::eTransferDst }
		, m_pointBuffers{ m_device, resources, cuT( "Point" ) }
		, m_spotBuffers{ m_device, resources, cuT( "Spot" ) }
		, m_aabbBuffer{ m_device, resources
			, cuT( "ClustersAABB" )
			, BufferCreateFlags::eNone
			, frscls::MaxClusterCount
			, BufferUsageFlags::eStorageBuffer | BufferUsageFlags::eTransferDst }
	{
		m_clustersIndirect.create();
		m_mergePathPartitions.create();
		m_allLightsAABBBuffer.create();
		m_reducedLightsAABBBuffer.create();
		m_aabbBuffer.create();
		doUpdate( {}, {}, nullptr );
	}

	FrustumClusters::~FrustumClusters()noexcept
	{
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
		m_sortAttachs[MortonIndicesOutput] = createComputeLightsMortonCodePass( graph, m_device, *this
			, m_allLightsAABBBuffer, m_reducedLightsAABBBuffer
			, m_pointBuffers.outputMortonCodes, m_spotBuffers.outputMortonCodes
			, m_pointBuffers.outputIndices, m_spotBuffers.outputIndices );
		m_sortAttachs[MortonIndicesInput] = createBucketSortLightsPass( graph, m_device, *this
			, m_sortAttachs[0]
			, m_pointBuffers.inputMortonCodes, m_spotBuffers.inputMortonCodes
			, m_pointBuffers.inputIndices, m_spotBuffers.inputIndices );
		createMergeSortLightsPass( graph, m_device, *this
			, m_sortAttachs[MortonIndicesInput], m_sortAttachs[MortonIndicesOutput], m_mergePathPartitions );
		createBuildLightsBVHPass( graph, m_device, *this
			, m_allLightsAABBBuffer, m_sortAttachs[MortonIndicesOutput], m_mergePathPartitions, m_pointBuffers.bvh, m_spotBuffers.bvh );
		createAssignLightsToClustersPass( graph, m_device, *this
			, m_clustersCameraUbo, m_allLightsAABBBuffer, m_aabbBuffer, m_pointBuffers.bvh, m_spotBuffers.bvh, m_sortAttachs[MortonIndicesOutput]
			, m_pointBuffers.clusterIndex, m_spotBuffers.clusterIndex, m_pointBuffers.clusterGrid, m_spotBuffers.clusterGrid );
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
			m_dimensions->x = frscls::MaxClusterGridWidthHeight;
			m_dimensions->y = u32( frscls::MaxClusterGridWidthHeight / fAspectRatio );
		}
		else if ( m_rawRenderSize->x < m_rawRenderSize->y )
		{
			m_dimensions->x = u32( frscls::MaxClusterGridWidthHeight * fAspectRatio );
			m_dimensions->y = frscls::MaxClusterGridWidthHeight;
		}
		else
		{
			m_dimensions->x = frscls::MaxClusterGridWidthHeight;
			m_dimensions->y = frscls::MaxClusterGridWidthHeight;
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
