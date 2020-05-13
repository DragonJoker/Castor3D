/*
See LICENSE file in root folder
*/
#ifndef ___C3D_DeferredSsaoPass_H___
#define ___C3D_DeferredSsaoPass_H___

#include "OpaqueModule.hpp"

#include "Castor3D/Miscellaneous/MiscellaneousModule.hpp"
#include "Castor3D/Render/Ssao/SsaoModule.hpp"
#include "Castor3D/Render/Technique/Opaque/LightPass.hpp"
#include "Castor3D/Shader/Ubos/MatrixUbo.hpp"

#define C3D_DebugSSAO 0

namespace castor3d
{
	class SsaoPass
	{
	public:
		/**
		 *\~english
		 *\brief		Constructor.
		 *\param[in]	engine			The engine.
		 *\param[in]	size			The render area dimensions.
		 *\param[in]	ssaoConfig		The SSAO configuration.
		 *\param[in]	linearisedDepth	The linearised depth buffer.
		 *\param[in]	gpResult		The geometry pass result.
		 *\~french
		 *\brief		Constructeur.
		 *\param[in]	engine			Le moteur.
		 *\param[in]	size			Les dimensions de la zone de rendu.
		 *\param[in]	ssaoConfig		La configuration du SSAO.
		 *\param[in]	linearisedDepth	Le depth buffer linéarisé.
		 *\param[in]	gpResult		Le résultat de la geometry pass.
		 */
		SsaoPass( Engine & engine
			, VkExtent2D const & size
			, SsaoConfig & ssaoConfig
			, TextureUnit const & linearisedDepth
			, GeometryPassResult const & gpResult
			, GpInfoUbo const & gpInfoUbo );
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
		ashes::Semaphore const & render( ashes::Semaphore const & toWait )const;
		/**
		 *\copydoc		castor3d::RenderTechniquePass::accept
		 */
		void accept( PipelineVisitorBase & visitor );
		/**
		 *\~english
		 *\return		The SSAO pass result.
		 *\~french
		 *\return		Le résultat de la passe SSAO.
		 */
		TextureUnit const & getResult()const;

	private:
		Engine & m_engine;
		SsaoConfig & m_ssaoConfig;
		MatrixUbo m_matrixUbo;
		std::shared_ptr< SsaoConfigUbo > m_ssaoConfigUbo;
		std::shared_ptr< SsaoRawAOPass > m_rawAoPass;
		std::shared_ptr< SsaoBlurPass > m_horizontalBlur;
		std::shared_ptr< SsaoBlurPass > m_verticalBlur;
	};
}

#endif
