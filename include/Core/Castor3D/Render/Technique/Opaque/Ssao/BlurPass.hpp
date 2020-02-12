/*
See LICENSE file in root folder
*/
#ifndef ___C3D_SsaoBlurPass_H___
#define ___C3D_SsaoBlurPass_H___

#include "SsaoModule.hpp"
#include "Castor3D/Render/Technique/TechniqueModule.hpp"

#include "Castor3D/Buffer/UniformBuffer.hpp"
#include "Castor3D/Render/ToTexture/RenderQuad.hpp"
#include "Castor3D/Material/Texture/TextureUnit.hpp"

#include <ashespp/Command/CommandBuffer.hpp>
#include <ashespp/Image/ImageView.hpp>
#include <ashespp/RenderPass/FrameBuffer.hpp>
#include <ashespp/RenderPass/RenderPass.hpp>
#include <ashespp/Sync/Semaphore.hpp>

namespace castor3d
{
	class SsaoBlurPass
		: public RenderQuad
	{
	public:
		/**
		 *\~english
		 *\brief		Constructor.
		 *\param[in]	engine			The engine.
		 *\param[in]	size			The render area dimensions.
		 *\param[in]	config			The SSAO configuration.
		 *\param[in]	ssaoConfigUbo	The SSAO configuration UBO.
		 *\param[in]	axis			The axis to which the blur applies ({0, 1} for vertical, {1, 0} for horizontal).
		 *\param[in]	input			The texture to blur.
		 *\param[in]	normals			The normals buffer.
		 *\~french
		 *\brief		Constructeur.
		 *\param[in]	engine			Le moteur.
		 *\param[in]	size			Les dimensions de la zone de rendu.
		 *\param[in]	config			La configuration du SSAO.
		 *\param[in]	ssaoConfigUbo	L'UBO de configuration du SSAO.
		 *\param[in]	axis			L'axe dans lequel le flou s'applique ({0, 1} pour vertical, {1, 0} pour horizontal).
		 *\param[in]	input			La texture à flouter.
		 *\param[in]	normals			Le tampon de normales.
		 */
		SsaoBlurPass( Engine & engine
			, VkExtent2D const & size
			, SsaoConfig const & config
			, SsaoConfigUbo & ssaoConfigUbo
			, castor::Point2i const & axis
			, TextureUnit const & input
			, ashes::ImageView const & normals );
		/**
		 *\~english
		 *\brief		Destructor.
		 *\~french
		 *\brief		Destructeur.
		 */
		~SsaoBlurPass();
		/**
		 *\~english
		 *\brief		Updates the blur configuration UBO.
		 *\~french
		 *\brief		Met à jour l'UBO de configuration du flou.
		 */
		void update()const;
		/**
		 *\~english
		 *\brief		Applies the blur.
		 *\~french
		 *\brief		Applique le flou.
		 */
		ashes::Semaphore const & blur( ashes::Semaphore const & toWait )const;
		/**
		 *\copydoc		castor3d::RenderTechniquePass::accept
		 */
		C3D_API void accept( bool horizontal
			, SsaoConfig & config
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

	private:
		void doFillDescriptorSet( ashes::DescriptorSetLayout & descriptorSetLayout
			, ashes::DescriptorSet & descriptorSet )override;
		void doRegisterFrame( ashes::CommandBuffer & commandBuffer )const override;

	public:
		static VkFormat constexpr ResultFormat = VK_FORMAT_R8G8B8A8_UNORM;

	private:
		struct Configuration
		{
			castor::Point2i axis;
			castor::Point2i dummy;
			castor::Point4f gaussian[2];
		};

		Engine & m_engine;
		SsaoConfigUbo & m_ssaoConfigUbo;
		TextureUnit const & m_input;
		ashes::ImageView const & m_normals;
		castor3d::ShaderModule m_vertexShader;
		castor3d::ShaderModule m_pixelShader;
		SsaoConfig const & m_config;
		ashes::PipelineShaderStageCreateInfoArray m_program;
		VkExtent2D m_size;
		TextureUnit m_result;
		ashes::RenderPassPtr m_renderPass;
		ashes::FrameBufferPtr m_fbo;
		RenderPassTimerSPtr m_timer;
		ashes::CommandBufferPtr m_commandBuffer;
		ashes::SemaphorePtr m_finished;
		UniformBufferUPtr< Configuration > m_configurationUbo;

	};
}

#endif
