/*
See LICENSE file in root folder
*/
#ifndef ___GL4_RENDER_SYSTEM_PREREQUISITES_H___
#define ___GL4_RENDER_SYSTEM_PREREQUISITES_H___

#include <CastorUtilsPrerequisites.hpp>
#include <Castor3DPrerequisites.hpp>
#include <GlRendererPrerequisites.hpp>

#ifdef CASTOR_PLATFORM_WINDOWS
#	ifdef Gl4RenderSystem_EXPORTS
#		define C3D_Gl4_API __declspec( dllexport )
#	else
#		define C3D_Gl4_API __declspec( dllimport )
#	endif
#else
#	define C3D_Gl4_API
#endif

namespace Gl4Render
{
	class RenderSystem;
}

#endif
