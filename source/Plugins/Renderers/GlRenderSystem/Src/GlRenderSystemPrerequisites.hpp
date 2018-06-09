/*
See LICENSE file in root folder
*/
#ifndef ___GL_RENDER_SYSTEM_PREREQUISITES_H___
#define ___GL_RENDER_SYSTEM_PREREQUISITES_H___

#include <CastorUtilsPrerequisites.hpp>
#include <Castor3DPrerequisites.hpp>
#include <GlRendererPrerequisites.hpp>

#ifdef CASTOR_PLATFORM_WINDOWS
#	ifdef GlRenderSystem_EXPORTS
#		define C3D_Gl_API __declspec( dllexport )
#	else
#		define C3D_Gl_API __declspec( dllimport )
#	endif
#else
#	define C3D_Gl_API
#endif

namespace GlRender
{
	class RenderSystem;
}

#endif
