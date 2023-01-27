/*
See LICENSE file in root folder
*/
#ifndef ___C3D_BackgroundPass_H___
#define ___C3D_BackgroundPass_H___

#include "BackgroundPassBase.hpp"
#include "Castor3D/Shader/Ubos/MatrixUbo.hpp"

namespace castor3d
{
	class BackgroundRenderer
	{
	private:
		C3D_API BackgroundRenderer( crg::FramePassGroup & graph
			, crg::FramePassArray previousPasses
			, RenderDevice const & device
			, ProgressBar * progress
			, castor::String const & name
			, SceneBackground & background
			, HdrConfigUbo const & hdrConfigUbo
			, SceneUbo const & sceneUbo
			, crg::ImageViewIdArray const & colour
			, bool clearColour
			, crg::ImageViewId const * depth
			, crg::ImageViewId const * depthObj );

	public:
		BackgroundRenderer( crg::FramePassGroup & graph
			, crg::FramePass const * previousPass
			, RenderDevice const & device
			, ProgressBar * progress
			, castor::String const & name
			, SceneBackground & background
			, HdrConfigUbo const & hdrConfigUbo
			, SceneUbo const & sceneUbo
			, crg::ImageViewId const & colour
			, bool clearColour = true )
			: BackgroundRenderer{ graph
				, ( previousPass
					? crg::FramePassArray{ previousPass }
					: crg::FramePassArray{} )
				, device
				, progress
				, name
				, background
				, hdrConfigUbo
				, sceneUbo
				, { colour }
				, clearColour
				, nullptr
				, nullptr }
		{
		}

		BackgroundRenderer( crg::FramePassGroup & graph
			, crg::FramePass const * previousPass
			, RenderDevice const & device
			, ProgressBar * progress
			, castor::String const & name
			, SceneBackground & background
			, HdrConfigUbo const & hdrConfigUbo
			, SceneUbo const & sceneUbo
			, crg::ImageViewId const & colour
			, bool clearColour
			, crg::ImageViewId const & depth
			, crg::ImageViewId const * depthObj )
			: BackgroundRenderer{ graph
				, ( previousPass
					? crg::FramePassArray{ previousPass }
					: crg::FramePassArray{} )
				, device
				, progress
				, name
				, background
				, hdrConfigUbo
				, sceneUbo
				, { colour }
				, clearColour
				, &depth
				, depthObj }
		{
		}

		BackgroundRenderer( crg::FramePassGroup & graph
			, crg::FramePassArray previousPasses
			, RenderDevice const & device
			, ProgressBar * progress
			, castor::String const & name
			, SceneBackground & background
			, HdrConfigUbo const & hdrConfigUbo
			, SceneUbo const & sceneUbo
			, crg::ImageViewIdArray const & colour
			, bool clearColour
			, crg::ImageViewId const & depth
			, crg::ImageViewId const * depthObj )
			: BackgroundRenderer{ graph
				, std::move( previousPasses )
				, device
				, progress
				, name
				, background
				, hdrConfigUbo
				, sceneUbo
				, colour
				, clearColour
				, &depth
				, depthObj }
		{
		}

		C3D_API ~BackgroundRenderer();
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

		crg::FramePass const & getPass()const
		{
			return *m_backgroundPassDesc;
		}

	private:
		crg::FramePass const & doCreatePass( crg::FramePassGroup & graph
			, crg::FramePassArray previousPasses
			, castor::String const & name
			, SceneBackground & background
			, HdrConfigUbo const & hdrConfigUbo
			, SceneUbo const & sceneUbo
			, crg::ImageViewIdArray const & colour
			, bool clearColour
			, crg::ImageViewId const * depth
			, crg::ImageViewId const * depthObj
			, ProgressBar * progress );

	private:
		RenderDevice const & m_device;
		crg::ImageViewIdArray m_colour;
		MatrixUbo m_matrixUbo;
		UniformBufferOffsetT< ModelBufferConfiguration > m_modelUbo;
		crg::FramePass const * m_backgroundPassDesc{};
		BackgroundPassBase * m_backgroundPass{};
	};
}

#endif
