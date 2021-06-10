/*
See LICENSE file in root folder
*/
#ifndef ___C3D_LightingPass_H___
#define ___C3D_LightingPass_H___

#include "OpaqueModule.hpp"
#include "Castor3D/Render/Technique/Opaque/Lighting/LightingModule.hpp"

#include "Castor3D/Material/Texture/TextureUnit.hpp"
#include "Castor3D/Miscellaneous/MiscellaneousModule.hpp"
#include "Castor3D/Render/GlobalIllumination/LightPropagationVolumes/LightPropagationVolumesModule.hpp"
#include "Castor3D/Render/Passes/CommandsSemaphore.hpp"
#include "Castor3D/Render/ShadowMap/ShadowMapModule.hpp"
#include "Castor3D/Render/Technique/Opaque/OpaqueResolvePass.hpp"
#include "Castor3D/Render/Technique/Opaque/Lighting/LightPassResult.hpp"
#include "Castor3D/Render/Technique/Opaque/ReflectiveShadowMapGI/RsmConfig.hpp"

#include <CastorUtils/Design/DelayedInitialiser.hpp>

#include <ashespp/Command/CommandBuffer.hpp>
#include <ashespp/Sync/Fence.hpp>
#include <ashespp/Sync/Semaphore.hpp>

#include <unordered_set>

namespace castor3d
{
	class LightingPass
	{
	public:
		enum class Type
		{
			eNoShadow = 0u,
			eNoShadowVoxelConeTracingGI,
			eShadowNoGI,
			eShadowVoxelConeTracingGI,
			eShadowRsmGI,
			eShadowLpvGI,
			eShadowLpvGGI,
			eShadowLayeredLpvGI,
			eShadowLayeredLpvGGI,
			CU_ScopedEnumBounds( eNoShadow ),
		};
		static_assert( uint32_t( Type::eCount ) == uint32_t( GlobalIlluminationType::eCount ) + 2u );
		using TypeLightPasses = std::array< LightPassUPtr, size_t( LightType::eCount ) >;
		using LightPasses = std::array< TypeLightPasses, size_t( Type::eCount ) >;

	public:
		/**
		 *\~english
		 *\brief		Initialises deferred rendering related stuff.
		 *\param[in]	engine				The engine.
		 *\param[in]	device				The GPU device.
		 *\param[in]	size				The render area dimensions.
		 *\param[in]	scene				The scene.
		 *\param[in]	gpResult			The geometry pass buffers.
		 *\param[in]	smDirectionalResult	The directional lights shadow map.
		 *\param[in]	smPointResult		The point lights shadow map.
		 *\param[in]	smSpotResult		The spot lights shadow map.
		 *\param[in]	lpvResult			The LPV result.
		 *\param[in]	llpvResult			The Layered LPV result.
		 *\param[in]	vctFirstBounce		The VCT first bounce result.
		 *\param[in]	vctSecondaryBounce	The VCT secondary bounce result.
		 *\param[in]	depthView			The depth buffer attach.
		 *\param[in]	sceneUbo			The scene UBO.
		 *\param[in]	gpInfoUbo			The GBuffer configuration UBO.
		 *\param[in]	lpvConfigUbo		The LPV configuration UBO.
		 *\param[in]	llpvConfigUbo		The Layered LPV configuration UBO.
		 *\param[in]	vctConfigUbo		The VCT configuration UBO.
		 *\~french
		 *\brief		Initialise les données liées au deferred rendering.
		 *\param[in]	engine				Le moteur.
		 *\param[in]	device				Le device GPU.
		 *\param[in]	size				Les dimensions de la zone de rendu.
		 *\param[in]	scene				La scène.
		 *\param[in]	gpResult			Les textures de la passe de géométries.
		 *\param[in]	smDirectionalResult	La shadow map des source lumineuses directionnelles.
		 *\param[in]	smPointResult		La shadow map des source lumineuses omnidirectionnelles.
		 *\param[in]	smSpotResult		La shadow map des source lumineuses projecteurs.
		 *\param[in]	lpvResult			Le résultat du LPV.
		 *\param[in]	llpvResult			Le résultat du Layered LPV.
		 *\param[in]	vctFirstBounce		Le résultat du premier rebond de VCT.
		 *\param[in]	vctSecondaryBounce	Le résultat du second rebond de VCT.
		 *\param[in]	depthView			L'attache du tampon de profondeur.
		 *\param[in]	sceneUbo			L'UBO de scène.
		 *\param[in]	gpInfoUbo			L'UBO de configuration du GBuffer.
		 *\param[in]	lpvConfigUbo		L'UBO de configuration des LPV.
		 *\param[in]	llpvConfigUbo		L'UBO de configuration des Layered LPV.
		 *\param[in]	vctConfigUbo		L'UBO de configuration du VCT.
		 */
		LightingPass( crg::FrameGraph & graph
			, crg::FramePass const & previousPass
			, RenderDevice const & device
			, castor::Size const & size
			, Scene & scene
			, OpaquePassResult const & gpResult
			, ShadowMapResult const & smDirectionalResult
			, ShadowMapResult const & smPointResult
			, ShadowMapResult const & smSpotResult
			, LightVolumePassResult const & lpvResult
			, LightVolumePassResultArray const & llpvResult
			, Texture const & vctFirstBounce
			, Texture const & vctSecondaryBounce
			, Texture const & depthView
			, SceneUbo const & sceneUbo
			, GpInfoUbo const & gpInfoUbo
			, LpvGridConfigUbo const & lpvConfigUbo
			, LayeredLpvGridConfigUbo const & llpvConfigUbo
			, VoxelizerUbo const & vctConfigUbo );
		/**
		 *\~english
		 *\brief		Destructor.
		 *\~french
		 *\brief		Destructeur.
		 */
		~LightingPass();
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
		 *\~english
		 *\brief		Renders the light passes on currently bound framebuffer.
		 *\param[in]	scene	The scene.
		 *\param[in]	camera	The viewing camera.
		 *\param[in]	gp		The geometry pass result.
		 *\param[out]	toWait	The semaphore from previous render pass.
		 *\~french
		 *\brief		Dessine les passes d'éclairage sur le tampon d'image donné.
		 *\param[in]	scene	La scène.
		 *\param[in]	camera	La caméra.
		 *\param[in]	gp		Le résultat de la geometry pass.
		 *\param[out]	toWait	Le sémaphore de la passe de rendu précédente.
		 */
		ashes::Semaphore const & render( Scene const & scene
			, Camera const & camera
			, OpaquePassResult const & gp
			, ashes::Semaphore const & toWait );
		/**
		 *\copydoc		castor3d::RenderTechniquePass::accept
		 */
		void accept( PipelineVisitorBase & visitor );
		/**
		 *\~english
		 *\return		The light pass diffuse result.
		 *\~french
		 *\return		Le résultat diffus de la passe d'éclairage.
		 */
		inline LightPassResult const & getResult()const
		{
			return m_result;
		}

	private:
		void doUpdateLightPasses( GpuUpdater & updater
			, LightType lightType );
		ashes::Semaphore const & doRenderLights( Scene const & scene
			, Camera const & camera
			, LightType type
			, OpaquePassResult const & gp
			, ashes::Semaphore const & toWait
			, uint32_t & index );
		LightPass * doGetLightPass( LightType lightType
			, bool shadows
			, GlobalIlluminationType giType )const;

	private:
		crg::FrameGraph & m_graph;
		crg::FramePass const & m_previousPass;
		RenderDevice const & m_device;
		OpaquePassResult const & m_gpResult;
		ShadowMapResult const & m_smDirectionalResult;
		ShadowMapResult const & m_smPointResult;
		ShadowMapResult const & m_smSpotResult;
		LightVolumePassResult const & m_lpvResult;
		LightVolumePassResultArray const & m_llpvResult;
		Texture const & m_vctFirstBounce;
		Texture const & m_vctSecondaryBounce;
		Texture const & m_depthView;
		SceneUbo const & m_sceneUbo;
		GpInfoUbo const & m_gpInfoUbo;
		LpvGridConfigUbo const & m_lpvConfigUbo;
		LayeredLpvGridConfigUbo const & m_llpvConfigUbo;
		VoxelizerUbo const & m_vctConfigUbo;
		Texture const & m_srcDepth;
		castor::Size const m_size;
		LightPassResult m_result;
		LightPasses m_lightPasses;
		RenderPassTimerSPtr m_timer;
		ashes::FencePtr m_fence;
		std::unordered_set< LightPass * > m_active;
		bool m_voxelConeTracing{ false };
	};
}

#endif
