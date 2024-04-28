/*
See LICENSE file in root folder
*/
#ifndef ___C3D_FrustumClusters_H___
#define ___C3D_FrustumClusters_H___

#include "Castor3D/Scene/SceneModule.hpp"

#include "Castor3D/Buffer/GpuBufferOffset.hpp"
#include "Castor3D/Render/Clustered/ClustersConfig.hpp"
#include "Castor3D/Shader/Ubos/ClustersUbo.hpp"
#include "Castor3D/Shader/Ubos/CameraUbo.hpp"

#include <CastorUtils/Design/GroupChangeTracked.hpp>
#include <CastorUtils/Design/Signal.hpp>

#include <ashespp/Buffer/Buffer.hpp>

namespace castor3d
{
	class DebugDrawer;

	class FrustumClusters
	{
	public:
		C3D_API FrustumClusters( FrustumClusters const & ) = delete;
		C3D_API FrustumClusters( FrustumClusters && )noexcept = delete;
		C3D_API FrustumClusters & operator=( FrustumClusters const & ) = delete;
		C3D_API FrustumClusters & operator=( FrustumClusters && )noexcept = delete;
		C3D_API ~FrustumClusters()noexcept = default;

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
		 *\brief			Debug update.
		 *\param[in, out]	drawer	The debug drawer.
		 *\~french
		 *\brief			Mise à jour du debug.
		 *\param[in, out]	drawer	Le debug drawer.
		 */
		C3D_API void updateDebug( DebugDrawer & drawer );
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
		 *\param[in]	level	The BVH level.
		 *\return		The number of nodes for given BVH level.
		 *\~french
		 *\param[in]	level	Le niveau du BVH.
		 *\return		Le nombre de noeuds du niveau donné d'un BVH.
		 */
		C3D_API static uint32_t getNumLevelNodes( uint32_t level );
		/**
		 *\~english
		 *\param[in]	numLeaves	The number of leaf nodes.
		 *\return		The number of levels needed for a BVH that consists of a number of leaf nodes.
		 *\~french
		 *\param[in]	numLeaves	Le nombre de noeuds feuilles.
		 *\return		Le nombre de niveaux nécessaire pour un BVH contenant le nombre de noeuds feuille donné.
		 */
		C3D_API static uint32_t getNumLevels( uint32_t numLeaves );
		/**
		 *\~english
		 *\param[in]	numLeaves	The number of leaf nodes.
		 *\return		The number of (child) nodes needed to represent a BVH that consists of a number of leaf nodes.
		 *\~french
		 *\param[in]	numLeaves	Le nombre de noeuds feuilles.
		 *\return		Le nombre de noeuds (enfants) nécessaire pour un représenter un BVH contenant le nombre de noeuds feuille donné.
		 */
		C3D_API static uint32_t getNumNodes( uint32_t numLeaves );

		C3D_API static uint32_t getBucketSortBucketSize();

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
			CU_Require( m_pointBuffers.clusterGrid );
			return *m_pointBuffers.clusterGrid;
		}

		auto & getSpotLightClusterGridBuffer()const noexcept
		{
			CU_Require( m_spotBuffers.clusterGrid );
			return *m_spotBuffers.clusterGrid;
		}

		auto & getPointLightClusterIndexBuffer()const noexcept
		{
			CU_Require( m_pointBuffers.clusterIndex );
			return *m_pointBuffers.clusterIndex;
		}

		auto & getSpotLightClusterIndexBuffer()const noexcept
		{
			CU_Require( m_spotBuffers.clusterIndex );
			return *m_spotBuffers.clusterIndex;
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
			return m_pointBuffers.bvh->getBuffer();
		}

		auto & getSpotLightBVHBuffer()const noexcept
		{
			return m_spotBuffers.bvh->getBuffer();
		}

		ashes::BufferBase & getPointLightIndicesBuffer( uint32_t index )const noexcept
		{
			return m_pointBuffers.indices[index]->getBuffer();
		}

		ashes::BufferBase & getSpotLightIndicesBuffer( uint32_t index )const noexcept
		{
			return m_spotBuffers.indices[index]->getBuffer();
		}

		ashes::BufferBase & getPointLightMortonCodesBuffer( uint32_t index )const noexcept
		{
			return m_pointBuffers.mortonCodes[index]->getBuffer();
		}

		ashes::BufferBase & getSpotLightMortonCodesBuffer( uint32_t index )const noexcept
		{
			return m_spotBuffers.mortonCodes[index]->getBuffer();
		}

		ashes::BufferBase & getInputPointLightIndicesBuffer()const noexcept
		{
			return getPointLightIndicesBuffer( m_pointLightMortonIndicesInput );
		}

		ashes::BufferBase & getInputSpotLightIndicesBuffer()const noexcept
		{
			return getSpotLightIndicesBuffer( m_spotLightMortonIndicesInput );
		}

		castor::Vector< ashes::BufferBase const * > getOutputPointLightIndicesBuffers()const noexcept
		{
			return { &getPointLightIndicesBuffer( 1u - m_pointLightMortonIndicesInput )
				, &getPointLightIndicesBuffer( m_pointLightMortonIndicesInput ) };
		}

		castor::Vector< ashes::BufferBase const * > getOutputSpotLightIndicesBuffers()const noexcept
		{
			return { &getSpotLightIndicesBuffer( 1u - m_spotLightMortonIndicesInput )
				, &getSpotLightIndicesBuffer( m_spotLightMortonIndicesInput ) };
		}

		castor::Vector< ashes::BufferBase const * > getInputPointLightIndicesBuffers()const noexcept
		{
			return { &getPointLightIndicesBuffer( m_pointLightMortonIndicesInput )
				, &getPointLightIndicesBuffer( 1u - m_pointLightMortonIndicesInput ) };
		}

		castor::Vector< ashes::BufferBase const * > getInputSpotLightIndicesBuffers()const noexcept
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

		castor::Vector< ashes::BufferBase const * > getOutputPointLightMortonCodesBuffers()const noexcept
		{
			return { &getPointLightMortonCodesBuffer( 1u - m_pointLightMortonIndicesInput )
				, &getPointLightMortonCodesBuffer( m_pointLightMortonIndicesInput ) };
		}

		castor::Vector< ashes::BufferBase const * > getOutputSpotLightMortonCodesBuffers()const noexcept
		{
			return { &getSpotLightMortonCodesBuffer( 1u - m_spotLightMortonIndicesInput )
				, &getSpotLightMortonCodesBuffer( m_spotLightMortonIndicesInput ) };
		}

		castor::Vector< ashes::BufferBase const * > getInputPointLightMortonCodesBuffers()const noexcept
		{
			return { &getPointLightMortonCodesBuffer( m_pointLightMortonIndicesInput )
				, &getPointLightMortonCodesBuffer( 1u - m_pointLightMortonIndicesInput ) };
		}

		castor::Vector< ashes::BufferBase const * > getInputSpotLightMortonCodesBuffers()const noexcept
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

		auto & getCameraUbo()const noexcept
		{
			return m_clustersCameraUbo;
		}

		OnClustersBuffersChanged onClusterBuffersChanged;

	private:
		struct AABB
		{
			castor::Point4f min;
			castor::Point4f max;
		};

		struct Buffers
		{
			Buffers( RenderDevice const & device
				, castor::String const & name );

			// Fixed size buffers, related to lights
			castor::Array< ashes::BufferPtr< u32 >, 2u > mortonCodes;
			castor::Array< ashes::BufferPtr< u32 >, 2u > indices;
			ashes::BufferPtr< AABB > bvh;
			// Variable size buffers, related to frustum dimensions
			ashes::BufferBasePtr clusterGrid;
			ashes::BufferBasePtr clusterIndex;
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
		CameraUbo m_clustersCameraUbo;
		ashes::BufferPtr< VkDispatchIndirectCommand > m_clustersIndirect;

		// Fixed size buffers, related to lights
		ashes::BufferPtr< AABB > m_allLightsAABBBuffer;
		ashes::BufferPtr< AABB > m_reducedLightsAABBBuffer;
		ashes::BufferPtr< s32 > m_mergePathPartitionsBuffer;
		// Light type specific buffers
		Buffers m_pointBuffers;
		Buffers m_spotBuffers;

		// Variable size buffers, related to frustum dimensions
		ashes::BufferBasePtr m_aabbBuffer;
		ashes::BufferBasePtr m_clusterFlags;
		ashes::BufferBasePtr m_uniqueClusters;
		castor::Vector< ashes::BufferBasePtr > m_toDelete;

		ashes::PipelineShaderStageCreateInfoArray m_displayClustersAABBProgram;
		ashes::VkDescriptorSetLayoutBindingArray m_displayClustersAABBBindings;
		ashes::WriteDescriptorSetArray m_displayClustersAABBWrites;

		ashes::PipelineShaderStageCreateInfoArray m_displayLightsAABBProgram;
		ashes::VkDescriptorSetLayoutBindingArray m_displayLightsAABBBindings;
		ashes::WriteDescriptorSetArray m_displayLightsAABBWrites;

		ashes::PipelineShaderStageCreateInfoArray m_displayPointLightsBVHProgram;
		ashes::VkDescriptorSetLayoutBindingArray m_displayPointLightsBVHBindings;
		ashes::WriteDescriptorSetArray m_displayPointLightsBVHWrites;

		ashes::PipelineShaderStageCreateInfoArray m_displaySpotLightsBVHProgram;
		ashes::VkDescriptorSetLayoutBindingArray m_displaySpotLightsBVHBindings;
		ashes::WriteDescriptorSetArray m_displaySpotLightsBVHWrites;
	};
}

#endif
