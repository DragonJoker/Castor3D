/*
See LICENSE file in root folder
*/
#ifndef ___C3D_BackgroundPass_H___
#define ___C3D_BackgroundPass_H___

#include "BackgroundPassBase.hpp"
#include "Castor3D/Shader/Ubos/CameraUbo.hpp"

namespace castor3d
{
	class BackgroundRenderer
	{
	public:
		C3D_API BackgroundRenderer( crg::FramePassGroup & graph
			, crg::FramePassArray const & previousPasses
			, RenderDevice const & device
			, ProgressBar * progress
			, SceneBackground & background
			, HdrConfigUbo const & hdrConfigUbo
			, SceneUbo const & sceneUbo
			, crg::ImageViewIdArray const & colour
			, bool clearColour
			, bool clearDepth
			, bool forceVisible
			, crg::ImageViewIdArray const & depth
			, crg::ImageViewId const * depthObj );

		BackgroundRenderer( crg::FramePassGroup & graph
			, crg::FramePass const * previousPass
			, RenderDevice const & device
			, ProgressBar * progress
			, SceneBackground & background
			, HdrConfigUbo const & hdrConfigUbo
			, SceneUbo const & sceneUbo
			, crg::ImageViewId const & colour
			, bool clearColour = true
			, bool forceVisible = false )
			: BackgroundRenderer{ graph
				, ( previousPass
					? crg::FramePassArray{ previousPass }
					: crg::FramePassArray{} )
				, device
				, progress
				, background
				, hdrConfigUbo
				, sceneUbo
				, crg::ImageViewIdArray{ colour }
				, clearColour
				, false
				, forceVisible
				, crg::ImageViewIdArray{}
				, nullptr }
		{
		}

		BackgroundRenderer( crg::FramePassGroup & graph
			, crg::FramePass const * previousPass
			, RenderDevice const & device
			, ProgressBar * progress
			, SceneBackground & background
			, HdrConfigUbo const & hdrConfigUbo
			, SceneUbo const & sceneUbo
			, crg::ImageViewId const & colour
			, bool clearColour
			, bool clearDepth
			, bool forceVisible
			, crg::ImageViewId const & depth
			, crg::ImageViewId const * depthObj )
			: BackgroundRenderer{ graph
				, ( previousPass
					? crg::FramePassArray{ previousPass }
					: crg::FramePassArray{} )
				, device
				, progress
				, background
				, hdrConfigUbo
				, sceneUbo
				, crg::ImageViewIdArray{ colour }
				, clearColour
				, clearDepth
				, forceVisible
				, crg::ImageViewIdArray{ depth }
				, depthObj }
		{
		}

		BackgroundRenderer( crg::FramePassGroup & graph
			, crg::FramePassArray const & previousPasses
			, RenderDevice const & device
			, ProgressBar * progress
			, SceneBackground & background
			, HdrConfigUbo const & hdrConfigUbo
			, SceneUbo const & sceneUbo
			, crg::ImageViewIdArray const & colour
			, bool clearColour
			, bool clearDepth
			, bool forceVisible
			, crg::ImageViewId const & depth
			, crg::ImageViewId const * depthObj )
			: BackgroundRenderer{ graph
				, previousPasses
				, device
				, progress
				, background
				, hdrConfigUbo
				, sceneUbo
				, colour
				, clearColour
				, clearDepth
				, forceVisible
				, crg::ImageViewIdArray{ depth }
				, depthObj }
		{
		}

		C3D_API ~BackgroundRenderer()noexcept;
		/**
		 *\~english
		 *\brief			Updates the render pass, CPU wise.
		 *\param[in, out]	updater	The update data.
		 *\~french
		 *\brief			Met à jour la passe de rendu, au niveau CPU.
		 *\param[in, out]	updater	Les données d'update.
		 */
		C3D_API void update( CpuUpdater & updater );
		/**
		 *\~english
		 *\brief			Updates the render pass, GPU wise.
		 *\param[in, out]	updater	The update data.
		 *\~french
		 *\brief			Met à jour la passe de rendu, au niveau GPU.
		 *\param[in, out]	updater	Les données d'update.
		 */
		C3D_API void update( GpuUpdater & updater );

		crg::FramePass const & getPass()const noexcept
		{
			return *m_backgroundPassDesc;
		}

	private:
		crg::FramePass const & doCreatePass( crg::FramePassGroup & graph
			, crg::FramePassArray const & previousPasses
			, SceneBackground & background
			, HdrConfigUbo const & hdrConfigUbo
			, SceneUbo const & sceneUbo
			, crg::ImageViewIdArray const & colour
			, bool clearColour
			, bool clearDepth
			, bool forceVisible
			, crg::ImageViewIdArray const & depth
			, crg::ImageViewId const * depthObj
			, ProgressBar * progress );

	private:
		RenderDevice const & m_device;
		crg::ImageViewIdArray m_colour;
		CameraUbo m_cameraUbo;
		UniformBufferOffsetT< ModelBufferConfiguration > m_modelUbo;
		crg::FramePass const * m_backgroundPassDesc{};
		BackgroundPassBase * m_backgroundPass{};
	};
}

#endif
