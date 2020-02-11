/*
See LICENSE file in root folder
*/
#ifndef ___C3D_DeferredStencilPass_H___
#define ___C3D_DeferredStencilPass_H___

#include "Castor3D/Render/Technique/Opaque/LightPass.hpp"

#include <ashespp/Command/CommandBuffer.hpp>
#include <ashespp/Descriptor/DescriptorSet.hpp>
#include <ashespp/Descriptor/DescriptorSetLayout.hpp>
#include <ashespp/Descriptor/DescriptorSetPool.hpp>
#include <ashespp/Pipeline/GraphicsPipeline.hpp>
#include <ashespp/Pipeline/PipelineLayout.hpp>
#include <ashespp/Sync/Semaphore.hpp>

namespace castor3d
{
	class StencilPass
	{
	public:
		/**
		 *\~english
		 *\brief		Constructor.
		 *\param[in]	engine			The engine.
		 *\param[in]	depthView		The depth buffer view.
		 *\param[in]	matrixUbo		The matrix UBO.
		 *\param[in]	modelMatrixUbo	The model matrix UBO.
		 *\~french
		 *\brief		Constructeur.
		 *\param[in]	engine			Le moteur.
		 *\param[in]	depthView		L'attache du tampon de profondeur.
		 *\param[in]	matrixUbo		L'UBO des matrices.
		 *\param[in]	modelMatrixUbo	L'UBO des matrices modèle.
		 */
		StencilPass( Engine const & engine
			, ashes::ImageView const & depthView
			, MatrixUbo & matrixUbo
			, ModelMatrixUbo & modelMatrixUbo );
		/**
		 *\~english
		 *\brief		Initialises the program and its pipeline.
		 *\param[in]	vertexLayout	The vertex buffer layout.
		 *\param[in]	vbo				The vertex buffer containing the object to render.
		 *\~french
		 *\brief		Initialise le programme et son pipeline.
		 *\param[in]	vertexLayout	Le layout du tampon de sommets.
		 *\param[in]	vbo				Le tampon de sommets contenant l'objet à dessiner.
		 */
		void initialise( ashes::PipelineVertexInputStateCreateInfo const & vertexLayout
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
		 *\param[in]	toWait	The semaphore to wait.
		 *\~french
		 *\brief		Dessine la passe de stencil.
		 *\param[in]	toWait	Le sémaphore à attendre.
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
		ashes::ImageView const & m_depthView;
		MatrixUbo & m_matrixUbo;
		ModelMatrixUbo & m_modelMatrixUbo;
		ashes::DescriptorSetLayoutPtr m_descriptorLayout;
		ashes::DescriptorSetPoolPtr m_descriptorPool;
		ashes::DescriptorSetPtr m_descriptorSet;
		ashes::RenderPassPtr m_renderPass;
		ashes::FrameBufferPtr m_frameBuffer;
		ashes::PipelineLayoutPtr m_pipelineLayout;
		ashes::PipelineShaderStageCreateInfoArray m_program;
		ashes::GraphicsPipelinePtr m_pipeline;
		ashes::VertexBufferBase const * m_vbo{ nullptr };
		ashes::CommandBufferPtr m_commandBuffer;
		ashes::SemaphorePtr m_finished;
	};
}

#endif
