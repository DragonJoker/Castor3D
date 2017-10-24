/* See LICENSE file in root folder */
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
