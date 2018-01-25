/*
See LICENSE file in root folder
*/
#ifndef ___GL_RENDER_SYSTEM_H___
#define ___GL_RENDER_SYSTEM_H___

#include "GlRenderSystemPrerequisites.hpp"

#include <Render/RenderSystem.hpp>

namespace GlRender
{
	class GlRenderSystem
		: public castor3d::RenderSystem
	{
	public:
		explicit GlRenderSystem( castor3d::Engine & engine );
		~GlRenderSystem();

		static castor3d::RenderSystemUPtr create( castor3d::Engine & engine );

	private:
		/**
		 *\copydoc		castor3d::RenderSystem::doInitialise
		 */
		void doInitialise()override;
		/**
		 *\copydoc		castor3d::RenderSystem::doCleanup
		 */
		void doCleanup()override;

	public:
		C3D_Gl_API static castor::String Name;
		C3D_Gl_API static castor::String Type;
	};
}

#endif
