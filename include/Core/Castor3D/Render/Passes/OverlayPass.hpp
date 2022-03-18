/*
See LICENSE file in root folder
*/
#ifndef ___C3D_OverlayPass_H___
#define ___C3D_OverlayPass_H___

#include "PassesModule.hpp"
#include "Castor3D/Overlay/OverlayModule.hpp"

#include <RenderGraph/RunnablePass.hpp>
#include <RenderGraph/RunnablePasses/RenderPass.hpp>

namespace castor3d
{
	class OverlayPass
		: public crg::RunnablePass
	{
	public:
		/**
		 *\~english
		 *\brief		Constructor.
		 *\param[in]	pass	The parent frame pass.
		 *\param[in]	context	The rendering context.
		 *\param[in]	graph	The runnable graph.
		 *\param[in]	device	The GPU device.
		 *\param[in]	scene	The scene.
		 *\param[in]	size	The render area dimensions.
		 *\param[in]	output	The result texture.
		 *\~french
		 *\brief		Constructeur.
		 *\param[in]	pass	La frame pass parente.
		 *\param[in]	context	Le contexte de rendu.
		 *\param[in]	graph	Le runnable graph.
		 *\param[in]	device	Le device GPU.
		 *\param[in]	scene	La scène.
		 *\param[in]	size	Les dimensions de la zone de rendu.
		 *\param[in]	output	La texture résultat.
		 */
		C3D_API OverlayPass( crg::FramePass const & pass
			, crg::GraphContext & context
			, crg::RunnableGraph & graph
			, RenderDevice const & device
			, Scene const & scene
			, VkExtent2D const & size
			, Texture const & output );
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
		/**
		 *\~english
		 *\brief		Uploads overlays GPU buffers to VRAM.
		 *\param[in]	cb	The command buffer on which transfer commands are recorded.
		 *\~french
		 *\brief		Met à jour tous les tampons GPU d'incrustations en VRAM.
		 *\param[in]	cb	Le command buffer sur lequel les commandes de transfert sont enregistrées.
		 */
		C3D_API void upload( ashes::CommandBuffer const & cb );

	private:
		void doInitialise();
		void doRecordInto( crg::RecordContext & context, 
			VkCommandBuffer commandBuffer
			, uint32_t index );

	private:
		RenderDevice const & m_device;
		Scene const & m_scene;
		crg::RenderPassHolder m_renderPass;
		OverlayRendererUPtr m_renderer;
	};
}

#endif
