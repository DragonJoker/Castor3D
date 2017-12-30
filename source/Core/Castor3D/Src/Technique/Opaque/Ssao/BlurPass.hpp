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
		 *\param[in]	engine	The engine.
		 *\param[in]	size	The render area dimensions.
		 *\~french
		 *\brief		Constructeur.
		 *\param[in]	engine	Le moteur.
		 *\param[in]	size	Les dimensions de la zone de rendu.
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
		 *\param[in]	axis	Receives rendering info.
		 *\~french
		 *\brief		Applique le flou sur la texture donnée.
		 *\param[in]	input	La texture à flouter.
		 *\param[in]	axis	Reçoit les informations de rendu.
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
