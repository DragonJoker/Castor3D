/*
See LICENSE file in root folder
*/
#ifndef ___GL_CONTEXT_H___
#define ___GL_CONTEXT_H___

#include "Miscellaneous/GlQuery.hpp"

#include <Render/Context.hpp>

namespace GlRender
{
	class GlContext
		: public castor3d::Context
		, public Holder
	{
	public:
		GlContext( GlRenderSystem & renderSystem, OpenGl & p_gl );
		virtual ~GlContext();

		GlContextImpl & getImpl();

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

	private:
		std::unique_ptr< GlContextImpl > m_implementation;
		GlRenderSystem * m_glRenderSystem;
	};
}

#endif
