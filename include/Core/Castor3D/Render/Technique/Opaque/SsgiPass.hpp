/*
See LICENSE file in root folder
*/
#ifndef ___C3D_DeferredSsgiPass_H___
#define ___C3D_DeferredSsgiPass_H___

#include "OpaqueModule.hpp"

#include "Castor3D/Render/RenderModule.hpp"
#include "Castor3D/Render/Passes/PassesModule.hpp"
#include "Castor3D/Render/Technique/Opaque/Ssao/SsaoModule.hpp"
#include "Castor3D/Render/Technique/Opaque/Ssgi/SsgiModule.hpp"
#include "Castor3D/Render/Technique/Opaque/LightPass.hpp"
#include "Castor3D/Shader/Ubos/MatrixUbo.hpp"
#include "Castor3D/Render/Technique/RenderTechniqueVisitor.hpp"

#define C3D_DebugSSGI 0

namespace castor3d
{
	class SsgiPass
	{
	public:
		/**
		 *\~english
		 *\brief		Constructor.
		 *\param[in]	engine			The engine.
		 *\param[in]	size			The render area dimensions.
		 *\param[in]	ssaoConfig		The SSAO configuration.
		 *\param[in]	ssgiConfig		The SSGI configuration.
		 *\param[in]	linearisedDepth	The linearised depth buffer.
		 *\param[in]	scene			The scene lighting result.
		 *\~french
		 *\brief		Constructeur.
		 *\param[in]	engine			Le moteur.
		 *\param[in]	size			Les dimensions de la zone de rendu.
		 *\param[in]	ssaoConfig		La configuration du SSAO.
		 *\param[in]	ssgiConfig		La configuration du SSGI.
		 *\param[in]	linearisedDepth	Le depth buffer linéarisé.
		 *\param[in]	scene			Le résultat de l'éclairage de la scène.
		 */
		SsgiPass( Engine & engine
			, VkExtent2D const & size
			, SsaoConfig & ssaoConfig
			, SsgiConfig & ssgiConfig
			, TextureLayout const & linearisedDepth
			, TextureLayoutSPtr scene );
		/**
		 *\~english
		 *\brief		Destructor.
		 *\~french
		 *\brief		Destructeur.
		 */
		~SsgiPass();
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
		void accept( RenderTechniqueVisitor & visitor );
		/**
		 *\~english
		 *\return		The SSAO pass result.
		 *\~french
		 *\return		Le résultat de la passe SSAO.
		 */
		TextureLayout const & getResult()const;

	private:
		Engine & m_engine;
		VkExtent2D m_size;
		SsaoConfig & m_ssaoConfig;
		SsgiConfig & m_ssgiConfig;
		MatrixUbo m_matrixUbo;
		std::shared_ptr< RawSsgiPass > m_rawSsgi;
		std::shared_ptr< GaussianBlur > m_gaussianBlur;
		std::shared_ptr< CombinePass > m_combine;
	};
}

#endif
