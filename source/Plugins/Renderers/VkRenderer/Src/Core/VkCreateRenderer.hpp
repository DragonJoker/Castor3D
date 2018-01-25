/**
*\file
*	VkCreateRenderer.h
*\author
*	Sylvain Doremus
*/
#pragma once

#include <Core/Renderer.hpp>

#if defined( _WIN32 ) && !defined( VkRenderer_STATIC_LIB )
#	ifdef VkRenderer_EXPORTS
#		define VkRenderer_API __declspec( dllexport )
#	else
#		define VkRenderer_API __declspec( dllimport )
#	endif
#else
#	define VkRenderer_API
#endif

extern "C"
{
	/**
	*\brief
	*	Crée un renderer Vulkan.
	*\param[in] connection
	*	La connection avec la fenêtre.
	*/
	VkRenderer_API renderer::Renderer * createRenderer();
}
