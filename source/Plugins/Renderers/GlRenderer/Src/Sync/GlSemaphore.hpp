/*
This file belongs to Renderer.
See LICENSE file in root folder.
*/
#pragma once

#include "GlRendererPrerequisites.hpp"

#include <Sync/Semaphore.hpp>

namespace gl_renderer
{
	/**
	*\brief
	*	Classe de Semaphore.
	*\remarks
	*	Un sémaphore est un élément de synchronisation servant pour les files.
	*/
	class Semaphore
		: public renderer::Semaphore
	{
	public:
		/**
		*\brief
		*	Constructeur.
		*\param[in] device
		*	Le LogicalDevice parent.
		*/
		explicit Semaphore( renderer::Device const & device );
		/**
		*\brief
		*	Conversion implicite vers VkSemaphore.
		*/
		inline GLuint getSemaphore()const
		{
			return m_semaphore;
		}

	private:
		GLuint m_semaphore;
	};
}
