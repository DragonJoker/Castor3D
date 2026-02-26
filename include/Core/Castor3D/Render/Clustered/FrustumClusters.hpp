/*
See LICENSE file in root folder
*/
#ifndef ___C3D_FrustumClusters_H___
#define ___C3D_FrustumClusters_H___

#include "Castor3D/Scene/SceneModule.hpp"

#include "Castor3D/Buffer/GpuBufferOffset.hpp"
#include "Castor3D/Render/Clustered/ClustersConfig.hpp"
#include "Castor3D/Shader/Ubos/ClustersUbo.hpp"
#include "Castor3D/Shader/Ubos/FroxelsUbo.hpp"
#include "Castor3D/Shader/Ubos/CameraUbo.hpp"

#include <CastorUtils/Design/GroupChangeTracked.hpp>
#include <CastorUtils/Design/Signal.hpp>

#include <ashespp/Buffer/Buffer.hpp>

namespace c3d
{
	class DebugDrawer;

	class FrustumClusters
	{
	public:
		C3D_API FrustumClusters( FrustumClusters const & ) = delete;
		C3D_API FrustumClusters( FrustumClusters && )noexcept = delete;
		C3D_API FrustumClusters & operator=( FrustumClusters const & ) = delete;
		C3D_API FrustumClusters & operator=( FrustumClusters && )noexcept = delete;

		C3D_API FrustumClusters( RenderDevice const & device
			, crg::ResourcesCache & resources
			, Camera const & camera
			, ClustersConfig const & config );
		C3D_API ~FrustumClusters()noexcept;
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
		C3D_API void updateDebug( DebugDrawer & drawer )const;
		/**
		 *\~english
		 *\brief		Registers the clusters related frame passes.
		 *\~french
		 *\brief		Enregistre les passes liées aux clusters.
		 */
		C3D_API void createFramePasses( crg::FramePassGroup & graph
			, RenderUbo const & renderUbo );
		/**
		 *\~english
		 *\brief		Creates the programs used to display clusters debug data.
		 *\~french
		 *\brief		Crée les programmes utilisés pour afficher les données de débogage des clusters.
		 */
		C3D_API void createDebugDisplayPrograms( CameraUbo const & cameraUbo );
		/**
		 *\~english
		 *\return		The parent scene.
		 *\~french
		 *\brief		La scène parente.
		 */
		C3D_API Scene const & getScene()const noexcept;
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

		Point3ui const & getDimensions()const noexcept
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

		BufferBase const & getPointLightClusterGridBuffer()const noexcept
		{
			return m_pointBuffers.clusterGrid;
		}

		BufferBase const & getSpotLightClusterGridBuffer()const noexcept
		{
			return m_spotBuffers.clusterGrid;
		}

		BufferBase const & getPointLightClusterIndexBuffer()const noexcept
		{
			return m_pointBuffers.clusterIndex;
		}

		BufferBase const & getSpotLightClusterIndexBuffer()const noexcept
		{
			return m_spotBuffers.clusterIndex;
		}

		BufferBase const & getReducedLightsAABBBuffer()const noexcept
		{
			return m_reducedLightsAABBBuffer;
		}

		BufferBase const & getAllLightsAABBBuffer()const noexcept
		{
			return m_allLightsAABBBuffer;
		}

		BufferBase const & getPointLightBVHBuffer()const noexcept
		{
			return m_pointBuffers.bvh;
		}

		BufferBase const & getSpotLightBVHBuffer()const noexcept
		{
			return m_spotBuffers.bvh;
		}

		crg::Attachment const & getPointLightIndicesBuffer()const noexcept
		{
			return *m_sortAttachs[MortonIndicesOutput].pointLightIndices;
		}

		crg::Attachment const & getSpotLightIndicesBuffer()const noexcept
		{
			return *m_sortAttachs[MortonIndicesOutput].spotLightIndices;
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
		static constexpr uint32_t MortonIndicesInput{ 1u };
		static constexpr uint32_t MortonIndicesOutput{ 0u };

	private:
		struct AABB
		{
			Point4f min;
			Point4f max;
		};

		struct Buffers
		{
			Buffers( RenderDevice const & device
				, crg::ResourcesCache & resources
				, String const & name );
			~Buffers()noexcept;

			// Fixed size buffers, related to lights
			Array< BufferT< u32 >, 2u > mortonCodes;
			Array< BufferT< u32 >, 2u > indices;
			BufferT< AABB > bvh;
			// Variable size buffers, related to frustum dimensions
			BufferT< Point2ui > clusterGrid;
			BufferT< u32 > clusterIndex;
			//
			crg::BufferViewIdArray inputIndices;
			crg::BufferViewIdArray outputIndices;
			crg::BufferViewIdArray inputMortonCodes;
			crg::BufferViewIdArray outputMortonCodes;
		};

	private:
		void doUpdate( Size const & rawRenderSize, Point2f finalRenderSize, Viewport const * viewport );

	private:
		RenderDevice const & m_device;
		Camera const & m_camera;
		ClustersConfig const & m_config;
		bool m_clustersDirty{ true };
		bool m_lightsDirty{ true };
		int32_t m_first{ 5 };
		Size m_rawRenderSize;
		Point2f m_finalRenderSize;
		Point3ui m_dimensions{ 32u, 16u, 64u };
		u32 m_clusterCount{ 32u * 16u * 64u };
		GroupChangeTracked< Point2f > m_clusterSize;
		GroupChangeTracked< Matrix4x4f > m_cameraProjection;
		GroupChangeTracked< Matrix4x4f > m_cameraView;
		ClustersUbo m_clustersUbo;
		CameraUbo m_clustersCameraUbo;
		BufferT< VkDispatchIndirectCommand > m_clustersIndirect;
		BufferT< s32 > m_mergePathPartitions;

		// Fixed size buffers, related to lights
		BufferT< AABB > m_allLightsAABBBuffer;
		BufferT< AABB > m_reducedLightsAABBBuffer;
		// Light type specific buffers
		Buffers m_pointBuffers;
		Buffers m_spotBuffers;
		Array< ClustersLightSortAttachs, 2u > m_sortAttachs;

		// Variable size buffers, related to frustum dimensions
		BufferT< AABB > m_aabbBuffer;
		Buffer m_clusterFlagsBuffer;
		Buffer m_uniqueClusters;

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
