/*
See LICENSE file in root folder
*/
#ifndef ___Test_RenderSystemPrerequisites_H___
#define ___Test_RenderSystemPrerequisites_H___

#include <CastorUtilsPrerequisites.hpp>
#include <Castor3DPrerequisites.hpp>
#include <TestRendererPrerequisites.hpp>

#ifdef CU_PlatformWindows
#	ifdef TestRenderSystem_EXPORTS
#		define C3D_Test_API __declspec( dllexport )
#	else
#		define C3D_Test_API __declspec( dllimport )
#	endif
#else
#	define C3D_Test_API
#endif

namespace TestRender
{
	class RenderSystem;
}

#endif
