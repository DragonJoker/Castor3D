/*
See LICENSE file in root folder
*/
#ifndef ___C3D_DebugOverlays_H___
#define ___C3D_DebugOverlays_H___

#include "Castor3D/Cache/OverlayCache.hpp"
#include "Castor3D/Gui/GuiModule.hpp"
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
		 *\brief		Collects all GPU times
		 *\~french
		 *\brief		Collecte tous les temps GPU
		 */
		void endGpuTasks();
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
		 *\~french
		 *\brief		Enregistre un timer de passe de rendu.
		 *\param[in]	category	Le nom de la catégorie du timer.
		 *\param[in]	timer		Le timer à enregistrer.
		 */
		void registerTimer( castor::String const & category
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

		castor::Nanoseconds getAvgFrameTime()const
		{
			return m_averageTime;
		}

	private:
		void doCreateMainDebugPanel();
		void doCreateRenderPassesDebugPanel();
		void doCompute();

	private:
		class DebugPanel
		{
		public:
			DebugPanel( DebugPanel const & ) = delete;
			DebugPanel & operator=( DebugPanel const & ) = delete;
			DebugPanel & operator=( DebugPanel && )noexcept = delete;
			DebugPanel( castor::String const & name
				, castor::String const & label
				, Engine & engine
				, PanelCtrl & parent
				, castor::Function< castor::String() > value );
			DebugPanel( DebugPanel && rhs )noexcept;
			~DebugPanel()noexcept;

			void update();

		protected:
			Engine & m_engine;
			castor::Function< castor::String() > m_v;
			PanelCtrlRPtr m_panel;
			StaticCtrlRPtr m_label;
			StaticCtrlRPtr m_value;
		};

		class DebugPanels
		{
			using DebugPanelArray = castor::Vector< DebugPanel >;

		public:
			DebugPanels( DebugPanels const & ) = delete;
			DebugPanels & operator=( DebugPanels const & ) = delete;
			DebugPanels & operator=( DebugPanels && )noexcept = delete;
			DebugPanels( castor::String const & title
				, Engine & engine
				, PanelCtrl & parent );
			DebugPanels( DebugPanels && rhs )noexcept;
			~DebugPanels()noexcept;

			void update();
			uint32_t updatePosition( uint32_t y );
			void add( castor::String const & name
				, castor::String const & label
				, castor::Function< castor::String() > value );

		private:
			Engine & m_engine;
			ExpandablePanelCtrlRPtr m_panel;
			StaticCtrlRPtr m_title;
			DebugPanelArray m_panels;
		};

		using DebugPanelsPtr = castor::RawUniquePtr< DebugPanels >;

		class MainDebugPanel
		{
		public:
			explicit MainDebugPanel( Engine & engine );
			~MainDebugPanel()noexcept;
			void update();
			void setVisible( bool visible );
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
			void doUpdatePosition();

		private:
			Engine & m_engine;
			PanelCtrlRPtr m_panel;
			DebugPanelsPtr m_times;
			DebugPanelsPtr m_fps;
			DebugPanelsPtr m_counts;
		};

		class PassOverlays
		{
		private:
			struct TimeOverlays
			{
				castor::Nanoseconds time{ 0_ns };
				StaticCtrlRPtr name;
				StaticCtrlRPtr value;
			};

		public:
			PassOverlays( PassOverlays const & ) = delete;
			PassOverlays & operator=( PassOverlays const & ) = delete;
			PassOverlays & operator=( PassOverlays && )noexcept = delete;
			PassOverlays( Engine & engine
				, PanelCtrl & parent
				, castor::String const & name
				, uint32_t leftOffset
				, uint32_t index );
			PassOverlays( PassOverlays && rhs )noexcept;
			~PassOverlays()noexcept;
			void retrieveGpuTime()const;
			void compute();
			bool update( uint32_t & top );
			void addTimer( FramePassTimer & timer );
			bool removeTimer( FramePassTimer & timer );

			bool isVisible()const noexcept
			{
				return m_visible;
			}

			castor::Nanoseconds getGpuTime()const noexcept
			{
				return m_gpu.time;
			}

			castor::Nanoseconds getCpuTime()const noexcept
			{
				return m_cpu.time;
			}

			castor::String const & getName()const noexcept
			{
				return m_name;
			}

		private:
			PanelCtrl * m_parent{};
			castor::String m_name;
			bool m_visible{ true };
			uint32_t m_visibleCount{};
			castor::Map< FramePassTimer *, crg::OnFramePassDestroyConnection > m_timers;
			PanelCtrlRPtr m_panel;
			StaticCtrlRPtr m_passName;
			TimeOverlays m_cpu;
			TimeOverlays m_gpu;
		};

		using PassOverlaysPtr = castor::RawUniquePtr< PassOverlays >;
		class CategoryOverlays;
		using CategoryOverlaysPtr = castor::RawUniquePtr< CategoryOverlays >;
		using CategoriesOverlays = castor::Vector< CategoryOverlaysPtr >;

		class CategoryOverlays
		{
		private:
			struct TimeOverlays
			{
				castor::Nanoseconds time{ 0_ns };
				StaticCtrlRPtr name;
				StaticCtrlRPtr value;
			};

		public:
			CategoryOverlays( CategoryOverlays const & ) = delete;
			CategoryOverlays & operator=( CategoryOverlays const & ) = delete;
			CategoryOverlays();
			CategoryOverlays( castor::String const & category
				, Engine & engine
				, PanelCtrl & parent
				, uint32_t leftOffset
				, bool expanded = false );
			CategoryOverlays & operator=( CategoryOverlays && rhs )noexcept;
			CategoryOverlays( CategoryOverlays && rhs )noexcept;
			~CategoryOverlays()noexcept;
			void addTimer( castor::String const & name
				, castor::StringArray & categories
				, FramePassTimer & timer );
			bool removeTimer( castor::String const & name
				, castor::StringArray & categories
				, FramePassTimer & timer );
			void retrieveGpuTime()const;
			void compute();
			bool update( uint32_t & top );
			void setVisible( bool visible );
			bool hasVisibleChild()const noexcept;
			PanelCtrl * getContainer()const;
			void dumpFrameTimes( castor::String prefix
				, Parameters & params )const;

			castor::Nanoseconds getGpuTime()const noexcept
			{
				return m_gpu.time;
			}

			castor::Nanoseconds getCpuTime()const noexcept
			{
				return m_cpu.time;
			}

			auto & getName()const noexcept
			{
				return m_categoryName;
			}

			auto & getCategories()const noexcept
			{
				return m_categories;
			}

			auto & getCategories()noexcept
			{
				return m_categories;
			}

		private:
			Engine * m_engine{};
			PanelCtrl * m_parent{};
			castor::String m_categoryName{};
			uint32_t m_leftOffset{};
			int m_posX{};
			bool m_visible{ true };
			bool m_parentVisible{ true };
			castor::Vector< PassOverlaysPtr > m_passes{};
			CategoriesOverlays m_categories{};
			ExpandablePanelCtrlRPtr m_container{};
			StaticCtrlRPtr m_name{};
			TimeOverlays m_cpu{};
			TimeOverlays m_gpu{};
		};

	private:
#if defined( NDEBUG )
		static uint32_t constexpr FrameSamplesCount = 100u;
#else
		static uint32_t constexpr FrameSamplesCount = 20u;
#endif
		static uint32_t constexpr PanelHeight = 20u;
		static uint32_t constexpr DebugPanelWidth = 320u;
		static uint32_t constexpr DebugLabelWidth = 190u;
		static uint32_t constexpr DebugValueWidth = 110u;
		static uint32_t constexpr DebugLineWidth = DebugLabelWidth + DebugValueWidth;
		static uint32_t constexpr PassMainPanelLeft = 20u;
		static uint32_t constexpr PassPanelLeft = DebugPanelWidth + 10u;
		static uint32_t constexpr CategoryNameWidth = 250u;
		static uint32_t constexpr CpuNameWidth = 30u;
		static uint32_t constexpr CpuValueWidth = 75u;
		static uint32_t constexpr GpuNameWidth = 30u;
		static uint32_t constexpr GpuValueWidth = 75u;
		static uint32_t constexpr CategoryLineWidth = CategoryNameWidth
			+ CpuNameWidth
			+ CpuValueWidth
			+ GpuNameWidth
			+ GpuValueWidth;
		static uint32_t constexpr PanelBaseLevel = 65536u;

		castor::Mutex m_mutex;
		castor::PreciseTimer m_taskTimer{};
		castor::PreciseTimer m_frameTimer{};
		castor::PreciseTimer m_debugTimer{};
		castor::RawUniquePtr< MainDebugPanel > m_debugPanel;
		PanelCtrlRPtr m_passesContainer;
		CategoryOverlays m_renderPasses;
		castor::Array< castor::Nanoseconds, FrameSamplesCount > m_framesTimes{};
		uint32_t m_frameIndex{ 0 };
		uint64_t m_frameCount{ 0 };
		bool m_visible{ false };
		castor::Nanoseconds m_cpuTime{ 0 };
		castor::Nanoseconds m_gpuTime{ 0 };
		castor::Nanoseconds m_totalTime{ 0 };
		castor::Nanoseconds m_externalTime{ 0 };
		float m_fps{ 0.0f };
		float m_averageFps{ 0.0f };
		castor::Nanoseconds m_averageTime{ 0 };
		std::locale m_timesLocale{};
		RenderInfo m_renderInfo;
		bool m_dirty{ false };
	};
}

#endif
