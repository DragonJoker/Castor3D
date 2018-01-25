/*
See LICENSE file in root folder
*/
#ifndef ___VK_RENDER_SYSTEM_PREREQUISITES_H___
#define ___VK_RENDER_SYSTEM_PREREQUISITES_H___

#include <CastorUtilsPrerequisites.hpp>
#include <Castor3DPrerequisites.hpp>
#include <VkRendererPrerequisites.hpp>

#ifdef CASTOR_PLATFORM_WINDOWS
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
