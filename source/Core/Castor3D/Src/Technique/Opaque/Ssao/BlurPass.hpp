/*
See LICENSE file in root folder
*/
#ifndef ___C3D_SsaoBlurPass_H___
#define ___C3D_SsaoBlurPass_H___

#include "Render/Viewport.hpp"
#include "RenderToTexture/RenderQuad.hpp"
#include "Texture/TextureUnit.hpp"

#include <Buffer/PushConstantsBuffer.hpp>
#include <Miscellaneous/PushConstantRange.hpp>

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
			, castor::Size const & size
			, SsaoConfig const & config
			, SsaoConfigUbo & ssaoConfigUbo
			, castor::Point2i const & axis
			, TextureUnit const & input
			, TextureUnit const & normals );
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
		void blur();
		/**
		 *\~english
		 *\return		The pass result.
		 *\~french
		 *\return		Le résultat de la passe.
		 */
		inline TextureUnit const & getResult()const
		{
			return m_result;
		}

	private:
		void doFillDescriptorSet( renderer::DescriptorSetLayout & descriptorSetLayout
			, renderer::DescriptorSet & descriptorSet )override;
		void doRegisterFrame( renderer::CommandBuffer & commandBuffer )override;

	private:
		Engine & m_engine;
		SsaoConfigUbo & m_ssaoConfigUbo;
		TextureUnit const & m_input;
		TextureUnit const & m_normals;
		renderer::ShaderProgram & m_program;
		castor::Size m_size;
		TextureUnit m_result;
		renderer::PushConstantsBuffer< renderer::IVec2 > m_axisUniform;
		renderer::PushConstantRange m_pushConstantRange;
		renderer::RenderPassPtr m_renderPass;
		renderer::FrameBufferPtr m_fbo;
		RenderPassTimerSPtr m_timer;
		renderer::CommandBufferPtr m_commandBuffer;

	};
}

#endif
