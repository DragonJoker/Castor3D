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
#ifndef ___GL_CONTEXT_H___
#define ___GL_CONTEXT_H___

#include "Miscellaneous/GlQuery.hpp"

#include <Render/Context.hpp>

namespace GlRender
{
	class GlContext
		: public Castor3D::Context
		, public Holder
	{
	public:
		GlContext( GlRenderSystem & p_renderSystem, OpenGl & p_gl );
		virtual ~GlContext();

		GlContextImpl & GetImpl();

	private:
		/**
		 *\copydoc		Castor3D::Context::DoInitialise
		 */
		virtual bool DoInitialise();
		/**
		 *\copydoc		Castor3D::Context::DoCleanup
		 */
		virtual void DoCleanup();
		/**
		 *\copydoc		Castor3D::Context::DoDestroy
		 */
		virtual void DoDestroy();
		/**
		 *\copydoc		Castor3D::Context::DoSetCurrent
		 */
		virtual void DoSetCurrent();
		/**
		 *\copydoc		Castor3D::Context::DoEndCurrent
		 */
		virtual void DoEndCurrent();
		/**
		 *\copydoc		Castor3D::Context::DoSwapBuffers
		 */
		virtual void DoSwapBuffers();

	private:
		std::unique_ptr< GlContextImpl > m_implementation;
		GlRenderSystem * m_glRenderSystem;
	};
}

#endif
