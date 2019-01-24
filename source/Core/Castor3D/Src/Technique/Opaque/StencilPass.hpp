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
			, ashes::TextureView const & depthView
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
		void initialise( ashes::VertexLayout const & vertexLayout
			, ashes::VertexBufferBase & vbo );
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
		ashes::Semaphore const & render( ashes::Semaphore const & toWait );
		/**
		*\~english
		*name
		*	Getters.
		*\~french
		*name
		*	Accesseurs.
		*/
		/**@{*/
		inline ashes::Semaphore const & getSemaphore()const
		{
			CU_Require( m_finished );
			return *m_finished;
		}
		/**@}*/

	private:
		Engine const & m_engine;
		ashes::TextureView const & m_depthView;
		MatrixUbo & m_matrixUbo;
		ModelMatrixUbo & m_modelMatrixUbo;
		ashes::DescriptorSetLayoutPtr m_descriptorLayout;
		ashes::DescriptorSetPoolPtr m_descriptorPool;
		ashes::DescriptorSetPtr m_descriptorSet;
		ashes::RenderPassPtr m_renderPass;
		ashes::FrameBufferPtr m_frameBuffer;
		ashes::PipelineLayoutPtr m_pipelineLayout;
		ashes::ShaderStageStateArray m_program;
		ashes::PipelinePtr m_pipeline;
		ashes::VertexBufferBase const * m_vbo{ nullptr };
		ashes::CommandBufferPtr m_commandBuffer;
		ashes::SemaphorePtr m_finished;
	};
}

#endif
