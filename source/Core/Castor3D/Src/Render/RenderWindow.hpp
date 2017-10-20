/*
See LICENSE file in root folder
*/
#ifndef ___C3D_RENDER_WINDOW_H___
#define ___C3D_RENDER_WINDOW_H___

#include "Castor3DPrerequisites.hpp"
#include "Event/Frame/FrameListener.hpp"
#include "Miscellaneous/PickingPass.hpp"
#include "Miscellaneous/WindowHandle.hpp"

#include <Design/Named.hpp>
#include <Design/OwnedBy.hpp>
#include <Graphics/Size.hpp>

namespace castor3d
{
	/*!
	\author 	Sylvain DOREMUS
	\version	0.1
	\date		09/02/2010
	\~english
	\brief		Render window representation
	\remark		Manages a window where you can render a scene.
	\~french
	\brief		Implémentation d'une fenêtre de rendu
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
		\brief		RenderTarget loader
		\~english
		\brief		Loader de RenderTarget
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
			C3D_API explicit TextWriter( castor::String const & p_tabs );
			/**
			 *\~english
			 *\brief		Writes a render window into a text file
			 *\param[in]	p_window	the render window
			 *\param[in]	p_file		the file
			 *\~french
			 *\brief		Ecrit une fenêtre de rendu dans un fichier texte
			 *\param[in]	p_window	La fenêtre de rendu
			 *\param[in]	p_file		Le fichier
			 */
			C3D_API bool operator()( RenderWindow const & p_window, castor::TextFile & p_file )override;
		};

	public:
		/**
		 *\~english
		 *\brief		Constructor
		 *\param[in]	engine	The engine
		 *\param[in]	p_name		The window name
		 *\~french
		 *\brief		Constructor
		 *\param[in]	engine	Le moteur
		 *\param[in]	p_name		Le nom de la fenêtre
		 */
		C3D_API RenderWindow( castor::String const & p_name, Engine & engine );
		/**
		 *\~english
		 *\brief		Destructor
		 *\~french
		 *\brief		Destructeur
		 */
		C3D_API ~RenderWindow();
		/**
		 *\~english
		 *\brief		sets the handle, initialises the window.
		 *\param[in]	p_size		The window size.
		 *\param[in]	p_handle	The handle.
		 *\return		\p false if any problem occured
		 *\~french
		 *\brief		Définit l'identifiant de la fenêtre, initialise la fenêtre.
		 *\param[in]	p_size		Les dimensions de la fenêtre.
		 *\param[in]	p_handle	Le handle.
		 *\return		\p false si un problème quelconque a été rencontré.
		 */
		C3D_API bool initialise( castor::Size const & p_size, WindowHandle const & p_handle );
		/**
		 *\~english
		 *\brief		Cleans up the instance
		 *\~french
		 *\brief		Nettoie l'instance
		 */
		C3D_API void cleanup();
		/**
		 *\~english
		 *\brief		Renders one frame
		 *\param[in]	p_bForce		Forces the rendering
		 *\~english
		 *\brief		Renders one frame
		 *\param[in]	p_bForce		Dit si on force le rendu
		 */
		C3D_API void render( bool p_bForce );
		/**
		 *\~english
		 *\brief		Resizes the window
		 *\param[in]	x, y	The new size
		 *\~french
		 *\brief		Redimensionne la fenêtre
		 *\param[in]	x, y	Les nouvelles dimensions
		 */
		C3D_API void resize( int x, int y );
		/**
		 *\~english
		 *\brief		Resizes the window
		 *\param[in]	p_size	The new size
		 *\~french
		 *\brief		Redimensionne la fenêtre
		 *\param[in]	p_size	Les nouvelles dimensions
		 */
		C3D_API void resize( castor::Size const & p_size );
		/**
		 *\~english
		 *\brief		sets the camera
		 *\param[in]	p_pCamera	The camera
		 *\~french
		 *\brief		Définit la caméra
		 *\param[in]	p_pCamera	La caméra
		 */
		C3D_API void setCamera( CameraSPtr p_pCamera );
		/**
		 *\~english
		 *\brief		Changes fullscreen status from contex
		 *\param[in]	val	The new fullscreen status
		 *\~french
		 *\brief		Change le statut de plein écran à partir du contexte
		 *\param[in]	val	Le nouveau statut de plein écran
		 */
		C3D_API void updateFullScreen( bool val );
		/**
		 *\~english
		 *\brief		Retrieves the Scene
		 *\return		The Scene
		 *\~french
		 *\brief		Récupère la Scene
		 *\return		La Scene
		 */
		C3D_API SceneSPtr getScene()const;
		/**
		 *\~english
		 *\brief		Retrieves the Camera
		 *\return		The Camera
		 *\~french
		 *\brief		Récupère la Camera
		 *\return		La Camera
		 */
		C3D_API CameraSPtr getCamera()const;
		/**
		 *\~english
		 *\brief		Retrieves the ViewportType
		 *\return		The ViewportType
		 *\~french
		 *\brief		Récupère le ViewportType
		 *\return		Le ViewportType
		 */
		C3D_API ViewportType getViewportType()const;
		/**
		 *\~english
		 *\brief		sets the ViewportType
		 *\param[in]	val	The new ViewportType
		 *\~french
		 *\brief		Définit le ViewportType
		 *\param[in]	val	Le nouveau ViewportType
		 */
		C3D_API void setViewportType( ViewportType val );
		/**
		 *\~english
		 *\brief		sets the Scene
		 *\param[in]	p_scene	The new Scene
		 *\~french
		 *\brief		Définit la Scene
		 *\param[in]	p_scene	La nouvelle Scene
		 */
		C3D_API void setScene( SceneSPtr p_scene );
		/**
		 *\~english
		 *\brief		Retrieves the window dimensions
		 *\return		The window dimensions
		 *\~french
		 *\brief		Récupère les dimensions de la fenêtre
		 *\return		Les dimensions de la fenêtre
		 */
		C3D_API castor::Size getSize()const;
		/**
		 *\~english
		 *\brief		Retrieves the stereo status
		 *\return		\p true if stereo is used
		 *\~french
		 *\brief		Récupère le statut d'utilisation stéréo
		 *\return		\p true si le rendu stéréo est utilisé
		 */
		C3D_API bool isUsingStereo()const;
		/**
		 *\~english
		 *\brief		Defines the stereo status
		 *\param[in]	p_bStereo	\p true if stereo is to be used
		 *\~french
		 *\brief		Définit le statut d'utilisation stéréo
		 *\param[in]	p_bStereo	\p true si le rendu stéréo est à utiliser
		 */
		C3D_API void setStereo( bool p_bStereo );
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
		 *\brief		Defines the intra ocular distance
		 *\param[in]	p_rIao	The intra ocular distance
		 *\~french
		 *\brief		Définit la distance inter oculaire
		 *\param[in]	p_rIao	La distance inter oculaire
		 */
		C3D_API void setIntraOcularDistance( real p_rIao );
		/**
		 *\~english
		 *\brief		Retrieves the window pixel format
		 *\return		The window pixel format
		 *\~french
		 *\brief		Récupère le format des pixels de la fenêtre
		 *\return		Le format des pixels de la fenêtre
		 */
		C3D_API castor::PixelFormat getPixelFormat()const;
		/**
		 *\~english
		 *\brief		sets the window pixel format
		 *\param[in]	val	The new window pixel format
		 *\~french
		 *\brief		Définit le format des pixels de la fenêtre
		 *\param[in]	val	Le nouveau format des pixels de la fenêtre
		 */
		C3D_API void setPixelFormat( castor::PixelFormat val );
		/**
		 *\~english
		 *\brief		Retrieves the window index
		 *\return		The window index
		 *\~french
		 *\brief		Récupère l'index de la fenêtre
		 *\return		L'index de la fenêtre
		 */
		inline uint32_t getIndex()const
		{
			return m_index;
		}
		/**
		 *\~english
		 *\brief		Retrieves the window handle
		 *\return		The window handle
		 *\~french
		 *\brief		Récupère le handle de la fenêtre
		 *\return		Le handle de la fenêtre
		 */
		inline WindowHandle const & getHandle()const
		{
			return m_handle;
		}
		/**
		 *\~english
		 *\brief		Retrieves the intialisation status
		 *\return		The intialisation status
		 *\~french
		 *\brief		Récupère le statut de l'initialisation
		 *\return		Le statut de l'initialisation
		 */
		inline bool isInitialised()const
		{
			return m_initialised;
		}
		/**
		 *\~english
		 *\brief		Retrieves the FrameListener
		 *\return		The FrameListener
		 *\~french
		 *\brief		Récupère le FrameListener
		 *\return		Le FrameListener
		 */
		inline FrameListenerSPtr getListener()const
		{
			return m_wpListener.lock();
		}
		/**
		 *\~english
		 *\brief		Retrieves the Context
		 *\return		The Context
		 *\~french
		 *\brief		Récupère la Context
		 *\return		La Context
		 */
		inline ContextSPtr getContext()const
		{
			return m_context;
		}
		/**
		 *\~english
		 *\brief		Retrieves the render target
		 *\return		The value
		 *\~french
		 *\brief		Récupère la cible du rendu
		 *\return		La valeur
		 */
		inline RenderTargetSPtr getRenderTarget()const
		{
			return m_renderTarget.lock();
		}
		/**
		 *\~english
		 *\brief		sets the render target
		 *\param[in]	p_pTarget	The new value
		 *\~french
		 *\brief		Définit la cible du rendu
		 *\param[in]	p_pTarget	La nouvelle valeur
		 */
		inline void setRenderTarget( RenderTargetSPtr p_pTarget )
		{
			m_renderTarget = p_pTarget;
		}
		/**
		 *\~english
		 *\brief		sets the Context
		 *\param[in]	p_context	The new Context
		 *\~french
		 *\brief		Définit le Context
		 *\param[in]	p_context	Le nouveau Context
		 */
		inline void setContext( ContextSPtr p_context )
		{
			m_context = p_context;
		}
		/**
		 *\~english
		 *\brief		Tells the context is using vsync
		 *\~french
		 *\brief		Dit si le contexte utilise la vsync
		 */
		inline bool getVSync()const
		{
			return m_bVSync;
		}
		/**
		 *\~english
		 *\brief		Defines the vsync usage
		 *\param[in]	p_value	The usage
		 *\~french
		 *\brief		Définit l'utilisation de la vsync
		 *\param[in]	p_value	L'utilisation
		 */
		inline void setVSync( bool p_value )
		{
			m_bVSync = p_value;
		}
		/**
		 *\~english
		 *\brief		Tells the rendering is fullscreen
		 *\~french
		 *\brief		Dit si le rendu est en plein écran
		 */
		inline bool isFullscreen()const
		{
			return m_bFullscreen;
		}
		/**
		 *\~english
		 *\brief		Defines the fullscreen rendering status
		 *\param[in]	p_value	The status
		 *\~french
		 *\brief		Définit le statut du rendu plein écran
		 *\param[in]	p_value	Le statut
		 */
		inline void setFullscreen( bool p_value )
		{
			m_bFullscreen = p_value;
		}
		/**
		 *\~english
		 *\return		The window's back buffers.
		 *\~french
		 *\return		Les tampons de rendu de la fenêtre.
		 */
		BackBuffersSPtr getBackBuffers()const
		{
			return m_backBuffers;
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
		void doRender( WindowBuffer p_eTargetBuffer, TextureUnit const & p_texture );
		void doUpdateSize();

	private:
		//!\~english	Total number of render windows.
		//!\~french		Nombre total de fenêtres de rendu.
		static uint32_t s_nbRenderWindows;
		//!\~english	This window's index.
		//!\~french		Index de la fenêtre.
		uint32_t m_index;
		//!\~english	The handle of the display window.
		//!\~french		Handle de la fenêtre sustème.
		WindowHandle m_handle;
		//!\~english	The render target, which receives the main render.
		//!\~french		La render target, recevant le rendu principal.
		RenderTargetWPtr m_renderTarget;
		//!\~english	The events listener.
		//!\~french		Gestionnaire d'évènements.
		FrameListenerWPtr m_wpListener;
		//!\~english	Tells if the window is initalised.
		//!\~french		Dit si la fenêtre est initialisée.
		bool m_initialised{ false };
		//!\~english	The rendering context.
		//!\~french		Le contexte de rendu.
		ContextSPtr m_context;
		//!\~english	Tells VSync is activated.
		//!\~french		Dit si la VSync est activée.
		bool m_bVSync{ false };
		//!\~english	Tells fullscreen is activated.
		//!\~french		Dit si le rendu est en plein écran.
		bool m_bFullscreen{ false };
		//!\~english	The window's back buffers.
		//!\~french		Les tampons de rendu de la fenêtre.
		BackBuffersSPtr m_backBuffers;
		//!\~english	The window size.
		//!\~french		Les dimensions de la fenêtre.
		castor::Size m_size;
		//!\~english	Tells we need to save a frame.
		//!\~french		Dit si l'on veut sauvegarder une immage.
		bool m_toSave{ false };
		//!\~english	The pixel buffer holding the saved image.
		//!\~french		Le tampon de pixels contenant l'image sauvegardée.
		castor::PxBufferBaseSPtr m_saveBuffer;
		//!\~english	The picking pass.
		//!\~french		La passe de picking.
		PickingPassUPtr m_pickingPass;
	};
}

#endif
