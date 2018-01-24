/*
See LICENSE file in root folder
*/
#ifndef ___C3D_RenderWindow_H___
#define ___C3D_RenderWindow_H___

#include "Event/Frame/FrameListener.hpp"
#include "Miscellaneous/PickingPass.hpp"
#include "RenderToTexture/RenderQuad.hpp"
#include "Shader/Ubos/MatrixUbo.hpp"

#include <Design/Named.hpp>
#include <Design/OwnedBy.hpp>
#include <Graphics/Size.hpp>

#include <Core/Device.hpp>
#include <Core/SwapChain.hpp>

namespace castor3d
{
	/*!
	\author 	Sylvain DOREMUS
	\version	0.1
	\date		09/02/2010
	\~english
	\brief		Render window representation.
	\remark		Manages a window where you can render a scene.
	\~french
	\brief		Implémentation d'une fenêtre de rendu.
	\remark		Gère une fenêtre dans laquelle une scène peut être rendue
	*/
	class RenderWindow
		: public castor::OwnedBy< Engine >
		, public castor::Named
	{
	public:
		/*!
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
		 *\param[in]	handle	The handle.
		 *\return		\p false if any problem occured
		 *\~french
		 *\brief		Définit l'identifiant de la fenêtre, initialise la fenêtre.
		 *\param[in]	size	Les dimensions de la fenêtre.
		 *\param[in]	handle	Le handle.
		 *\return		\p false si un problème quelconque a été rencontré.
		 */
		C3D_API bool initialise( castor::Size const & size
			, renderer::WindowHandle && handle );
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
		 *\~english
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
		C3D_API real getIntraOcularDistance()const;
		/**
		 *\~english
		 *\brief		Defines the intra ocular distance.
		 *\param[in]	intraOcularDistance	The intra ocular distance.
		 *\~french
		 *\brief		Définit la distance inter oculaire.
		 *\param[in]	intraOcularDistance	La distance inter oculaire.
		 */
		C3D_API void setIntraOcularDistance( real intraOcularDistance );
		/**
		 *\~english
		 *\return		The window pixel format.
		 *\~french
		 *\return		Le format des pixels de la fenêtre.
		 */
		C3D_API castor::PixelFormat getPixelFormat()const;
		/**
		 *\~english
		 *\brief		Sets the window pixel format.
		 *\param[in]	value	The new window pixel format.
		 *\~french
		 *\brief		Définit le format des pixels de la fenêtre.
		 *\param[in]	value	Le nouveau format des pixels de la fenêtre.
		 */
		C3D_API void setPixelFormat( castor::PixelFormat value );
		/**
		 *\~english
		 *\return		The window index.
		 *\~french
		 *\return		L'index de la fenêtre.
		 */
		inline uint32_t getIndex()const
		{
			return m_index;
		}
		/**
		 *\~english
		 *\return		The intialisation status.
		 *\~french
		 *\return		Le statut de l'initialisation.
		 */
		inline bool isInitialised()const
		{
			return m_initialised;
		}
		/**
		 *\~english
		 *\return		The FrameListener.
		 *\~french
		 *\return		Le FrameListener.
		 */
		inline FrameListenerSPtr getListener()const
		{
			return m_listener.lock();
		}
		/**
		 *\~english
		 *\return		The context.
		 *\~french
		 *\return		Le contexte.
		 */
		inline renderer::Device const & getDevice()const
		{
			REQUIRE( m_device );
			return *m_device;
		}
		/**
		 *\~english
		 *\return		The render target.
		 *\~french
		 *\return		La cible du rendu.
		 */
		inline RenderTargetSPtr getRenderTarget()const
		{
			return m_renderTarget.lock();
		}
		/**
		 *\~english
		 *\brief		Sets the render target.
		 *\param[in]	value	The new value.
		 *\~french
		 *\brief		Définit la cible du rendu.
		 *\param[in]	value	La nouvelle valeur.
		 */
		inline void setRenderTarget( RenderTargetSPtr value )
		{
			m_renderTarget = value;
		}
		/**
		 *\~english
		 *\brief		Sets the Context.
		 *\param[in]	value	The new value
		 *\~french
		 *\brief		Définit le Context.
		 *\param[in]	value	La nouvelle valeur.
		 */
		inline void setDevice( renderer::DevicePtr && value )
		{
			m_device = std::move( value );
		}
		/**
		 *\~english
		 *\brief		Tells the context is using vsync.
		 *\~french
		 *\brief		Dit si le contexte utilise la vsync.
		 */
		inline bool getVSync()const
		{
			return m_vsync;
		}
		/**
		 *\~english
		 *\brief		Defines the vsync usage.
		 *\param[in]	value	The usage.
		 *\~french
		 *\brief		Définit l'utilisation de la vsync.
		 *\param[in]	value	L'utilisation.
		 */
		inline void setVSync( bool value )
		{
			m_vsync = value;
		}
		/**
		 *\~english
		 *\brief		Tells the rendering is fullscreen.
		 *\~french
		 *\brief		Dit si le rendu est en plein écran.
		 */
		inline bool isFullscreen()const
		{
			return m_fullscreen;
		}
		/**
		 *\~english
		 *\brief		Defines the fullscreen rendering status.
		 *\param[in]	value	The status.
		 *\~french
		 *\brief		Définit le statut du rendu plein écran.
		 *\param[in]	value	Le statut.
		 */
		inline void setFullscreen( bool value )
		{
			m_fullscreen = value;
		}
		/**
		 *\~english
		 *\brief		Tells the next frame must be saved.
		 *\~french
		 *\brief		Dit que la prochaine image doit être enregistrée.
		 */
		inline void saveFrame()
		{
			m_toSave = true;
		}
		/**
		 *\~english
		 *\return		The saved image.
		 *\~french
		 *\return		L'image enregistrée.
		 */
		inline castor::PxBufferBaseSPtr getSavedFrame()const
		{
			return m_saveBuffer;
		}
		/**
		 *\~english
		 *\return		The picking pass.
		 *\~french
		 *\return		La passe de picking.
		 */
		inline PickingPass & getPickingPass()
		{
			REQUIRE( m_pickingPass );
			return *m_pickingPass;
		}

	private:
		void doCreateProgram();
		void doCreateSwapChainDependent();
		bool doPrepareFrames();
		void doResetSwapChain();

	private:
		static uint32_t s_nbRenderWindows;
		uint32_t m_index;
		MatrixUbo m_matrixUbo;
		renderer::DevicePtr m_device;
		renderer::SwapChainPtr m_swapChain;
		renderer::RenderPassPtr m_renderPass;
		std::vector< renderer::FrameBufferPtr > m_frameBuffers;
		renderer::CommandBufferPtrArray m_commandBuffers;
		renderer::SignalConnection< renderer::SwapChain::OnReset > m_swapChainReset;
		renderer::ShaderProgram * m_program{ nullptr };
		RenderQuadUPtr m_renderQuad;
		RenderTargetWPtr m_renderTarget;
		FrameListenerWPtr m_listener;
		bool m_initialised{ false };
		bool m_vsync{ false };
		bool m_fullscreen{ false };
		castor::Size m_size;
		bool m_toSave{ false };
		castor::PxBufferBaseSPtr m_saveBuffer;
		PickingPassUPtr m_pickingPass;
	};
}

#endif
