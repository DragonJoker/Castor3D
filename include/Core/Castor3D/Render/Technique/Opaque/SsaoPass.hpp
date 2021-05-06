/*
See LICENSE file in root folder
*/
#ifndef ___C3D_DeferredSsaoPass_H___
#define ___C3D_DeferredSsaoPass_H___

#include "OpaqueModule.hpp"

#include "Castor3D/Miscellaneous/MiscellaneousModule.hpp"
#include "Castor3D/Render/Ssao/SsaoModule.hpp"
#include "Castor3D/Render/Technique/Opaque/Lighting/LightPass.hpp"
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
		 *\param[in]	gpInfoUbo		The GBuffer configuration UBO.
		 *\~french
		 *\brief		Constructeur.
		 *\param[in]	engine			Le moteur.
		 *\param[in]	size			Les dimensions de la zone de rendu.
		 *\param[in]	ssaoConfig		La configuration du SSAO.
		 *\param[in]	linearisedDepth	Le depth buffer linéarisé.
		 *\param[in]	gpResult		Le résultat de la geometry pass.
		 *\param[in]	gpInfoUbo		L'UBO de configuration du GBuffer
		 */
		C3D_API SsaoPass( RenderDevice const & device
			, castor::Size const & size
			, SsaoConfig & ssaoConfig
			, TextureUnit const & linearisedDepth
			, OpaquePassResult const & gpResult
			, GpInfoUbo const & gpInfoUbo );
		/**
		 *\~english
		 *\brief		Destructor.
		 *\~french
		 *\brief		Destructeur.
		 */
		C3D_API ~SsaoPass();
		/**
		 *\~english
		 *\brief			Updates the render pass, CPU wise.
		 *\param[in, out]	updater	The update data.
		 *\~french
		 *\brief			Met à jour la passe de rendu, au niveau CPU.
		 *\param[in, out]	updater	Les données d'update.
		 */
		C3D_API void update( CpuUpdater & updater );
		/**
		 *\~english
		 *\brief		Renders the SSAO pass.
		 *\param[in]	toWait	The semaphore to wait.
		 *\~french
		 *\brief		Dessine la passe SSAO.
		 *\param[in]	toWait	Le sémaphore à attendre.
		 */
		C3D_API ashes::Semaphore const & render( ashes::Semaphore const & toWait )const;
		/**
		 *\copydoc		castor3d::RenderTechniquePass::accept
		 */
		C3D_API void accept( PipelineVisitorBase & visitor );
		/**
		 *\~english
		 *\return		The SSAO pass result.
		 *\~french
		 *\return		Le résultat de la passe SSAO.
		 */
		C3D_API TextureUnit const & getResult()const;
		/**
		 *\~english
		 *\return		The bent normals.
		 *\~french
		 *\return		Les bent normals.
		 */
		C3D_API TextureUnit const & getBentNormals()const;

		inline SsaoConfig const & getConfig()const
		{
			return m_ssaoConfig;
		}

	private:
		RenderDevice const & m_device;
		Engine & m_engine;
		SsaoConfig & m_ssaoConfig;
		TextureUnit const & m_linearisedDepth;
		OpaquePassResult const & m_gpResult;
		GpInfoUbo const & m_gpInfoUbo;
		VkExtent2D m_size;
		MatrixUbo m_matrixUbo;
		std::shared_ptr< SsaoConfigUbo > m_ssaoConfigUbo;
		std::shared_ptr< SsaoRawAOPass > m_rawAoPass;
		std::shared_ptr< SsaoBlurPass > m_horizontalBlur;
		std::shared_ptr< SsaoBlurPass > m_verticalBlur;
	};
}

#endif
