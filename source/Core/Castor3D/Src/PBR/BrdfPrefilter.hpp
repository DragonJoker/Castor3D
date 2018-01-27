/*
See LICENSE file in root folder
*/
#ifndef ___C3D_BrdfPrefilter_H___
#define ___C3D_BrdfPrefilter_H___

#include "RenderToTexture/RenderQuad.hpp"

namespace castor3d
{
	/*!
	\author 	Sylvain DOREMUS
	\date		02/03/2017
	\version	0.9.0
	\~english
	\brief		Computes the convoluted BRDF 2D texture.
	\~french
	\brief		Calcule la texture 2D contenant le BRDF convolu.
	*/
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
			, castor::Size const & size
			, renderer::TextureView const & dstTexture );
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
		renderer::ShaderProgram & doCreateProgram();

	private:
		RenderSystem & m_renderSystem;
		NonTexturedQuad m_vertexData;
		renderer::VertexBufferPtr< NonTexturedQuad > m_vertexBuffer;
		renderer::GeometryBuffersPtr m_geometryBuffers;
		renderer::VertexLayoutPtr m_vertexLayout;
		renderer::RenderPassPtr m_renderPass;
		renderer::FrameBufferPtr m_frameBuffer;
		renderer::PipelineLayoutPtr m_pipelineLayout;
		renderer::PipelinePtr m_pipeline;
		renderer::CommandBufferPtr m_commandBuffer;
	};
}

#endif
