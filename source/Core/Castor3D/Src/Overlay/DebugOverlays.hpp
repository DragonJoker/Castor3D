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
		void registerTimer( RenderPassTimer & timer );
		/**
		 *\~english
		 *\brief		Unregisters a render pass timer.
		 *\param[in]	timer	The timer to unregister.
		 *\~french
		 *\brief		Désenregistre un timer de passe de rendu.
		 *\param[in]	timer	Le timer à désenregistrer.
		 */
		void unregisterTimer( RenderPassTimer & timer );

	private:
		void doCreateDebugPanel( OverlayCache & cache );

	private:
		class DebugPanel
		{
		public:
			DebugPanel( castor::String const & name
				, castor::String const & label
				, uint32_t index
				, OverlayCache & cache );
			virtual ~DebugPanel();
			void setVisible( bool visible );
			virtual void update() = 0;

		protected:
			OverlayCache & m_cache;
			PanelOverlaySPtr m_panel;
			TextOverlaySPtr m_label;
			TextOverlaySPtr m_value;
		};

		class TimeDebugPanel
			: public DebugPanel
		{
		public:
			TimeDebugPanel( castor::String const & name
				, castor::String const & label
				, uint32_t index
				, OverlayCache & cache
				, castor::Nanoseconds const & value );
			void update()override;

		private:
			castor::Nanoseconds const & m_v;
		};

		class CountDebugPanel
			: public DebugPanel
		{
		public:
			CountDebugPanel( castor::String const & name
				, castor::String const & label
				, uint32_t index
				, OverlayCache & cache
				, uint32_t const & value );
			void update()override;

		private:
			uint32_t const & m_v;
		};

		class FloatDebugPanel
			: public DebugPanel
		{
		public:
			FloatDebugPanel( castor::String const & name
				, castor::String const & label
				, uint32_t index
				, OverlayCache & cache
				, float const & value );
			void update()override;

		private:
			float const & m_v;
		};

		class RenderPassOverlays
		{
		public:
			RenderPassOverlays( RenderPassOverlays const & ) = delete;
			RenderPassOverlays & operator=( RenderPassOverlays const & ) = delete;
			RenderPassOverlays( RenderPassOverlays && ) = default;
			RenderPassOverlays & operator=( RenderPassOverlays && ) = default;
			RenderPassOverlays( castor::String const & category
				, uint32_t index
				, OverlayCache & cache );
			~RenderPassOverlays();
			void addTimer( RenderPassTimer & timer );
			bool removeTimer( RenderPassTimer & timer );
			castor::Nanoseconds update();
			void setVisible( bool visible );

		private:
			OverlayCache & m_cache;
			std::vector< std::reference_wrapper< RenderPassTimer > > m_timers;
			PanelOverlaySPtr m_panel;
			PanelOverlaySPtr m_titlePanel;
			TextOverlaySPtr m_title;
			TextOverlaySPtr m_cpuName;
			TextOverlaySPtr m_cpuValue;
			TextOverlaySPtr m_gpuName;
			TextOverlaySPtr m_gpuValue;
		};

		using DebugPanelArray = std::vector< std::unique_ptr< DebugPanel > >;
		using RenderPassOverlaysArray = std::map< castor::String, RenderPassOverlays >;

	private:
#if defined( _NDEBUG )
		static const uint32_t FRAME_SAMPLES_COUNT = 100;
#else
		static const uint32_t FRAME_SAMPLES_COUNT = 20;
#endif
		castor::PreciseTimer m_taskTimer;
		castor::PreciseTimer m_frameTimer;
		castor::PreciseTimer m_debugTimer;
		DebugPanelArray m_debugPanels;
		RenderPassOverlaysArray m_renderPasses;
		std::array< castor::Nanoseconds, FRAME_SAMPLES_COUNT > m_framesTimes;
		uint32_t m_frameIndex{ 0 };
		bool m_visible{ false };
		castor::Nanoseconds m_cpuTime{ 0 };
		castor::Nanoseconds m_gpuClientTime{ 0 };
		castor::Nanoseconds m_gpuTotalTime{ 0 };
		castor::Nanoseconds m_gpuTime{ 0 };
		castor::Nanoseconds m_totalTime{ 0 };
		castor::Nanoseconds m_externalTime{ 0 };
		float m_averageFps{ 0.0f };
		castor::Nanoseconds m_averageTime{ 0 };
		std::locale m_timesLocale;
		uint32_t m_totalVertexCount;
		uint32_t m_totalFaceCount;
		uint32_t m_totalObjectsCount;
		uint32_t m_visibleObjectsCount;
		uint32_t m_particlesCount;
		uint32_t m_totalLightsCount;
		uint32_t m_visibleLightsCount;
	};
}

#endif
