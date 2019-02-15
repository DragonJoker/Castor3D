/*
See LICENSE file in root folder
*/
#ifndef ___C3D_RENDER_LOOP_H___
#define ___C3D_RENDER_LOOP_H___

#include "Castor3DPrerequisites.hpp"
#include "Render/RenderInfo.hpp"

#include <Ashes/Core/WindowHandle.hpp>
#include <Multithreading/ThreadPool.hpp>

#include <chrono>

namespace castor3d
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
		: public castor::OwnedBy< Engine >
	{
	public:
		/**
		 *\~english
		 *\brief		Constructor.
		 *\param[in]	engine		The engine.
		 *\param[in]	wantedFPS	The wanted FPS count.
		 *\param[in]	isAsync		Tells if the render loop is asynchronous.
		 *\~french
		 *\brief		Constructeur.
		 *\param[in]	engine		Le moteur.
		 *\param[in]	wantedFPS	Le nombre voulu du FPS.
		 *\param[in]	isAsync		Dit si la boucle de rendu est asynchrone.
		 */
		C3D_API RenderLoop( Engine & engine
			, uint32_t wantedFPS
			, bool isAsync );
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
		C3D_API void cleanup();
		/**
		 *\~english
		 *\brief		Creates a render context.
		 *\remarks		If the main context has not been created yet, the first created context will become the main context.
						For asynchronous render loop, this means that the calling thread won't have associated context, preventing calls to renderSyncFrame.
		 *\param[in]	handle	The native window handle.
		 *\param[in]	window	The render window used to initialise the render context, receives the context.
		 *\~french
		 *\brief		Crée un contexte de rendu.
		 *\remarks		Si le contexte principal n'a pas encore été créé, le premier contexte créé deviendra le contexte principal.
						Pour la boucles de rendu asynchrone, cela signifie que le thread appelant cette fonction sera sans contexte associé, prévenant l'appel de renderSyncFrame.
		 *\param[in]	handle	Le handle de la fenêtre native.
		 *\param[in]	window	La fenêtre de rendu utilisée pour initialiser le contexte de rendu, recevra le contexte.
		 */
		C3D_API void createDevice( ashes::WindowHandle && handle
			, RenderWindow & window );
		/**
		 *\~english
		 *\brief		Show or hide debug overlays.
		 *\param[in]	show	The status.
		 *\~french
		 *\brief		Affiche ou cache les incrustations de débogage.
		 *\param[in]	show	Le statut.
		 */
		C3D_API void showDebugOverlays( bool show );
		/**
		 *\~english
		 *\brief		Updates the V-Sync status.
		 *\param[in]	enable	The status.
		 *\~french
		 *\brief		Met à jour le statut de synchronisation verticale.
		 *\param[in]	enable	Le statut.
		 */
		C3D_API virtual void enableVSync( bool enable );
		/**
		 *\~english
		 *\brief		Flushs all events of all frame listeners.
		 *\~french
		 *\brief		Vide tous les évènements de tous les frame listeners.
		 */
		C3D_API void flushEvents();
		/**
		 *\~english
		 *\brief		Registers a render pass timer.
		 *\param[in]	timer	The timer to register.
		 *\return		The query ID.
		 *\~french
		 *\brief		Enregistre un timer de passe de rendu.
		 *\param[in]	timer	Le timer à enregistrer.
		 *\return		L'ID de la requête.
		 */
		C3D_API uint32_t registerTimer( RenderPassTimer & timer );
		/**
		 *\~english
		 *\brief		Unregisters a render pass timer.
		 *\param[in]	timer	The timer to unregister.
		 *\~french
		 *\brief		Désenregistre un timer de passe de rendu.
		 *\param[in]	timer	Le timer à désenregistrer.
		 */
		C3D_API void unregisterTimer( RenderPassTimer & timer );
		/**
		 *\~english
		 *\return		The debug overlays shown status.
		 *\~french
		 *\return		Le statut d'affichage des incrustations de débogage.
		 */
		C3D_API bool hasDebugOverlays()const;
		/**
		 *\~english
		 *\brief		Starts threaded render loop.
		 *\~french
		 *\brief		Commence le rendu threadé.
		 */
		C3D_API virtual void beginRendering() = 0;
		/**
		 *\~english
		 *\brief		Renders one frame, only if not in render loop.
		 *\~french
		 *\brief		Rend une image, uniquement hors de la boucle de rendu.
		 */
		C3D_API virtual void renderSyncFrame() = 0;
		/**
		 *\~english
		 *\brief		Pauses the render loop.
		 *\~french
		 *\brief		Met la boucle de rendu en pause.
		 */
		C3D_API virtual void pause() = 0;
		/**
		 *\~english
		 *\brief		Resumes the render loop.
		 *\~french
		 *\brief		Redémarre la boucle de rendu.
		 */
		C3D_API virtual void resume() = 0;
		/**
		 *\~english
		 *\brief		Ends the render, cleans up engine.
		 *\remarks		Ends the threaded render loop, if any.
		 *\~french
		 *\brief		Termine le rendu, nettoie le moteur.
		 *\remarks		Arrête la boucle de rendu threadé, si elle existe.
		 */
		C3D_API virtual void endRendering() = 0;
		/**
		 *\~english
		 *\return		The wanted frame time, in milliseconds.
		 *\~french
		 *\return		Le temps voulu pour une frame, en millisecondes.
		 */
		inline castor::Milliseconds getFrameTime()
		{
			return m_frameTime;
		}
		/**
		 *\~english
		 *\return		The wanted refresh rate.
		 *\~french
		 *\return		La vitesse de rafraichissement.
		 */
		inline uint32_t getWantedFps()
		{
			return m_wantedFPS;
		}

	protected:
		/**
		 *\~english
		 *\brief		Asks for render context creation.
		 *\param[in]	handle	The native window handle.
		 *\param[in]	window	The render window used to initialise the render context, receives the context.
		 *\return		The created context, or the existing one.
		 *\~french
		 *\brief		Demande la création du contexte de rendu.
		 *\param[in]	handle	Le handle de la fenêtre native.
		 *\param[in]	window	La fenêtre de rendu utilisée pour initialiser le contexte de rendu, recevra le contexte.
		 *\return		Le contexte créé, ou l'existant.
		 */
		C3D_API ashes::DevicePtr doCreateDevice( ashes::WindowHandle && handle
			, RenderWindow & window );
		/**
		 *\~english
		 *\brief		Starts threaded render loop.
		 *\~french
		 *\brief		Commence le rendu threadé.
		 */
		C3D_API void doRenderFrame();
		/**
		 *\~english
		 *\brief		Asks for main render context creation.
		 *\param[in]	handle	The native window handle.
		 *\param[in]	window	The render window used to initialise the render context, receives the context.
		 *\~french
		 *\brief		Demande la création du contexte de rendu principal.
		 *\param[in]	handle	Le handle de la fenêtre native.
		 *\param[in]	window	La fenêtre de rendu utilisée pour initialiser le contexte de rendu, recevra le contexte.
		 */
		C3D_API virtual ashes::DevicePtr doCreateMainDevice( ashes::WindowHandle && handle
			, RenderWindow & window ) = 0;

	private:
		struct TechniqueQueues
		{
			RenderQueueArray queues;
			ShadowMapLightTypeArray shadowMaps;
		};
		void doProcessEvents( EventType eventType );
		void doGpuStep( RenderInfo & info );
		void doCpuStep();
		void doUpdateQueues( std::vector< TechniqueQueues > & queues );

	protected:
		//!\~english	The current RenderSystem.
		//!\~french		Le RenderSystem courant.
		RenderSystem & m_renderSystem;
		//!\~english	The wanted FPS, used in threaded render mode.
		//!\~french		Le nombre de FPS souhaité, utilisé en rendu threadé.
		uint32_t m_wantedFPS;
		//!\~english	The wanted time for a frame.
		//!\~french		Le temps voulu pour une frame.
		castor::Milliseconds m_frameTime;
		//!\~english	The debug overlays.
		//!\~french		Les incrustations de débogage.
		std::unique_ptr< DebugOverlays > m_debugOverlays;
		//!\~english	The pool used to update the render queues.
		//!\~french		Le pool de mise à jour des files de rendu.
		castor::ThreadPool m_queueUpdater;
	};
}

#endif
