/*
See LICENSE file in root folder
*/
#ifndef ___C3D_BrdfPrefilter_H___
#define ___C3D_BrdfPrefilter_H___

#include "PbrModule.hpp"

#include "Castor3D/Render/Passes/CommandsSemaphore.hpp"
#include "Castor3D/Render/Passes/RenderQuad.hpp"

#include <ashespp/RenderPass/FrameBuffer.hpp>

namespace castor3d
{
	class BrdfPrefilter
	{
	public:
		/**
		 *\~english
		 *\brief		Constructor.
		 *\param[in]	engine		The engine.
		 *\param		device		The current device.
		 *\param[in]	size		The render dimensions.
		 *\param[in]	dstTexture	The cube texture destination.
		 *\~french
		 *\brief		Constructeur.
		 *\param[in]	engine		Le moteur.
		 *\param		device		Le device actuel.
		 *\param[in]	size		Les dimensions du rendu.
		 *\param[in]	dstTexture	La texture cube destination.
		 */
		C3D_API explicit BrdfPrefilter( Engine & engine
			, RenderDevice const & device
			, castor::Size const & size
			, ashes::ImageView const & dstTexture );
		/**
		 *\~english
		 *\brief		Computes the convoluted BRDF.
		 *\~french
		 *\brief		Calcule le BRDF circonvolu.
		 */
		C3D_API void render( QueueData const & queueData );

	private:
		ashes::PipelineShaderStageCreateInfoArray doCreateProgram();

	private:
		RenderSystem & m_renderSystem;
		RenderDevice const & m_device;
		ashes::VertexBufferPtr< TexturedQuad > m_vertexBuffer;
		ashes::PipelineVertexInputStateCreateInfoPtr m_vertexLayout;
		ashes::RenderPassPtr m_renderPass;
		ashes::FrameBufferPtr m_frameBuffer;
		ashes::PipelineLayoutPtr m_pipelineLayout;
		ashes::GraphicsPipelinePtr m_pipeline;
		CommandsSemaphore m_commands;
	};
}

#endif
