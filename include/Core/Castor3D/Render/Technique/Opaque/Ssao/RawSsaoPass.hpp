/*
See LICENSE file in root folder
*/
#ifndef ___C3D_RawSsaoPass_H___
#define ___C3D_RawSsaoPass_H___

#include "SsaoModule.hpp"

#include "Castor3D/Render/RenderInfo.hpp"
#include "Castor3D/Render/Technique/RenderTechniqueVisitor.hpp"
#include "Castor3D/Render/Technique/Opaque/Ssao/SsaoConfig.hpp"
#include "Castor3D/Render/Technique/Opaque/LightPass.hpp"
#include "Castor3D/Material/Texture/TextureUnit.hpp"

#include <ShaderAST/Shader.hpp>

namespace castor3d
{
	class RawSsaoPass
	{
	public:
		/**
		 *\~english
		 *\brief		Constructor.
		 *\param[in]	engine					The engine.
		 *\param[in]	size					The render area dimensions.
		 *\param[in]	config					The SSAO configuration.
		 *\param[in]	ssaoConfigUbo			The SSAO configuration UBO.
		 *\param[in]	linearisedDepthBuffer	The linearised depth buffer.
		 *\param[in]	normals					The normals buffer.
		 *\~french
		 *\brief		Constructeur.
		 *\param[in]	engine					Le moteur.
		 *\param[in]	size					Les dimensions de la zone de rendu.
		 *\param[in]	config					La configuration du SSAO.
		 *\param[in]	ssaoConfigUbo			L'UBO de configuration du SSAO.
		 *\param[in]	linearisedDepthBuffer	Le tampon de profondeur linéarisé.
		 *\param[in]	normals					Le tampon de normales.
		 */
		RawSsaoPass( Engine & engine
			, VkExtent2D const & size
			, SsaoConfig const & config
			, SsaoConfigUbo & ssaoConfigUbo
			, TextureUnit const & linearisedDepthBuffer
			, ashes::ImageView const & normals );
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
		 *\param[in]	toWait	The semaphore from the previous render pass.
		 *\~french
		 *\brief		Dessine la passe SSAO sur le tampon d'image actif.
		 *\param[in]	toWait	Le sémaphore de la passe de rendu précédente.
		 */
		ashes::Semaphore const & compute( ashes::Semaphore const & toWait )const;
		/**
		 *\copydoc		castor3d::RenderTechniquePass::accept
		 */
		C3D_API void accept( SsaoConfig & config
			, RenderTechniqueVisitor & visitor );
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
		/**@}*/

	public:
		static VkFormat constexpr ResultFormat = VK_FORMAT_R32G32B32A32_SFLOAT;

	private:
		Engine & m_engine;
		SsaoConfigUbo & m_ssaoConfigUbo;
		TextureUnit const & m_linearisedDepthBuffer;
		ashes::ImageView const & m_normals;
		VkExtent2D m_size;
		TextureUnit m_result;
		castor3d::ShaderModule m_vertexShader;
		castor3d::ShaderModule m_pixelShader;
		ashes::PipelineShaderStageCreateInfoArray m_program;
		ashes::SamplerPtr m_sampler;
		ashes::DescriptorSetLayoutPtr m_descriptorLayout;
		ashes::DescriptorSetPoolPtr m_descriptorPool;
		ashes::DescriptorSetPtr m_descriptor;
		ashes::PipelineLayoutPtr m_pipelineLayout;
		ashes::RenderPassPtr m_renderPass;
		ashes::FrameBufferPtr m_frameBuffer;
		ashes::VertexBufferPtr< NonTexturedQuad > m_vertexBuffer;
		ashes::PipelineVertexInputStateCreateInfoPtr m_vertexLayout;
		ashes::GraphicsPipelinePtr m_pipeline;
		ashes::CommandBufferPtr m_commandBuffer;
		ashes::SemaphorePtr m_finished;
		RenderPassTimerSPtr m_timer;
	};
}

#endif
