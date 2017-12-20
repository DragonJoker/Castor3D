/*
See LICENSE file in root folder
*/
#ifndef ___C3D_LineariseDepthPass_H___
#define ___C3D_LineariseDepthPass_H___

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
	\brief		Depth linearisation pass, with mipmaps generation.
	\~french
	\brief		Passe linéarisation de profondeur, avec génération des mipmaps.
	*/
	class LineariseDepthPass
	{
	public:
		/**
		 *\~english
		 *\brief		Constructor.
		 *\param[in]	engine		The engine.
		 *\param[in]	size		The render area dimensions.
		 *\~french
		 *\brief		Constructeur.
		 *\param[in]	engine		Le moteur.
		 *\param[in]	size		Les dimensions de la zone de rendu.
		 */
		LineariseDepthPass( Engine & engine
			, castor::Size const & size
			, MatrixUbo & matrixUbo
			, SsaoConfigUbo & ssaoConfigUbo );
		/**
		 *\~english
		 *\brief		Destructor.
		 *\~french
		 *\brief		Destructeur.
		 */
		~LineariseDepthPass();
		/**
		 *\~english
		 *\brief		Linearises given depth buffer.
		 *\param[in]	depthBuffer	The non linearised depth buffer.
		 *\param[in]	viewport	The viewport from which clip infos are retrieved.
		 *\~french
		 *\brief		Linéarise le tampon de profondeur donné.
		 *\param[in]	depthBuffer	Le tampon de profondeur non linéarisé.
		 *\param[in]	viewport	Le viewport depuis lequel on récupère les information de clip.
		 */
		void linearise( TextureUnit const & depthBuffer
			, Viewport const & viewport );
		/**
		 *\~english
		 *\return		The linearised depth buffer.
		 *\~french
		 *\return		Le tampon de profondeur linéarisé.
		 */
		inline TextureUnit const & getResult()const
		{
			return m_result;
		}

	private:
		void doInitialiseLinearisePass();
		void doInitialiseMinifyPass();
		void doCleanupLinearisePass();
		void doCleanupMinifyPass();

	public:
		static constexpr uint32_t MaxMipLevel = 5u;

	private:
		Engine & m_engine;
		MatrixUbo & m_matrixUbo;
		SsaoConfigUbo & m_ssaoConfigUbo;
		castor::Size m_size;
		ShaderProgramSPtr m_lineariseProgram;
		PushUniform3fSPtr m_clipInfo;
		RenderPipelineUPtr m_linearisePipeline;
		ShaderProgramSPtr m_minifyProgram;
		PushUniform1iSPtr m_previousLevel;
		RenderPipelineUPtr m_minifyPipeline;
		TextureUnit m_result;
		std::array< FrameBufferSPtr, MaxMipLevel + 1u > m_fbos;
		std::array< TextureAttachmentSPtr, MaxMipLevel + 1u > m_resultAttaches;
		RenderPassTimerSPtr m_timer;

	};
}

#endif
