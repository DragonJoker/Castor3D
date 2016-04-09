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
#ifndef ___C3D_RENDER_LOOP_SYNC_H___
#define ___C3D_RENDER_LOOP_SYNC_H___

#include "RenderLoop.hpp"

namespace Castor3D
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
		 *\param[in]	p_engine		The engine.
		 *\param[in]	p_renderSystem	The render system.
		 *\param[in]	p_wantedFPS		The wanted FPS count.
		 *\~french
		 *\brief		Constructeur.
		 *\param[in]	p_engine		Le moteur.
		 *\param[in]	p_renderSystem	Le système de rendu.
		 *\param[in]	p_wantedFPS		Le nombre voulu du FPS.
		 */
		C3D_API RenderLoopSync( Engine & p_engine, RenderSystem * p_renderSystem, uint32_t p_wantedFPS = 100 );
		/**
		 *\~english
		 *\brief		Destructor.
		 *\~french
		 *\brief		Destructeur.
		 */
		C3D_API ~RenderLoopSync();

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
		 *\copydoc		Castor3D::RenderLoop::DoPause
		 */
		C3D_API virtual void DoPause();
		/**
		 *\copydoc		Castor3D::RenderLoop::DoResume
		 */
		C3D_API virtual void DoResume();
		/**
		 *\copydoc		Castor3D::RenderLoop::DoEndRendering
		 */
		C3D_API virtual void DoEndRendering();
		/**
		 *\copydoc		Castor3D::RenderLoop::DoCreateMainContext
		 */
		C3D_API virtual ContextSPtr DoCreateMainContext( RenderWindow & p_window );

	private:
		//!\~english Tells if the render loop is active.	\~french Dit si la boucle de rendu est active.
		bool m_active;
	};
}

#endif
