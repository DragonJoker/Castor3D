/*
See LICENSE file in root folder
*/
#ifndef ___C3D_DebugOverlays_H___
#define ___C3D_DebugOverlays_H___

#include "Castor3D/Cache/OverlayCache.hpp"
#include "Castor3D/Render/RenderInfo.hpp"

#include <CastorUtils/Miscellaneous/PreciseTimer.hpp>

#include <ashespp/Miscellaneous/QueryPool.hpp>

#include <RenderGraph/FramePassTimer.hpp>

namespace castor3d
{
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
		 *\return		The render infos.
		 *\~french
		 *\brief		Réinitialise tous les compteurs d'image.
		 *\return		Les informations de rendu.
		 */
		RenderInfo & beginFrame();
		/**
		 *\~english
		 *\brief		Updates the overlays texts.
		 *\param[in]	first	Tells if this is the first pass.
		 *\return		The elapsed time for this frame.
		 *\~french
		 *\brief		Met à jour les textes des incrustations de débogage.
		 *\param[in]	first	Dit s'il s'agit du rendu de la première frame
		 *\return		Le temps écoulé pour cette frame.
		 */
		castor::Microseconds endFrame( bool first );
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
		 *\param[in]	category	The timer category name.
		 *\param[in]	timer		The timer to register.
		 *\return		The query ID.
		 *\~french
		 *\brief		Enregistre un timer de passe de rendu.
		 *\param[in]	category	Le nom de la catégorie du timer.
		 *\param[in]	timer		Le timer à enregistrer.
		 *\return		L'ID de la requête.
		 */
		uint32_t registerTimer( castor::String const & category
			, FramePassTimer & timer );
		/**
		 *\~english
		 *\brief		Unregisters a render pass timer.
		 *\param[in]	category	The timer category name.
		 *\param[in]	timer		The timer to unregister.
		 *\~french
		 *\brief		Désenregistre un timer de passe de rendu.
		 *\param[in]	category	Le nom de la catégorie du timer.
		 *\param[in]	timer		Le timer à désenregistrer.
		 */
		void unregisterTimer( castor::String const & category
			, FramePassTimer & timer );
		/**
		 *\~english
		 *\brief			Fills the parameters with all passes times, in nanoseconds.
		 *\param[in,out]	params	Receives the times.
		 *\~french
		 *\brief			Remplit les paramètres avec les temps de toutes les passes, en nanosecondes.
		 *\param[in,out]	params	Reçoit les temps.
		 */
		void dumpFrameTimes( Parameters & params );
		/**
		 *\~english
		 *\return		The debug overlays shown status.
		 *\~french
		 *\return		Le statut d'affichage des incrustations de débogage.
		 */
		bool isShown()const
		{
			return m_visible;
		}

		void enableDetailed( bool value )
		{
			m_detailed = value;
		}

		castor::Nanoseconds getAvgFrameTime()const
		{
			return m_averageTime;
		}

	private:
		void doCreateDebugPanel( OverlayCache & cache );
		void doCompute();

	private:
		template< typename T >
		class DebugPanelT
		{
		public:
			DebugPanelT( DebugPanelT const & ) = delete;
			DebugPanelT & operator=( DebugPanelT const & ) = delete;
			DebugPanelT( DebugPanelT && ) = default;
			DebugPanelT & operator=( DebugPanelT && ) = delete;
			DebugPanelT( castor::String const & name
				, castor::String const & label
				, PanelOverlaySPtr panel
				, OverlayCache & cache
				, T const & value );
			~DebugPanelT();
			void update();
			void updatePosition( int y );

		protected:
			OverlayCache & m_cache;
			T const & m_v;
			TextOverlaySPtr m_label;
			TextOverlaySPtr m_value;
		};

		template< typename T >
		class DebugPanelsT
		{
			using DebugPanelArray = std::vector< DebugPanelT< T > >;

		public:
			DebugPanelsT( DebugPanelsT const & ) = delete;
			DebugPanelsT & operator=( DebugPanelsT const & ) = delete;
			DebugPanelsT( DebugPanelsT && ) = default;
			DebugPanelsT & operator=( DebugPanelsT && ) = delete;
			DebugPanelsT( castor::String const & title
				, PanelOverlaySPtr panel
				, OverlayCache & cache );
			~DebugPanelsT();
			void update();
			int updatePosition( int y );
			void add( castor::String const & name
				, castor::String const & label
				, T const & value );

		private:
			OverlayCache & m_cache;
			PanelOverlaySPtr m_panel;
			PanelOverlaySPtr m_titlePanel;
			TextOverlaySPtr m_titleText;
			DebugPanelArray m_panels;
		};

		template< typename T >
		using DebugPanelsPtrT = std::unique_ptr< DebugPanelsT< T > >;

		class MainDebugPanel
		{
		public:
			explicit MainDebugPanel( OverlayCache & cache );
			~MainDebugPanel();
			void update();
			void setVisible( bool visible );
			void updatePosition();
			void addTimePanel( castor::String const & name
				, castor::String const & label
				, castor::Nanoseconds const & value );
			void addCountPanel( castor::String const & name
				, castor::String const & label
				, uint32_t const & value );
			void addFpsPanel( castor::String const & name
				, castor::String const & label
				, float const & value );

		private:
			OverlayCache & m_cache;
			PanelOverlaySPtr m_panel;
			DebugPanelsPtrT< castor::Nanoseconds > m_times;
			DebugPanelsPtrT< float > m_fps;
			DebugPanelsPtrT< uint32_t > m_counts;
		};

		class PassOverlays
		{
		private:
			struct TimeOverlays
			{
				castor::Nanoseconds time{ 0_ns };
				TextOverlaySPtr name;
				TextOverlaySPtr value;
			};

		public:
			PassOverlays( PassOverlays const & ) = delete;
			PassOverlays & operator=( PassOverlays const & ) = delete;
			PassOverlays( PassOverlays && ) = default;
			PassOverlays & operator=( PassOverlays && ) = delete;
			PassOverlays( OverlayCache & cache
				, OverlayRPtr parent
				, castor::String const & category
				, castor::String const & name
				, uint32_t index
				, bool const & detailed );
			~PassOverlays();
			void retrieveGpuTime();
			void compute();
			void update( uint32_t & top );
			void addTimer( FramePassTimer & timer );
			bool removeTimer( FramePassTimer & timer );

			castor::Nanoseconds getGpuTime()const
			{
				return m_gpu.time;
			}

			castor::Nanoseconds getCpuTime()const
			{
				return m_cpu.time;
			}

			castor::String const & getName()const
			{
				return m_name;
			}

		private:
			OverlayCache & m_cache;
			bool const & m_detailed;
			castor::String m_name;
			std::map< FramePassTimer *, crg::OnFramePassDestroyConnection > m_timers;
			PanelOverlaySPtr m_panel;
			bool m_visible{ true };
			TextOverlaySPtr m_passName;
			TimeOverlays m_cpu;
			TimeOverlays m_gpu;
		};

		using PassOverlaysPtr = std::unique_ptr< PassOverlays >;

		class CategoryOverlays
		{
		private:
			struct TimeOverlays
			{
				castor::Nanoseconds time{ 0_ns };
				TextOverlaySPtr name;
				TextOverlaySPtr value;
			};

		public:
			CategoryOverlays( CategoryOverlays const & ) = delete;
			CategoryOverlays & operator=( CategoryOverlays const & ) = delete;
			CategoryOverlays( CategoryOverlays && ) = default;
			CategoryOverlays & operator=( CategoryOverlays && ) = default;
			CategoryOverlays();
			CategoryOverlays( castor::String const & category
				, OverlayCache & cache
				, bool const & detailed );
			~CategoryOverlays();
			void addTimer( FramePassTimer & timer );
			bool removeTimer( FramePassTimer & timer );
			void retrieveGpuTime();
			void compute();
			void update( uint32_t & top );
			void setVisible( bool visible );

			uint32_t getPanelCount()const
			{
				return m_visibleCount;
			}

			castor::Nanoseconds getGpuTime()const
			{
				return m_gpu.time;
			}

			castor::Nanoseconds getCpuTime()const
			{
				return m_cpu.time;
			}

		private:
			OverlayCache * m_cache{};
			bool const * m_detailed{};
			castor::String m_categoryName;
			int m_posX{ 0 };
			std::vector< PassOverlaysPtr > m_passes;
			PanelOverlaySPtr m_container;
			uint32_t m_visibleCount{};
			bool m_visible{ true };
			bool m_parentVisible{ true };
			PanelOverlaySPtr m_firstLinePanel;
			TextOverlaySPtr m_name;
			TimeOverlays m_cpu;
			TimeOverlays m_gpu;
		};

		using CategoryOverlaysArray = std::map< castor::String, CategoryOverlays >;

	private:
#if defined( NDEBUG )
		static uint32_t constexpr FrameSamplesCount = 100u;
#else
		static uint32_t constexpr FrameSamplesCount = 20u;
#endif
		static uint32_t constexpr PanelHeight = 20u;
		static uint32_t constexpr DebugPanelWidth = 320u;
		static uint32_t constexpr DebugTitleOffset = 10u;
		static uint32_t constexpr DebugTitleWidth = DebugPanelWidth - ( DebugTitleOffset - 2u );
		static uint32_t constexpr PassMainPanelLeft = 20u;
		static uint32_t constexpr PassPanelLeft = DebugPanelWidth + 10u;
		static uint32_t constexpr PassNameWidth = 230u;
		static uint32_t constexpr CategoryNameWidth = 250u;
		static uint32_t constexpr CpuNameWidth = 30u;
		static uint32_t constexpr CpuValueWidth = 75u;
		static uint32_t constexpr GpuNameWidth = 30u;
		static uint32_t constexpr GpuValueWidth = 75u;
		static uint32_t constexpr PanelBaseLevel = 65536u;

		std::mutex m_mutex;
		castor::PreciseTimer m_taskTimer{};
		castor::PreciseTimer m_frameTimer{};
		castor::PreciseTimer m_debugTimer{};
		std::unique_ptr< MainDebugPanel > m_debugPanel;
		CategoryOverlaysArray m_renderPasses;
		std::array< castor::Nanoseconds, FrameSamplesCount > m_framesTimes{};
		uint32_t m_frameIndex{ 0 };
		uint64_t m_frameCount{ 0 };
		bool m_visible{ false };
		bool m_detailed{ false };
		castor::Nanoseconds m_cpuTime{ 0 };
		castor::Nanoseconds m_gpuClientTime{ 0 };
		castor::Nanoseconds m_gpuTotalTime{ 0 };
		castor::Nanoseconds m_gpuTime{ 0 };
		castor::Nanoseconds m_totalTime{ 0 };
		castor::Nanoseconds m_externalTime{ 0 };
		float m_fps{ 0.0f };
		float m_averageFps{ 0.0f };
		castor::Nanoseconds m_averageTime{ 0 };
		std::locale m_timesLocale{};
		RenderInfo m_renderInfo;
		ashes::QueryPoolPtr m_queries;
		uint32_t m_queriesCount{ 0u };
		bool m_dirty{ false };
	};
}

#include "Castor3D/Overlay/DebugOverlays.inl"

#endif
