/*
See LICENSE file in root folder
*/
#ifndef ___C3D_DeferredSsaoPass_H___
#define ___C3D_DeferredSsaoPass_H___

#include "LightPass.hpp"

#include "Technique/Opaque/Ssao/BlurPass.hpp"
#include "Technique/Opaque/Ssao/LineariseDepthPass.hpp"
#include "Technique/Opaque/Ssao/SsaoConfig.hpp"
#include "Technique/Opaque/Ssao/SsaoConfigUbo.hpp"
#include "Technique/Opaque/Ssao/RawSsaoPass.hpp"
#include "Render/RenderInfo.hpp"
#include "Shader/Ubos/MatrixUbo.hpp"

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
		 *\param[in]	gpResult	The geometry pass result.
		 *\param[in]	viewport	The viewport containing depth bounds.
		 *\~french
		 *\brief		Constructeur.
		 *\param[in]	engine		Le moteur.
		 *\param[in]	size		Les dimensions de la zone de rendu.
		 *\param[in]	config		La configuration du SSAO.
		 *\param[in]	gpResult	Le résultat de la geometry pass.
		 *\param[in]	viewport	Le viewport contenant les bornes de profondeur.
		 */
		SsaoPass( Engine & engine
			, renderer::Extent2D const & size
			, SsaoConfig const & config
			, GeometryPassResult const & gpResult
			, Viewport const & viewport );
		/**
		 *\~english
		 *\brief		Destructor.
		 *\~french
		 *\brief		Destructeur.
		 */
		~SsaoPass();
		/**
		 *\~english
		 *\brief		Updates the configuration UBO.
		 *\~french
		 *\brief		Met à jour l'UBO de configuration.
		 */
		void update( Camera const & camera );
		/**
		 *\~english
		 *\brief		Renders the SSAO pass.
		 *\~french
		 *\brief		Dessine la passe SSAO.
		 */
		void render( renderer::Semaphore const & toWait )const;
		/**
		 *\~english
		 *\return		The SSAO pass result.
		 *\~french
		 *\return		Le résultat de la passe SSAO.
		 */
		TextureUnit const & getResult()const;
		/**
		 *\~english
		 *\return		The semaphore signaling the end of the pass.
		 *\~french
		 *\return		Le sémaphore signalant la fin de la passe.
		 */
		renderer::Semaphore const & getSemaphore()const;

	private:
		Engine & m_engine;
		SsaoConfig const & m_config;
		MatrixUbo m_matrixUbo;
		SsaoConfigUbo m_ssaoConfigUbo;
		LineariseDepthPass m_linearisePass;
		RawSsaoPass m_rawSsaoPass;
		SsaoBlurPass m_horizontalBlur;
		SsaoBlurPass m_verticalBlur;
	};
}

#endif
