/*
This file belongs to VkLib.
See LICENSE file in root folder
*/
#pragma once

#include "OpenGLFunctions.hpp"

namespace gl_renderer
{
	/**
	*\~french
	*\brief
	*	Loads OpenGL functions.
	*\~english
	*\brief
	*	Charge les fonctions d'OpenGL.
	*/
	class OpenGLLibrary
	{
	public:
		OpenGLLibrary( OpenGLLibrary const & ) = delete;
		OpenGLLibrary & operator =( OpenGLLibrary const & ) = delete;
		OpenGLLibrary( OpenGLLibrary && ) = default;
		OpenGLLibrary & operator =( OpenGLLibrary && ) = default;
		/**
		*\~english
		*\brief
		*	Constructor.
		*	Loads the functions.
		*\~french
		*\brief
		*	Constructeur.
		*	Charge les fonctions.
		*/
		OpenGLLibrary();
	};
}
