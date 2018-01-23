/*
This file belongs to Renderer.
See LICENSE file in root folder.
*/
#ifndef ___Renderer_Semaphore_HPP___
#define ___Renderer_Semaphore_HPP___
#pragma once

#include "RendererPrerequisites.hpp"

namespace renderer
{
	/**
	*\brief
	*	Classe de Semaphore.
	*\remarks
	*	Un sémaphore est un élément de synchronisation servant pour les files.
	*/
	class Semaphore
	{
	protected:
		/**
		*\brief
		*	Constructeur.
		*\param[in] device
		*	Le LogicalDevice parent.
		*/
		explicit Semaphore( Device const & device );

	public:
		/**
		*\~english
		*\brief
		*	Destructor.
		*\~french
		*\brief
		*	Destructeur.
		*/
		virtual ~Semaphore() = default;
	};
}

#endif
