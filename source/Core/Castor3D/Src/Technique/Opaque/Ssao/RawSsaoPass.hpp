/*
See LICENSE file in root folder
*/
#ifndef ___C3D_RawSsaoPass_H___
#define ___C3D_RawSsaoPass_H___

#include "SsaoConfigUbo.hpp"
#include "Technique/Opaque/Ssao/SsaoConfig.hpp"
#include "Render/RenderInfo.hpp"
#include "Technique/Opaque/LightPass.hpp"
#include "Texture/TextureUnit.hpp"

namespace castor3d
{
	class MatrixUbo;
	class SsaoConfigUbo;
	/*!
	\author		Sylvain DOREMUS
	\version	0.10.0
	\date		18/12/2017
	\~english
	\brief		Raw ambient occlusion pass.
	\~french
	\brief		Passe d'occlusion ambiante.
	*/
	class RawSsaoPass
	{
	public:
		/**
		 *\~english
		 *\brief		Constructor.
		 *\param[in]	engine					The engine.
		 *\param[in]	size					The render area dimensions.
		 *\param[in]	config					The SSAO configuration.
		 *\param[in]	matrixUbo				The matrices UBO.
		 *\param[in]	ssaoConfigUbo			The SSAO configuration UBO.
		 *\param[in]	linearisedDepthBuffer	The linearised depth buffer.
		 *\param[in]	normals					The normals buffer.
		 *\~french
		 *\brief		Constructeur.
		 *\param[in]	engine					Le moteur.
		 *\param[in]	size					Les dimensions de la zone de rendu.
		 *\param[in]	config					La configuration du SSAO.
		 *\param[in]	matrixUbo				L'UBO des matrices.
		 *\param[in]	ssaoConfigUbo			L'UBO de configuration du SSAO.
		 *\param[in]	linearisedDepthBuffer	Le tampon de profondeur linéarisé.
		 *\param[in]	normals					Le tampon de normales.
		 */
		RawSsaoPass( Engine & engine
			, renderer::Extent2D const & size
			, SsaoConfig const & config
			, SsaoConfigUbo & ssaoConfigUbo
			, TextureUnit const & linearisedDepthBuffer
			, renderer::TextureView const & normals );
		/**
		 *\~english
		 *\brief		Destructor.
		 *\~french
		 *\brief		Destructeur.
		 */
		~RawSsaoPass();
		/**
		 *\~english
		 *\brief		Renders the SSAO pass on currently bound framebuffer.
		 *\~french
		 *\brief		Dessine la passe SSAO sur le tampon d'image actif.
		 */
		void compute( renderer::Semaphore const & toWait )const;
		/**
		*\~english
		*name
		*	Getters.
		*\~french
		*name
		*	Accesseurs.
		*/
		/**@{*/
		inline TextureUnit const & getResult()const
		{
			return m_result;
		}

		inline renderer::Semaphore const & getSemaphore()const
		{
			REQUIRE( m_finished );
			return *m_finished;
		}
		/**@}*/

	private:
		Engine & m_engine;
		SsaoConfigUbo & m_ssaoConfigUbo;
		TextureUnit const & m_linearisedDepthBuffer;
		renderer::TextureView const & m_normals;
		renderer::Extent2D m_size;
		TextureUnit m_result;
		renderer::ShaderStageStateArray m_program;
		renderer::SamplerPtr m_sampler;
		renderer::DescriptorSetLayoutPtr m_descriptorLayout;
		renderer::DescriptorSetPoolPtr m_descriptorPool;
		renderer::DescriptorSetPtr m_descriptor;
		renderer::PipelineLayoutPtr m_pipelineLayout;
		renderer::RenderPassPtr m_renderPass;
		renderer::FrameBufferPtr m_frameBuffer;
		renderer::VertexBufferPtr< NonTexturedQuad > m_vertexBuffer;
		renderer::VertexLayoutPtr m_vertexLayout;
		renderer::PipelinePtr m_pipeline;
		renderer::CommandBufferPtr m_commandBuffer;
		renderer::SemaphorePtr m_finished;
		RenderPassTimerSPtr m_timer;

	};
}

#endif
