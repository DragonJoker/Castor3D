/*
See LICENSE file in root folder
*/
#ifndef ___C3D_RenderWindow_H___
#define ___C3D_RenderWindow_H___

#include "RenderModule.hpp"
#include "Castor3D/Overlay/OverlayRenderer.hpp"

#include "Castor3D/Event/UserInput/UserInputListener.hpp"
#include "Castor3D/Render/ToTexture/RenderQuad.hpp"
#include "Castor3D/Shader/Ubos/MatrixUbo.hpp"

#include <CastorUtils/Data/TextWriter.hpp>
#include <CastorUtils/Design/Named.hpp>
#include <CastorUtils/Graphics/Size.hpp>

#include <ashespp/Core/Device.hpp>
#include <ashespp/Core/SwapChain.hpp>
#include <ashespp/Sync/Fence.hpp>
#include <ashespp/Sync/Semaphore.hpp>

namespace castor3d
{
	class RenderWindow
		: public castor::OwnedBy< Engine >
		, public castor::Named
		, public MouseEventHandler
	{
	private:
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
		/**
		\author		Sylvain DOREMUS
		\date		14/02/2010
		\~english
		\brief		RenderTarget loader.
		\~english
		\brief		Loader de RenderTarget.
		*/
		class TextWriter
			: public castor::TextWriter< RenderWindow >
		{
		public:
			/**
			 *\~english
			 *\brief		Constructor
			 *\~french
			 *\brief		Constructeur
			 */
			C3D_API explicit TextWriter( castor::String const & tabs );
			/**
			 *\~english
			 *\brief		Writes a render window into a text file.
			 *\param[in]	window	the render window.
			 *\param[in]	file	the file.
			 *\~french
			 *\brief		Ecrit une fenêtre de rendu dans un fichier texte.
			 *\param[in]	window	La fenêtre de rendu.
			 *\param[in]	file	Le fichier.
			 */
			C3D_API bool operator()( RenderWindow const & window, castor::TextFile & file )override;
		};

		class InputListener
			: public UserInputListener
		{
		public:
			InputListener( Engine & engine
				, RenderWindowSPtr window )
				: UserInputListener{ engine, window->getName() + "UIListener" }
				, m_window{ std::move( window ) }
			{
			}

		private:
			EventHandlerSPtr doGetMouseTargetableHandler( castor::Position const & position )const override
			{
				return m_window;
			}

			/**@name General */
			//@{

			/**
			 *copydoc		castor3d::UserInputListener::Initialise
			 */
			bool doInitialise()override
			{
				doAddHandler( m_window );
				return true;
			}
			/**
			 *copydoc		castor3d::UserInputListener::Cleanup
			 */
			void doCleanup()override
			{
				doRemoveHandler( m_window );
			}

		private:
			RenderWindowSPtr m_window;
		};

	public:
		/**
		 *\~english
		 *\brief		Constructor
		 *\param[in]	engine	The engine.
		 *\param[in]	name	The window name.
		 *\~french
		 *\brief		Constructor
		 *\param[in]	engine	Le moteur.
		 *\param[in]	name	Le nom de la fenêtre.
		 */
		C3D_API RenderWindow( castor::String const & name
			, Engine & engine );
		/**
		 *\~english
		 *\brief		Destructor
		 *\~french
		 *\brief		Destructeur
		 */
		C3D_API ~RenderWindow();
		/**
		 *\~english
		 *\brief		Sets the handle, initialises the window.
		 *\param[in]	size	The window size.
		 *\param[in]	handle	The native window handle.
		 *\return		\p false if any problem occured
		 *\~french
		 *\brief		Définit l'identifiant de la fenêtre, initialise la fenêtre.
		 *\param[in]	size	Les dimensions de la fenêtre.
		 *\param[in]	handle	Le handle de la fenêtre native.
		 *\return		\p false si un problème quelconque a été rencontré.
		 */
		C3D_API bool initialise( castor::Size const & size
			, ashes::WindowHandle handle );
		/**
		 *\~english
		 *\brief		Cleans up the instance.
		 *\~french
		 *\brief		Nettoie l'instance.
		 */
		C3D_API void cleanup();
		/**
		 *\~english
		 *\brief		Renders one frame.
		 *\param[in]	force	Forces the rendering.
		 *\~french
		 *\brief		Renders one frame.
		 *\param[in]	force	Dit si on force le rendu.
		 */
		C3D_API void render( bool force );
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
		 *\brief		Sets the window pixel format.
		 *\param[in]	value	The new window pixel format.
		 *\~french
		 *\brief		Définit le format des pixels de la fenêtre.
		 *\param[in]	value	Le nouveau format des pixels de la fenêtre.
		 */
		C3D_API void setPixelFormat( VkFormat value );
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
		*name
		*	Getters.
		*\~french
		*name
		*	Accesseurs.
		*/
		/**@{*/
		inline uint32_t getIndex()const
		{
			return m_index;
		}

		inline bool isInitialised()const
		{
			return m_initialised;
		}

		inline FrameListenerSPtr getListener()const
		{
			return m_listener.lock();
		}

		inline RenderDevice const & getDevice()const
		{
			CU_Require( m_device );
			return *m_device;
		}

		inline RenderTargetSPtr getRenderTarget()const
		{
			return m_renderTarget.lock();
		}

		inline bool isVSyncEnabled()const
		{
			return m_vsync;
		}

		inline bool isFullscreen()const
		{
			return m_fullscreen;
		}

		inline castor::PxBufferBaseSPtr getSavedFrame()const
		{
			return m_saveBuffer;
		}

		C3D_API GeometrySPtr getPickedGeometry()const;
		C3D_API BillboardBaseSPtr getPickedBillboard()const;
		C3D_API SubmeshSPtr getPickedSubmesh()const;
		C3D_API uint32_t getPickedFace()const;
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
		inline void setRenderTarget( RenderTargetSPtr value )
		{
			m_renderTarget = value;
		}

		inline void setDevice( RenderDeviceSPtr value )
		{
			m_device = std::move( value );
		}

		inline void enableVSync( bool value )
		{
			m_vsync = value;
		}

		inline void setFullscreen( bool value )
		{
			m_fullscreen = value;
		}

		inline void enableSaveFrame()
		{
			m_toSave = true;
		}
		/**@}*/

	private:
		void doCreateSwapchain();
		void doCreateRenderingResources();
		void doCreateRenderPass();
		void doCreateCommandBuffers();
		void doCreateFrameBuffers();
		ashes::ImageViewCRefArray doPrepareAttaches( uint32_t backBuffer );
		void doCreateProgram();
		void doCreateSwapChainDependent();
		bool doPrepareFrames();
		void doResetSwapChain();
		RenderingResources * getResources();
		bool doCheckNeedReset( VkResult errCode
			, bool acquisition
			, char const * const action );
		void doCleanup( bool enableDevice );

		void doProcessMouseEvent( MouseEventSPtr event )override;

	private:
		static uint32_t s_nbRenderWindows;
		uint32_t m_index;
		RenderDeviceSPtr m_device;
		ashes::Surface const * m_surface;
		ashes::SwapChainPtr m_swapChain;
		ashes::ImageArray m_swapChainImages;
		ashes::CommandPoolPtr m_commandPool;
		RenderingResourcesArray m_renderingResources;
		size_t m_resourceIndex{ 0u };
		ashes::RenderPassPtr m_renderPass;
		ashes::StagingTexturePtr m_stagingTexture;
		ashes::CommandBufferPtr m_transferCommandBuffer;
		ashes::ImageViewArray m_views;
		std::vector< ashes::FrameBufferPtr > m_frameBuffers;
		ashes::CommandBufferPtrArray m_commandBuffers;
		ashes::PipelineShaderStageCreateInfoArray m_program;
		RenderQuadUPtr m_renderQuad;
		RenderTargetWPtr m_renderTarget;
		OverlayRendererSPtr m_overlayRenderer;
		FrameListenerWPtr m_listener;
		bool m_initialised{ false };
		bool m_vsync{ false };
		bool m_fullscreen{ false };
		castor::Size m_size;
		bool m_toSave{ false };
		bool m_dirty{ true };
		castor::PxBufferBaseSPtr m_saveBuffer;
		PickingPassSPtr m_pickingPass;
		castor::Position m_mousePosition;
	};
}

#endif
