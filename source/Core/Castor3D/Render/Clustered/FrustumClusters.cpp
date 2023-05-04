#include "Castor3D/Render/Clustered/FrustumClusters.hpp"

#include "Castor3D/Engine.hpp"
#include "Castor3D/Buffer/GpuBufferPool.hpp"
#include "Castor3D/Cache/LightCache.hpp"
#include "Castor3D/Render/RenderDevice.hpp"
#include "Castor3D/Scene/Camera.hpp"
#include "Castor3D/Scene/Scene.hpp"
#include "Castor3D/Scene/Light/DirectionalLight.hpp"
#include "Castor3D/Scene/Light/Light.hpp"
#include "Castor3D/Scene/Light/PointLight.hpp"
#include "Castor3D/Scene/Light/SpotLight.hpp"

CU_ImplementSmartPtr( castor3d, FrustumClusters )

namespace castor3d
{
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
	{
		doUpdate();
	}

	void FrustumClusters::update( CpuUpdater & updater )
	{
		auto & lightCache = m_camera.getScene()->getLightCache();
		m_clustersDirty = lightCache.hasClusteredLights()
			&& m_first;
		doUpdate();
		m_clustersUbo.cpuUpdate( m_dimensions.value()
			, m_camera.getNear()
			, m_clusterSize.value()
			, m_nearK.value() );
		m_lightsDirty = lightCache.hasClusteredLights()
			&& ( m_clustersDirty || lightCache.isDirty() );
		m_first = m_camera.getEngine()->areUpdateOptimisationsEnabled()
			? false
			: true;
	}

	void FrustumClusters::doUpdate()
	{
		// The half-angle of the field of view in the Y-direction.
		auto fieldOfViewY = m_camera.getFovY() * 0.5f;
		f32 zNear = m_camera.getNear();
		f32 zFar = m_camera.getFar();
		u32 clusterSize = m_clusterSize.value();
		auto size = getSafeBandedSize( m_camera.getSize() );

		// Number of clusters in the screen X direction.
		u32 clusterDimX = u32( std::ceil( f32( size->x ) / f32( clusterSize ) ) );
		// Number of clusters in the screen Y direction.
		u32 clusterDimY = u32( std::ceil( f32( size->y ) / f32( clusterSize ) ) );

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

		if ( !m_aabbBuffer )
		{
			constexpr usize maxCellCount = usize( 32u ) * 32u * 128u;
			constexpr usize maxIndices = maxCellCount * MaxLightsPerCluster;
			m_aabbBuffer = makeBuffer< AABB >( m_device
				, maxCellCount
				, VK_BUFFER_USAGE_STORAGE_BUFFER_BIT | VK_BUFFER_USAGE_TRANSFER_DST_BIT
				, VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT
				, "C3D_ClustersAABB" );
			m_pointIndexBuffer = makeBuffer< u32 >( m_device
				, maxIndices
				, VK_BUFFER_USAGE_STORAGE_BUFFER_BIT | VK_BUFFER_USAGE_TRANSFER_DST_BIT
				, VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT
				, "C3D_PointClustersIndex" );
			m_pointClusterBuffer = makeBuffer< castor::Point2ui >( m_device
				, maxCellCount
				, VK_BUFFER_USAGE_STORAGE_BUFFER_BIT | VK_BUFFER_USAGE_TRANSFER_DST_BIT
				, VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT
				, "C3D_PointClustersGrid" );
			m_spotIndexBuffer = makeBuffer< u32 >( m_device
				, maxIndices
				, VK_BUFFER_USAGE_STORAGE_BUFFER_BIT | VK_BUFFER_USAGE_TRANSFER_DST_BIT
				, VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT
				, "C3D_SpotClustersIndex" );
			m_spotClusterBuffer = makeBuffer< castor::Point2ui >( m_device
				, maxCellCount
				, VK_BUFFER_USAGE_STORAGE_BUFFER_BIT | VK_BUFFER_USAGE_TRANSFER_DST_BIT
				, VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT
				, "C3D_SpotClustersGrid" );
		}
	}
}
