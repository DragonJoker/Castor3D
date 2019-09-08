/*
See LICENSE file in root folder
*/
#ifndef ___VK_RENDER_SYSTEM_PREREQUISITES_H___
#define ___VK_RENDER_SYSTEM_PREREQUISITES_H___

#include <CastorUtils/CastorUtilsPrerequisites.hpp>
#include <Castor3D/Castor3DPrerequisites.hpp>

#ifdef CU_PlatformWindows
#	ifdef VkRenderSystem_EXPORTS
#		define C3D_Vk_API __declspec( dllexport )
#	else
#		define C3D_Vk_API __declspec( dllimport )
#	endif
#else
#	define C3D_Vk_API
#endif

namespace VkRender
{
	class RenderSystem;
}

#endif
