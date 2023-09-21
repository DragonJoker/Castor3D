/*
See LICENSE file in root folder
*/
#ifndef ___C3D_FrustumClusters_H___
#define ___C3D_FrustumClusters_H___

#include "Castor3D/Scene/SceneModule.hpp"

#include "Castor3D/Buffer/GpuBufferOffset.hpp"
#include "Castor3D/Render/Clustered/ClustersConfig.hpp"
#include "Castor3D/Shader/Ubos/ClustersUbo.hpp"

#include <CastorUtils/Design/GroupChangeTracked.hpp>
#include <CastorUtils/Design/Signal.hpp>

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
			, Camera const & camera
			, ClustersConfig const & config );
		/**
		 *\~english
		 *\brief			CPU side update.
		 *\param[in, out]	updater	The update data.
		 *\~french
		 *\brief			Mise à jour niveau CPU.
		 *\param[in, out]	updater	Les données d'update.
		 */
		C3D_API void update( CpuUpdater & updater );
		/**
		 *\~english
		 *\brief		Registers the clusters related frame passes.
		 *\~french
		 *\brief		Enregistre les passes liées aux clusters.
		 */
		C3D_API crg::FramePass const & createFramePasses( crg::FramePassGroup & graph
			, crg::FramePass const * previousPass
			, RenderTechnique & technique
			, CameraUbo const & cameraUbo
			, RenderNodesPass *& nodesPass );
		/**
		 *\~english
		 *\brief		Compute the number of nodes for given BVH level.
		 *\param[in]	numLeaves	The number of leaf nodes.
		 *\~french
		 *\brief		Calcule le nombre de noeuds du niveau donné d'un BVH.
		 *\param[in]	numLeaves	Le nombre de noeuds feuilles.
		 */
		C3D_API static uint32_t getNumLevelNodes( uint32_t level );
		/**
		 *\~english
		 *\brief		Compute the number of levels needed for a BVH that consists of a number of leaf nodes.
		 *\param[in]	numLeaves	The number of leaf nodes.
		 *\~french
		 *\brief		Calcule le nombre de niveaux nécessaire pour un BVH contenant le nombre de noeuds feuille donné.
		 *\param[in]	numLeaves	Le nombre de noeuds feuilles.
		 */
		C3D_API static uint32_t getNumLevels( uint32_t numLeaves );
		/**
		 *\~english
		 *\brief		Compute the number of (child) nodes needed to represent a BVH that consists of a number of leaf nodes.
		 *\param[in]	numLeaves	The number of leaf nodes.
		 *\~french
		 *\brief		Calcule le nombre de noeuds (enfants) nécessaire pour un représenter un BVH contenant le nombre de noeuds feuille donné.
		 *\param[in]	numLeaves	Le nombre de noeuds feuilles.
		 */
		C3D_API static uint32_t getNumNodes( uint32_t numLeaves );

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

		auto & getClustersAABBBuffer()const noexcept
		{
			CU_Require( m_aabbBuffer );
			return *m_aabbBuffer;
		}

		auto & getPointLightClusterGridBuffer()const noexcept
		{
			CU_Require( m_pointLightClusterGridBuffer );
			return *m_pointLightClusterGridBuffer;
		}

		auto & getSpotLightClusterGridBuffer()const noexcept
		{
			CU_Require( m_spotLightClusterGridBuffer );
			return *m_spotLightClusterGridBuffer;
		}

		auto & getPointLightClusterIndexBuffer()const noexcept
		{
			CU_Require( m_pointLightClusterIndexBuffer );
			return *m_pointLightClusterIndexBuffer;
		}

		auto & getSpotLightClusterIndexBuffer()const noexcept
		{
			CU_Require( m_spotLightClusterIndexBuffer );
			return *m_spotLightClusterIndexBuffer;
		}

		auto & getAllLightsAABBBuffer()const noexcept
		{
			return m_allLightsAABBBuffer->getBuffer();
		}

		auto & getReducedLightsAABBBuffer()const noexcept
		{
			return m_reducedLightsAABBBuffer->getBuffer();
		}

		auto & getPointLightBVHBuffer()const noexcept
		{
			return m_pointBVHBuffer->getBuffer();
		}

		auto & getSpotLightBVHBuffer()const noexcept
		{
			return m_spotBVHBuffer->getBuffer();
		}

		ashes::BufferBase & getPointLightIndicesBuffer( uint32_t index )const noexcept
		{
			return m_pointIndicesBuffers[index]->getBuffer();
		}

		ashes::BufferBase & getSpotLightIndicesBuffer( uint32_t index )const noexcept
		{
			return m_spotIndicesBuffers[index]->getBuffer();
		}

		ashes::BufferBase & getPointLightMortonCodesBuffer( uint32_t index )const noexcept
		{
			return m_pointMortonCodesBuffers[index]->getBuffer();
		}

		ashes::BufferBase & getSpotLightMortonCodesBuffer( uint32_t index )const noexcept
		{
			return m_spotMortonCodesBuffers[index]->getBuffer();
		}

		ashes::BufferBase & getInputPointLightIndicesBuffer()const noexcept
		{
			return getPointLightIndicesBuffer( m_pointLightMortonIndicesInput );
		}

		ashes::BufferBase & getInputSpotLightIndicesBuffer()const noexcept
		{
			return getSpotLightIndicesBuffer( m_spotLightMortonIndicesInput );
		}

		std::vector< ashes::BufferBase const * > getOutputPointLightIndicesBuffers()const noexcept
		{
			return { &getPointLightIndicesBuffer( 1u - m_pointLightMortonIndicesInput )
				, &getPointLightIndicesBuffer( m_pointLightMortonIndicesInput ) };
		}

		std::vector< ashes::BufferBase const * > getOutputSpotLightIndicesBuffers()const noexcept
		{
			return { &getSpotLightIndicesBuffer( 1u - m_spotLightMortonIndicesInput )
				, &getSpotLightIndicesBuffer( m_spotLightMortonIndicesInput ) };
		}

		std::vector< ashes::BufferBase const * > getInputPointLightIndicesBuffers()const noexcept
		{
			return { &getPointLightIndicesBuffer( m_pointLightMortonIndicesInput )
				, &getPointLightIndicesBuffer( 1u - m_pointLightMortonIndicesInput ) };
		}

		std::vector< ashes::BufferBase const * > getInputSpotLightIndicesBuffers()const noexcept
		{
			return { &getSpotLightIndicesBuffer( m_spotLightMortonIndicesInput )
				, &getSpotLightIndicesBuffer( 1u - m_spotLightMortonIndicesInput ) };
		}

		ashes::BufferBase & getOutputPointLightIndicesBuffer()const noexcept
		{
			return getPointLightIndicesBuffer( 1u - m_pointLightMortonIndicesInput );
		}

		ashes::BufferBase & getOutputSpotLightIndicesBuffer()const noexcept
		{
			return getSpotLightIndicesBuffer( 1u - m_spotLightMortonIndicesInput );
		}

		ashes::BufferBase & getInputPointLightMortonCodesBuffer()const noexcept
		{
			return getPointLightMortonCodesBuffer( m_pointLightMortonIndicesInput );
		}

		ashes::BufferBase & getInputSpotLightMortonCodesBuffer()const noexcept
		{
			return getSpotLightMortonCodesBuffer( m_spotLightMortonIndicesInput );
		}

		std::vector< ashes::BufferBase const * > getOutputPointLightMortonCodesBuffers()const noexcept
		{
			return { &getPointLightMortonCodesBuffer( 1u - m_pointLightMortonIndicesInput )
				, &getPointLightMortonCodesBuffer( m_pointLightMortonIndicesInput ) };
		}

		std::vector< ashes::BufferBase const * > getOutputSpotLightMortonCodesBuffers()const noexcept
		{
			return { &getSpotLightMortonCodesBuffer( 1u - m_spotLightMortonIndicesInput )
				, &getSpotLightMortonCodesBuffer( m_spotLightMortonIndicesInput ) };
		}

		std::vector< ashes::BufferBase const * > getInputPointLightMortonCodesBuffers()const noexcept
		{
			return { &getPointLightMortonCodesBuffer( m_pointLightMortonIndicesInput )
				, &getPointLightMortonCodesBuffer( 1u - m_pointLightMortonIndicesInput ) };
		}

		std::vector< ashes::BufferBase const * > getInputSpotLightMortonCodesBuffers()const noexcept
		{
			return { &getSpotLightMortonCodesBuffer( m_spotLightMortonIndicesInput )
				, &getSpotLightMortonCodesBuffer( 1u - m_spotLightMortonIndicesInput ) };
		}

		ashes::BufferBase & getOutputPointLightMortonCodesBuffer()const noexcept
		{
			return getPointLightMortonCodesBuffer( 1u - m_pointLightMortonIndicesInput );
		}

		ashes::BufferBase & getOutputSpotLightMortonCodesBuffer()const noexcept
		{
			return getSpotLightMortonCodesBuffer( 1u - m_spotLightMortonIndicesInput );
		}

		ashes::BufferBase & getMergePathPartitionsBuffer()const noexcept
		{
			return m_mergePathPartitionsBuffer->getBuffer();
		}

		ashes::BufferBase & getClusterFlagsBuffer()const noexcept
		{
			return *m_clusterFlags;
		}

		ashes::BufferBase & getUniqueClustersBuffer()const noexcept
		{
			return *m_uniqueClusters;
		}

		ashes::BufferBase & getClustersIndirectBuffer()const noexcept
		{
			return m_clustersIndirect->getBuffer();
		}

		auto & getCamera()const noexcept
		{
			return m_camera;
		}

		auto & getConfig()const noexcept
		{
			return m_config;
		}

		OnClustersBuffersChanged onClusterBuffersChanged;

	private:
		struct AABB
		{
			castor::Point4f min;
			castor::Point4f max;
		};

	private:
		void doUpdate();

	private:
		RenderDevice const & m_device;
		Camera const & m_camera;
		ClustersConfig const & m_config;
		bool m_clustersDirty{ true };
		bool m_lightsDirty{ true };
		int32_t m_first{ 5 };
		uint32_t m_pointLightMortonIndicesInput{ 1u };
		uint32_t m_spotLightMortonIndicesInput{ 1u };
		castor::GroupChangeTracked< castor::Point3ui > m_dimensions;
		castor::GroupChangeTracked< castor::Point2ui > m_clusterSize;
		castor::GroupChangeTracked< castor::Matrix4x4f > m_cameraProjection;
		castor::GroupChangeTracked< castor::Matrix4x4f > m_cameraView;
		ClustersUbo m_clustersUbo;
		ashes::BufferPtr< VkDispatchIndirectCommand > m_clustersIndirect;

		// Fixed size buffers, related to lights
		ashes::BufferPtr< AABB > m_allLightsAABBBuffer;
		ashes::BufferPtr< AABB > m_reducedLightsAABBBuffer;
		std::array< ashes::BufferPtr< u32 >, 2u > m_pointMortonCodesBuffers;
		std::array< ashes::BufferPtr< u32 >, 2u > m_spotMortonCodesBuffers;
		std::array< ashes::BufferPtr< u32 >, 2u > m_pointIndicesBuffers;
		std::array< ashes::BufferPtr< u32 >, 2u > m_spotIndicesBuffers;
		ashes::BufferPtr< AABB > m_pointBVHBuffer;
		ashes::BufferPtr< AABB > m_spotBVHBuffer;
		ashes::BufferPtr< s32 > m_mergePathPartitionsBuffer;

		// Variable size buffers, related to frustum dimensions
		ashes::BufferBasePtr m_aabbBuffer;
		ashes::BufferBasePtr m_pointLightClusterGridBuffer;
		ashes::BufferBasePtr m_spotLightClusterGridBuffer;
		ashes::BufferBasePtr m_pointLightClusterIndexBuffer;
		ashes::BufferBasePtr m_spotLightClusterIndexBuffer;
		ashes::BufferBasePtr m_clusterFlags;
		ashes::BufferBasePtr m_uniqueClusters;
		std::vector< ashes::BufferBasePtr > m_toDelete;
	};
}

#endif
