/*
See LICENSE file in root folder
*/
#ifndef ___C3D_SsaoBlurPass_H___
#define ___C3D_SsaoBlurPass_H___

#include "Render/Viewport.hpp"
#include "RenderToTexture/RenderQuad.hpp"
#include "Technique/RenderTechniqueVisitor.hpp"
#include "Texture/TextureUnit.hpp"

#include <Buffer/PushConstantsBuffer.hpp>
#include <Miscellaneous/PushConstantRange.hpp>

#include <GlslShader.hpp>

namespace castor3d
{
	class MatrixUbo;
	class SsaoConfigUbo;
	/*!
	\author		Sylvain DOREMUS
	\version	0.10.0
	\date		18/12/2017
	\~english
	\brief		Ambient occlusion blur pass.
	\~french
	\brief		Passe de flou de l'occlusion ambiante.
	*/
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
			, renderer::Extent2D const & size
			, SsaoConfig const & config
			, SsaoConfigUbo & ssaoConfigUbo
			, castor::Point2i const & axis
			, TextureUnit const & input
			, renderer::TextureView const & normals );
		/**
		 *\~english
		 *\brief		Destructor.
		 *\~french
		 *\brief		Destructeur.
		 */
		~SsaoBlurPass();
		/**
		 *\~english
		 *\brief		Applies the blur.
		 *\~french
		 *\brief		Applique le flou.
		 */
		renderer::Semaphore const & blur( renderer::Semaphore const & toWait )const;
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
		void doFillDescriptorSet( renderer::DescriptorSetLayout & descriptorSetLayout
			, renderer::DescriptorSet & descriptorSet )override;
		void doRegisterFrame( renderer::CommandBuffer & commandBuffer )const override;

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
		renderer::TextureView const & m_normals;
		glsl::Shader m_vertexShader;
		glsl::Shader m_pixelShader;
		renderer::ShaderStageStateArray m_program;
		renderer::Extent2D m_size;
		TextureUnit m_result;
		renderer::RenderPassPtr m_renderPass;
		renderer::FrameBufferPtr m_fbo;
		RenderPassTimerSPtr m_timer;
		renderer::CommandBufferPtr m_commandBuffer;
		renderer::SemaphorePtr m_finished;
		renderer::UniformBufferPtr< Configuration > m_configurationUbo;

	};
}

#endif
