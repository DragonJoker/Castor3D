/*
This file belongs to RendererLib.
See LICENSE file in root folder.
*/
#ifndef ___Renderer_VertexInputAttributeDescription_HPP___
#define ___Renderer_VertexInputAttributeDescription_HPP___
#pragma once

#include "RendererPrerequisites.hpp"

namespace renderer
{
	/**
	*\~english
	*\brief
	*	Structure specifying vertex input attribute description.
	*\~french
	*\brief
	*	Structure définissant la description d'un attribut d'entrée de sommet.
	*/
	struct VertexInputAttributeDescription
	{
		/**
		*\~english
		*	The shader binding location number for this attribute.
		*\~french
		*\brief
		*	Le numéro de la position de l'attribut dans le shader.
		*/
		uint32_t location;
		/**
		*\~english
		*\brief
		*	The binding number which this attribute takes its data from.
		*\~french
		*\brief
		*	Le numéro d'attache depuis laquelle cet attribut récupère ses données.
		*/
		uint32_t binding;
		/**
		*\~english
		*	The type and size of the attribute data.
		*\~french
		*\brief
		*	Le type et la taille des données de l'attribut.
		*/
		Format format;
		/**
		*\~english
		*\brief
		*	A byte offset of this attribute relative to the start of an element in the vertex input binding.
		*\~french
		*\brief
		*	Un offset en octets pour cet attribut, relatif au début d'un élément dans l'attache d'entrée de sommets.
		*/
		uint32_t offset;
	};
}

#endif
