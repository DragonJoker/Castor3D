/*
See LICENSE file in root folder
*/
#ifndef ___C3D_RsmGIPass_HPP___
#define ___C3D_RsmGIPass_HPP___

#include "LightingModule.hpp"

#include "Castor3D/Buffer/UniformBuffer.hpp"
#include "Castor3D/Material/Texture/TextureUnit.hpp"
#include "Castor3D/Miscellaneous/MiscellaneousModule.hpp"
#include "Castor3D/Render/ShadowMap/ShadowMapModule.hpp"
#include "Castor3D/Render/Technique/Opaque/OpaqueModule.hpp"
#include "Castor3D/Render/ToTexture/RenderQuad.hpp"
#include "Castor3D/Scene/Light/LightModule.hpp"

#include <ShaderAST/Shader.hpp>

namespace castor3d
{
	class RsmGIPass
		: public RenderQuad
	{
	public:
		using Configuration = RsmGIConfig;

	public:
		/**
		 *\~english
		 *\brief		Constructor.
		 *\param[in]	engine			The engine.
		 *\param[in]	size			The render area dimensions.
		 *\param[in]	config			The SSGI configuration.
		 *\param[in]	linearisedDepth	The linearised depth buffer.
		 *\param[in]	scene			The scene buffer.
		 *\~french
		 *\brief		Constructeur.
		 *\param[in]	engine			Le moteur.
		 *\param[in]	size			Les dimensions de la zone de rendu.
		 *\param[in]	config			La configuration du SSGI.
		 *\param[in]	linearisedDepth	Le tampon de profondeur linéarisé.
		 *\param[in]	scene			Le tampon de scène.
		 */
		C3D_API RsmGIPass( Engine & engine
			, LightType lightType
			, VkExtent2D const & size
			, RsmConfig const & config
			, ShadowMapResult const & smResult
			, LightPassResult const & lpResult );
		/**
		 *\~english
		 *\brief		Renders the SSGI pass.
		 *\param[in]	toWait	The semaphore from the previous render pass.
		 *\~french
		 *\brief		Dessine la passe SSGI.
		 *\param[in]	toWait	Le sémaphore de la passe de rendu précédente.
		 */
		C3D_API ashes::Semaphore const & compute( ashes::Semaphore const & toWait )const;
		C3D_API CommandsSemaphore getCommands( RenderPassTimer const & timer
			, uint32_t index )const;
		/**
		 *\copydoc		castor3d::RenderTechniquePass::accept
		 */
		C3D_API void accept( SsgiConfig & config
			, PipelineVisitorBase & visitor );

	public:
		static VkFormat constexpr ResultFormat = VK_FORMAT_R32G32B32A32_SFLOAT;

	private:
		void doFillDescriptorSet( ashes::DescriptorSetLayout & descriptorSetLayout
			, ashes::DescriptorSet & descriptorSet )override;
		void doRegisterFrame( ashes::CommandBuffer & commandBuffer )const override;

	private:
		RsmConfig const & m_config;
		ShadowMapResult const & m_smResult;
		LightPassResult const & m_lpResult;
		ShaderModule m_vertexShader;
		ShaderModule m_pixelShader;
		UniformBufferUPtr< Configuration > m_rsmConfigUbo;
		ashes::BufferPtr< castor::Point2f > m_rsmSamplesSsbo;
		ashes::RenderPassPtr m_renderPass;
		ashes::FrameBufferPtr m_frameBuffer;
		RenderPassTimerSPtr m_timer;
		ashes::SemaphorePtr m_finished;
	};
}

#endif
