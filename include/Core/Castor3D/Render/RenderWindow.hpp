/*
See LICENSE file in root folder
*/
#ifndef ___C3D_RenderWindow_H___
#define ___C3D_RenderWindow_H___

#include "RenderModule.hpp"
#include "Castor3D/Miscellaneous/MiscellaneousModule.hpp"

#include "Castor3D/Event/UserInput/UserInputListener.hpp"
#include "Castor3D/Render/Passes/CommandsSemaphore.hpp"
#include "Castor3D/Render/Passes/RenderQuad.hpp"
#include "Castor3D/Render/ToTexture/RenderToTextureModule.hpp"
#include "Castor3D/Shader/Ubos/CameraUbo.hpp"

#include <CastorUtils/Data/TextWriter.hpp>
#include <CastorUtils/Design/ArrayView.hpp>
#include <CastorUtils/Design/Named.hpp>
#include <CastorUtils/Graphics/Size.hpp>

#include <RenderGraph/ResourceHandler.hpp>

#include <ashespp/Core/Device.hpp>
#include <ashespp/Core/SwapChain.hpp>
#include <ashespp/Sync/Fence.hpp>
#include <ashespp/Sync/Semaphore.hpp>

#include <array>

#include <CastorUtils/Config/BeginExternHeaderGuard.hpp>
#include <atomic>
#include <mutex>
#include <CastorUtils/Config/EndExternHeaderGuard.hpp>

namespace castor3d
{
	class RenderWindow
		: public castor::OwnedBy< Engine >
		, public castor::Named
	{
	private:
		struct Configuration
		{
			castor::Point4f multiply;
			castor::Point4f add;
			castor::Point4f data;
		};

		struct RenderingResources
		{
			RenderingResources( ashes::SemaphorePtr imageAvailableSemaphore
				, ashes::SemaphorePtr finishedRenderingSemaphore
				, ashes::FencePtr fence
				, ashes::CommandBufferPtr commandBuffer
				, uint32_t imageIndex )
				: imageAvailableSemaphore{ castor::move( imageAvailableSemaphore ) }
				, finishedRenderingSemaphore{ castor::move( finishedRenderingSemaphore ) }
				, fence{ castor::move( fence ) }
				, commandBuffer{ castor::move( commandBuffer ) }
				, imageIndex{ imageIndex }
			{
			}

			~RenderingResources()noexcept
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
		using RenderingResourcesPtr = castor::RawUniquePtr< RenderingResources >;
		using RenderingResourcesArray = castor::Vector< RenderingResourcesPtr >;

	public:
		class EvtHandler
			: public MouseEventHandler
		{
		public:
			explicit EvtHandler( RenderWindow & window )
				: MouseEventHandler{ window.getName() }
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
				: UserInputListener{ engine, window.getName() + cuT( "UIListener" ) }
				, m_window{ &window }
			{
			}

		private:
			EventHandlerRPtr doGetMouseTargetableHandler( castor::Position const & position )const override
			{
				return m_handler;
			}

			/**@name General */
			//@{

			/**
			 *copydoc		castor3d::UserInputListener::doInitialise
			 */
			bool doInitialise()override
			{
				m_handler = doAddHandler( castor::makeUniqueDerived< EventHandler, EvtHandler >( *m_window ) );
				return true;
			}
			/**
			 *copydoc		castor3d::UserInputListener::doCleanup
			 */
			void doCleanup()override
			{
				doRemoveHandler( *m_handler );
			}

		private:
			RenderWindow * m_window;
			EventHandlerRPtr m_handler{};
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
		C3D_API ~RenderWindow()noexcept;
		/**
		 *\~english
		 *\brief		Initialises the render window with given RenderTarget.
		 *\~french
		 *\brief		Initialises la render window avec la RenderTarget donnée.
		 */
		C3D_API void initialise( RenderWindowDesc const & desc );
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
		 *\brief			Uploads overlays GPU buffers to VRAM.
		 *\param[in,out]	uploader	Receives the upload requests.
		 *\~french
		 *\brief			Met à jour tous les tampons GPU d'incrustations en VRAM.
		 *\param[in,out]	uploader	Reçoit les requêtes d'upload.
		 */
		C3D_API void upload( UploadData & uploader );
		/**
		 *\~english
		 *\brief			Renders one frame.
		 *\param[in,out]	info		Receives the render infos.
		 *\param[in]		signalOnly	\p true to only signal the semaphores.
		 *\param[in]		toWait		The semaphores to wait.
		 *\~french
		 *\brief			Renders one frame.
		 *\param[in,out]	info		Reçoit les infos de rendu.
		 *\param[in]		signalOnly	\p true pour ne faire que signaler les semaphores.
		 *\param[in]		toWait		Les sémaphores à attendre.
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
		C3D_API void setCamera( Camera & camera )const;
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
		C3D_API SceneRPtr getScene()const;
		/**
		 *\~english
		 *\return		The camera.
		 *\~french
		 *\return		La caméra.
		 */
		C3D_API CameraRPtr getCamera()const;
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
		C3D_API void setViewportType( ViewportType value )const;
		/**
		 *\~english
		 *\brief		Sets the Scene.
		 *\param[in]	value	The new value.
		 *\~french
		 *\brief		Définit la Scene.
		 *\param[in]	value	La nouvelle valeur.
		 */
		C3D_API void setScene( Scene & value )const;
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
		C3D_API void setStereo( bool stereo )const;
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
		C3D_API void setIntraOcularDistance( float intraOcularDistance )const;
		/**
		 *\~english
		 *\return		The window pixel format.
		 *\~french
		 *\return		Le format des pixels de la fenêtre.
		 */
		C3D_API VkFormat getPixelFormat()const;
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
		 *\brief		Destroys the loading scene.
		 *\~french
		 *\brief		Détruit la loading scene.
		 */
		C3D_API void destroyLoadingScreen();
		/**
		 *\~english
		 *\brief		Creates the loading scene.
		 *\~french
		 *\brief		Crée la loading scene.
		 */
		C3D_API void createLoadingScreen();
		/**
		*\~english
		*name
		*	Getters.
		*\~french
		*name
		*	Accesseurs.
		*/
		/**@{*/
		C3D_API GeometryRPtr getPickedGeometry()const;
		C3D_API Submesh const * getPickedSubmesh()const;
		C3D_API uint32_t getPickedFace()const;
		C3D_API ShadowMapLightTypeArray getShadowMaps()const;
		C3D_API ShadowBuffer * getShadowBuffer()const;

		uint32_t getIndex()const noexcept
		{
			return m_index;
		}

		FrameListenerRPtr getListener()const noexcept
		{
			return m_listener;
		}

		RenderDevice const & getDevice()const noexcept
		{
			return m_device;
		}

		RenderTargetRPtr getRenderTarget()const noexcept
		{
			return m_renderTarget;
		}

		bool isVSyncEnabled()const noexcept
		{
			return m_vsync;
		}

		bool isFullscreen()const noexcept
		{
			return m_fullscreen;
		}

		bool isSkipped()const noexcept
		{
			return m_skip;
		}

		castor::PxBufferBaseRPtr getSavedFrame()const noexcept
		{
			return m_saveBuffer.get();
		}

		ashes::Surface const & getSurface()const noexcept
		{
			CU_Require( m_surface );
			return *m_surface;
		}

		ProgressBar & getProgressBar()noexcept
		{
			CU_Require( m_progressBar );
			return *m_progressBar;
		}

		EventHandlerRPtr getEventHandler()const noexcept
		{
			return m_evtHandler.get();
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
		C3D_API void allowHdrSwapchain( bool value = true );

		void enableVSync( bool value )noexcept
		{
			m_vsync = value;
		}

		void setFullscreen( bool value )noexcept
		{
			m_fullscreen = value;
		}

		void enableSaveFrame( bool value = true )noexcept
		{
			m_toSave = value;
		}
		/**@}*/

	private:
		void doCreateRenderPass();
		void doDestroyRenderPass()noexcept;
		void doCreateProgram();
		void doDestroyProgram()noexcept;
		void doCreateSwapchain();
		void doDestroySwapchain()noexcept;
		void doCreateRenderingResources();
		void doDestroyRenderingResources()noexcept;
		ashes::ImageViewCRefArray doPrepareAttaches( size_t index );
		void doCreateFrameBuffers();
		void doDestroyFrameBuffers()noexcept;
		void doCreateLoadingScreen();
		void doDestroyLoadingScreen()noexcept;
		void doCreatePickingPass( QueueData const & queueData );
		void doDestroyPickingPass()noexcept;
		void doCreateRenderQuad();
		void doDestroyRenderQuad()noexcept;
		void doRecordCommandBuffer( uint32_t index );
		void doCreateCommandBuffers();
		void doDestroyCommandBuffers()noexcept;
		void doCreateIntermediateViews( QueueData const & queueData );
		void doDestroyIntermediateViews()noexcept;
		void doCreateSaveData();
		void doDestroySaveData()noexcept;
		void doResetSwapChain();
		void doResetSwapChainAndCommands();
		RenderingResources * doGetResources();
		crg::SemaphoreWaitArray doSubmitLoadingFrame( QueueData const & queueData
			, RenderingResources const & resources
			, LoadingScreen & loadingScreen
			, crg::Fence *& fence
			, crg::SemaphoreWaitArray toWait );
		void doPresentLoadingFrame( QueueData const & queueData
			, crg::Fence * fence
			, RenderingResources & resources
			, crg::SemaphoreWaitArray const & toWait );
		void doWaitFrame( QueueData const & queueData
			, crg::SemaphoreWaitArray const & toWait );
		void doSubmitFrame( QueueData const & queueData
			, RenderingResources const * resources
			, crg::SemaphoreWaitArray const & toWait );
		void doPresentFrame( QueueData const & queueData
			, RenderingResources * resources );
		bool doCheckNeedReset( VkResult errCode
			, bool acquisition
			, char const * const action );
		void doInitialiseTransferCommands( QueueData const & queueData
			, CommandsSemaphore & commands
			, uint32_t index );
		void doProcessDeviceLost();

	private:
		static uint32_t s_nbRenderWindows;
		castor::RawUniquePtr< EvtHandler > m_evtHandler;
		uint32_t m_index{};
		RenderDevice & m_device;
		ashes::SurfacePtr m_surface;
		QueuesData * m_queues{};
		QueueData const * m_reservedQueue{};
		ashes::CommandPoolPtr m_commandBufferPool;
		ashes::SwapChainPtr m_swapChain;
		castor::Vector< ashes::ImageViewArray > m_swapchainViews;
		VkFormat m_swapchainFormat;
		RenderingResourcesArray m_renderingResources;
		size_t m_resourceIndex{ 0u };
		crg::ResourcesCache m_resources;
		ashes::RenderPassPtr m_renderPass;
		ashes::BufferBasePtr m_snapshotBuffer;
		castor::ByteArrayView m_snapshotData;
		castor::Vector< CommandsSemaphore > m_transferCommands;
		castor::Vector< ashes::FrameBufferPtr > m_frameBuffers;
		castor::Vector< ashes::CommandBufferPtrArray > m_commandBuffers;
		ashes::PipelineShaderStageCreateInfoArray m_program;
		RenderQuadUPtr m_renderQuad;
		RenderTargetRPtr m_renderTarget{};
		FrameListenerRPtr m_listener{};
		bool m_vsync{ false };
		bool m_fullscreen{ false };
		castor::Size m_size;
		bool m_toSave{ false };
		VkFormat m_savedFormat{};
		mutable std::atomic_bool m_initialised{ false };
		mutable std::atomic_bool m_skip{ false };
		std::atomic_bool m_loading;
		castor::PxBufferBaseUPtr m_saveBuffer;
		PickingUPtr m_picking;
		castor::Position m_mousePosition;
		Texture3DTo2DUPtr m_texture3Dto2D;
		IntermediateView m_tex3DTo2DIntermediate;
		IntermediateViewArray m_intermediateBarrierViews;
		IntermediateViewArray m_intermediateSampledViews;
		UniformBufferOffsetT< Configuration > m_configUbo;
		ProgressBarUPtr m_progressBar;
		LoadingScreenUPtr m_loadingScreen;
		castor::Mutex m_renderMutex;
		bool m_allowHdrSwapchain{};
		bool m_hasHdrSupport{};
	};

	struct RootContext;

	struct WindowContext
	{
		RootContext * root{};
		RenderWindowDesc window{};
	};

	C3D_API Engine * getEngine( WindowContext const & context );
}

#endif
