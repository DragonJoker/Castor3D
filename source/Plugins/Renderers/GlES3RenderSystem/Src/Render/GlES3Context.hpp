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
#ifndef ___C3DGLES3_CONTEXT_H___
#define ___C3DGLES3_CONTEXT_H___

#include "Miscellaneous/GlES3Query.hpp"

#include <Render/Context.hpp>

namespace GlES3Render
{
	class GlES3Context
		: public Castor3D::Context
		, public Holder
	{
	public:
		GlES3Context( GlES3RenderSystem & p_renderSystem, OpenGlES3 & p_gl );
		~GlES3Context();

		GlES3ContextImpl & GetImpl();

	private:
		/**
		 *\copydoc		Castor3D::Context::DoInitialise
		 */
		bool DoInitialise()override;
		/**
		 *\copydoc		Castor3D::Context::DoCleanup
		 */
		void DoCleanup()override;
		/**
		 *\copydoc		Castor3D::Context::DoDestroy
		 */
		void DoDestroy()override;
		/**
		 *\copydoc		Castor3D::Context::DoSetCurrent
		 */
		void DoSetCurrent()override;
		/**
		 *\copydoc		Castor3D::Context::DoEndCurrent
		 */
		void DoEndCurrent()override;
		/**
		 *\copydoc		Castor3D::Context::DoSwapBuffers
		 */
		void DoSwapBuffers()override;

	private:
		std::unique_ptr< GlES3ContextImpl > m_implementation;
		Castor3D::GpuInformations m_gpuInformations;
	};
}

#endif
