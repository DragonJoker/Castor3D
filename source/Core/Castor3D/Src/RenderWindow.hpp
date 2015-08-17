﻿/*
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

#pragma warning( push )
#pragma warning( disable:4251 )
#pragma warning( disable:4275 )

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
	class C3D_API RenderWindow
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
		class C3D_API TextLoader
			: public Castor::Loader< RenderWindow, Castor::eFILE_TYPE_TEXT, Castor::TextFile >
			, public Castor::NonCopyable
		{
		public:
			/**
			 *\~english
			 *\brief		Constructor
			 *\~french
			 *\brief		Constructeur
			 */
			TextLoader( Castor::File::eENCODING_MODE p_eEncodingMode = Castor::File::eENCODING_MODE_ASCII );
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
			virtual bool operator()( RenderWindow const & p_window, Castor::TextFile & p_file );
		};
		/*!
		\author		Sylvain DOREMUS
		\date		08/04/2014
		\~english
		\brief		RenderWindow loader
		\~english
		\brief		Loader de RenderWindow
		*/
		class C3D_API BinaryParser
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
			BinaryParser( Castor::Path const & p_path );
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
			virtual bool Fill( RenderWindow const & p_obj, BinaryChunk & p_chunk )const;
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
			virtual bool Parse( RenderWindow & p_obj, BinaryChunk & p_chunk )const;
		};

	public:
		/**
		 *\~english
		 *\brief		Constructor
		 *\param[in]	p_pRoot			The engine
		 *\~french
		 *\brief		Constructor
		 *\param[in]	p_pRoot			Le moteur
		 */
		RenderWindow( Engine * p_pRoot );
		/**
		 *\~english
		 *\brief		Destructor
		 *\~french
		 *\brief		Destructeur
		 */
		virtual ~RenderWindow();
		/**
		 *\~english
		 *\brief		Sets the handle, initialises the renderer
		 *\param[in]	p_handle	The handle
		 *\return		\p false if any problem occured
		 *\~french
		 *\brief		Définit le handle, initialise le renderer
		 *\param[in]	p_handle	Le handle
		 *\return		\p false si un problème quelconque a été rencontré
		 */
		bool Initialise( WindowHandle const & p_handle );
		/**
		 *\~english
		 *\brief		Cleans up the instance
		 *\~french
		 *\brief		Nettoie l'instance
		 */
		void Cleanup();
		/**
		 *\~english
		 *\brief		Renders one frame
		 *\param[in]	p_bForce		Forces the rendering
		 *\~english
		 *\brief		Renders one frame
		 *\param[in]	p_bForce		Dit si on force le rendu
		 */
		virtual void RenderOneFrame( bool p_bForce );
		/**
		 *\~english
		 *\brief		Resizes the window
		 *\param[in]	x, y	The new size
		 *\~french
		 *\brief		Redimensionne la fenêtre
		 *\param[in]	x, y	Les nouvelles dimensions
		 */
		void Resize( int x, int y );
		/**
		 *\~english
		 *\brief		Resizes the window
		 *\param[in]	p_size	The new size
		 *\~french
		 *\brief		Redimensionne la fenêtre
		 *\param[in]	p_size	Les nouvelles dimensions
		 */
		void Resize( Castor::Size const & p_size );
		/**
		 *\~english
		 *\brief		Sets the camera
		 *\param[in]	p_pCamera	The camera
		 *\~french
		 *\brief		Définit la caméra
		 *\param[in]	p_pCamera	La caméra
		 */
		virtual void SetCamera( CameraSPtr p_pCamera );
		/**
		 *\~english
		 *\brief		Retrieves the multisampling status
		 *\return		The multisampling status
		 *\~french
		 *\brief		Récupère le statut de multisampling
		 *\return		Le statut de multisampling
		 */
		bool IsMultisampling()const;
		/**
		 *\~english
		 *\brief		Retrieves the samples count
		 *\return		The samples count
		 *\~french
		 *\brief		Récupère le nombre de samples
		 *\return		Le nombre de samples
		 */
		int32_t GetSamplesCount()const;
		/**
		 *\~english
		 *\brief		Changes fullscreen status from contex
		 *\param[in]	val	The new fullscreen status
		 *\~french
		 *\brief		Change le statut de plein écran à partir du contexte
		 *\param[in]	val	Le nouveau statut de plein écran
		 */
		void UpdateFullScreen( bool val );
		/**
		 *\~english
		 *\brief		Sets the samples count
		 *\param[in]	val	The new samples count
		 *\~french
		 *\brief		Définit le nombre de samples
		 *\param[in]	val	Le nouveau nombre de samples
		 */
		void SetSamplesCount( int32_t val );
		/**
		 *\~english
		 *\brief		Retrieves the Scene
		 *\return		The Scene
		 *\~french
		 *\brief		Récupère la Scene
		 *\return		La Scene
		 */
		SceneSPtr GetScene()const;
		/**
		 *\~english
		 *\brief		Retrieves the Camera
		 *\return		The Camera
		 *\~french
		 *\brief		Récupère la Camera
		 *\return		La Camera
		 */
		CameraSPtr GetCamera()const;
		/**
		 *\~english
		 *\brief		Retrieves the eTOPOLOGY
		 *\return		The eTOPOLOGY
		 *\~french
		 *\brief		Récupère le eTOPOLOGY
		 *\return		Le eTOPOLOGY
		 */
		eTOPOLOGY GetPrimitiveType()const;
		/**
		 *\~english
		 *\brief		Retrieves the eVIEWPORT_TYPE
		 *\return		The eVIEWPORT_TYPE
		 *\~french
		 *\brief		Récupère le eVIEWPORT_TYPE
		 *\return		Le eVIEWPORT_TYPE
		 */
		eVIEWPORT_TYPE GetViewportType()const;
		/**
		 *\~english
		 *\brief		Sets the eTOPOLOGY
		 *\param[in]	val	The new eTOPOLOGY
		 *\~french
		 *\brief		Définit le eTOPOLOGY
		 *\param[in]	val	Le nouveau eTOPOLOGY
		 */
		void SetPrimitiveType( eTOPOLOGY val );
		/**
		 *\~english
		 *\brief		Sets the eVIEWPORT_TYPE
		 *\param[in]	val	The new eVIEWPORT_TYPE
		 *\~french
		 *\brief		Définit le eVIEWPORT_TYPE
		 *\param[in]	val	Le nouveau eVIEWPORT_TYPE
		 */
		void SetViewportType( eVIEWPORT_TYPE val );
		/**
		 *\~english
		 *\brief		Sets the Scene
		 *\param[in]	p_pScene	The new Scene
		 *\~french
		 *\brief		Définit la Scene
		 *\param[in]	p_pScene	La nouvelle Scene
		 */
		void SetScene( SceneSPtr p_pScene );
		/**
		 *\~english
		 *\brief		Retrieves the window dimensions
		 *\return		The window dimensions
		 *\~french
		 *\brief		Récupère les dimensions de la fenêtre
		 *\return		Les dimensions de la fenêtre
		 */
		Castor::Size GetSize()const;
		/**
		 *\~english
		 *\brief		Retrieves the stereo status
		 *\return		\p true if stereo is used
		 *\~french
		 *\brief		Récupère le statut d'utilisation stéréo
		 *\return		\p true si le rendu stéréo est utilisé
		 */
		bool IsUsingStereo()const;
		/**
		 *\~english
		 *\brief		Defines the stereo status
		 *\param[in]	p_bStereo	\p true if stereo is to be used
		 *\~french
		 *\brief		Définit le statut d'utilisation stéréo
		 *\param[in]	p_bStereo	\p true si le rendu stéréo est à utiliser
		 */
		void SetStereo( bool p_bStereo );
		/**
		 *\~english
		 *\brief		Retrieves the intra ocular distance
		 *\return		The intra ocular distance
		 *\~french
		 *\brief		Récupère la distance inter oculaire
		 *\return		La distance inter oculaire
		 */
		real GetIntraOcularDistance()const;
		/**
		 *\~english
		 *\brief		Defines the intra ocular distance
		 *\param[in]	p_rIao	The intra ocular distance
		 *\~french
		 *\brief		Définit la distance inter oculaire
		 *\param[in]	p_rIao	La distance inter oculaire
		 */
		void SetIntraOcularDistance( real p_rIao );
		/**
		 *\~english
		 *\brief		Retrieves the stereo status
		 *\return		\p true if stereo is used
		 *\~french
		 *\brief		Récupère le statut d'utilisation stéréo
		 *\return		\p true si le rendu stéréo est utilisé
		 */
		bool IsUsingDeferredRendering()const;
		/**
		 *\~english
		 *\brief		Retrieves the window pixel format
		 *\return		The window pixel format
		 *\~french
		 *\brief		Récupère le format des pixels de la fenêtre
		 *\return		Le format des pixels de la fenêtre
		 */
		Castor::ePIXEL_FORMAT GetPixelFormat()const;
		/**
		 *\~english
		 *\brief		Sets the window pixel format
		 *\param[in]	val	The new window pixel format
		 *\~french
		 *\brief		Définit le format des pixels de la fenêtre
		 *\param[in]	val	Le nouveau format des pixels de la fenêtre
		 */
		void SetPixelFormat( Castor::ePIXEL_FORMAT val );
		/**
		 *\~english
		 *\brief		Retrieves the window depth format
		 *\return		The window depth format
		 *\~french
		 *\brief		Récupère le format de profondeur de la fenêtre
		 *\return		Le format de profondeur de la fenêtre
		 */
		Castor::ePIXEL_FORMAT GetDepthFormat()const;
		/**
		 *\~english
		 *\brief		Sets the window depth format
		 *\param[in]	val	The new window depth format
		 *\~french
		 *\brief		Définit le format de profondeur de la fenêtre
		 *\param[in]	val	Le nouveau format de profondeur de la fenêtre
		 */
		void SetDepthFormat( Castor::ePIXEL_FORMAT val );
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
			return m_bInitialised;
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
			return m_pContext;
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
			return m_pRenderTarget.lock();
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
			m_pRenderTarget = p_pTarget;
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
		 *\param[in]	p_bVal	The usage
		 *\~french
		 *\brief		Définit l'utilisation de la vsync
		 *\param[in]	p_bVal	L'utilisation
		 */
		inline void SetVSync( bool p_bVal )
		{
			m_bVSync = p_bVal;
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
		 *\param[in]	p_bVal	The status
		 *\~french
		 *\brief		Définit le statut du rendu plein écran
		 *\param[in]	p_bVal	Le statut
		 */
		inline void SetFullscreen( bool p_bVal )
		{
			m_bFullscreen = p_bVal;
		}
		/**
		 *\~english
		 *\brief		Tells the RenderWindow is initialised
		 *\~french
		 *\brief		Dit que la RenderWindow est initialisée
		 */
		void SetInitialised()
		{
			m_bInitialised = true;
		}
		/**
		 *\~english
		 *\brief		Retrieves the window name
		 *\return		The name
		 *\~french
		 *\brief		Récupère le nom de la fenêtre
		 *\return		Le nom
		 */
		Castor::String const & GetName()const
		{
			return m_strName;
		}
		/**
		 *\~english
		 *\brief		Retrieves the Engine
		 *\~french
		 *\brief		Récupère l'Engine
		 */
		virtual Engine * GetEngine()const
		{
			return m_pEngine;
		}

	protected:
		/**
		 *\~english
		 *\brief		Begins the scene on GPU side
		 *\~french
		 *\brief		Démarre la scène côté GPU
		 */
		virtual void DoBeginScene() = 0;
		/**
		 *\~english
		 *\brief		Ends the scene on GPU side
		 *\~french
		 *\brief		Termine la scène côté GPU
		 */
		virtual void DoEndScene() = 0;

	private:
		Castor::String DoGetName();
		void DoRender( eBUFFER p_eTargetBuffer, DynamicTextureSPtr p_pTexture );
		void DoUpdateSize();

	private:
		//!\~english The core engine	\~french Le moteur
		Engine * m_pEngine;
		//!\~english Total number of render windows	\~french Nombre total de fenêtres de rendu
		static uint32_t s_nbRenderWindows;
		//!\~english This window's index	\~french Index de la fenêtre
		uint32_t m_index;
		//!\~english The handle of the display window	\~french Handle de la fenêtre sustème
		WindowHandle m_handle;
		//!\~english The render target, which receives the main render	\~french La render target, recevant le rendu principal
		RenderTargetWPtr m_pRenderTarget;
		//!\~english The events listener	\~french Gestionnaire d'évènements
		FrameListenerWPtr m_wpListener;
		//!\~english Tells if the window is initalised	\~french Dit si la fenêtre est initialisée
		bool m_bInitialised;
		//!\~english The name	\~french Nom de cette RenderTarget
		Castor::String m_strName;
		//!\~english The rendering context	\~french Le contexte de rendu
		ContextSPtr m_pContext;
		//!\~english Tells VSync is activated	\~french Dit si la VSync est activée
		bool m_bVSync;
		//!\~english Tells fullscreen is activated	\~french Dit si le rendu est en plein écran
		bool m_bFullscreen;
		//!\~english Tells the window has been resized and needs size update	\~french Dit que la fenêtre a été redimensionnée et nécessite une mise à jour de taille
		bool m_bResized;
		//!\~english Depth and stencil buffers states	\~french Etats des tampons de profondeur et stencil
		DepthStencilStateWPtr m_wpDepthStencilState;
		//!\~english Rasteriser states	\~french Etats du rasteriser
		RasteriserStateWPtr m_wpRasteriserState;
	};
}

#pragma warning( pop )

#endif
