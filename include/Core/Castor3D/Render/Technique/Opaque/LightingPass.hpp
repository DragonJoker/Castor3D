/*
See LICENSE file in root folder
*/
#ifndef ___C3D_LightingPass_H___
#define ___C3D_LightingPass_H___

#include "OpaqueModule.hpp"
#include "Castor3D/Render/Technique/Opaque/Lighting/LightingModule.hpp"

#include "Castor3D/Buffer/GpuBufferOffset.hpp"
#include "Castor3D/Miscellaneous/MiscellaneousModule.hpp"
#include "Castor3D/Render/GlobalIllumination/LightPropagationVolumes/LightPropagationVolumesModule.hpp"
#include "Castor3D/Render/ShadowMap/ShadowMapModule.hpp"
#include "Castor3D/Render/Technique/Opaque/Lighting/RunnableLightingPass.hpp"

namespace castor3d
{
	class LightingPass
	{
	public:
		/**
		 *\~english
		 *\brief		Initialises deferred rendering related stuff.
		 *\param[in]	graph				The render graph.
		 *\param[in]	previousPass		The previous frame pass.
		 *\param[in]	device				The GPU device.
		 *\param[in]	progress			The progress bar.
		 *\param[in]	scene				The scene.
		 *\param[in]	depth				The depth texture.
		 *\param[in]	gpResult			The geometry pass buffers.
		 *\param[in]	smDirectionalResult	The directional lights shadow map.
		 *\param[in]	smPointResult		The point lights shadow map.
		 *\param[in]	smSpotResult		The spot lights shadow map.
		 *\param[in]	lpResult			The lighting pass result.
		 *\param[in]	sceneUbo			The scene UBO.
		 *\param[in]	gpInfoUbo			The GBuffer configuration UBO.
		 *\~french
		 *\brief		Initialise les données liées au deferred rendering.
		 *\param[in]	graph				Le render graph.
		 *\param[in]	previousPass		La frame pass précédente.
		 *\param[in]	device				Le device GPU.
		 *\param[in]	progress			La barre de progression.
		 *\param[in]	scene				La scène.
		 *\param[in]	depth				La texxture de profondeur.
		 *\param[in]	gpResult			Les textures de la passe de géométries.
		 *\param[in]	smDirectionalResult	La shadow map des source lumineuses directionnelles.
		 *\param[in]	smPointResult		La shadow map des source lumineuses omnidirectionnelles.
		 *\param[in]	smSpotResult		La shadow map des source lumineuses projecteurs.
		 *\param[in]	lpResult			Le résulta de la lighting pass.
		 *\param[in]	sceneUbo			L'UBO de scène.
		 *\param[in]	gpInfoUbo			L'UBO de configuration du GBuffer.
		 */
		LightingPass( crg::FramePassGroup & graph
			, crg::FramePassArray const & previousPasses
			, RenderDevice const & device
			, ProgressBar * progress
			, Scene & scene
			, Texture const & depth
			, Texture const & depthObj
			, OpaquePassResult const & gpResult
			, ShadowMapResult const & smDirectionalResult
			, ShadowMapResult const & smPointResult
			, ShadowMapResult const & smSpotResult
			, LightPassResult const & lpResult
			, crg::ImageId const & targetColourResult
			, SceneUbo const & sceneUbo
			, GpInfoUbo const & gpInfoUbo );
		/**
		 *\~english
		 *\brief			Updates the render pass, CPU wise.
		 *\param[in, out]	updater	The update data.
		 *\~french
		 *\brief			Met à jour la passe de rendu, au niveau CPU.
		 *\param[in, out]	updater	Les données d'update.
		 */
		void update( CpuUpdater & updater );
		/**
		 *\~english
		 *\brief			Updates the render pass, GPU wise.
		 *\param[in, out]	updater	The update data.
		 *\~french
		 *\brief			Met à jour la passe de rendu, au niveau GPU.
		 *\param[in, out]	updater	Les données d'update.
		 */
		void update( GpuUpdater & updater );
		/**
		 *\copydoc		castor3d::RenderTechniquePass::accept
		 */
		void accept( PipelineVisitorBase & visitor );

		bool isEnabled()const
		{
			return m_lightPass
				&& m_lightPass->hasEnabledLights();
		}

		crg::FramePass const & getLastPass()const
		{
			return *m_lastPass;
		}

	private:
		crg::FramePass const & doCreateDepthBlitPass( crg::FramePassGroup & graph
			, crg::FramePassArray const & previousPasses
			, ProgressBar * progress );
		crg::FramePass const & doCreateLightingPass( crg::FramePassGroup & graph
			, crg::FramePass const & previousPass
			, Scene const & scene
			, ProgressBar * progress );
		void doUpdateLightPasses( CpuUpdater & updater
			, LightType lightType );

	private:
		RenderDevice const & m_device;
		Texture const & m_depth;
		Texture const & m_depthObj;
		OpaquePassResult const & m_gpResult;
		ShadowMapResult const & m_smDirectionalResult;
		ShadowMapResult const & m_smPointResult;
		ShadowMapResult const & m_smSpotResult;
		LightPassResult const & m_lpResult;
		crg::ImageId const & m_targetColourResult;
		SceneUbo const & m_sceneUbo;
		GpInfoUbo const & m_gpInfoUbo;
		crg::FramePassGroup & m_group;
		castor::Size const m_size;
		RunnableLightingPass * m_lightPass{};
		crg::FramePass const * m_lastPass{};
	};
}

#endif
