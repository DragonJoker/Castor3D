/*
See LICENSE file in root folder
*/
#ifndef ___C3D_DeferredSsaoPass_H___
#define ___C3D_DeferredSsaoPass_H___

#include "LightPass.hpp"

#include "Miscellaneous/SsaoConfig.hpp"
#include "Render/RenderInfo.hpp"
#include "Shader/Ubos/GpInfoUbo.hpp"
#include "Shader/Ubos/MatrixUbo.hpp"
#include "Texture/TextureUnit.hpp"

namespace castor3d
{
	/*!
	\author		Sylvain DOREMUS
	\version	0.10.0
	\date		08/06/2017
	\~english
	\brief		Screen Space Ambient Occlusion pass.
	\~french
	\brief		Passe de Occlusion Ambiante en Espace Ecran.
	*/
	class SsaoPass
	{
	public:
		/**
		 *\~english
		 *\brief		Constructor.
		 *\param[in]	engine		The engine.
		 *\param[in]	size		The render area dimensions.
		 *\param[in]	config		The SSAO configuration.
		 *\param[in]	gpInfoUbo	The geometry pass UBO.
		 *\~french
		 *\brief		Constructeur.
		 *\param[in]	engine		Le moteur.
		 *\param[in]	size		Les dimensions de la zone de rendu.
		 *\param[in]	config		La configuration du SSAO.
		 *\param[in]	gpInfoUbo	L'UBO de la passe géométrique.
		 */
		SsaoPass( Engine & engine
			, castor::Size const & size
			, SsaoConfig const & config
			, GpInfoUbo & gpInfoUbo );
		/**
		 *\~english
		 *\brief		Destructor.
		 *\~french
		 *\brief		Destructeur.
		 */
		~SsaoPass();
		/**
		 *\~english
		 *\brief		Renders the SSAO pass on currently bound framebuffer.
		 *\param[in]	gp		The geometry pass result.
		 *\param[in]	info	Receives rendering info.
		 *\~french
		 *\brief		Dessine la passe SSAO sur le tampon d'image donné.
		 *\param[in]	gp		Le résultat de la geometry pass.
		 *\param[in]	info	Reçoit les informations de rendu.
		 */
		void render( GeometryPassResult const & gp
			, RenderInfo & info );
		/**
		 *\~english
		 *\return		The SSAO pass result.
		 *\~french
		 *\return		Le résultat de la passe SSAO.
		 */
		inline TextureUnit const & getResult()const
		{
			return m_blurResult;
		}

	private:
		void doInitialiseQuadRendering();
		void doInitialiseSsaoPass();
		void doInitialiseBlurPass();
		void doCleanupQuadRendering();
		void doCleanupSsaoPass();
		void doCleanupBlurPass();
		void doRenderSsao( GeometryPassResult const & gp );
		void doRenderBlur();

	private:
		Engine & m_engine;
		SsaoConfig m_config;
		// Quad rendering
		castor::Size m_size;
		MatrixUbo m_matrixUbo;
		Viewport m_viewport;
		// Raw SSAO pass
		VertexBufferSPtr m_ssaoVertexBuffer;
		GeometryBuffersSPtr m_ssaoGeometryBuffers;
		RenderPipelineUPtr m_ssaoPipeline;
		ShaderProgramSPtr m_ssaoProgram;
		castor::Point3fArray m_ssaoKernel;
		TextureUnit m_ssaoNoise;
		TextureUnit m_ssaoResult;
		FrameBufferSPtr m_ssaoFbo;
		TextureAttachmentSPtr m_ssaoResultAttach;
		UniformBuffer m_ssaoConfig;
		Uniform3fSPtr m_kernelUniform;
		GpInfoUbo & m_gpInfoUbo;
		RenderPassTimerSPtr m_ssaoTimer;
		// SSAO blur pass
		VertexBufferSPtr m_blurVertexBuffer;
		GeometryBuffersSPtr m_blurGeometryBuffers;
		RenderPipelineUPtr m_blurPipeline;
		ShaderProgramSPtr m_blurProgram;
		TextureUnit m_blurResult;
		FrameBufferSPtr m_blurFbo;
		TextureAttachmentSPtr m_blurResultAttach;
		UniformBuffer m_blurConfig;
		RenderPassTimerSPtr m_blurTimer;

	};
}

#endif
