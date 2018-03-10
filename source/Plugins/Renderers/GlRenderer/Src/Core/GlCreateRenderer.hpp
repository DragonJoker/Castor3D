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
	*\~french
	*\brief
	*	Crée un renderer OpenGL.
	*\param[in] configuration
	*	La configuration de création.
	*\~english
	*\brief
	*	Creates an OpenGL renderer.
	*\param[in] configuration
	*	The creation options.
	*/
	GlRenderer_API renderer::Renderer * createRenderer( renderer::Renderer::Configuration const & configuration );
}
