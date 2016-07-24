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
		 *\param[in]	p_wantedFPS		The wanted FPS count.
		 *\~french
		 *\brief		Constructeur.
		 *\param[in]	p_engine		Le moteur.
		 *\param[in]	p_wantedFPS		Le nombre voulu du FPS.
		 */
		C3D_API RenderLoopSync( Engine & p_engine, uint32_t p_wantedFPS = 100 );
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
