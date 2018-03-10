/*
This file belongs to RendererLib.
See LICENSE file in root folder.
*/
#ifndef ___Renderer_SharingMode_HPP___
#define ___Renderer_SharingMode_HPP___
#pragma once

namespace renderer
{
	/*
	*\~french
	*\brief
	*	Les tampons et les images sont créés en utilisant un mode de partage controlant comment une file peut y accéder.
	*\~english
	*\brief
	*	Buffer and image objects are created with a sharing mode controlling how they can be accessed from queues.
	*/
	enum class SharingMode
	{
		/**
		*\~french
		*\brief
		*	Définit que chaque intervalle ou sous-ressource de l'objet sera exclusif à une file à la fois.
		*\~english
		*\brief
		*	Specifies that access to any range or image subresource of the object will be exclusive to a single queue family at a time.
		*/
		eExclusive = 0,
		/**
		*\~french
		*\brief
		*	Définit les accès concurrents à chaque intervalle ou sous-ressource d'image de l'objet depuis plusieurs files sera supporté.
		*\~english
		*\brief
		*	Specifies that concurrent access to any range or image subresource of the object from multiple queue families is supported.
		*/
		eConcurrent = 1,
	};
}

#endif
