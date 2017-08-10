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
#ifndef ___TRS_CONTEXT_H___
#define ___TRS_CONTEXT_H___

#include "Miscellaneous/TestQuery.hpp"

#include <Render/Context.hpp>

namespace TestRender
{
	class TestContext
		: public castor3d::Context
	{
	public:
		TestContext( TestRenderSystem & p_renderSystem );
		virtual ~TestContext();

	private:
		/**
		 *\copydoc		castor3d::Context::doInitialise
		 */
		bool doInitialise()override;
		/**
		 *\copydoc		castor3d::Context::doCleanup
		 */
		void doCleanup()override;
		/**
		 *\copydoc		castor3d::Context::doDestroy
		 */
		void doDestroy()override;
		/**
		 *\copydoc		castor3d::Context::doSetCurrent
		 */
		void doSetCurrent()override;
		/**
		 *\copydoc		castor3d::Context::doEndCurrent
		 */
		void doEndCurrent()override;
		/**
		 *\copydoc		castor3d::Context::doSwapBuffers
		 */
		void doSwapBuffers()override;
		/**
		 *\copydoc		castor3d::Context::doMemoryBarrier
		 */
		void doMemoryBarrier( castor3d::MemoryBarriers const & p_barriers )override;
	};
}

#endif
