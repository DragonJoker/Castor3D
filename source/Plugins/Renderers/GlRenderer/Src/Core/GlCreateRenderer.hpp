/**
*\file
*	GlCreateRenderer.h
*\author
*	Sylvain Doremus
*/
#pragma once

#include <Core/Renderer.hpp>

#if defined( _WIN32 ) && !defined( GlRenderer_STATIC_LIB )
#	ifdef GlRenderer_EXPORTS
#		define GlRenderer_API __declspec( dllexport )
#	else
#		define GlRenderer_API __declspec( dllimport )
#	endif
#else
#	define GlRenderer_API
#endif

extern "C"
{
	/**
	*\brief
	*	Crée un renderer OpenGL.
	*\param[in] connection
	*	La connection avec la fenêtre.
	*/
	GlRenderer_API renderer::Renderer * createRenderer();
}
