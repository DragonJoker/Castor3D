/*
See LICENSE file in root folder
*/
#ifndef ___C3D_RENDER_LOOP_SYNC_H___
#define ___C3D_RENDER_LOOP_SYNC_H___

#include "RenderLoop.hpp"

namespace castor3d
{
	/*!
	\author 	Sylvain DOREMUS
	\date 		14/10/2015
	\version	0.8.0
	\~english
	\brief		Synchronous render loop.
	\~french
	\brief		Boucle de rendu synchrone.
	*/
	class RenderLoopSync
		: public RenderLoop
	{
	public:
		/**
		 *\~english
		 *\brief		Constructor.
		 *\param[in]	engine		The engine.
		 *\param[in]	p_wantedFPS		The wanted FPS count.
		 *\~french
		 *\brief		Constructeur.
		 *\param[in]	engine		Le moteur.
		 *\param[in]	p_wantedFPS		Le nombre voulu du FPS.
		 */
		C3D_API RenderLoopSync( Engine & engine, uint32_t p_wantedFPS = 100 );
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
		 *\copydoc		castor3d::RenderLoop::Pause
		 */
		C3D_API void pause()override;
		/**
		 *\copydoc		castor3d::RenderLoop::Resume
		 */
		C3D_API void resume()override;
		/**
		 *\copydoc		castor3d::RenderLoop::endRendering
		 */
		C3D_API void endRendering()override;

	private:
		/**
		 *\copydoc		castor3d::RenderLoop::doCreateMainContext
		 */
		C3D_API ContextSPtr doCreateMainContext( RenderWindow & p_window )override;

	private:
		//!\~english Tells if the render loop is active.	\~french Dit si la boucle de rendu est active.
		bool m_active;
	};
}

#endif
