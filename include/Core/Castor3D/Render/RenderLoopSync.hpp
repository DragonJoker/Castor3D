/*
See LICENSE file in root folder
*/
#ifndef ___C3D_RENDER_LOOP_SYNC_H___
#define ___C3D_RENDER_LOOP_SYNC_H___

#include "Castor3D/Render/RenderLoop.hpp"

namespace castor3d
{
	class RenderLoopSync
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
		C3D_API RenderLoopSync( Engine & engine
			, uint32_t wantedFPS );
		/**
		 *\~english
		 *\brief		Destructor.
		 *\~french
		 *\brief		Destructeur.
		 */
		C3D_API ~RenderLoopSync();
		/**
		 *\copydoc		castor3d::RenderLoop::beginRendering
		 */
		C3D_API void beginRendering()override;
		/**
		 *\copydoc		castor3d::RenderLoop::renderSyncFrame
		 */
		C3D_API void renderSyncFrame()override;
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
		 *\copydoc		castor3d::RenderLoop::doCreateMainDevice
		 */
		C3D_API RenderDeviceSPtr doCreateMainDevice( ashes::WindowHandle handle
			, RenderWindow & window )override;

	private:
		//!\~english Tells if the render loop is active.	\~french Dit si la boucle de rendu est active.
		bool m_active;
	};
}

#endif
