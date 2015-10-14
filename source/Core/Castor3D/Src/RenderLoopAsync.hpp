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
#ifndef ___C3D_RENDER_LOOP_ASYNC_H___
#define ___C3D_RENDER_LOOP_ASYNC_H___

#include "RenderLoop.hpp"

#include <atomic>

namespace Castor3D
{
	/*!
	\author 	Sylvain DOREMUS
	\date 		09/02/2010
	\version	0.1
	\~english
	\brief		Main System
	\remark		Holds the render windows, the plugins, the render drivers...
	\~french
	\brief		Moteur principal
	\remark		Contient les fenêtres de rendu, les plugins, drivers de rendu...
	*/
	class RenderLoopAsync
		: public RenderLoop
	{
	public:
		/**
		 *\~english
		 *\brief		Constructor.
		 *\param[in]	p_engine		The engine.
		 *\param[in]	p_renderSystem	The render system.
		 *\param[in]	p_wantedFPS		The wanted FPS count.
		 *\~french
		 *\brief		Constructeur.
		 *\param[in]	p_engine		Le moteur.
		 *\param[in]	p_renderSystem	Le système de rendu.
		 *\param[in]	p_wantedFPS		Le nombre voulu du FPS.
		 */
		C3D_API RenderLoopAsync( Engine & p_engine, RenderSystem * p_renderSystem, uint32_t p_wantedFPS = 100 );
		/**
		 *\~english
		 *\brief		Destructor.
		 *\~french
		 *\brief		Destructeur.
		 */
		C3D_API ~RenderLoopAsync();
		/**
		 *\~english
		 *\brief		Retrieves the end status.
		 *\remark		Thread-safe.
		 *\return		\p true if ended.
		 *\~french
		 *\brief		Récupère le statut de fin.
		 *\remark		Thread-safe.
		 *\return		\p true si arrêté.
		 */
		C3D_API bool IsEnded()const;
		/**
		 *\~english
		 *\brief		Tells the engine the render is ended.
		 *\remark		Thread-safe.
		 *\~french
		 *\brief		Dit que le rendu est stoppé.
		 *\remark		Thread-safe.
		 */
		C3D_API void SetEnded( bool p_value = true );
		/**
		 *\~english
		 *\brief		Retrieves the render start status.
		 *\remark		Thread-safe.
		 *\return		\p true if started.
		 *\~french
		 *\brief		Récupère le statut de début de rendu.
		 *\remark		Thread-safe.
		 *\return		\p true si démarré.
		 */
		C3D_API bool IsStarted()const;
		/**
		 *\~english
		 *\brief		Tells the engine the render is started.
		 *\remark		Thread-safe.
		 *\~french
		 *\brief		Dit que le rendu est démarré.
		 *\remark		Thread-safe.
		 */
		C3D_API void SetStarted( bool p_value = true );
		/**
		 *\~english
		 *\brief		Retrieves the context creation status.
		 *\remark		Thread-safe.
		 *\return		\p true if created.
		 *\~french
		 *\brief		Récupère le statut de création du contexte de rendu.
		 *\remark		Thread-safe.
		 *\return		\p true si créé.
		 */
		C3D_API bool IsCreated()const;
		/**
		 *\~english
		 *\brief		Tells the engine the render context is created.
		 *\remark		Thread-safe.
		 *\~french
		 *\brief		Dit que le contexte de rendu est créé.
		 *\remark		Thread-safe.
		 */
		C3D_API void SetCreated( bool p_value = true );
		/**
		 *\~english
		 *\brief		Retrieves the render to-create status.
		 *\remark		Thread-safe.
		 *\return		\p true if the render context is to create.
		 *\~french
		 *\brief		Récupère le statut de demande de création du contexte.
		 *\remark		Thread-safe.
		 *\return		\p true si à créer.
		 */
		C3D_API bool IsToCreate()const;
		/**
		 *\~english
		 *\brief		Tells if the engine the render context is to create.
		 *\remark		Thread-safe.
		 *\~french
		 *\brief		Dit si le contexte de rendu est à créer.
		 *\remark		Thread-safe.
		 */
		C3D_API void SetToCreate( bool p_value = true );
		/**
		 *\~english
		 *\brief		Thread-safe
		 *\return		\p true if the render loop is interrupted.
		 *\~french
		 *\brief		Thread-safe.
		 *\return		\p true si la boucle de rendu est interrompue.
		 */
		C3D_API bool IsInterrupted()const;
		/**
		 *\~english
		 *\brief		Interrupts the render loop.
		 *\remark		Thread-safe.
		 *\~french
		 *\brief		Interrompt la boucle de rendu.
		 *\remark		Thread-safe.
		 */
		C3D_API void Interrupt();

	private:
		/**
		 *\copydoc		Castor3D::RenderLoop::DoStartRendering
		 */
		C3D_API virtual void DoStartRendering();
		/**
		 *\copydoc		Castor3D::RenderLoop::DoRenderSyncFrame
		 */
		C3D_API virtual void DoRenderSyncFrame();
		/**
		 *\copydoc		Castor3D::RenderLoop::DoEndRendering
		 */
		C3D_API virtual void DoEndRendering();
		/**
		 *\copydoc		Castor3D::RenderLoop::DoCreateMainContext
		 */
		C3D_API virtual ContextSPtr DoCreateMainContext( RenderWindow & p_window );
		/**
		 *\~english
		 *\brief		The threaded render loop.
		 *\remark		The main context is created here, since OpenGL needs each thread to have its context.
		 *\~french
		 *\brief		La boucle de rendu threadée.
		 *\remark		Le contexte principal est créé ici, car OpenGL demande à chaque thread d'avoir son contexte.
		 */
		C3D_API void DoMainLoop();
		C3D_API void DoSetWindow( RenderWindow * p_window );
		C3D_API RenderWindow * DoGetWindow()const;

	private:
		//!\~english The main loop, in case of threaded rendering	\~french La boucle principale, au cas où on fait du rendu threadé
		std::unique_ptr< std::thread > m_mainLoopThread;
		//!\~english Tells if render has ended, by any reason.	\~french Dit si le rendu est terminé, quelle qu'en soit la raison.
		std::atomic_bool m_ended;
		//!\~english Tells if render is running.	\~french Dit si le rendu est en cours.
		std::atomic_bool m_started;
		//!\~english Tells if render context is to be created.	\~french Dit si le contexte de rendu est à créer.
		std::atomic_bool m_createContext;
		//!\~english Tells if render context is created.	\~french Dit si le contexte de rendu est créé.
		std::atomic_bool m_created;
		//!\~english Tells if the loop is interrupted.	\~french Dit si la boucle est interrompue.
		std::atomic_bool m_interrupted;
		//!\~english The mutex, to make the main loop thread-safe	\~french Le mutex utilisé pour que la boucle principale soit thread-safe
		mutable std::mutex m_mutexWindow;
		//!\~english The render window used to initalise the main rendering context	\~french La render window utilisée pour initialiser le contexte de rendu principal
		RenderWindowRPtr m_window;
	};
}

#endif
