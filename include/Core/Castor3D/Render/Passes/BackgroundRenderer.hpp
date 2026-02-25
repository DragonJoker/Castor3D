/*
See LICENSE file in root folder
*/
#ifndef ___C3D_BackgroundPass_H___
#define ___C3D_BackgroundPass_H___

#include "BackgroundPassBase.hpp"
#include "Castor3D/Shader/Ubos/Ubo.hpp"
#include "Castor3D/Shader/Ubos/CameraUbo.hpp"

namespace c3d
{
	class BackgroundRenderer
	{
	public:
		C3D_API BackgroundRenderer( crg::FramePassGroup & graph
			, RenderDevice const & device
			, ProgressBar * progress
			, SceneBackground & background
			, RenderUbo const & renderUbo
			, SceneUbo const & sceneUbo
			, Texture & colour
			, bool clearColour
			, bool clearDepth
			, bool forceVisible
			, Texture * depth
			, Texture const * depthObj = nullptr );

		BackgroundRenderer( crg::FramePassGroup & graph
			, RenderDevice const & device
			, ProgressBar * progress
			, SceneBackground & background
			, RenderUbo const & renderUbo
			, SceneUbo const & sceneUbo
			, Texture & colour
			, bool clearColour = true
			, bool forceVisible = false )
			: BackgroundRenderer{ graph
				, device
				, progress
				, background
				, renderUbo
				, sceneUbo
				, colour
				, clearColour
				, false
				, forceVisible
				, nullptr
				, nullptr }
		{
		}
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

	private:
		void doCreatePass( crg::FramePassGroup & graph
			, SceneBackground & background
			, RenderUbo const & renderUbo
			, SceneUbo const & sceneUbo
			, Texture & colour
			, bool clearColour
			, bool clearDepth
			, bool forceVisible
			, Texture * depth
			, Texture const * depthObj
			, ProgressBar * progress );

	private:
		RenderDevice const & m_device;
		Texture & m_colour;
		CameraUbo m_cameraUbo;
		UboT< ModelBufferConfiguration > m_modelUbo;
		BackgroundPassBase * m_backgroundPass{};
	};
}

#endif
