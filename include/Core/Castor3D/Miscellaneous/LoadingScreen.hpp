/*
See LICENSE file in root folder
*/
#ifndef ___C3D_LoadingScreen_H___
#define ___C3D_LoadingScreen_H___

#include "Castor3D/Miscellaneous/MiscellaneousModule.hpp"
#include "Castor3D/Render/RenderModule.hpp"
#include "Castor3D/Render/Culling/CullingModule.hpp"
#include "Castor3D/Render/Overlays/OverlaysModule.hpp"
#include "Castor3D/Render/Passes/PassesModule.hpp"
#include "Castor3D/Scene/SceneModule.hpp"
#include "Castor3D/Scene/Background/BackgroundModule.hpp"

#include "Castor3D/Miscellaneous/ProgressBar.hpp"
#include "Castor3D/Shader/Ubos/CameraUbo.hpp"
#include "Castor3D/Shader/Ubos/RenderUbo.hpp"
#include "Castor3D/Shader/Ubos/SceneUbo.hpp"

#include <CastorUtils/Math/RangedValue.hpp>

#include <RenderGraph/FrameGraph.hpp>
#include <RenderGraph/RunnablePass.hpp>
#include <RenderGraph/RunnablePasses/RenderQuad.hpp>

#include <ashespp/AshesPPPrerequisites.hpp>

#include <CastorUtils/Config/BeginExternHeaderGuard.hpp>
#include <atomic>
#include <CastorUtils/Config/EndExternHeaderGuard.hpp>

namespace c3d
{
	class LoadingScreen
	{
	public:
		C3D_API LoadingScreen( ProgressBar & progressBar
			, RenderDevice const & device
			, crg::ResourcesCache & resources
			, SceneRPtr scene
			, VkRenderPass renderPass
			, Size const & size );
		C3D_API ~LoadingScreen()noexcept;

		C3D_API void enable()noexcept;
		C3D_API void disable()noexcept;
		C3D_API void update( CpuUpdater & updater );
		C3D_API void update( GpuUpdater & updater );
		C3D_API void setRenderPass( VkRenderPass renderPass
			, Size const & renderSize
			, PixelFormat swapchainFormat );
		/**
		 *\~english
		 *\brief			Uploads overlays GPU buffers to VRAM.
		 *\param[in,out]	uploader	Receives the upload requests.
		 *\~french
		 *\brief			Met à jour tous les tampons GPU d'incrustations en VRAM.
		 *\param[in,out]	uploader	Reçoit les requêtes d'upload.
		 */
		C3D_API void upload( UploadData & uploader );
		C3D_API void record();
		/**
		 *\~english
		 *\brief		Renders the loading screen.
		 *\param[in]	queue		The queue receiving the render commands.
		 *\param[in]	toWait		The semaphores from the previous render pass.
		 *\param[in]	fence		The fence to wait, \p nullptr to prevent waiting.
		 *\return		The semaphores signaled by this render.
		 *\~french
		 *\brief		Dessine l'écran de chargement.
		 *\param[in]	queue		La queue recevant les commandes d'initialisation.
		 *\param[in]	toWait		Les sémaphores de la passe de rendu précédente.
		 *\param[in]	fence		La fence à attendre, \p nullptr pour ne pas attendre.
		 *\return		Les sémaphores signalés par ce dessin.
		 */
		C3D_API SemaphoreWaitArray render( ashes::Queue const & queue
			, SemaphoreWaitArray const & toWait );
		/**
		 *\~english
		 *\brief		Renders the loading screen.
		 *\param[in]	queue		The queue receiving the render commands.
		 *\param[in]	toWait		The semaphore from the previous render pass.
		 *\param[in]	fence		The fence to wait, \p nullptr to prevent waiting.
		 *\return		The semaphores signaled by this render.
		 *\~french
		 *\brief		Dessine l'écran de chargement.
		 *\param[in]	queue		La queue recevant les commandes d'initialisation.
		 *\param[in]	toWait		Le sémaphore de la passe de rendu précédente.
		 *\param[in]	fence		La fence à attendre, \p nullptr pour ne pas attendre.
		 *\return		Les sémaphores signalés par ce dessin.
		 */
		SemaphoreWaitArray render( ashes::Queue const & queue
			, SemaphoreWait const & toWait )
		{
			return render( queue
				, SemaphoreWaitArray{ toWait } );
		}
		/**
		*\~english
		*name
		*	Mutators.
		*\~french
		*name
		*	Mutateurs.
		*/
		/**@{*/
		void initGlobalRange( uint32_t value )
		{
			m_progressBar.initGlobalRange( value );
		}

		void stepGlobal( String const & globalTitle )
		{
			m_progressBar.stepGlobal( globalTitle );
		}

		void initLocalRange( String const & globalLabel
			, uint32_t value )
		{
			m_progressBar.initLocalRange( globalLabel, value );
		}

		void stepLocal( String const & label )
		{
			m_progressBar.stepLocal( label );
		}
		/**@}*/
		/**
		*\~english
		*name
		*	Getters.
		*\~french
		*name
		*	Accesseurs.
		*/
		/**@{*/
		bool isEnabled()const
		{
			return m_enabled;
		}

		ProgressBar & getProgressBar()
		{
			return m_progressBar;
		}

		PixelFormat getFormat()const noexcept
		{
			return m_colour.getFormat();
		}

		Texture const & getResult()const noexcept
		{
			return m_colour;
		}
		/**@}*/

	private:
		void doCreateOpaquePass();
		void doCreateTransparentPass();
		void doCreateOverlayPass();

	public:
		C3D_API static String const SceneName;

	private:
		RenderDevice const & m_device;
		ProgressBar & m_progressBar;
		RawUniquePtr< crg::FrameGraph > m_graph;
		std::atomic_bool m_enabled{};
		std::atomic_bool m_needsRecreate{};
		SceneRPtr m_scene;
		SceneBackground & m_background;
		VkRenderPass m_renderPass;
		Size m_initialRenderSize;
		Size m_renderSize;
		CameraRPtr m_camera;
		SceneCullerUPtr m_culler;
		PixelFormat m_swapchainFormat{ PixelFormat::eR8G8B8A8_UNORM };
		Texture m_colour;
		Texture m_depth;
		CameraUbo m_cameraUbo;
		RenderUbo m_renderUbo;
		SceneUbo const * m_sceneUbo{};
		BackgroundRendererUPtr m_backgroundRenderer;
		RenderTechniquePass * m_opaquePass{};
		RenderTechniquePass * m_transparentPass{};
		OverlayPass * m_overlayPass{};
		crg::RunnableGraphPtr m_runnable;
	};
}

#endif
