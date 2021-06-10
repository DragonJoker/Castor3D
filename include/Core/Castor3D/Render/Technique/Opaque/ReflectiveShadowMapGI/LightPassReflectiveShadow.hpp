/*
See LICENSE file in root folder
*/
#ifndef ___C3D_LightPassReflectiveShadow_H___
#define ___C3D_LightPassReflectiveShadow_H___

#include "ReflectiveShadowMapGIModule.hpp"

#include "Castor3D/Render/Technique/Opaque/Lighting/LightPassShadow.hpp"

#include "Castor3D/Material/Texture/TextureLayout.hpp"
#include "Castor3D/Material/Texture/TextureUnit.hpp"
#include "Castor3D/Render/Passes/DownscalePass.hpp"
#include "Castor3D/Render/Technique/Opaque/OpaquePassResult.hpp"
#include "Castor3D/Render/Technique/Opaque/Lighting/LightPassResult.hpp"
#include "Castor3D/Render/Technique/Opaque/ReflectiveShadowMapGI/RsmGIPass.hpp"
#include "Castor3D/Render/Technique/Opaque/ReflectiveShadowMapGI/RsmInterpolatePass.hpp"
#include "Castor3D/Scene/Scene.hpp"

namespace castor3d
{
	template< LightType LtType >
	class LightPassReflectiveShadow
		: public LightPassShadow< LtType >
	{
	public:
		using my_traits = LightPassShadowTraits< LtType >;
		using my_light_type = typename my_traits::light_type;
		using my_pass_type = typename my_traits::light_pass_type;
		using my_shadow_pass_type = typename my_traits::shadow_pass_type;

	public:
		/**
		 *\~english
		 *\brief		Constructor.
		 *\param[in]	device		The GPU device.
		 *\param[in]	lpConfig	The light pass configuration.
		 *\~french
		 *\brief		Constructeur.
		 *\param[in]	device		Le device GPU.
		 *\param[in]	lpConfig	La configuration de la passe d'éclairage.
		 */
		LightPassReflectiveShadow( crg::FrameGraph & graph
			, crg::FramePass const *& previousPass
			, RenderDevice const & device
			, RsmLightPassConfig const & lpConfig )
			: LightPassShadow< LtType >{ graph
				, previousPass
				, device
				, "ReflectiveShadow"
				, { lpConfig.base.lpResult, lpConfig.base.gpInfoUbo, true, false, true } }
			, m_gpResult{ lpConfig.gpResult }
			, m_smResult{ lpConfig.smResult }
			, m_lpResult{ lpConfig.base.lpResult }
			, m_gpInfoUbo{ lpConfig.base.gpInfoUbo }
			, m_downscalePass{ castor::makeUnique< DownscalePass >( graph
				, previousPass
				, m_device
				, cuT( "Reflective Shadow Maps" )
				, crg::ImageViewIdArray{ m_lpResult[LpTexture::eDiffuse].wholeViewId
					, m_gpResult[DsTexture::eData1].wholeViewId }
				, VkExtent2D{ getExtent( m_lpResult[LpTexture::eDiffuse].wholeViewId ).width >> 2
					, getExtent( m_lpResult[LpTexture::eDiffuse].wholeViewId ).height >> 2 } ) }
		{
		}
		/**
		 *\copydoc		castor3d::LightPass::initialise
		 */
		void initialise( Scene const & scene
			, OpaquePassResult const & gp
			, SceneUbo & sceneUbo
			, RenderPassTimer & timer )override
		{
			auto & lightCache = scene.getLightCache();
			auto previousPass = &m_previousPass;
			m_rsmGiPass = std::make_unique< RsmGIPass >( m_graph
				, previousPass
				, m_device
				, scene.getLightCache()
				, LtType
				, VkExtent2D{ getExtent( m_lpResult[LpTexture::eDiffuse].wholeViewId ).width >> 2
					, getExtent( m_lpResult[LpTexture::eDiffuse].wholeViewId ).height >> 2 }
				, m_gpInfoUbo
				, m_gpResult
				, m_smResult
				, m_downscalePass->getResult() );
			m_interpolatePass = std::make_unique< RsmInterpolatePass >( m_graph
				, previousPass
				, m_device
				, scene.getLightCache()
				, LtType
				, VkExtent2D{ getExtent( m_lpResult[LpTexture::eDiffuse].wholeViewId ).width >> 2
					, getExtent( m_lpResult[LpTexture::eDiffuse].wholeViewId ).height >> 2 }
				, m_gpInfoUbo
				, m_gpResult
				, m_smResult
				, m_rsmGiPass->getConfigUbo()
				, m_rsmGiPass->getSamplesSsbo()
				, m_rsmGiPass->getResult()[0]
				, m_rsmGiPass->getResult()[1]
				, m_lpResult[LpTexture::eIndirectDiffuse].targetViewId );
			LightPassShadow< LtType >::initialise( scene, gp, sceneUbo, timer );
		}
		/**
		 *\copydoc		castor3d::LightPass::cleanup
		 */
		void cleanup()override
		{
			LightPassShadow< LtType >::cleanup();
			m_interpolatePass.reset();
			m_rsmGiPass.reset();
			m_downscalePass.reset();
		}
		/**
		 *\copydoc		castor3d::LightPass::accept
		 */
		void accept( PipelineVisitorBase & visitor )override
		{
			LightPassShadow< LtType >::accept( visitor );

			if ( m_rsmGiPass )
			{
				m_rsmGiPass->accept( visitor );
			}

			if ( m_downscalePass )
			{
				m_downscalePass->accept( visitor );
			}
		}

	protected:
		/**
		 *\copydoc		castor3d::LightPass::doUpdate
		 */
		void doUpdate( bool first
			, castor::Size const & size
			, Light const & light
			, Camera const & camera )override
		{
			my_pass_type::doUpdate( first
				, size
				, light
				, camera );
			m_rsmGiPass->update( light );
		}

	private:
		VkClearValue doGetIndirectClearColor()const override
		{
			return opaqueBlackClearColor;
		}

	private:
		OpaquePassResult const & m_gpResult;
		ShadowMapResult const & m_smResult;
		LightPassResult const & m_lpResult;
		GpInfoUbo const & m_gpInfoUbo;
		DownscalePassUPtr m_downscalePass;
		RsmGIPassUPtr m_rsmGiPass;
		RsmInterpolatePassUPtr m_interpolatePass;
	};
}

#endif
