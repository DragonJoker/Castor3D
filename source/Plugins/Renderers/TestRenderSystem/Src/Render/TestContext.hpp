/* See LICENSE file in root folder */
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
		explicit TestContext( TestRenderSystem & p_renderSystem );
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
