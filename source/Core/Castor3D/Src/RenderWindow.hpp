/*
This source file is part of Castor3D (http://castor3d.developpez.com/castor3d.htm)

This program is free software; you can redistribute it and/or modify it under
the terms of the GNU Lesser General Public License as published by the Free Software
Foundation; either version 2 of the License, or (at your option) any later
version.

This program is distributed in the hope that it will be useful, but WITHOUT
ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS
FOR A PARTICULAR PURPOSE. See the GNU Lesser General Public License for more details.

You should have received a copy of the GNU Lesser General Public License along with
the program; if not, write to the Free Software Foundation, Inc., 59 Temple
Place - Suite 330, Boston, MA 02111-1307, USA, or go to
http://www.gnu.org/copyleft/lesser.txt.
*/
#ifndef ___C3D_RENDER_WINDOW_H___
#define ___C3D_RENDER_WINDOW_H___

#include "Castor3DPrerequisites.hpp"
#include "FrameListener.hpp"
#include "BinaryParser.hpp"
#include "WindowHandle.hpp"

#include <OwnedBy.hpp>

namespace Castor3D
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
		: public Castor::OwnedBy< Engine >
		, public Castor::Named
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
		class TextLoader
			: public Castor::Loader< RenderWindow, Castor::eFILE_TYPE_TEXT, Castor::TextFile >
		{
		public:
			/**
			 *\~english
			 *\brief		Constructor
			 *\~french
			 *\brief		Constructeur
			 */
			C3D_API TextLoader( Castor::File::eENCODING_MODE p_encodingMode = Castor::File::eENCODING_MODE_ASCII );
			/**
			 *\~english
			 *\brief		Writes a render window into a text file
			 *\param[in]	p_target	the render window
			 *\param[in]	p_file		the file
			 *\~french
			 *\brief		Ecrit une fenêtre de rendu dans un fichier texte
			 *\param[in]	p_target	La fenêtre de rendu
			 *\param[in]	p_file		Le fichier
			 */
			C3D_API virtual bool operator()( RenderWindow const & p_window, Castor::TextFile & p_file );
		};
		/*!
		\author		Sylvain DOREMUS
		\date		08/04/2014
		\~english
		\brief		RenderWindow loader
		\~english
		\brief		Loader de RenderWindow
		*/
		class BinaryParser
			: public Castor3D::BinaryParser< RenderWindow >
		{
		public:
			/**
			 *\~english
			 *\brief		Constructor
			 *\param[in]	p_path	The current folder path
			 *\~french
			 *\brief		Constructeur
			 *\param[in]	p_path	Le chemin d'accès au dossier courant
			 */
			C3D_API BinaryParser( Castor::Path const & p_path );
			/**
			 *\~english
			 *\brief		Function used to fill the chunk from specific data
			 *\param[in]	p_obj	The object to write
			 *\param[out]	p_chunk	The chunk to fill
			 *\return		\p false if any error occured
			 *\~french
			 *\brief		Fonction utilisée afin de remplir le chunk de données spécifiques
			 *\param[in]	p_obj	L'objet à écrire
			 *\param[out]	p_chunk	Le chunk à remplir
			 *\return		\p false si une erreur quelconque est arrivée
			 */
			C3D_API virtual bool Fill( RenderWindow const & p_obj, BinaryChunk & p_chunk )const;
			/**
			 *\~english
			 *\brief		Function used to retrieve specific data from the chunk
			 *\param[out]	p_obj	The object to read
			 *\param[in]	p_chunk	The chunk containing data
			 *\return		\p false if any error occured
			 *\~french
			 *\brief		Fonction utilisée afin de récupérer des données spécifiques à partir d'un chunk
			 *\param[out]	p_obj	L'objet à lire
			 *\param[in]	p_chunk	Le chunk contenant les données
			 *\return		\p false si une erreur quelconque est arrivée
			 */
			C3D_API virtual bool Parse( RenderWindow & p_obj, BinaryChunk & p_chunk )const;
		};

	public:
		/**
		 *\~english
		 *\brief		Constructor
		 *\param[in]	p_engine	The engine
		 *\~french
		 *\brief		Constructor
		 *\param[in]	p_engine	Le moteur
		 */
		C3D_API RenderWindow( Engine & p_engine, Castor::String const & p_name );
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
		 *\param[in]	p_size		The window size.
		 *\param[in]	p_handle	The handle.
		 *\return		\p false if any problem occured
		 *\~french
		 *\brief		Définit l'identifiant de la fenêtre, initialise la fenêtre.
		 *\param[in]	p_size		Les dimensions de la fenêtre.
		 *\param[in]	p_handle	Le handle.
		 *\return		\p false si un problème quelconque a été rencontré.
		 */
		C3D_API bool Initialise( Castor::Size const & p_size, WindowHandle const & p_handle );
		/**
		 *\~english
		 *\brief		Cleans up the instance
		 *\~french
		 *\brief		Nettoie l'instance
		 */
		C3D_API void Cleanup();
		/**
		 *\~english
		 *\brief		Renders one frame
		 *\param[in]	p_bForce		Forces the rendering
		 *\~english
		 *\brief		Renders one frame
		 *\param[in]	p_bForce		Dit si on force le rendu
		 */
		C3D_API void Render( bool p_bForce );
		/**
		 *\~english
		 *\brief		Resizes the window
		 *\param[in]	x, y	The new size
		 *\~french
		 *\brief		Redimensionne la fenêtre
		 *\param[in]	x, y	Les nouvelles dimensions
		 */
		C3D_API void Resize( int x, int y );
		/**
		 *\~english
		 *\brief		Resizes the window
		 *\param[in]	p_size	The new size
		 *\~french
		 *\brief		Redimensionne la fenêtre
		 *\param[in]	p_size	Les nouvelles dimensions
		 */
		C3D_API void Resize( Castor::Size const & p_size );
		/**
		 *\~english
		 *\brief		Sets the camera
		 *\param[in]	p_pCamera	The camera
		 *\~french
		 *\brief		Définit la caméra
		 *\param[in]	p_pCamera	La caméra
		 */
		C3D_API void SetCamera( CameraSPtr p_pCamera );
		/**
		 *\~english
		 *\brief		Retrieves the multisampling status
		 *\return		The multisampling status
		 *\~french
		 *\brief		Récupère le statut de multisampling
		 *\return		Le statut de multisampling
		 */
		C3D_API bool IsMultisampling()const;
		/**
		 *\~english
		 *\brief		Retrieves the samples count
		 *\return		The samples count
		 *\~french
		 *\brief		Récupère le nombre de samples
		 *\return		Le nombre de samples
		 */
		C3D_API int32_t GetSamplesCount()const;
		/**
		 *\~english
		 *\brief		Changes fullscreen status from contex
		 *\param[in]	val	The new fullscreen status
		 *\~french
		 *\brief		Change le statut de plein écran à partir du contexte
		 *\param[in]	val	Le nouveau statut de plein écran
		 */
		C3D_API void UpdateFullScreen( bool val );
		/**
		 *\~english
		 *\brief		Sets the samples count
		 *\param[in]	val	The new samples count
		 *\~french
		 *\brief		Définit le nombre de samples
		 *\param[in]	val	Le nouveau nombre de samples
		 */
		C3D_API void SetSamplesCount( int32_t val );
		/**
		 *\~english
		 *\brief		Retrieves the Scene
		 *\return		The Scene
		 *\~french
		 *\brief		Récupère la Scene
		 *\return		La Scene
		 */
		C3D_API SceneSPtr GetScene()const;
		/**
		 *\~english
		 *\brief		Retrieves the Camera
		 *\return		The Camera
		 *\~french
		 *\brief		Récupère la Camera
		 *\return		La Camera
		 */
		C3D_API CameraSPtr GetCamera()const;
		/**
		 *\~english
		 *\brief		Retrieves the eVIEWPORT_TYPE
		 *\return		The eVIEWPORT_TYPE
		 *\~french
		 *\brief		Récupère le eVIEWPORT_TYPE
		 *\return		Le eVIEWPORT_TYPE
		 */
		C3D_API eVIEWPORT_TYPE GetViewportType()const;
		/**
		 *\~english
		 *\brief		Sets the eVIEWPORT_TYPE
		 *\param[in]	val	The new eVIEWPORT_TYPE
		 *\~french
		 *\brief		Définit le eVIEWPORT_TYPE
		 *\param[in]	val	Le nouveau eVIEWPORT_TYPE
		 */
		C3D_API void SetViewportType( eVIEWPORT_TYPE val );
		/**
		 *\~english
		 *\brief		Sets the Scene
		 *\param[in]	p_scene	The new Scene
		 *\~french
		 *\brief		Définit la Scene
		 *\param[in]	p_scene	La nouvelle Scene
		 */
		C3D_API void SetScene( SceneSPtr p_scene );
		/**
		 *\~english
		 *\brief		Retrieves the window dimensions
		 *\return		The window dimensions
		 *\~french
		 *\brief		Récupère les dimensions de la fenêtre
		 *\return		Les dimensions de la fenêtre
		 */
		C3D_API Castor::Size GetSize()const;
		/**
		 *\~english
		 *\brief		Retrieves the stereo status
		 *\return		\p true if stereo is used
		 *\~french
		 *\brief		Récupère le statut d'utilisation stéréo
		 *\return		\p true si le rendu stéréo est utilisé
		 */
		C3D_API bool IsUsingStereo()const;
		/**
		 *\~english
		 *\brief		Defines the stereo status
		 *\param[in]	p_bStereo	\p true if stereo is to be used
		 *\~french
		 *\brief		Définit le statut d'utilisation stéréo
		 *\param[in]	p_bStereo	\p true si le rendu stéréo est à utiliser
		 */
		C3D_API void SetStereo( bool p_bStereo );
		/**
		 *\~english
		 *\brief		Retrieves the intra ocular distance
		 *\return		The intra ocular distance
		 *\~french
		 *\brief		Récupère la distance inter oculaire
		 *\return		La distance inter oculaire
		 */
		C3D_API real GetIntraOcularDistance()const;
		/**
		 *\~english
		 *\brief		Defines the intra ocular distance
		 *\param[in]	p_rIao	The intra ocular distance
		 *\~french
		 *\brief		Définit la distance inter oculaire
		 *\param[in]	p_rIao	La distance inter oculaire
		 */
		C3D_API void SetIntraOcularDistance( real p_rIao );
		/**
		 *\~english
		 *\brief		Retrieves the window pixel format
		 *\return		The window pixel format
		 *\~french
		 *\brief		Récupère le format des pixels de la fenêtre
		 *\return		Le format des pixels de la fenêtre
		 */
		C3D_API Castor::ePIXEL_FORMAT GetPixelFormat()const;
		/**
		 *\~english
		 *\brief		Sets the window pixel format
		 *\param[in]	val	The new window pixel format
		 *\~french
		 *\brief		Définit le format des pixels de la fenêtre
		 *\param[in]	val	Le nouveau format des pixels de la fenêtre
		 */
		C3D_API void SetPixelFormat( Castor::ePIXEL_FORMAT val );
		/**
		 *\~english
		 *\brief		Retrieves the window depth format
		 *\return		The window depth format
		 *\~french
		 *\brief		Récupère le format de profondeur de la fenêtre
		 *\return		Le format de profondeur de la fenêtre
		 */
		C3D_API Castor::ePIXEL_FORMAT GetDepthFormat()const;
		/**
		 *\~english
		 *\brief		Sets the window depth format
		 *\param[in]	val	The new window depth format
		 *\~french
		 *\brief		Définit le format de profondeur de la fenêtre
		 *\param[in]	val	Le nouveau format de profondeur de la fenêtre
		 */
		C3D_API void SetDepthFormat( Castor::ePIXEL_FORMAT val );
		/**
		 *\~english
		 *\brief		Retrieves the window index
		 *\return		The window index
		 *\~french
		 *\brief		Récupère l'index de la fenêtre
		 *\return		L'index de la fenêtre
		 */
		inline uint32_t GetIndex()const
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
		inline WindowHandle const & GetHandle()const
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
		inline bool IsInitialised()const
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
		inline FrameListenerSPtr GetListener()const
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
		inline ContextSPtr GetContext()const
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
		inline RenderTargetSPtr GetRenderTarget()const
		{
			return m_renderTarget.lock();
		}
		/**
		 *\~english
		 *\brief		Sets the render target
		 *\param[in]	p_pTarget	The new value
		 *\~french
		 *\brief		Définit la cible du rendu
		 *\param[in]	p_pTarget	La nouvelle valeur
		 */
		inline void SetRenderTarget( RenderTargetSPtr p_pTarget )
		{
			m_renderTarget = p_pTarget;
		}
		/**
		 *\~english
		 *\brief		Sets the Context
		 *\param[in]	p_context	The new Context
		 *\~french
		 *\brief		Définit le Context
		 *\param[in]	p_context	Le nouveau Context
		 */
		inline void SetContext( ContextSPtr p_context )
		{
			m_context = p_context;
		}
		/**
		 *\~english
		 *\brief		Tells the context is using vsync
		 *\~french
		 *\brief		Dit si le contexte utilise la vsync
		 */
		inline bool GetVSync()const
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
		inline void SetVSync( bool p_value )
		{
			m_bVSync = p_value;
		}
		/**
		 *\~english
		 *\brief		Tells the rendering is fullscreen
		 *\~french
		 *\brief		Dit si le rendu est en plein écran
		 */
		inline bool IsFullscreen()const
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
		inline void SetFullscreen( bool p_value )
		{
			m_bFullscreen = p_value;
		}
		/**
		 *\~english
		 *\return		The window's back buffers.
		 *\~french
		 *\return		Les tampons de rendu de la fenêtre.
		 */
		BackBuffersSPtr GetBackBuffers()const
		{
			return m_backBuffers;
		}
		/**
		 *\~english
		 *\brief		Tells the next frame must be saved.
		 *\~french
		 *\brief		Dit que la prochaine image doit être enregistrée.
		 */
		inline void SaveFrame()
		{
			m_toSave = true;
		}
		/**
		 *\~english
		 *\return		The saved image.
		 *\~french
		 *\return		L'image enregistrée.
		 */
		inline Castor::PxBufferBaseSPtr GetSavedFrame()const
		{
			return m_saveBuffer;
		}

	private:
		void DoRender( eBUFFER p_eTargetBuffer, TextureUnit const & p_texture );
		void DoUpdateSize();

	private:
		//!\~english Total number of render windows	\~french Nombre total de fenêtres de rendu
		static uint32_t s_nbRenderWindows;
		//!\~english This window's index	\~french Index de la fenêtre
		uint32_t m_index;
		//!\~english The handle of the display window	\~french Handle de la fenêtre sustème
		WindowHandle m_handle;
		//!\~english The render target, which receives the main render	\~french La render target, recevant le rendu principal
		RenderTargetWPtr m_renderTarget;
		//!\~english The events listener	\~french Gestionnaire d'évènements
		FrameListenerWPtr m_wpListener;
		//!\~english Tells if the window is initalised	\~french Dit si la fenêtre est initialisée
		bool m_initialised;
		//!\~english The rendering context	\~french Le contexte de rendu
		ContextSPtr m_context;
		//!\~english Tells VSync is activated	\~french Dit si la VSync est activée
		bool m_bVSync;
		//!\~english Tells fullscreen is activated	\~french Dit si le rendu est en plein écran
		bool m_bFullscreen;
		//!\~english Depth and stencil buffers states	\~french Etats des tampons de profondeur et stencil
		DepthStencilStateWPtr m_wpDepthStencilState;
		//!\~english Rasteriser states	\~french Etats du rasteriser
		RasteriserStateWPtr m_wpRasteriserState;
		//!\~english The window 's back buffers.	\~french Les tampons de rendu de la fenêtre.
		BackBuffersSPtr m_backBuffers;
		//!\~english The window size.	\~french Les dimensions de la fenêtre.
		Castor::Size m_size;
		//!\~english  Tells we need to save a frame.	\~english Dit si l'on veut sauvegarder une immage.
		bool m_toSave = false;
		//!\~english  The pixel buffer holding the saved image.	\~english Le tampon de pixels contenant l'image sauvegardée.
		Castor::PxBufferBaseSPtr m_saveBuffer;
	};
}

#endif
