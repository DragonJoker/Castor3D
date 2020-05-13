/*
See LICENSE file in root folder
*/
#ifndef ___C3D_SsgiRawGIPass_HPP___
#define ___C3D_SsgiRawGIPass_HPP___

#include "SsgiModule.hpp"

#include "Castor3D/Material/Texture/TextureUnit.hpp"
#include "Castor3D/Miscellaneous/MiscellaneousModule.hpp"
#include "Castor3D/Render/ToTexture/RenderQuad.hpp"

#include <ShaderAST/Shader.hpp>

#define Ssgi_DebugSsgiPass 1
#define Ssgi_CulledBuffer 0

namespace castor3d
{
	class SsgiRawGIPass
		: public RenderQuad
	{
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
		C3D_API SsgiRawGIPass( Engine & engine
			, VkExtent2D const & size
			, SsgiConfig const & config
			, TextureLayout const & hdrResult
			, TextureLayout const & linearisedDepth );
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
		void doFillDescriptorSet( ashes::DescriptorSetLayout & descriptorSetLayout
			, ashes::DescriptorSet & descriptorSet )override;
		void doRegisterFrame( ashes::CommandBuffer & commandBuffer )const override;

	private:
		TextureLayout const & m_hdrResult;
		TextureLayout const & m_linearisedDepth;
		ShaderModule m_vertexShader;
		ShaderModule m_pixelShader;
		ashes::RenderPassPtr m_renderPass;
		TextureUnit m_result;
		ashes::FrameBufferPtr m_frameBuffer;
		RenderPassTimerSPtr m_timer;
		ashes::SemaphorePtr m_finished;
	};
}

#endif
