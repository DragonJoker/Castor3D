/*
See LICENSE file in root folder
*/
#ifndef ___C3D_RENDER_LOOP_ASYNC_H___
#define ___C3D_RENDER_LOOP_ASYNC_H___

#include "Castor3D/Render/RenderLoop.hpp"

#include <atomic>

namespace castor3d
{
	class RenderLoopAsync
		: public RenderLoop
	{
	public:
		/**
		 *\~english
		 *\brief		Constructor.
		 *\param[in]	engine		The engine.
		 *\param[in]	wantedFPS	The wanted FPS count.
		 *\~french
		 *\brief		Constructeur.
		 *\param[in]	engine		Le moteur.
		 *\param[in]	wantedFPS	Le nombre voulu du FPS.
		 */
		C3D_API RenderLoopAsync( Engine & engine
			, uint32_t wantedFPS );
		/**
		 *\~english
		 *\brief		Destructor.
		 *\~french
		 *\brief		Destructeur.
		 */
		C3D_API ~RenderLoopAsync();
		/**
		 *\~english
		 *\brief		Retrieves the render start status.
		 *\remarks		Thread-safe.
		 *\return		\p true if started.
		 *\~french
		 *\brief		Récupère le statut de début de rendu.
		 *\remarks		Thread-safe.
		 *\return		\p true si démarré.
		 */
		C3D_API bool isRendering()const;
		/**
		 *\~english
		 *\brief		Retrieves the render paused status.
		 *\remarks		Thread-safe.
		 *\return		\p true if paused.
		 *\~french
		 *\brief		Récupère le statut de rendu en pause.
		 *\remarks		Thread-safe.
		 *\return		\p true si en pause.
		 */
		C3D_API bool isPaused()const;
		/**
		 *\~english
		 *\brief		Thread-safe
		 *\return		\p true if the render loop is interrupted.
		 *\~french
		 *\brief		Thread-safe.
		 *\return		\p true si la boucle de rendu est interrompue.
		 */
		C3D_API bool isInterrupted()const;
		/**
		 *\~english
		 *\brief		Retrieves the render loop end status.
		 *\remarks		Thread-safe.
		 *\return		\p true if created.
		 *\~french
		 *\brief		Récupère le statut de fin de la boucle de rendu.
		 *\remarks		Thread-safe.
		 *\return		\p true si créé.
		 */
		C3D_API bool isEnded()const;
		/**
		 *\copydoc		castor3d::RenderLoop::enableVSync
		 */
		C3D_API void enableVSync( bool enable )override;
		/**
		 *\copydoc		castor3d::RenderLoop::beginRendering
		 */
		C3D_API void beginRendering()override;
		/**
		 *\copydoc		castor3d::RenderLoop::renderSyncFrame
		 */
		C3D_API void renderSyncFrame( castor::Milliseconds tslf )override;
		/**
		 *\copydoc		castor3d::RenderLoop::pause
		 */
		C3D_API void pause()override;
		/**
		 *\copydoc		castor3d::RenderLoop::resume
		 */
		C3D_API void resume()override;
		/**
		 *\copydoc		castor3d::RenderLoop::endRendering
		 */
		C3D_API void endRendering()override;

	private:
		/**
		 *\~english
		 *\brief		The threaded render loop.
		 *\remarks		The main context is created here, since OpenGL needs each thread to have its context.
		 *\~french
		 *\brief		La boucle de rendu threadée.
		 *\remarks		Le contexte principal est créé ici, car OpenGL demande à chaque thread d'avoir son contexte.
		 */
		C3D_API void doMainLoop();

	private:
		std::unique_ptr< std::thread > m_mainLoopThread;
		std::atomic_bool m_ended;
		std::atomic_bool m_rendering;
		std::atomic_bool m_frameEnded;
		std::atomic_bool m_paused;
		std::atomic_bool m_interrupted;
		castor::Milliseconds m_savedTime{ 0 };
	};
}

#endif
