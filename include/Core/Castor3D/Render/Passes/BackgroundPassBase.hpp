/*
See LICENSE file in root folder
*/
#ifndef ___C3D_BackgroundPassBase_H___
#define ___C3D_BackgroundPassBase_H___

#include "PassesModule.hpp"
#include "Castor3D/Scene/Background/BackgroundModule.hpp"
#include "Castor3D/Shader/Ubos/UbosModule.hpp"

#include "Castor3D/Render/Viewport.hpp"

#include <RenderGraph/RunnablePasses/RenderMesh.hpp>

#include <ashespp/Pipeline/GraphicsPipeline.hpp>

namespace castor3d
{
	class BackgroundPassBase
	{
	public:
		/**
		 *\~english
		 *\brief		Constructor.
		 *\param[in]	pass		The parent frame pass.
		 *\param[in]	context		The rendering context.
		 *\param[in]	graph		The runnable graph.
		 *\param[in]	device		The GPU device.
		 *\param[in]	background	The scene background.
		 *\~french
		 *\brief		Constructeur.
		 *\param[in]	pass		La frame pass parente.
		 *\param[in]	context		Le contexte de rendu.
		 *\param[in]	graph		Le runnable graph.
		 *\param[in]	device		Le device GPU.
		 *\param[in]	background	Le fond de la scène.
		 */
		C3D_API BackgroundPassBase( crg::FramePass const & pass
			, crg::GraphContext & context
			, crg::RunnableGraph & graph
			, RenderDevice const & device
			, SceneBackground & background
			, crg::ImageViewIdArray const & colour );
		C3D_API virtual ~BackgroundPassBase() = default;
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

	protected:
		C3D_API bool doIsEnabled()const;

	private:
		virtual void doResetPipeline( uint32_t index ) = 0;

	protected:
		RenderDevice const & m_device;
		SceneBackground const * m_background;
		Viewport m_viewport;
		OnBackgroundChangedConnection m_onBackgroundChanged;
		crg::ImageViewId m_target;
		uint32_t m_passIndex{};
	};
}

#endif
