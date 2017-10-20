﻿/*
See LICENSE file in root folder
*/
#ifndef ___C3D_DEBUG_OVERLAYS_H___
#define ___C3D_DEBUG_OVERLAYS_H___

#include "Cache/OverlayCache.hpp"
#include "Render/RenderInfo.hpp"

#include <Miscellaneous/PreciseTimer.hpp>
#include <Design/OwnedBy.hpp>

namespace castor3d
{
	/*!
	\author 	Sylvain DOREMUS
	\date 		17/09/2015
	\version	0.8.0
	\~english
	\brief		Debug overlays class.
	\remarks	Contains all datas and methods to update and display debug overlays.
				<br />If not shown, all the methods will be deactivated.
	\~french
	\brief		Classe de gestion des incrustations de debogage.
	\remarks	Contient toutes les données et méthodes pour mettre à jour et afficher les incrustations de débogage.
				<br />Si non affichés, toutes les méthodes seront désactivées.
	*/
	class DebugOverlays
		: public castor::OwnedBy< Engine >
	{
	public:
		/**
		 *\~english
		 *\brief		Constructor.
		 *\param[in]	engine	The engine.
		 *\~french
		 *\brief		Constructeur.
		 *\param[in]	engine	Le moteur.
		 */
		explicit DebugOverlays( Engine & engine );
		/**
		 *\~english
		 *\brief		Destructor.
		 *\~french
		 *\brief		Destructeur.
		 */
		~DebugOverlays();
		/**
		 *\~english
		 *\brief		Initialisation function, retrieves the overlays from the given overlay cache.
		 *\param[in]	cache	The overlay cache.
		 *\~french
		 *\brief		Fonction d'initialisation, récupère les incrustations à partir du cache d'incrustations donné.
		 *\param[in]	cache	Le cache d'incrustations.
		 */
		void initialise( OverlayCache & cache );
		/**
		 *\~english
		 *\brief		Clean up function.
		 *\~french
		 *\brief		Fonction de nettoyage.
		 */
		void cleanup();
		/**
		 *\~english
		 *\brief		Resets all frame counters.
		 *\~french
		 *\brief		Réinitialise tous les compteurs d'image.
		 */
		void beginFrame();
		/**
		 *\~english
		 *\brief		Updates the overlays texts.
		 *\param[in]	info	The render informations.
		 *\~french
		 *\brief		Met à jour les textes des incrustations de débogage.
		 *\param[in]	info	Les informations de rendu.
		 */
		void endFrame( RenderInfo const & info );
		/**
		 *\~english
		 *\brief		Used to add to the GPU time, the time elapsed between now and the last call of either endGpuTask or endCpuTask
		 *\~french
		 *\brief		Utilisé pour ajouter au temps GPU le temps écoulé entre maintenant et le dernier appel de endGpuTask ou endCpuTask
		 */
		void endGpuTask();
		/**
		 *\~english
		 *\brief		Used to add to the GPU time, the time elapsed between now and the last call of either endGpuTask or endCpuTask
		 *\~french
		 *\brief		Utilisé pour ajouter au temps GPU le temps écoulé entre maintenant et le dernier appel de endGpuTask ou endCpuTask
		 */
		void endCpuTask();
		/**
		 *\~english
		 *\brief		Show or hide debug overlays.
		 *\param[in]	show	The status.
		 *\~french
		 *\brief		Affiche ou cache les incrustations de débogage.
		 *\param[in]	show	Le statut.
		 */
		void show( bool show );
		/**
		 *\~english
		 *\brief		Registers a render pass timer.
		 *\param[in]	timer	The timer to register.
		 *\~french
		 *\brief		Enregistre un timer de passe de rendu.
		 *\param[in]	timer	Le timer à enregistrer.
		 */
		void registerTimer( RenderPassTimer const & timer );
		/**
		 *\~english
		 *\brief		Unregisters a render pass timer.
		 *\param[in]	timer	The timer to unregister.
		 *\~french
		 *\brief		Désenregistre un timer de passe de rendu.
		 *\param[in]	timer	Le timer à désenregistrer.
		 */
		void unregisterTimer( RenderPassTimer const & timer );

	private:
		struct RenderPassOverlays
		{
			std::vector< std::reference_wrapper< RenderPassTimer const > > m_timers;
			PanelOverlaySPtr m_panel;
			TextOverlaySPtr m_title;
			TextOverlaySPtr m_cpuName;
			TextOverlaySPtr m_cpuValue;
			TextOverlaySPtr m_gpuName;
			TextOverlaySPtr m_gpuValue;
		};
		using RenderPassOverlaysArray = std::map< castor::String, RenderPassOverlays >;

	private:
#if defined( _NDEBUG )
		static const uint32_t FRAME_SAMPLES_COUNT = 100;
#else
		static const uint32_t FRAME_SAMPLES_COUNT = 20;
#endif
		//!\~english	The tasks (CPU or GPU) timer.
		//!\~french		Le timer pour les tâches (CPU ou GPU).
		castor::PreciseTimer m_taskTimer;
		//!\~english	The frame timer.
		//!\~french		Le timer de frame.
		castor::PreciseTimer m_frameTimer;
		//!\~english	The debug timer.
		//!\~french		Le timer de debug.
		castor::PreciseTimer m_debugTimer;
		//!\~english	The base debug panel overlay.
		//!\~french		Le panneau d'incrustations de débogage.
		OverlayWPtr m_debugPanel;
		//!\~english	The CPU time value overlay.
		//!\~french		L'incrustation contenant la valeur de temps CPU.
		TextOverlaySPtr m_debugCpuTime;
		//!\~english	The GPU client time value overlay.
		//!\~french		L'incrustation contenant la valeur de temps client GPU (Temps perdu).
		TextOverlaySPtr m_debugGpuClientTime;
		//!\~english	The GPU server time value overlay.
		//!\~french		L'incrustation contenant la valeur de temps serveur GPU (Temps de rendu).
		TextOverlaySPtr m_debugGpuServerTime;
		//!\~english	The debug time value overlay.
		//!\~french		L'incrustation contenant la valeur du temps de débogage.
		TextOverlaySPtr m_debugTime;
		//!\~english	The external time value overlay.
		//!\~french		L'incrustation contenant la valeur du temps externe.
		TextOverlaySPtr m_externTime;
		//!\~english	The total time value overlay.
		//!\~french		L'incrustation contenant la valeur de temps total.
		TextOverlaySPtr m_debugTotalTime;
		//!\~english	The average FPS value overlay.
		//!\~french		L'incrustation contenant la valeur du nombre moyen d'images par secondes.
		TextOverlaySPtr m_debugAverageFps;
		//!\~english	The average time value overlay.
		//!\~french		L'incrustation contenant la valeur du temps moyen par images.
		TextOverlaySPtr m_debugAverageTime;
		//!\~english	The vertex count value overlay.
		//!\~french		L'incrustation contenant la valeur du nombre de sommets.
		TextOverlaySPtr m_debugVertexCount;
		//!\~english	The faces count value overlay.
		//!\~french		L'incrustation contenant la valeur du nombre de faces.
		TextOverlaySPtr m_debugFaceCount;
		//!\~english	The objects count value overlay.
		//!\~french		L'incrustation contenant la valeur du nombre d'objets.
		TextOverlaySPtr m_debugObjectCount;
		//!\~english	The visible objects count value overlay.
		//!\~french		L'incrustation contenant la valeur du nombre d'objets visibles.
		TextOverlaySPtr m_debugVisibleObjectCount;
		//!\~english	The particles count value overlay.
		//!\~french		L'incrustation contenant la valeur du nombre de particules.
		TextOverlaySPtr m_debugParticlesCount;
		//!\~english	The render passes overlays.
		//!\~french		Les incrustations des passes de rendu.
		RenderPassOverlaysArray m_renderPasses;
		//!\~english	The times of the 100 last frames.
		//!\~french		Les temps des 100 dernières frames.
		std::array< castor::Nanoseconds, FRAME_SAMPLES_COUNT > m_framesTimes;
		//!\~english	The current frame index in m_framesTimes.
		//!\~french		L'index de la frame courante, dans m_framesTimes.
		uint32_t m_frameIndex{ 0 };
		//!\~english	Tells if the debug overlays are successfully loaded.
		//!\~french		Dit si les incrustations de débogage sont chargées correctement.
		bool m_valid{ false };
		//!\~english	Defines if the debug overlays are shown.
		//!\~french		Définit si les incrustations de débogage sont affichées ou cachées.
		bool m_visible{ false };
		//!\~english	The CPU time.
		//!\~french		Le temps CPU.
		castor::Nanoseconds m_cpuTime{ 0 };
		//!\~english	The GPU time.
		//!\~french		Le temps GPU.
		castor::Nanoseconds m_gpuTime{ 0 };
		//!\~english	The time spent out of the render loop.
		//!\~french		Le temps passé hors de la boucle de rendu.
		castor::Nanoseconds m_externalTime{ 0 };
		//!\~english	The locale used to display times.
		//!\~french		La locale utilisée pour afficher les temps.
		std::locale m_timesLocale;
	};
}

#endif
