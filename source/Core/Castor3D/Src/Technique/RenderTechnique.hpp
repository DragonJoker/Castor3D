/*
See LICENSE file in root folder
*/
#ifndef ___C3D_RenderTechnique_H___
#define ___C3D_RenderTechnique_H___

#include "HDR/ToneMapping.hpp"
#include "Technique/Opaque/Ssao/SsaoConfig.hpp"
#include "Render/RenderInfo.hpp"
#include "Texture/TextureUnit.hpp"
#include "Technique/DepthPass.hpp"
#include "Technique/RenderTechniqueFbo.hpp"
#include "Technique/Opaque/DeferredRendering.hpp"
#include "Technique/Transparent/WeightedBlendRendering.hpp"
#include "ShadowMap/ShadowMap.hpp"

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
		 *\param[in]	index		The base texture index.
		 *\return		\p true if ok.
		 *\~french
		 *\brief		Fonction d'initialisation.
		 *\param[in]	index		L'index de texture de base.
		 *\return		\p true if ok.
		 */
		C3D_API bool initialise( uint32_t & index );
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
		C3D_API void render( castor::Point2r const & jitter
			, TextureUnit const & velocity
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
		 *\return		The render area dimensions.
		 *\~french
		 *\return		Les dimensions de la zone de rendu.
		 */
		inline castor::Size const & getSize()const
		{
			return m_size;
		}
		/**
		 *\~english
		 *\return		The colour texture holding the render's result.
		 *\~french
		 *\return		La texture de couleurs contenant le résultat du rendu.
		 */
		inline TextureLayout const & getResult()const
		{
			REQUIRE( m_frameBuffer.m_colourTexture );
			return *m_frameBuffer.m_colourTexture;
		}
		/**
		 *\~english
		 *\return		The depth texture holding the render's result.
		 *\~french
		 *\return		La texture de profondeurs contenant le résultat du rendu.
		 */
		inline TextureLayout const & getDepth()const
		{
			REQUIRE( m_frameBuffer.m_colourTexture );
			return *m_frameBuffer.m_depthBuffer;
		}
		/**
		 *\~english
		 *\return		The opaque nodes render pass.
		 *\~french
		 *\return		La passe de rendu des noeuds opaques.
		 */
		inline RenderTechniquePass const & getOpaquePass()const
		{
			REQUIRE( m_opaquePass );
			return *m_opaquePass;
		}
		/**
		 *\~english
		 *\return		The transparent nodes render pass.
		 *\~french
		 *\return		La passe de rendu des noeuds transparents.
		 */
		inline RenderTechniquePass const & getTransparentPass()const
		{
			REQUIRE( m_transparentPass );
			return *m_transparentPass;
		}
		/**
		 *\~english
		 *\return		\p true if the samples count is greater than 1.
		 *\~french
		 *\return		\p true si le nombre d'échantillons est plus grand que 1.
		 */
		inline bool isMultisampling()const
		{
			return false;
		}

	public:
		using ShadowMapArray = std::vector< ShadowMapUPtr >;

	private:
		void doInitialiseShadowMaps();
		void doCleanupShadowMaps();
		void doUpdateShadowMaps( RenderQueueArray & queues );
		void doRenderShadowMaps();
		void doRenderEnvironmentMaps();
		void doRenderOpaque( castor::Point2r const & jitter
			, TextureUnit const & velocity
			, RenderInfo & info );
		void doUpdateParticles( RenderInfo & info );
		void doRenderTransparent( castor::Point2r const & jitter
			, TextureUnit const & velocity
			, RenderInfo & info );
		void doApplyPostEffects();

	private:
		//!\~english	The technique intialisation status.
		//!\~french		Le statut d'initialisation de la technique.
		bool m_initialised;
		//!\~english	The parent render target.
		//!\~french		La render target parente.
		RenderTarget & m_renderTarget;
		//!\~english	The render system.
		//!\~french		Le render system.
		RenderSystem & m_renderSystem;
		//!\~english	The render area dimension.
		//!\~french		Les dimensions de l'aire de rendu.
		castor::Size m_size;
		//!\~english	The HDR frame buffer.
		//!\~french		Le tampon d'image HDR.
		RenderTechniqueFbo m_frameBuffer;
		//!\~english	The pass used to render opaque nodes.
		//!\~french		La passe utilisée pour dessiner les noeuds opaques.
		std::unique_ptr< RenderTechniquePass > m_opaquePass;
		//!\~english	The pass used to render transparent nodes.
		//!\~french		La passe utilisée pour dessiner les noeuds transparents.
		std::unique_ptr< RenderTechniquePass > m_transparentPass;
		//!\~english	The SSAO configuration.
		//!\~french		La configuration du SSAO.
		SsaoConfig m_ssaoConfig;
		//!\~english	The deferred rendering used for opaque meshes.
		//!\~french		Le rendu différé utilisé pour les maillages opaques.
		std::unique_ptr< DeferredRendering > m_deferredRendering;
		//!\~english	The weighted blend rendering used for transparent meshes.
		//!\~french		Le rendu weighted blend utilisé pour les maillages transparents.
		std::unique_ptr< WeightedBlendRendering > m_weightedBlendRendering;
		//!\~english	The particles timer.
		//!\~french		Le timer de particules.
		RenderPassTimerSPtr m_particleTimer;
		//!\~english	The post effect timer.
		//!\~french		Le timer d'effets post-rendu.
		RenderPassTimerSPtr m_postFxTimer;
		//!\~english	The directional lights shadow maps.
		//!\~french		Les textures d'ombres pour les lumières directionnelles.
		ShadowMapArray m_directionalShadowMaps;
		//!\~english	The point lights shadow maps.
		//!\~french		Les textures d'ombres pour les lumières omni-directionnelles.
		ShadowMapArray m_pointShadowMaps;
		//!\~english	The spot lights shadow maps.
		//!\~french		Les textures d'ombres pour les lumières projecteurs.
		ShadowMapArray m_spotShadowMaps;
		//!\~english	The active shadow maps.
		//!\~french		Les textures d'ombres actives.
		ShadowMapLightTypeArray m_activeShadowMaps;
	};
}

#endif
