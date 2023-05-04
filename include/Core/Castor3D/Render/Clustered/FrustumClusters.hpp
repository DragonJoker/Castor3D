/*
See LICENSE file in root folder
*/
#ifndef ___C3D_FrustumClusters_H___
#define ___C3D_FrustumClusters_H___

#include "ClusteredModule.hpp"
#include "Castor3D/Scene/SceneModule.hpp"

#include "Castor3D/Buffer/GpuBufferOffset.hpp"
#include "Castor3D/Shader/Ubos/ClustersUbo.hpp"

#include <CastorUtils/Design/GroupChangeTracked.hpp>

#include <ashespp/Buffer/Buffer.hpp>

namespace castor3d
{
	class FrustumClusters
	{
	public:
		C3D_API FrustumClusters( FrustumClusters const & ) = delete;
		C3D_API FrustumClusters( FrustumClusters && ) = delete;
		C3D_API FrustumClusters & operator=( FrustumClusters const & ) = delete;
		C3D_API FrustumClusters & operator=( FrustumClusters && ) = delete;
		C3D_API FrustumClusters( RenderDevice const & device
			, Camera const & camera );
		/**
		 *\~english
		 *\brief			CPU side update.
		 *\param[in, out]	updater	The update data.
		 *\~french
		 *\brief			Mise à jour niveau CPU.
		 *\param[in, out]	updater	Les données d'update.
		 */
		C3D_API void update( CpuUpdater & updater );

		castor::Point3ui const & getDimensions()const noexcept
		{
			return m_dimensions;
		}

		bool const & needsClustersUpdate()const noexcept
		{
			return m_clustersDirty;
		}

		bool const & needsLightsUpdate()const noexcept
		{
			return m_lightsDirty;
		}

		auto & getClustersUbo()const noexcept
		{
			return m_clustersUbo;
		}

		auto & getAabbBuffer()const noexcept
		{
			return *m_aabbBuffer;
		}

		auto & getPointLightIndexBuffer()const noexcept
		{
			return *m_pointIndexBuffer;
		}

		auto & getPointLightClusterBuffer()const noexcept
		{
			return *m_pointClusterBuffer;
		}

		auto & getSpotLightIndexBuffer()const noexcept
		{
			return *m_spotIndexBuffer;
		}

		auto & getSpotLightClusterBuffer()const noexcept
		{
			return *m_spotClusterBuffer;
		}

	private:
		struct AABB
		{
			castor::Point4f min;
			castor::Point4f max;
			castor::Point4f sphere;
		};

	private:
		void doUpdate();

	private:
		RenderDevice const & m_device;
		Camera const & m_camera;
		bool m_clustersDirty{ true };
		bool m_lightsDirty{ true };
		bool m_first{ true };
		castor::GroupChangeTracked< castor::Point3ui > m_dimensions;
		castor::GroupChangeTracked< uint32_t > m_clusterSize;
		castor::GroupChangeTracked< castor::Matrix4x4f > m_cameraProjection;
		castor::GroupChangeTracked< castor::Matrix4x4f > m_cameraView;
		castor::GroupChangeTracked< float > m_nearK;
		ClustersUbo m_clustersUbo;
		ashes::BufferPtr< AABB > m_aabbBuffer;
		ashes::BufferPtr< u32 > m_pointIndexBuffer;
		ashes::BufferPtr< castor::Point2ui > m_pointClusterBuffer;
		ashes::BufferPtr< u32 > m_spotIndexBuffer;
		ashes::BufferPtr< castor::Point2ui > m_spotClusterBuffer;
	};
}

#endif
