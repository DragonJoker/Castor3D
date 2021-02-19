/*
See LICENSE file in root folder
*/
#ifndef ___C3D_RenderTechnique_H___
#define ___C3D_RenderTechnique_H___

#include "TechniqueModule.hpp"

#include "Castor3D/Material/Texture/TextureUnit.hpp"
#include "Castor3D/Miscellaneous/MiscellaneousModule.hpp"
#include "Castor3D/Render/GlobalIllumination/LightPropagationVolumes/LightPropagationVolumesModule.hpp"
#include "Castor3D/Render/Passes/CommandsSemaphore.hpp"
#include "Castor3D/Render/ShadowMap/ShadowMap.hpp"
#include "Castor3D/Render/Ssao/SsaoConfig.hpp"
#include "Castor3D/Render/Technique/Opaque/OpaqueModule.hpp"
#include "Castor3D/Render/Technique/Transparent/TransparentModule.hpp"
#include "Castor3D/Render/Technique/Voxelize/VoxelizeModule.hpp"
#include "Castor3D/Scene/Background/BackgroundModule.hpp"
#include "Castor3D/Shader/Ubos/DebugConfig.hpp"
#include "Castor3D/Shader/Ubos/GpInfoUbo.hpp"
#include "Castor3D/Shader/Ubos/LayeredLpvGridConfigUbo.hpp"
#include "Castor3D/Shader/Ubos/LpvGridConfigUbo.hpp"
#include "Castor3D/Shader/Ubos/MatrixUbo.hpp"
#include "Castor3D/Shader/Ubos/VoxelizerUbo.hpp"

#include <CastorUtils/Design/DelayedInitialiser.hpp>

#include <CastorUtils/Design/Named.hpp>

namespace castor3d
{
	class RenderTechnique
		: public castor::OwnedBy< Engine >
		, public castor::Named
	{
		friend class RenderTechniquePass;

	public:
		/**
		 *\~english
		 *\brief		Constructor
		 *\param[in]	name			The technique name.
		 *\param[in]	renderTarget	The render target for this technique.
		 *\param[in]	renderSystem	The render system.
		 *\param[in]	parameters		The technique parameters.
		 *\param[in]	ssaoConfig		The SSAO configuration.
		 *\~french
		 *\brief		Constructeur
		 *\param[in]	name			Le nom de la technique.
		 *\param[in]	renderTarget	La render target pour cette technique.
		 *\param[in]	renderSystem	Le render system.
		 *\param[in]	parameters		Les paramètres de la technique.
		 *\param[in]	ssaoConfig		La configuration du SSAO.
		 */
		C3D_API RenderTechnique( castor::String const & name
			, RenderTarget & renderTarget
			, RenderSystem & renderSystem
			, Parameters const & parameters
			, SsaoConfig const & ssaoConfig );
		/**
		 *\~english
		 *\brief		Destructor
		 *\~french
		 *\brief		Destructeur
		 */
		C3D_API virtual ~RenderTechnique();
		/**
		 *\~english
		 *\brief		Initialisation function.
		 *\param[in]	device			The GPU device.
		 *\param[out]	intermediates	Receives the intermediate views used by the whole technique.
		 *\return		\p true if ok.
		 *\~french
		 *\brief		Fonction d'initialisation.
		 *\param[in]	device			Le device GPU.
		 *\param[out]	intermediates	Reçoit les vues intermédiaires utilisées par toute la technique.
		 *\return		\p true if ok.
		 */
		C3D_API bool initialise( RenderDevice const & device
			, std::vector< IntermediateView > & intermediates );
		/**
		 *\~english
		 *\brief		Cleanup function
		 *\param[in]	device	The GPU device.
		 *\~french
		 *\brief		Fonction de nettoyage
		 *\param[in]	device	Le device GPU.
		 */
		C3D_API void cleanup( RenderDevice const & device );
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
		 *\brief			Updates the render pass, GPU wise.
		 *\param[in, out]	updater	The update data.
		 *\~french
		 *\brief			Met à jour la passe de rendu, au niveau GPU.
		 *\param[in, out]	updater	Les données d'update.
		 */
		C3D_API void update( GpuUpdater & updater );
		/**
		 *\~english
		 *\brief		Render function
		 *\param[in]	device			The GPU device.
		 *\param[in]	waitSemaphores	The semaphores to wait for.
		 *\param[out]	info			Receives the render informations.
		 *\~french
		 *\brief		Fonction de rendu.
		 *\param[in]	device			Le device GPU.
		 *\param[in]	waitSemaphores	Les sémaphores à attendre.
		 *\param[out]	info			Reçoit les informations de rendu.
		 */
		C3D_API ashes::Semaphore const & render( RenderDevice const & device
			, ashes::SemaphoreCRefArray const & waitSemaphores
			, RenderInfo & info );
		/**
		 *\~english
		 *\brief		Writes the technique into a text file.
		 *\param[in]	file	The file.
		 *\~french
		 *\brief		Ecrit la technique dans un fichier texte.
		 *\param[in]	file	Le fichier.
		 */
		C3D_API bool writeInto( castor::TextFile & file );
		/**
		*\~english
		*\brief
		*	Visitor acceptance function.
		*\param visitor
		*	The ... visitor.
		*\~french
		*\brief
		*	Fonction d'acceptation de visiteur.
		*\param visitor
		*	Le ... visiteur.
		*/
		C3D_API void accept( RenderTechniqueVisitor & visitor );
		/**
		*\~english
		*name
		*	Getters.
		*\~french
		*name
		*	Accesseurs.
		*/
		/**@{*/
		castor::Size const & getSize()const
		{
			return m_size;
		}

		TextureLayout const & getResult()const
		{
			CU_Require( m_colourTexture.isTextured() );
			return *m_colourTexture.getTexture();
		}

		TextureLayout const & getDepth()const
		{
			return *m_depthBuffer.getTexture();
		}

		TextureLayoutSPtr getDepthPtr()const
		{
			return m_depthBuffer.getTexture();
		}

		MatrixUbo const & getMatrixUbo()const
		{
			return m_matrixUbo;
		}

		MatrixUbo & getMatrixUbo()
		{
			return m_matrixUbo;
		}

		RenderTechniquePass const & getOpaquePass()const
		{
			CU_Require( m_opaquePass );
			return *m_opaquePass;
		}

		RenderTechniquePass const & getTransparentPass()const
		{
			CU_Require( m_transparentPass );
			return *m_transparentPass;
		}

		ShadowMapLightTypeArray const & getShadowMaps()const
		{
			return m_allShadowMaps;
		}

		ashes::Semaphore const & getSemaphore()const
		{
			CU_Require( m_signalFinished );
			return *m_signalFinished;
		}

		RenderTarget const & getRenderTarget()const
		{
			return m_renderTarget;
		}

		bool isMultisampling()const
		{
			return false;
		}

		SsaoConfig const & getSsaoConfig()const
		{
			return m_ssaoConfig;
		}

		SsaoConfig & getSsaoConfig()
		{
			return m_ssaoConfig;
		}

		DebugConfig const & getDebugConfig()const
		{
			return m_debugConfig;
		}

		DebugConfig & getDebugConfig()
		{
			return m_debugConfig;
		}
		/**@}*/

	public:
		using ShadowMapArray = std::vector< ShadowMapUPtr >;

	private:
		void doCreateShadowMaps();
		void doCreateLpv( RenderDevice const & device );
		void doInitialiseShadowMaps( RenderDevice const & device );
		void doInitialiseLpv( RenderDevice const & device );
		void doInitialiseBackgroundPass( RenderDevice const & device );
		void doInitialiseDepthPass( RenderDevice const & device );
		void doInitialiseOpaquePass( RenderDevice const & device );
		void doInitialiseTransparentPass( RenderDevice const & device );
		void doCleanupShadowMaps( RenderDevice const & device );
		void doCleanupLpv( RenderDevice const & device );
		void doUpdateShadowMaps( CpuUpdater & updater );
		void doUpdateShadowMaps( GpuUpdater & updater );
		void doUpdateLpv( CpuUpdater & updater );
		void doUpdateLpv( GpuUpdater & updater );
		void doUpdateParticles( CpuUpdater & updater );
		void doUpdateParticles( GpuUpdater & updater );
		ashes::Semaphore const & doRenderShadowMaps( RenderDevice const & device
			, ashes::Semaphore const & semaphore );
		ashes::Semaphore const & doRenderLpv( RenderDevice const & device
			, ashes::Semaphore const & semaphore );
		ashes::Semaphore const & doRenderEnvironmentMaps( RenderDevice const & device
			, ashes::Semaphore const & semaphore );
		ashes::Semaphore const & doRenderDepth( RenderDevice const & device
			, ashes::SemaphoreCRefArray const & semaphores );
		ashes::Semaphore const & doRenderBackground( RenderDevice const & device
			, ashes::Semaphore const & semaphore );
		ashes::Semaphore const & doRenderBackground( RenderDevice const & device
			, ashes::SemaphoreCRefArray const & semaphores );
		ashes::Semaphore const & doRenderOpaque( RenderDevice const & device
			, ashes::Semaphore const & semaphore );
		ashes::Semaphore const & doRenderTransparent( RenderDevice const & device
			, ashes::Semaphore const & semaphore );

	private:
		bool m_initialised;
		RenderTarget & m_renderTarget;
		RenderSystem & m_renderSystem;
		castor::Size m_size;
		TextureUnit m_colourTexture;
		TextureUnit m_depthBuffer;
		MatrixUbo m_matrixUbo;
		GpInfoUbo m_gpInfoUbo;
		DebugConfig m_debugConfig;
		DepthPassUPtr m_depthPass;
		VoxelizerUPtr m_voxelizer;
		RenderTechniquePassUPtr m_opaquePass;
		RenderTechniquePassUPtr m_transparentPass;
		SsaoConfig m_ssaoConfig;
		DeferredRenderingUPtr m_deferredRendering;
		WeightedBlendRenderingUPtr m_weightedBlendRendering;
		RenderPassTimerSPtr m_particleTimer;
		ShadowMapUPtr m_directionalShadowMap;
		ShadowMapUPtr m_pointShadowMap;
		ShadowMapUPtr m_spotShadowMap;
		ShadowMapLightTypeArray m_allShadowMaps;
		ShadowMapLightTypeArray m_activeShadowMaps;
		LpvGridConfigUbo m_lpvConfigUbo;
		LayeredLpvGridConfigUbo m_llpvConfigUbo;
		VoxelizerUbo m_vctConfigUbo;
		LightVolumePassResultUPtr m_lpvResult;
		LightVolumePassResultArray m_llpvResult;
		LightPropagationVolumesLightType m_lightPropagationVolumes;
		LayeredLightPropagationVolumesLightType m_layeredLightPropagationVolumes;
		LightPropagationVolumesGLightType m_lightPropagationVolumesG;
		LayeredLightPropagationVolumesGLightType m_layeredLightPropagationVolumesG;
		ashes::SemaphorePtr m_signalFinished;
		ashes::RenderPassPtr m_bgRenderPass;
		ashes::FrameBufferPtr m_bgFrameBuffer;
		ashes::CommandBufferPtr m_bgCommandBuffer;
		ashes::CommandBufferPtr m_cbgCommandBuffer;
		OnBackgroundChangedConnection m_onBgChanged;
		OnBackgroundChangedConnection m_onCBgChanged;
		CommandsSemaphore m_colorTexTransition;
		CommandsSemaphore m_clearLpv;
	};
}

#endif
