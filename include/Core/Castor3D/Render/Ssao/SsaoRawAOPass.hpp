/*
See LICENSE file in root folder
*/
#ifndef ___C3D_SsaoRawAOPass_H___
#define ___C3D_SsaoRawAOPass_H___

#include "SsaoModule.hpp"

#include "Castor3D/Miscellaneous/MiscellaneousModule.hpp"
#include "Castor3D/Render/RenderInfo.hpp"
#include "Castor3D/Render/Ssao/SsaoConfig.hpp"
#include "Castor3D/Render/Technique/Opaque/Lighting/LightPass.hpp"
#include "Castor3D/Render/ToTexture/RenderQuad.hpp"
#include "Castor3D/Material/Texture/TextureUnit.hpp"

#include <ShaderAST/Shader.hpp>

namespace castor3d
{
	class SsaoRawAOPass
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
		SsaoRawAOPass( Engine & engine
			, RenderDevice const & device
			, VkExtent2D const & size
			, SsaoConfig const & config
			, SsaoConfigUbo & ssaoConfigUbo
			, GpInfoUbo const & gpInfoUbo
			, TextureUnit const & linearisedDepthBuffer
			, ashes::ImageView const & normals );
		/**
		 *\~english
		 *\brief		Destructor.
		 *\~french
		 *\brief		Destructeur.
		 */
		~SsaoRawAOPass();
		/**
		 *\~english
		 *\brief		Renders the SSAO pass.
		 *\param[in]	toWait	The semaphore from the previous render pass.
		 *\~french
		 *\brief		Dessine la passe SSAO.
		 *\param[in]	toWait	Le sémaphore de la passe de rendu précédente.
		 */
		ashes::Semaphore const & compute( ashes::Semaphore const & toWait )const;
		/**
		 *\copydoc		castor3d::RenderTechniquePass::accept
		 */
		C3D_API void accept( SsaoConfig & config
			, PipelineVisitorBase & visitor );
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
		struct RenderQuad
			: castor3d::RenderQuad
		{
			RenderQuad( Engine & engine
				, RenderDevice const & device
				, ashes::RenderPass const & renderPass
				, VkExtent2D const & size
				, SsaoConfigUbo & ssaoConfigUbo
				, GpInfoUbo const & gpInfoUbo
				, TextureUnit const & depth
				, ashes::ImageView const * normals );

			castor3d::ShaderModule vertexShader;
			castor3d::ShaderModule pixelShader;

		private:
			void doFillDescriptorSet( ashes::DescriptorSetLayout & descriptorSetLayout
				, ashes::DescriptorSet & descriptorSet )override;

			SsaoConfigUbo & m_ssaoConfigUbo;
			GpInfoUbo const & m_gpInfoUbo;
			ashes::ImageView const * m_depthView;
			SamplerSPtr m_depthSampler;
		};

	private:
		Engine & m_engine;
		RenderDevice const & m_device;
		SsaoConfig const & m_ssaoConfig;
		SsaoConfigUbo & m_ssaoConfigUbo;
		GpInfoUbo const & m_gpInfoUbo;
		TextureUnit const & m_linearisedDepthBuffer;
		ashes::ImageView const & m_normals;
		VkExtent2D m_size;
		TextureUnit m_result;
		ashes::RenderPassPtr m_renderPass;
		ashes::FrameBufferPtr m_frameBuffer;
		std::array< RenderQuad, 2u > m_quads;
		std::array < ashes::CommandBufferPtr, 2u > m_commandBuffers;
		ashes::SemaphorePtr m_finished;
		RenderPassTimerSPtr m_timer;
	};
}

#endif
