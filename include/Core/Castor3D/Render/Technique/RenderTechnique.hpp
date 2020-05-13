/*
See LICENSE file in root folder
*/
#ifndef ___C3D_RenderTechnique_H___
#define ___C3D_RenderTechnique_H___

#include "TechniqueModule.hpp"
#include "Castor3D/Material/Texture/TextureModule.hpp"
#include "Castor3D/Miscellaneous/MiscellaneousModule.hpp"
#include "Castor3D/Render/ShadowMap/ShadowMap.hpp"
#include "Castor3D/Render/Technique/Opaque/Ssao/SsaoConfig.hpp"
#include "Castor3D/Render/Technique/Opaque/Ssgi/SsgiConfig.hpp"
#include "Castor3D/Render/Technique/Opaque/DeferredRendering.hpp"
#include "Castor3D/Render/Technique/Transparent/WeightedBlendRendering.hpp"
#include "Castor3D/Scene/Background/BackgroundModule.hpp"
#include "Castor3D/Shader/Ubos/DebugConfig.hpp"
#include "Castor3D/Shader/Ubos/MatrixUbo.hpp"
#include "Castor3D/Shader/Ubos/HdrConfigUbo.hpp"

#if C3D_UseDepthPrepass
#	include "Castor3D/Render/Passes/DepthPass.hpp"
#endif

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
		 *\param[in]	config			The SSAO configuration.
		 *\~french
		 *\brief		Constructeur
		 *\param[in]	name			Le nom de la technique.
		 *\param[in]	renderTarget	La render target pour cette technique.
		 *\param[in]	renderSystem	Le render system.
		 *\param[in]	parameters		Les paramètres de la technique.
		 *\param[in]	config			La configuration du SSAO.
		 */
		C3D_API RenderTechnique( castor::String const & name
			, RenderTarget & renderTarget
			, RenderSystem & renderSystem
			, Parameters const & parameters
			, SsaoConfig const & ssaoConfig
			, SsgiConfig const & ssgiConfig );
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
		 *\return		\p true if ok.
		 *\~french
		 *\brief		Fonction d'initialisation.
		 *\return		\p true if ok.
		 */
		C3D_API bool initialise( std::vector< IntermediateView > & intermediates );
		/**
		 *\~english
		 *\brief		Cleanup function
		 *\~french
		 *\brief		Fonction de nettoyage
		 */
		C3D_API void cleanup();
		/**
		 *\~english
		 *\brief		Update function.
		 *\remarks		Gather the render queues, for further update.
		 *\param[out]	queues	Receives the render queues needed for the rendering of the frame.
		 *\~french
		 *\brief		Fonction de mise à jour.
		 *\remarks		Récupère les files de rendu, pour mise à jour ultérieure.
		 *\param[out]	queues	Reçoit les files de rendu nécessaires pour le dessin de la frame.
		 */
		C3D_API void update( RenderQueueArray & queues );
		/**
		 *\~english
		 *\brief		Updates GPU data.
		 *\param[in]	jitter	The jittering value.
		 *\param[out]	info	Receives the render informations.
		 *\~french
		 *\brief		Met à jour les données GPU.
		 *\param[in]	jitter	La valeur de jittering.
		 *\param[out]	info	Reçoit les informations de rendu.
		 */
		C3D_API void update( castor::Point2f const & jitter
			, RenderInfo & info );
		/**
		 *\~english
		 *\brief		Render function
		 *\param[in]	waitSemaphores	The semaphores to wait for.
		 *\param[out]	info			Receives the render informations.
		 *\~french
		 *\brief		Fonction de rendu.
		 *\param[in]	waitSemaphores	Les sémaphores à attendre.
		 *\param[out]	info			Reçoit les informations de rendu.
		 */
		C3D_API ashes::Semaphore const & render( ashes::SemaphoreCRefArray const & waitSemaphores
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
		*\~french
		*\brief
		*	Fonction d'acceptation de visiteur.
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
		inline castor::Size const & getSize()const
		{
			return m_size;
		}

		inline TextureLayout const & getResult()const
		{
			CU_Require( m_colourTexture );
			return *m_colourTexture;
		}

		inline TextureLayout const & getDepth()const
		{
			CU_Require( m_depthBuffer );
			return *m_depthBuffer;
		}

		inline TextureLayoutSPtr getDepthPtr()const
		{
			return m_depthBuffer;
		}

		inline MatrixUbo const & getMatrixUbo()const
		{
			return m_matrixUbo;
		}

		inline MatrixUbo & getMatrixUbo()
		{
			return m_matrixUbo;
		}

		inline RenderTechniquePass const & getOpaquePass()const
		{
			CU_Require( m_opaquePass );
			return *m_opaquePass;
		}

		inline RenderTechniquePass const & getTransparentPass()const
		{
			CU_Require( m_transparentPass );
			return *m_transparentPass;
		}

		inline ShadowMapLightTypeArray const & getShadowMaps()const
		{
			return m_allShadowMaps;
		}

		inline ashes::Semaphore const & getSemaphore()const
		{
			CU_Require( m_signalFinished );
			return *m_signalFinished;
		}

		inline RenderTarget const & getRenderTarget()const
		{
			return m_renderTarget;
		}

		inline bool isMultisampling()const
		{
			return false;
		}

		inline SsaoConfig const & getSsaoConfig()const
		{
			return m_ssaoConfig;
		}

		inline SsaoConfig & getSsaoConfig()
		{
			return m_ssaoConfig;
		}

		inline SsgiConfig const & getSsgiConfig()const
		{
			return m_ssgiConfig;
		}

		inline SsgiConfig & getSsgiConfig()
		{
			return m_ssgiConfig;
		}

		inline DebugConfig const & getDebugConfig()const
		{
			return m_debugConfig;
		}

		inline DebugConfig & getDebugConfig()
		{
			return m_debugConfig;
		}
		/**@}*/

	public:
		using ShadowMapArray = std::vector< ShadowMapUPtr >;

	private:
		void doCreateShadowMaps();
		void doInitialiseShadowMaps();
		void doInitialiseBackgroundPass();
#if C3D_UseDepthPrepass
		void doInitialiseDepthPass();
#endif
		void doInitialiseOpaquePass();
		void doInitialiseTransparentPass();
		void doCleanupShadowMaps();
		void doUpdateShadowMaps( RenderQueueArray & queues );
		void doUpdateParticles( RenderInfo & info );
		ashes::Semaphore const & doRenderShadowMaps( ashes::Semaphore const & semaphore );
		ashes::Semaphore const & doRenderEnvironmentMaps( ashes::Semaphore const & semaphore );
#if C3D_UseDepthPrepass
		ashes::Semaphore const & doRenderDepth( ashes::SemaphoreCRefArray const & semaphores );
		ashes::Semaphore const & doRenderBackground( ashes::Semaphore const & semaphore );
#endif
		ashes::Semaphore const & doRenderBackground( ashes::SemaphoreCRefArray const & semaphores );
		ashes::Semaphore const & doRenderOpaque( ashes::Semaphore const & semaphore );
		ashes::Semaphore const & doRenderTransparent( ashes::Semaphore const & semaphore );

	private:
		bool m_initialised;
		RenderTarget & m_renderTarget;
		RenderSystem & m_renderSystem;
		castor::Size m_size;
		TextureLayoutSPtr m_colourTexture;
		TextureLayoutSPtr m_depthBuffer;
		MatrixUbo m_matrixUbo;
		HdrConfigUbo m_hdrConfigUbo;
		GpInfoUbo m_gpInfoUbo;
		DebugConfig m_debugConfig;
#if C3D_UseDepthPrepass
		std::unique_ptr< DepthPass > m_depthPass;
#endif
		std::unique_ptr< RenderTechniquePass > m_opaquePass;
		std::unique_ptr< RenderTechniquePass > m_transparentPass;
		SsaoConfig m_ssaoConfig;
		SsgiConfig m_ssgiConfig;
		std::unique_ptr< DeferredRendering > m_deferredRendering;
		std::unique_ptr< WeightedBlendRendering > m_weightedBlendRendering;
		RenderPassTimerSPtr m_particleTimer;
		ShadowMapUPtr m_directionalShadowMap;
		ShadowMapUPtr m_pointShadowMap;
		ShadowMapUPtr m_spotShadowMap;
		ShadowMapLightTypeArray m_allShadowMaps;
		ShadowMapLightTypeArray m_activeShadowMaps;
		ashes::SemaphorePtr m_signalFinished;
		ashes::RenderPassPtr m_bgRenderPass;
		ashes::FrameBufferPtr m_bgFrameBuffer;
		ashes::CommandBufferPtr m_bgCommandBuffer;
		ashes::CommandBufferPtr m_cbgCommandBuffer;
		ashes::StagingBufferPtr m_stagingBuffer;
		ashes::CommandBufferPtr m_uploadCommandBuffer;
		OnBackgroundChangedConnection m_onBgChanged;
		OnBackgroundChangedConnection m_onCBgChanged;
	};
}

#endif
