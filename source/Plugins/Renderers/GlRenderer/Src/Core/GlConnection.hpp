/*
This file belongs to Renderer.
See LICENSE file in root folder
*/
#pragma once

#include "GlRendererPrerequisites.hpp"

#include <Core/Connection.hpp>

namespace gl_renderer
{
	/**
	*\brief
	*	Encapsulation de la connexion à une application dépendant de la plateforme.
	*/
	class Connection
		: public renderer::Connection
	{
	public:
		/**
		*\brief
		*	Constructeur.
		*\param[in] renderer
		*	L'instance de Renderer.
		*\param[in] deviceIndex
		*	L'indice du périphérique physique.
		*\param[in] handle
		*	Le descripteur de la fenêtre.
		*/
		Connection( renderer::Renderer const & renderer
			, uint32_t deviceIndex
			, renderer::WindowHandle && handle );
	};
}
