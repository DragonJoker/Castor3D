/*
See LICENSE file in root folder
*/
#ifndef ___C3D_RenderWindow_H___
#define ___C3D_RenderWindow_H___

#include "RenderModule.hpp"
#include "Castor3D/Miscellaneous/MiscellaneousModule.hpp"

#include "Castor3D/Event/UserInput/UserInputListener.hpp"
#include "Castor3D/Overlay/OverlayRenderer.hpp"
#include "Castor3D/Render/Passes/CommandsSemaphore.hpp"
#include "Castor3D/Render/Passes/RenderQuad.hpp"
#include "Castor3D/Render/ToTexture/RenderToTextureModule.hpp"
#include "Castor3D/Shader/Ubos/MatrixUbo.hpp"

#include <CastorUtils/Data/TextWriter.hpp>
#include <CastorUtils/Design/ArrayView.hpp>
#include <CastorUtils/Design/Named.hpp>
#include <CastorUtils/Graphics/Size.hpp>

#include <ashespp/Core/Device.hpp>
#include <ashespp/Core/SwapChain.hpp>
#include <ashespp/Sync/Fence.hpp>
#include <ashespp/Sync/Semaphore.hpp>

#include <array>

#pragma warning( push )
#pragma warning( disable:4365 )
#include <atomic>
#pragma warning( pop )

namespace castor3d
{
	struct DebugConfig
	{
		uint32_t debugIndex{ 0u };
	};

	struct RenderWindowDesc
	{
		castor::String name;
		RenderTargetSPtr renderTarget;
		bool enableVSync{};
		bool fullscreen{};
	};

	class RenderWindow
		: public castor::OwnedBy< Engine >
		, public castor::Named
	{
	private:
		struct Configuration
		{
			castor::Point4f multiply;
			castor::Point4f add;
		};

		struct RenderingResources
		{
			RenderingResources( ashes::SemaphorePtr imageAvailableSemaphore
				, ashes::SemaphorePtr finishedRenderingSemaphore
				, ashes::FencePtr fence
				, ashes::CommandBufferPtr commandBuffer
				, uint32_t imageIndex )
				: imageAvailableSemaphore{ std::move( imageAvailableSemaphore ) }
				, finishedRenderingSemaphore{ std::move( finishedRenderingSemaphore ) }
				, fence{ std::move( fence ) }
				, commandBuffer{ std::move( commandBuffer ) }
				, imageIndex{ imageIndex }
			{
			}

			~RenderingResources()
			{
				imageAvailableSemaphore.reset();
				finishedRenderingSemaphore.reset();
				fence.reset();
				commandBuffer.reset();
			}

			ashes::SemaphorePtr imageAvailableSemaphore;
			ashes::SemaphorePtr finishedRenderingSemaphore;
			ashes::FencePtr fence;
			ashes::CommandBufferPtr commandBuffer;
			uint32_t imageIndex{ 0u };
		};
		using RenderingResourcesPtr = std::unique_ptr< RenderingResources >;
		using RenderingResourcesArray = std::vector< RenderingResourcesPtr >;

	public:
		class EvtHandler
			: public MouseEventHandler
		{
		public:
			explicit EvtHandler( RenderWindow & window )
				: MouseEventHandler{}
				, m_window{ &window }
			{
			}

		private:
			void doProcessMouseEvent( MouseEventSPtr event )override;

		private:
			RenderWindow * m_window;
		};

		class InputListener
			: public UserInputListener
		{
		public:
			InputListener( Engine & engine
				, RenderWindow & window )
				: UserInputListener{ engine, window.getName() + "UIListener" }
				, m_window{ &window }
			{
			}

		private:
			EventHandler * doGetMouseTargetableHandler( castor::Position const & position )const override
			{
				return m_window->getEventHandler().get();
			}

			/**@name General */
			//@{

			/**
			 *copydoc		castor3d::UserInputListener::Initialise
			 */
			bool doInitialise()override
			{
				doAddHandler( m_window->getEventHandler() );
				return true;
			}
			/**
			 *copydoc		castor3d::UserInputListener::Cleanup
			 */
			void doCleanup()override
			{
				doRemoveHandler( *m_window->getEventHandler() );
			}

		private:
			RenderWindow * m_window;
		};

	public:
		/**
		 *\~english
		 *\brief		Constructor
		 *\param[in]	engine	The engine.
		 *\param[in]	name	The window name.
		 *\param[in]	size	The window size.
		 *\param[in]	handle	The native window handle.
		 *\~french
		 *\brief		Constructor
		 *\param[in]	engine	Le moteur.
		 *\param[in]	name	Le nom de la fenêtre.
		 *\param[in]	size	Les dimensions de la fenêtre.
		 *\param[in]	handle	Le handle de la fenêtre native.
		 */
		C3D_API RenderWindow( castor::String const & name
			, Engine & engine
			, castor::Size const & size
			, ashes::WindowHandle handle );
		/**
		 *\~english
		 *\brief		Destructor
		 *\~french
		 *\brief		Destructeur
		 */
		C3D_API ~RenderWindow();
		/**
		 *\~english
		 *\brief		Initialises the render window with given RenderTarget.
		 *\~french
		 *\brief		Initialises la render window avec la RenderTarget donnée.
		 */
		C3D_API void initialise( RenderTargetSPtr value );
		/**
		 *\~english
		 *\brief		Cleans up the instance.
		 *\~french
		 *\brief		Nettoie l'instance.
		 */
		C3D_API void cleanup();
		/**
		 *\~english
		 *\brief			Updates the render window, CPU wise.
		 *\param[in, out]	updater	The update data.
		 *\~french
		 *\brief			Met à jour la render window, au niveau CPU.
		 *\param[in, out]	updater	Les données d'update.
		 */
		C3D_API void update( CpuUpdater & updater );
		/**
		 *\~english
		 *\brief			Updates the render window, GPU wise.
		 *\param[in, out]	updater	The update data.
		 *\~french
		 *\brief			Met à jour la render window, au niveau GPU.
		 *\param[in, out]	updater	Les données d'update.
		 */
		C3D_API void update( GpuUpdater & updater );
		/**
		 *\~english
		 *\brief		Renders one frame.
		 *\param[in]	signalOnly	\p true to only signal the semaphores.
		 *\~french
		 *\brief		Renders one frame.
		 *\param[in]	signalOnly	\p true pour ne faire que signaler les semaphores.
		 */
		C3D_API void render( RenderInfo & info
			, bool signalOnly
			, crg::SemaphoreWaitArray & toWait );
		/**
		 *\~english
		 *\brief		Resizes the window.
		 *\param[in]	x, y	The new size.
		 *\~french
		 *\brief		Redimensionne la fenêtre.
		 *\param[in]	x, y	Les nouvelles dimensions.
		 */
		C3D_API void resize( uint32_t x, uint32_t y );
		/**
		 *\~english
		 *\brief		Resizes the window.
		 *\param[in]	size	The new size.
		 *\~french
		 *\brief		Redimensionne la fenêtre.
		 *\param[in]	size	Les nouvelles dimensions.
		 */
		C3D_API void resize( castor::Size const & size );
		/**
		 *\~english
		 *\brief		Sets the camera.
		 *\param[in]	camera	The camera.
		 *\~french
		 *\brief		Définit la caméra.
		 *\param[in]	camera	La caméra.
		 */
		C3D_API void setCamera( CameraSPtr camera );
		/**
		 *\~english
		 *\brief		Changes fullscreen status.
		 *\param[in]	value	The new status.
		 *\~french
		 *\brief		Change le statut de plein écran.
		 *\param[in]	value	Le nouveau statut.
		 */
		C3D_API void enableFullScreen( bool value );
		/**
		 *\~english
		 *\return		The scene.
		 *\~french
		 *\return		La scène.
		 */
		C3D_API SceneSPtr getScene()const;
		/**
		 *\~english
		 *\return		The camera.
		 *\~french
		 *\return		La caméra.
		 */
		C3D_API CameraSPtr getCamera()const;
		/**
		 *\~english
		 *\return		The ViewportType.
		 *\~french
		 *\return		Le ViewportType.
		 */
		C3D_API ViewportType getViewportType()const;
		/**
		 *\~english
		 *\brief		Sets the ViewportType.
		 *\param[in]	value	The new value.
		 *\~french
		 *\brief		Définit le ViewportType.
		 *\param[in]	value	La nouvelle valeur.
		 */
		C3D_API void setViewportType( ViewportType value );
		/**
		 *\~english
		 *\brief		Sets the Scene.
		 *\param[in]	value	The new value.
		 *\~french
		 *\brief		Définit la Scene.
		 *\param[in]	value	La nouvelle valeur.
		 */
		C3D_API void setScene( SceneSPtr value );
		/**
		 *\~english
		 *\return		The window dimensions.
		 *\~french
		 *\return		Les dimensions de la fenêtre.
		 */
		C3D_API castor::Size getSize()const;
		/**
		 *\~english
		 *\return		\p true if stereo is used.
		 *\~french
		 *\return		\p true si le rendu stéréo est utilisé.
		 */
		C3D_API bool isUsingStereo()const;
		/**
		 *\~english
		 *\brief		Defines the stereo status.
		 *\param[in]	stereo	\p true if stereo is to be used.
		 *\~french
		 *\brief		Définit le statut d'utilisation stéréo.
		 *\param[in]	stereo	\p true si le rendu stéréo est à utiliser.
		 */
		C3D_API void setStereo( bool stereo );
		/**
		 *\~english
		 *\brief		Retrieves the intra ocular distance
		 *\return		The intra ocular distance
		 *\~french
		 *\brief		Récupère la distance inter oculaire
		 *\return		La distance inter oculaire
		 */
		C3D_API float getIntraOcularDistance()const;
		/**
		 *\~english
		 *\brief		Defines the intra ocular distance.
		 *\param[in]	intraOcularDistance	The intra ocular distance.
		 *\~french
		 *\brief		Définit la distance inter oculaire.
		 *\param[in]	intraOcularDistance	La distance inter oculaire.
		 */
		C3D_API void setIntraOcularDistance( float intraOcularDistance );
		/**
		 *\~english
		 *\return		The window pixel format.
		 *\~french
		 *\return		Le format des pixels de la fenêtre.
		 */
		C3D_API VkFormat getPixelFormat()const;
		/**
		 *\~english
		 *\brief		Adds a scene that can be picked.
		 *\param[in]	scene	The scene.
		 *\~french
		 *\brief		Ajoute une scène qui peut être pickée.
		 *\param[in]	scene	La scène.
		 */
		C3D_API void addPickingScene( Scene & scene );
		/**
		 *\~english
		 *\brief		Picks a geometry at given mouse position.
		 *\param[in]	position	The position in the pass.
		 *\return		PickNodeType::eNone if nothing was picked.
		 *\~french
		 *\brief		Sélectionne la géométrie à la position de souris donnée.
		 *\param[in]	position	La position dans la passe.
		 *\return		PickNodeType si rien n'a été pické.
		 */
		C3D_API PickNodeType pick( castor::Position const & position );
		/**
		 *\~english
		 *\return		The intermediate views used by this render window.
		 *\~french
		 *\return		Les vues intermédiaires utilisées par cette render window.
		 */
		C3D_API IntermediateViewArray const & listIntermediateViews()const;
		/**
		*\~english
		*name
		*	Getters.
		*\~french
		*name
		*	Accesseurs.
		*/
		/**@{*/
		C3D_API GeometrySPtr getPickedGeometry()const;
		C3D_API BillboardBaseSPtr getPickedBillboard()const;
		C3D_API SubmeshSPtr getPickedSubmesh()const;
		C3D_API uint32_t getPickedFace()const;
		C3D_API ShadowMapLightTypeArray getShadowMaps()const;

		uint32_t getIndex()const
		{
			return m_index;
		}

		FrameListenerSPtr getListener()const
		{
			return m_listener.lock();
		}

		RenderDevice const & getDevice()const
		{
			return m_device;
		}

		RenderTargetSPtr getRenderTarget()const
		{
			return m_renderTarget.lock();
		}

		bool isVSyncEnabled()const
		{
			return m_vsync;
		}

		bool isFullscreen()const
		{
			return m_fullscreen;
		}

		bool isSkipped()const
		{
			return m_skip;
		}

		castor::PxBufferBaseSPtr getSavedFrame()const
		{
			return m_saveBuffer;
		}

		ashes::Surface const & getSurface()const
		{
			CU_Require( m_surface );
			return *m_surface;
		}

		DebugConfig & getDebugConfig()
		{
			return m_debugConfig;
		}

		ProgressBar & getProgressBar()
		{
			CU_Require( m_progressBar );
			return *m_progressBar;
		}

		EventHandlerSPtr getEventHandler()
		{
			return m_evtHandler;
		}
		/**@}*/
		/**
		*\~english
		*name
		*	Mutators.
		*\~french
		*name
		*	Mutateurs.
		*/
		/**@{*/
		C3D_API void enableLoading();

		void enableVSync( bool value )
		{
			m_vsync = value;
		}

		void setFullscreen( bool value )
		{
			m_fullscreen = value;
		}

		void enableSaveFrame()
		{
			m_toSave = true;
		}
		/**@}*/

	private:
		void doCreateRenderPass();
		void doDestroyRenderPass();
		void doCreateProgram();
		void doDestroyProgram();
		void doCreateSwapchain();
		void doDestroySwapchain();
		void doCreateRenderingResources();
		void doDestroyRenderingResources();
		void doCreateFrameBuffers();
		void doDestroyFrameBuffers();
		void doCreateLoadingScreen();
		void doDestroyLoadingScreen();
		void doCreatePickingPass( QueueData const & queueData );
		void doDestroyPickingPass();
		void doCreateRenderQuad( QueueData const & queueData );
		void doDestroyRenderQuad();
		void doCreateCommandBuffers( QueueData const & queueData );
		void doDestroyCommandBuffers();
		void doCreateIntermediateViews( QueueData const & queueData );
		void doDestroyIntermediateViews();
		void doCreateSaveData( QueueData const & queueData );
		void doDestroySaveData();
		void doResetSwapChain();
		RenderingResources * doGetResources();
		crg::SemaphoreWaitArray doSubmitLoadingFrame( RenderingResources & resources
			, LoadingScreen & loadingScreen
			, crg::Fence *& fence
			, crg::SemaphoreWaitArray toWait );
		void doPresentLoadingFrame( crg::Fence * fence
			, RenderingResources & resources
			, crg::SemaphoreWaitArray toWait );
		void doWaitFrame( crg::SemaphoreWaitArray toWait );
		void doSubmitFrame( RenderingResources * resources
			, crg::SemaphoreWaitArray toWait );
		void doPresentFrame( RenderingResources * resources );
		bool doCheckNeedReset( VkResult errCode
			, bool acquisition
			, char const * const action );

	private:
		static uint32_t s_nbRenderWindows;
		std::shared_ptr< EvtHandler > m_evtHandler;
		uint32_t m_index{};
		RenderDevice const & m_device;
		ashes::SurfacePtr m_surface;
		QueuesData const * m_queues{};
		QueueData const * m_queue{};
		ashes::SwapChainPtr m_swapChain;
		ashes::ImageArray m_swapChainImages;
		ashes::ImageViewArray m_swapchainViews;
		RenderingResourcesArray m_renderingResources;
		size_t m_resourceIndex{ 0u };
		ashes::RenderPassPtr m_renderPass;
		ashes::BufferBasePtr m_stagingBuffer;
		castor::ArrayView< uint8_t > m_stagingData;
		std::vector< CommandsSemaphore > m_transferCommands;
		std::vector< ashes::FrameBufferPtr > m_frameBuffers;
		std::vector< ashes::CommandBufferPtrArray > m_commandBuffers;
		ashes::PipelineShaderStageCreateInfoArray m_program;
		RenderQuadUPtr m_renderQuad;
		RenderTargetWPtr m_renderTarget;
		FrameListenerWPtr m_listener;
		bool m_vsync{ false };
		bool m_fullscreen{ false };
		castor::Size m_size;
		bool m_toSave{ false };
		VkFormat m_savedFormat{};
		mutable std::atomic_bool m_initialised{ false };
		mutable std::atomic_bool m_skip{ false };
		std::atomic_bool m_loading;
		castor::PxBufferBaseSPtr m_saveBuffer;
		PickingSPtr m_picking;
		castor::Position m_mousePosition;
		Texture3DTo2DUPtr m_texture3Dto2D;
		IntermediateView m_tex3DTo2DIntermediate;
		IntermediateViewArray m_intermediates;
		IntermediateViewArray m_intermediateBarrierViews;
		IntermediateViewArray m_intermediateSampledViews;
		DebugConfig m_debugConfig;
		UniformBufferOffsetT< Configuration > m_configUbo;
		ProgressBarUPtr m_progressBar;
		LoadingScreenUPtr m_loadingScreen;
	};
}

#endif
