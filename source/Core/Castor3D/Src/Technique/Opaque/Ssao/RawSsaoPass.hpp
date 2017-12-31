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
		 *\param[in]	engine			The engine.
		 *\param[in]	size			The render area dimensions.
		 *\param[in]	config			The SSAO configuration.
		 *\param[in]	matrixUbo		The matrices UBO.
		 *\param[in]	ssaoConfigUbo	The SSAO configuration UBO.
		 *\~french
		 *\brief		Constructeur.
		 *\param[in]	engine			Le moteur.
		 *\param[in]	size			Les dimensions de la zone de rendu.
		 *\param[in]	config			La configuration du SSAO.
		 *\param[in]	matrixUbo		L'UBO des matrices.
		 *\param[in]	ssaoConfigUbo	L'UBO de configuration du SSAO.
		 */
		RawSsaoPass( Engine & engine
			, castor::Size const & size
			, SsaoConfig const & config
			, MatrixUbo & matrixUbo
			, SsaoConfigUbo & ssaoConfigUbo );
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
		 *\param[in]	linearisedDepthBuffer	The linearised depth buffer.
		 *\param[in]	normals					The normals buffer.
		 *\~french
		 *\brief		Dessine la passe SSAO sur le tampon d'image actif.
		 *\param[in]	linearisedDepthBuffer	Le tampon de profondeur linéarisé.
		 *\param[in]	normals					Le tampon de normales.
		 */
		void compute( TextureUnit const & linearisedDepthBuffer
			, TextureUnit const & normals );
		/**
		 *\~english
		 *\return		The SSAO pass result.
		 *\~french
		 *\return		Le résultat de la passe SSAO.
		 */
		inline TextureUnit const & getResult()const
		{
			return m_result;
		}

	private:
		Engine & m_engine;
		MatrixUbo & m_matrixUbo;
		SsaoConfigUbo & m_ssaoConfigUbo;
		castor::Size m_size;
		TextureUnit m_result;
		ShaderProgramSPtr m_program;
		RenderPipelineUPtr m_pipeline;
		FrameBufferSPtr m_fbo;
		TextureAttachmentSPtr m_resultAttach;
		RenderPassTimerSPtr m_timer;

	};
}

#endif
