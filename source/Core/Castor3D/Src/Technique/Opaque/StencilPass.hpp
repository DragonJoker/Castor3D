/*
See LICENSE file in root folder
*/
#ifndef ___C3D_DeferredStencilPass_H___
#define ___C3D_DeferredStencilPass_H___

#include "LightPass.hpp"

#include <Command/CommandBuffer.hpp>
#include <Descriptor/DescriptorSet.hpp>
#include <Descriptor/DescriptorSetLayout.hpp>
#include <Descriptor/DescriptorSetPool.hpp>
#include <Pipeline/Pipeline.hpp>
#include <Pipeline/PipelineLayout.hpp>
#include <Sync/Semaphore.hpp>

namespace castor3d
{
	/*!
	\author		Sylvain DOREMUS
	\version	0.10.0
	\date		08/06/2017
	\~english
	\brief		Stencil pre-pass for light passes needing a mesh.
	\~french
	\brief		Pré-passe de stencil pour les passes d'éclairage nécessitant un maillage.
	*/
	class StencilPass
	{
	public:
		/**
		 *\~english
		 *\brief		Constructor.
		 *\param[in]	frameBuffer		The target framebuffer.
		 *\param[in]	depthAttach		The depth buffer attach.
		 *\param[in]	matrixUbo		The matrix UBO.
		 *\param[in]	modelMatrixUbo	The model matrix UBO.
		 *\~french
		 *\brief		Constructeur.
		 *\param[in]	frameBuffer		Le tampon d'image cible.
		 *\param[in]	depthAttach		L'attache du tampon de profondeur.
		 *\param[in]	matrixUbo		L'UBO des matrices.
		 *\param[in]	modelMatrixUbo	L'UBO des matrices modèle.
		 */
		StencilPass( Engine const & engine
			, renderer::TextureView const & depthView
			, MatrixUbo & matrixUbo
			, ModelMatrixUbo & modelMatrixUbo );
		/**
		 *\~english
		 *\brief		Initialises the program and its pipeline.
		 *\param[in]	vbo	The vertex buffer containing the object to render.
		 *\~french
		 *\brief		Initialise le programme et son pipeline.
		 *\param[in]	vbo	Le tampon de sommets contenant l'objet à dessiner.
		 */
		void initialise( renderer::VertexLayout const & vertexLayout
			, renderer::VertexBufferBase & vbo );
		/**
		*\~english
		*\brief		Cleans up the program and its pipeline.
		*\~french
		*\brief		Nettoie le programme et son pipeline.
		*/
		void cleanup();
		/**
		 *\~english
		 *\brief		Renders the stencil pass.
		 *\~french
		 *\brief		Dessine la passe de stencil.
		 */
		void render( renderer::Semaphore const & toWait );
		/**
		*\~english
		*name
		*	Getters.
		*\~french
		*name
		*	Accesseurs.
		*/
		/**@{*/
		inline renderer::Semaphore const & getSemaphore()const
		{
			REQUIRE( m_finished );
			return *m_finished;
		}
		/**@}*/

	private:
		Engine const & m_engine;
		renderer::TextureView const & m_depthView;
		MatrixUbo & m_matrixUbo;
		ModelMatrixUbo & m_modelMatrixUbo;
		renderer::DescriptorSetLayoutPtr m_descriptorLayout;
		renderer::DescriptorSetPoolPtr m_descriptorPool;
		renderer::DescriptorSetPtr m_descriptorSet;
		renderer::RenderPassPtr m_renderPass;
		renderer::FrameBufferPtr m_frameBuffer;
		renderer::PipelineLayoutPtr m_pipelineLayout;
		renderer::ShaderStageStateArray m_program;
		renderer::PipelinePtr m_pipeline;
		renderer::VertexBufferBase const * m_vbo{ nullptr };
		renderer::CommandBufferPtr m_commandBuffer;
		renderer::SemaphorePtr m_finished;
	};
}

#endif
