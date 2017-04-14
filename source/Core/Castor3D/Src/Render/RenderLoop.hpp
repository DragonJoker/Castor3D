/*
This source file is part of Castor3D (http://castor3d.developpez.com/castor3d.html)
Copyright (c) 2016 dragonjoker59@hotmail.com

Permission is hereby granted, free of charge, to any person obtaining a copy of
this software and associated documentation files (the "Software"), to deal in
the Software without restriction, including without limitation the rights to
use, copy, modify, merge, publish, distribute, sublicense, and/or sell copies
of the Software, and to permit persons to whom the Software is furnished to do
so, subject to the following conditions:

The above copyright notice and this permission notice shall be included in all
copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
SOFTWARE.
*/
#ifndef ___C3D_RENDER_LOOP_H___
#define ___C3D_RENDER_LOOP_H___

#include "Castor3DPrerequisites.hpp"
#include "Render/RenderInfo.hpp"

#include <Multithreading/ThreadPool.hpp>

#include <chrono>

namespace Castor3D
{
	/*!
	\author 	Sylvain DOREMUS
	\date 		14/10/2015
	\version	0.8.0
	\~english
	\brief		Render loop base class.
	\~french
	\brief		Classe de base d'une boucle de rendu.
	*/
	class RenderLoop
		: public Castor::OwnedBy< Engine >
	{
	public:
		/**
		 *\~english
		 *\brief		Constructor.
		 *\param[in]	p_engine		The engine.
		 *\param[in]	p_wantedFPS		The wanted FPS count.
		 *\param[in]	p_isAsync		Tells if the render loop is asynchronous.
		 *\~french
		 *\brief		Constructeur.
		 *\param[in]	p_engine		Le moteur.
		 *\param[in]	p_wantedFPS		Le nombre voulu du FPS.
		 *\param[in]	p_isAsync		Dit si la boucle de rendu est asynchrone.
		 */
		C3D_API RenderLoop( Engine & p_engine, uint32_t p_wantedFPS, bool p_isAsync );
		/**
		 *\~english
		 *\brief		Destructor.
		 *\~french
		 *\brief		Destructeur.
		 */
		C3D_API virtual ~RenderLoop();
		/**
		 *\~english
		 *\brief		Cleans up the render loop.
		 *\~french
		 *\brief		Nettoie la boucle de rendu.
		 */
		C3D_API void Cleanup();
		/**
		 *\~english
		 *\brief		Creates a render context.
		 *\remarks		If the main context has not been created yet, the first created context will become the main context.
						For asynchronous render loop, this means that the calling thread won't have associated context, preventing calls to RenderSyncFrame
		 *\param[in]	p_window	The render window used to initialise the render context, receives the context.
		 *\~french
		 *\brief		Crée un contexte de rendu.
		 *\remarks		Si le contexte principal n'a pas encore été créé, le premier contexte créé deviendra le contexte principal.
						Pour la boucles de rendu asynchrone, cela signifie que le thread appelant cette fonction sera sans contexte associé, prévenant l'appel de RenderSyncFrame.
		 *\param[in]	p_window	La fenêtre de rendu utilisée pour initialiser le contexte de rendu, recevra le contexte.
		 */
		C3D_API void CreateContext( RenderWindow & p_window );
		/**
		 *\~english
		 *\brief		Show or hide debug overlays.
		 *\param[in]	p_show	The status.
		 *\~french
		 *\brief		Affiche ou cache les incrustations de débogage.
		 *\param[in]	p_show	Le statut.
		 */
		C3D_API void ShowDebugOverlays( bool p_show );
		/**
		 *\~english
		 *\brief		Updates the V-Sync status.
		 *\param[in]	p_enable	The status.
		 *\~french
		 *\brief		Met à jour le statut de synchronisation verticale.
		 *\param[in]	p_enable	Le statut.
		 */
		C3D_API virtual void UpdateVSync( bool p_enable );
		/**
		 *\~english
		 *\brief		Flushs all events of all frame listeners.
		 *\~french
		 *\brief		Vide tous les évènements de tous les frame listeners.
		 */
		C3D_API void FlushEvents();
		/**
		 *\~english
		 *\brief		Starts threaded render loop.
		 *\~french
		 *\brief		Commence le rendu threadé.
		 */
		C3D_API virtual void StartRendering() = 0;
		/**
		 *\~english
		 *\brief		Renders one frame, only if not in render loop.
		 *\~french
		 *\brief		Rend une image, uniquement hors de la boucle de rendu.
		 */
		C3D_API virtual void RenderSyncFrame() = 0;
		/**
		 *\~english
		 *\brief		Pauses the render loop.
		 *\~french
		 *\brief		Met la boucle de rendu en pause.
		 */
		C3D_API virtual void Pause() = 0;
		/**
		 *\~english
		 *\brief		Resumes the render loop.
		 *\~french
		 *\brief		Redémarre la boucle de rendu.
		 */
		C3D_API virtual void Resume() = 0;
		/**
		 *\~english
		 *\brief		Ends the render, cleans up engine.
		 *\remarks		Ends the threaded render loop, if any.
		 *\~french
		 *\brief		Termine le rendu, nettoie le moteur.
		 *\remarks		Arrête la boucle de rendu threadé, si elle existe.
		 */
		C3D_API virtual void EndRendering() = 0;
		/**
		 *\~english
		 *\return		The wanted frame time, in milliseconds.
		 *\~french
		 *\return		Le temps voulu pour une frame, en millisecondes.
		 */
		inline std::chrono::milliseconds GetFrameTime()
		{
			return m_frameTime;
		}
		/**
		 *\~english
		 *\return		The wanted refresh rate.
		 *\~french
		 *\return		La vitesse de rafraichissement.
		 */
		inline uint32_t GetWantedFps()
		{
			return m_wantedFPS;
		}
		/**
		 *\~english
		 *\return		The debug overlays shown status.
		 *\~french
		 *\return		Le statut d'affichage des incrustations de débogage.
		 */
		inline bool HasDebugOverlays()const
		{
			return m_debugOverlays != nullptr;
		}

	protected:
		/**
		 *\~english
		 *\brief		Asks for render context creation.
		 *\param[in]	p_window	The render window used to initialise the render context, receives the context.
		 *\return		The created context, or the existing one.
		 *\~french
		 *\brief		Demande la création du contexte de rendu.
		 *\param[in]	p_window	La fenêtre de rendu utilisée pour initialiser le contexte de rendu, recevra le contexte.
		 *\return		Le contexte créé, ou l'existant.
		 */
		C3D_API ContextSPtr DoCreateContext( RenderWindow & p_window );
		/**
		 *\~english
		 *\brief		Starts threaded render loop.
		 *\~french
		 *\brief		Commence le rendu threadé.
		 */
		C3D_API void DoRenderFrame();
		/**
		 *\~english
		 *\brief		Asks for main render context creation.
		 *\param[in]	p_window	The render window used to initialise the render context, receives the context.
		 *\~french
		 *\brief		Demande la création du contexte de rendu principal.
		 *\param[in]	p_window	La fenêtre de rendu utilisée pour initialiser le contexte de rendu, recevra le contexte.
		 */
		C3D_API virtual ContextSPtr DoCreateMainContext( RenderWindow & p_window ) = 0;

	private:
		void DoProcessEvents( EventType p_eventType );
		void DoGpuStep( RenderInfo & p_info );
		void DoCpuStep();
		void DoUpdateQueues( RenderQueueArray & p_queues );

	protected:
		//!\~english	The current RenderSystem.
		//!\~french		Le RenderSystem courant.
		RenderSystem & m_renderSystem;
		//!\~english	The wanted FPS, used in threaded render mode.
		//!\~french		Le nombre de FPS souhaité, utilisé en rendu threadé.
		uint32_t m_wantedFPS;
		//!\~english	The wanted time for a frame.
		//!\~french		Le temps voulu pour une frame.
		std::chrono::milliseconds m_frameTime;
		//!\~english	The debug overlays.
		//!\~french		Les incrustations de débogage.
		std::unique_ptr< DebugOverlays > m_debugOverlays;
		//!\~english	The pool used to update the render queues.
		//!\~french		Le pool de mise à jour des files de rendu.
		Castor::ThreadPool m_queueUpdater;
	};
}

#undef DECLARE_MANAGED_MEMBER

#endif
