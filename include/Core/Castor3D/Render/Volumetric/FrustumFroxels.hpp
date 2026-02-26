/*
See LICENSE file in root folder
*/
#ifndef ___C3D_FrustumFroxels_H___
#define ___C3D_FrustumFroxels_H___

#include "Castor3D/Render/Clustered/ClusteredModule.hpp"
#include "Castor3D/Scene/SceneModule.hpp"

#include "Castor3D/Render/Viewport.hpp"
#include "Castor3D/Shader/Ubos/FroxelsUbo.hpp"

#include <CastorUtils/Design/GroupChangeTracked.hpp>

#include <ashespp/Buffer/Buffer.hpp>

namespace c3d
{
	class DebugDrawer;

	class FrustumFroxels
	{
	public:
		C3D_API FrustumFroxels( FrustumFroxels const & ) = delete;
		C3D_API FrustumFroxels( FrustumFroxels && )noexcept = delete;
		C3D_API FrustumFroxels & operator=( FrustumFroxels const & ) = delete;
		C3D_API FrustumFroxels & operator=( FrustumFroxels && )noexcept = delete;

		C3D_API FrustumFroxels( RenderDevice const & device
			, crg::ResourcesCache & resources
			, FrustumClusters const & clusters
			, FroxelsConfig const & config );
		C3D_API ~FrustumFroxels()noexcept;
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
		 *\brief		Registers the froxels related frame passes.
		 *\~french
		 *\brief		Enregistre les passes liées aux froxels.
		 */
		C3D_API void createFramePasses( crg::FramePassGroup & graph );
		/**
		 *\~english
		 *\brief		Creates the programs used to display froxels debug data.
		 *\~french
		 *\brief		Crée les programmes utilisés pour afficher les données de débogage des froxels.
		 */
		C3D_API void createDebugDisplayPrograms( CameraUbo const & cameraUbo
			, RenderUbo const & renderUbo );
		/**
		 *\~english
		 *\return		The parent scene.
		 *\~french
		 *\brief		La scène parente.
		 */
		C3D_API Scene const & getScene()const noexcept;
		/**
		 *\~english
		 *\return		The camera UBO.
		 *\~french
		 *\return		L'UBO de caméra.
		 */
		C3D_API CameraUbo const & getCameraUbo()const noexcept;
		/**
		 *\~english
		 *\return		The clusters UBO.
		 *\~french
		 *\return		L'UBO de clusters.
		 */
		C3D_API ClustersUbo const & getClustersUbo()const noexcept;
		/**
		 *\~english
		 *\return		The camera.
		 *\~french
		 *\return		La caméra.
		 */
		C3D_API Camera const & getCamera()const noexcept;

		Point3ui const & getDimensions()const noexcept
		{
			return m_finalDimensions;
		}

		bool const & needsLightsUpdate()const noexcept
		{
			return m_lightsDirty;
		}

		auto & getFroxelsUbo()const noexcept
		{
			return m_froxelsUbo;
		}

		BufferBase const & getCounterBuffer()const noexcept
		{
			return m_counterBuffer;
		}

		BufferBase const & getIndirectBuffer()const noexcept
		{
			return m_froxelsIndirectBuffer;
		}

		BufferBase const & getFroxelsPointLightsBuffer()const noexcept
		{
			return m_froxelsPointLightsBuffer;
		}

		BufferBase const & getFroxelsSpotLightsBuffer()const noexcept
		{
			return m_froxelsSpotLightsBuffer;
		}

		crg::BufferViewId const & getPointIndirectBuffer()const noexcept
		{
			return m_froxelsPointIndirectBuffer;
		}

		crg::BufferViewId const & getSpotIndirectBuffer()const noexcept
		{
			return m_froxelsSpotIndirectBuffer;
		}

		Point2f const & getRenderSize()const noexcept
		{
			return m_finalRenderSize;
		}

		Viewport const & getViewport()const noexcept
		{
			return m_viewport;
		}

	private:
		void doUpdate( Size const & renderSize );
		void doUpdateBlurKernel();

	private:
		RenderDevice const & m_device;
		FrustumClusters const & m_clusters;
		FroxelsConfig const & m_config;
		Viewport m_viewport;
		bool m_froxelsDirty{ true };
		bool m_lightsDirty{ true };
		int32_t m_first{ 5 };
		Size m_rawRenderSize;
		Point2f m_finalRenderSize;
		Point3ui m_rawDimensions{ 128u, 128u, 128u };
		Point3ui m_finalDimensions{ 128u, 128u, 128u };
		Matrix4x4f m_projection;
		Matrix4x4f m_projectionInverse;
		f64 m_powerBase{};
		f64 m_powerRatio{};
		u32 m_gridDepth{};
		u32 m_blurFilterHalfSize{};
		Vector< f32 > m_kernelWeights;
		Vector< f32 > m_depthSegments;
		GroupChangeTracked< Point2f > m_froxelSize;
		Point2f m_renderOffset;
		FroxelsUbo m_froxelsUbo;
		BufferT< u32 > m_counterBuffer;
		BufferT< VkDispatchIndirectCommand > m_froxelsIndirectBuffer;
		crg::BufferViewId m_froxelsPointIndirectBuffer;
		crg::BufferViewId m_froxelsSpotIndirectBuffer;
		BufferT< Point2ui > m_froxelsPointLightsBuffer;
		BufferT< Point2ui > m_froxelsSpotLightsBuffer;
		Texture m_rawFroxelsLightingU32R;
		Texture m_rawFroxelsLightingU32G;
		Texture m_rawFroxelsLightingU32B;
		Texture m_rawFroxelsLighting;
		Texture m_finalFroxelsLighting;

		ashes::PipelineShaderStageCreateInfoArray m_displayFroxelsAABBProgram;
		ashes::VkDescriptorSetLayoutBindingArray m_displayFroxelsAABBBindings;
		ashes::WriteDescriptorSetArray m_displayFroxelsAABBWrites;
	};
}

#endif
