﻿/*
See LICENSE file in root folder
*/
#ifndef ___C3D_FrustumClusters_H___
#define ___C3D_FrustumClusters_H___

#include "ClusteredModule.hpp"
#include "Castor3D/Scene/SceneModule.hpp"

#include "Castor3D/Buffer/GpuBufferOffset.hpp"
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
		/**
		 *\~english
		 *\brief		Registers the clusters related frame passes.
		 *\~french
		 *\brief		Enregistre les passes liées aux clusters.
		 */
		C3D_API crg::FramePass const & createFramePasses( crg::FramePassGroup & graph
			, crg::FramePass const * previousPass
			, CameraUbo const & cameraUbo );
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

		auto & getLightsAABBBuffer()const noexcept
		{
			CU_Require( m_lightsAABBBuffer );
			return *m_lightsAABBBuffer;
		}

		auto & getPointLightBVHBuffer()const noexcept
		{
			CU_Require( m_pointBVHBuffer );
			return *m_pointBVHBuffer;
		}

		auto & getSpotLightBVHBuffer()const noexcept
		{
			CU_Require( m_spotBVHBuffer );
			return *m_spotBVHBuffer;
		}

		void swapLightMortonIndicesIO()
		{
			m_lightMortonIndicesInput = 1u - m_lightMortonIndicesInput;
		}

		ashes::BufferBase & getPointLightIndicesBuffer( uint32_t index )const noexcept
		{
			return *m_pointIndicesBuffers[index];
		}

		ashes::BufferBase & getSpotLightIndicesBuffer( uint32_t index )const noexcept
		{
			return *m_spotIndicesBuffers[index];
		}

		ashes::BufferBase & getPointLightMortonCodesBuffer( uint32_t index )const noexcept
		{
			return *m_pointMortonCodesBuffers[index];
		}

		ashes::BufferBase & getSpotLightMortonCodesBuffer( uint32_t index )const noexcept
		{
			return *m_spotMortonCodesBuffers[index];
		}

		ashes::BufferBase & getInputPointLightIndicesBuffer()const noexcept
		{
			return getPointLightIndicesBuffer( m_lightMortonIndicesInput );
		}

		ashes::BufferBase & getInputSpotLightIndicesBuffer()const noexcept
		{
			return getSpotLightIndicesBuffer( m_lightMortonIndicesInput );
		}

		ashes::BufferBase & getOutputPointLightIndicesBuffer()const noexcept
		{
			return getPointLightIndicesBuffer( 1u - m_lightMortonIndicesInput );
		}

		ashes::BufferBase & getOutputSpotLightIndicesBuffer()const noexcept
		{
			return getSpotLightIndicesBuffer( 1u - m_lightMortonIndicesInput );
		}

		ashes::BufferBase & getInputPointLightMortonCodesBuffer()const noexcept
		{
			return getPointLightMortonCodesBuffer( m_lightMortonIndicesInput );
		}

		ashes::BufferBase & getInputSpotLightMortonCodesBuffer()const noexcept
		{
			return getSpotLightMortonCodesBuffer( m_lightMortonIndicesInput );
		}

		ashes::BufferBase & getOutputPointLightMortonCodesBuffer()const noexcept
		{
			return getPointLightMortonCodesBuffer( 1u - m_lightMortonIndicesInput );
		}

		ashes::BufferBase & getOutputSpotLightMortonCodesBuffer()const noexcept
		{
			return getSpotLightMortonCodesBuffer( 1u - m_lightMortonIndicesInput );
		}

		auto & getCamera()const noexcept
		{
			return m_camera;
		}

		OnClustersBuffersChanged onClusterBuffersChanged;

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
		uint32_t m_lightMortonIndicesInput{ 1u };
		castor::GroupChangeTracked< castor::Point3ui > m_dimensions;
		castor::GroupChangeTracked< uint32_t > m_clusterSize;
		castor::GroupChangeTracked< castor::Matrix4x4f > m_cameraProjection;
		castor::GroupChangeTracked< castor::Matrix4x4f > m_cameraView;
		castor::GroupChangeTracked< float > m_nearK;
		ClustersUbo m_clustersUbo;
		ashes::BufferBasePtr m_aabbBuffer;
		ashes::BufferBasePtr m_pointLightClusterGridBuffer;
		ashes::BufferBasePtr m_spotLightClusterGridBuffer;
		ashes::BufferBasePtr m_pointLightClusterIndexBuffer;
		ashes::BufferBasePtr m_spotLightClusterIndexBuffer;
		ashes::BufferBasePtr m_lightsAABBBuffer;
		std::array< ashes::BufferBasePtr, 2u > m_pointMortonCodesBuffers;
		std::array< ashes::BufferBasePtr, 2u > m_spotMortonCodesBuffers;
		std::array< ashes::BufferBasePtr, 2u > m_pointIndicesBuffers;
		std::array< ashes::BufferBasePtr, 2u > m_spotIndicesBuffers;
		ashes::BufferBasePtr m_pointBVHBuffer;
		ashes::BufferBasePtr m_spotBVHBuffer;
		std::vector< ashes::BufferBasePtr > m_toDelete;
	};
}

#endif
