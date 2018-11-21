/*
See LICENSE file in root folder
*/
#ifndef ___C3D_RenderTechnique_H___
#define ___C3D_RenderTechnique_H___

#include "HDR/ToneMapping.hpp"
#include "Technique/Opaque/Ssao/SsaoConfig.hpp"
#include "Render/RenderInfo.hpp"
#include "Shader/Ubos/MatrixUbo.hpp"
#include "ShadowMap/ShadowMap.hpp"
#include "Technique/DepthPass.hpp"
#include "Technique/RenderTechniqueVisitor.hpp"
#include "Technique/Opaque/DeferredRendering.hpp"
#include "Technique/Transparent/WeightedBlendRendering.hpp"
#include "Texture/TextureUnit.hpp"

#include <Design/Named.hpp>
#include <Design/OwnedBy.hpp>
#include <Graphics/Rectangle.hpp>

namespace castor3d
{
	class RenderTechniquePass;
	/*!
	\author		Sylvain DOREMUS
	\version	0.7.0.0
	\date		12/11/2012
	\~english
	\brief		Render technique base class
	\remarks	A render technique is the description of a way to render a render target
	\~french
	\brief		Classe de base d'une technique de rendu
	\remarks	Une technique de rendu est la description d'une manière de rendre une cible de rendu
	*/
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
			, SsaoConfig const & config );
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
		C3D_API bool initialise();
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
		 *\brief		Render function
		 *\param[in]	jitter		The jittering value.
		 *\param[out]	velocity	Receives the velocity render.
		 *\param[out]	info		Receives the render informations.
		 *\~french
		 *\brief		Fonction de rendu.
		 *\param[out]	jitter		La valeur de jittering.
		 *\param[out]	velocity	Reçoit le rendu des vélocités.
		 *\param[out]	info		Reçoit les informations de rendu.
		 */
		C3D_API ashes::Semaphore const & render( castor::Point2r const & jitter
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
		 *\brief		Displays debug dumps.
		 *\~french
		 *\brief		Affiche les dumps de debug.
		 */
		C3D_API void debugDisplay( castor::Size const & size )const;
		/**
		*\~english
		*\brief		Visitor acceptance function.
		*\~french
		*\brief		Fonction d'acceptation de visiteur.
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
			REQUIRE( m_colourTexture );
			return *m_colourTexture;
		}

		inline TextureLayout const & getDepth()const
		{
			REQUIRE( m_depthBuffer );
			return *m_depthBuffer;
		}

		inline MatrixUbo const & getMatrixUbo()const
		{
			return m_matrixUbo;
		}

		inline RenderTechniquePass const & getOpaquePass()const
		{
			REQUIRE( m_opaquePass );
			return *m_opaquePass;
		}

		inline RenderTechniquePass const & getTransparentPass()const
		{
			REQUIRE( m_transparentPass );
			return *m_transparentPass;
		}

		inline ShadowMapLightTypeArray const & getShadowMaps()const
		{
			return m_allShadowMaps;
		}

		inline ashes::Semaphore const & getSemaphore()const
		{
			REQUIRE( m_signalFinished );
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
		/**@}*/

	public:
		using ShadowMapArray = std::vector< ShadowMapUPtr >;

	private:
		void doInitialiseShadowMaps();
		void doInitialiseBackgroundPass();
		void doInitialiseOpaquePass();
		void doInitialiseTransparentPass();
		void doInitialiseDebugPass();
		void doCleanupShadowMaps();
		void doUpdateShadowMaps( RenderQueueArray & queues );
		void doUpdateParticles( RenderInfo & info );
		ashes::Semaphore const & doRenderShadowMaps( ashes::Semaphore const & semaphore );
		ashes::Semaphore const & doRenderEnvironmentMaps( ashes::Semaphore const & semaphore );
		ashes::Semaphore const & doRenderBackground( ashes::SemaphoreCRefArray const & semaphores );
		ashes::Semaphore const & doRenderOpaque( castor::Point2r const & jitter
			, RenderInfo & info
			, ashes::Semaphore const & semaphore );
		ashes::Semaphore const & doRenderTransparent( castor::Point2r const & jitter
			, RenderInfo & info
			, ashes::Semaphore const & semaphore );

	private:
		bool m_initialised;
		RenderTarget & m_renderTarget;
		RenderSystem & m_renderSystem;
		castor::Size m_size;
		TextureLayoutSPtr m_colourTexture;
		TextureLayoutSPtr m_depthBuffer;
		MatrixUbo m_matrixUbo;
		std::unique_ptr< RenderTechniquePass > m_opaquePass;
		std::unique_ptr< RenderTechniquePass > m_transparentPass;
		SsaoConfig m_ssaoConfig;
		HdrConfigUbo m_hdrConfigUbo;
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
		ashes::RenderPassPtr m_debugRenderPass;
		ashes::FrameBufferPtr m_debugFrameBuffer;
		ashes::StagingBufferPtr m_stagingBuffer;
		ashes::CommandBufferPtr m_uploadCommandBuffer;
		OnBackgroundChangedConnection m_onBgChanged;
		OnBackgroundChangedConnection m_onCBgChanged;
	};
}

#endif
