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
		 *\param[in]	camera		The viewing camera.
		 *\~french
		 *\brief		Constructeur.
		 *\param[in]	engine		Le moteur.
		 *\param[in]	size		Les dimensions de la zone de rendu.
		 *\param[in]	config		La configuration du SSAO.
		 *\param[in]	gpResult	Le résultat de la geometry pass.
		 *\param[in]	camera		La caméra de rendu.
		 */
		SsaoPass( Engine & engine
			, castor::Size const & size
			, SsaoConfig const & config
			, GeometryPassResult const & gpResult
			, Camera const & camera );
		/**
		 *\~english
		 *\brief		Destructor.
		 *\~french
		 *\brief		Destructeur.
		 */
		~SsaoPass();
		/**
		 *\~english
		 *\brief		Renders the SSAO pass.
		 *\~french
		 *\brief		Dessine la passe SSAO.
		 */
		void render( Camera const & camera );
		/**
		 *\~english
		 *\return		The SSAO pass result.
		 *\~french
		 *\return		Le résultat de la passe SSAO.
		 */
		TextureUnit const & getResult()const;

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
