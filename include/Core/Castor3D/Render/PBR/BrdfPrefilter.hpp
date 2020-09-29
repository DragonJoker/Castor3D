/*
See LICENSE file in root folder
*/
#ifndef ___C3D_BrdfPrefilter_H___
#define ___C3D_BrdfPrefilter_H___

#include "PbrModule.hpp"

#include "Castor3D/Render/ToTexture/RenderQuad.hpp"

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
		 *\param[in]	size		The render size.
		 *\param[in]	dstTexture	The cube texture destination.
		 *\~french
		 *\brief		Constructeur.
		 *\param[in]	engine		Le moteur.
		 *\param[in]	size		La taille du rendu.
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
		C3D_API void render();

	private:
		/**
		 *\~english
		 *\brief		Creates the render convolution shader program.
		 *\~french
		 *\brief		Crée le programme shader de convolution.
		 */
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
		ashes::CommandBufferPtr m_commandBuffer;
		ashes::FencePtr m_fence;
	};
}

#endif
