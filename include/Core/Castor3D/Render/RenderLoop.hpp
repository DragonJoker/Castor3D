/*
See LICENSE file in root folder
*/
#ifndef ___C3D_RenderLoop_H___
#define ___C3D_RenderLoop_H___

#include "Castor3D/Event/Frame/FrameEventModule.hpp"
#include "Castor3D/Render/ShadowMap/ShadowMapModule.hpp"

#include "Castor3D/Overlay/OverlayModule.hpp"
#include "Castor3D/Render/Passes/CommandsSemaphore.hpp"

#include <CastorUtils/Multithreading/AsyncJobQueue.hpp>

#include <RenderGraph/FramePassTimer.hpp>

#include <ashespp/Core/WindowHandle.hpp>

#include <chrono>
#include <unordered_set>

namespace castor3d
{
	class RenderLoop
		: public castor::OwnedBy< Engine >
	{
	public:
		static constexpr uint32_t UnlimitedFPS = 0xFFFFFFFFu;

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
		 *\param[in]	category	The timer category name.
		 *\param[in]	timer		The timer to register.
		 *\~french
		 *\brief		Enregistre un timer de passe de rendu.
		 *\param[in]	category	Le nom de la catégorie du timer.
		 *\param[in]	timer		Le timer à enregistrer.
		 */
		C3D_API void registerTimer( castor::String const & category
			, FramePassTimer & timer );
		/**
		 *\~english
		 *\brief		Unregisters a render pass timer.
		 *\param[in]	category	The timer category name.
		 *\param[in]	timer		The timer to unregister.
		 *\~french
		 *\brief		Désenregistre un timer de passe de rendu.
		 *\param[in]	category	Le nom de la catégorie du timer.
		 *\param[in]	timer		Le timer à désenregistrer.
		 */
		C3D_API void unregisterTimer( castor::String const & category
			, FramePassTimer & timer );
		/**
		 *\~english
		 *\brief		Registers a ShaderBuffer.
		 *\param[in]	buffer	The ShaderBuffer.
		 *\~french
		 *\brief		Enregistre un ShaderBuffer.
		 *\param[in]	buffer	Le ShaderBuffer.
		 */
		C3D_API void registerBuffer( ShaderBuffer const & buffer );
		/**
		 *\~english
		 *\brief		Unregisters a ShaderBuffer.
		 *\param[in]	buffer	The ShaderBuffer.
		 *\~french
		 *\brief		Désenregistre un ShaderBuffer.
		 *\param[in]	buffer	Le ShaderBuffer.
		 */
		C3D_API void unregisterBuffer( ShaderBuffer const & buffer );
		/**
		 *\~english
		 *\return		The debug overlays shown status.
		 *\~french
		 *\return		Le statut d'affichage des incrustations de débogage.
		 */
		C3D_API bool hasDebugOverlays()const;
		/**
		 *\~english
		 *\brief		The average frame time, in nanoseconds.
		 *\~french
		 *\brief		Le temps moyen pour une frame, en nanosecondes.
		 */
		C3D_API void dumpFrameTimes( Parameters & params )const;
		/**
		 *\~english
		 *\return		The average frame time, in nanoseconds.
		 *\~french
		 *\return		Le temps moyen pour une frame, en nanosecondes.
		 */
		C3D_API castor::Nanoseconds getAvgFrameTime()const;
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
		 *\param[in]	tslf	The time elapsed since last frame.
		 *\~french
		 *\brief		Rend une image, uniquement hors de la boucle de rendu.
		 *\param[in]	tslf	Le temps écoulé depuis la dernière frame.
		 */
		C3D_API virtual void renderSyncFrame( castor::Milliseconds tslf = 0_ms ) = 0;
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
		castor::Milliseconds getFrameTime()const
		{
			return m_frameTime;
		}
		/**
		 *\~english
		 *\return		The wanted refresh rate.
		 *\~french
		 *\return		La vitesse de rafraichissement.
		 */
		uint32_t getWantedFps()const
		{
			return m_wantedFPS;
		}
		/**
		 *\~english
		 *\return		The last frame time, in milliseconds.
		 *\~french
		 *\return		Le temps écoulé pour la dernière frame, en millisecondes.
		 */
		castor::Microseconds getLastFrameTime()const
		{
			return m_lastFrameTime;
		}

	protected:
		/**
		 *\~english
		 *\brief		Starts threaded render loop.
		 *\param[in]	tslf	The time elapsed since last frame.
		 *\~french
		 *\brief		Commence le rendu threadé.
		 *\param[in]	tslf	Le temps écoulé depuis la dernière frame.
		 */
		C3D_API void doRenderFrame( castor::Milliseconds tslf = 0_ms );

	private:
		void doProcessEvents( EventType eventType );
		void doProcessEvents( EventType eventType
			, RenderDevice const & device
			, QueueData const & queueData );
		void doGpuStep( RenderInfo & info );
		void doCpuStep( castor::Milliseconds tslf );

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
		//!\~english	The elapsed time for the last frame.
		//!\~french		Le temps écoulé pour la dernière frame.
		std::atomic< castor::Microseconds > m_lastFrameTime;
		//!\~english	The debug overlays.
		//!\~french		Les incrustations de débogage.
		std::unique_ptr< DebugOverlays > m_debugOverlays;
		std::mutex m_debugOverlaysMtx;
		struct UploadResources
		{
			//!\~english	The command buffer and semaphore used for UBO uploads.
			//!\~french		Le command buffer et le semaphore utilisé pour l'upload des UBO.
			CommandsSemaphore commands;
			//!\~english	The fence and semaphore used for UBO uploads.
			//!\~french		La fence et le semaphore utilisé pour l'upload des UBO.
			ashes::FencePtr fence;
			//!\~english	Tells if the smaphore has been used in previous frame.
			//!\~french		Dit si le sémaphore a été utilisé pendant la précédente frame.
			bool used{ true };
			uint32_t waitCount{ 1u };
		};
		std::array< UploadResources, 2u > m_uploadResources;
		uint32_t m_currentUpdate{ 0u };
		crg::SemaphoreWaitArray m_toWait;

	private:
		bool m_first = true;
		QueueData const * m_reservedQueue{};
		std::unordered_set< ShaderBuffer const * > m_shaderBuffers;
		std::mutex m_shaderBuffersMtx;
		std::unique_ptr< crg::FramePassTimer > m_uploadTimer;
		std::array< FramePassTimerUPtr, size_t( EventType::eCount ) > m_timerCpuEvents;
		std::array< FramePassTimerUPtr, size_t( EventType::eCount ) > m_timerGpuEvents;
	};
}

#endif
