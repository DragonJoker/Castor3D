/*
See LICENSE file in root folder
*/
#ifndef ___C3D_SsaoBlurPass_H___
#define ___C3D_SsaoBlurPass_H___

#include "Render/Viewport.hpp"
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
	\brief		Ambient occlusion blur pass.
	\~french
	\brief		Passe de flou de l'occlusion ambiante.
	*/
	class SsaoBlurPass
	{
	public:
		/**
		 *\~english
		 *\brief		Constructor.
		 *\param[in]	engine			The engine.
		 *\param[in]	size			The render area dimensions.
		 *\param[in]	config			The SSAO configuration.
		 *\param[in]	matrixUbo		The matrices UBO.
		 *\param[in]	ssaoConfigUbo	The SSAO configuration UBO.
		 *\param[in]	axis			The axis to which the blur applies ({0, 1} for vertical, {1, 0} for horizontal).
		 *\~french
		 *\brief		Constructeur.
		 *\param[in]	engine			Le moteur.
		 *\param[in]	size			Les dimensions de la zone de rendu.
		 *\param[in]	config			La configuration du SSAO.
		 *\param[in]	matrixUbo		L'UBO des matrices.
		 *\param[in]	ssaoConfigUbo	L'UBO de configuration du SSAO.
		 *\param[in]	axis			L'axe dans lequel le flou s'applique ({0, 1} pour vertical, {1, 0} pour horizontal).
		 */
		SsaoBlurPass( Engine & engine
			, castor::Size const & size
			, SsaoConfig const & config
			, MatrixUbo & matrixUbo
			, SsaoConfigUbo & ssaoConfigUbo
			, castor::Point2i const & axis );
		/**
		 *\~english
		 *\brief		Destructor.
		 *\~french
		 *\brief		Destructeur.
		 */
		~SsaoBlurPass();
		/**
		 *\~english
		 *\brief		Blurs given texture.
		 *\param[in]	input	The texture to blur.
		 *\param[in]	normals	The normals buffer.
		 *\~french
		 *\brief		Applique le flou sur la texture donnée.
		 *\param[in]	input	La texture à flouter.
		 *\param[in]	normals	Le tampon de normales.
		 */
		void blur( TextureUnit const & input
			, TextureUnit const & normals );
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
		Engine & m_engine;
		castor::Size m_size;
		castor::Point2i m_axis;
		MatrixUbo & m_matrixUbo;
		SsaoConfigUbo & m_ssaoConfigUbo;
		TextureUnit m_result;
		ShaderProgramSPtr m_program;
		PushUniform2i & m_axisUniform;
		RenderPipelineUPtr m_pipeline;
		FrameBufferSPtr m_fbo;
		TextureAttachmentSPtr m_resultAttach;
		RenderPassTimerSPtr m_timer;

	};
}

#endif
