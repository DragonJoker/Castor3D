/*
This file belongs to RendererLib.
See LICENSE file in root folder.
*/
#ifndef ___Renderer_VertexInputBindingDescription_HPP___
#define ___Renderer_VertexInputBindingDescription_HPP___
#pragma once

#include "RendererPrerequisites.hpp"

namespace renderer
{
	/**
	*\~english
	*\brief
	*	Structure specifying vertex input binding description.
	*\~french
	*\brief
	*	Structure définissant la description d'une attache d'entrée de sommet.
	*/
	struct VertexInputBindingDescription
	{
		/**
		*\~english
		*\brief
		*	The binding slot that this structure describes.
		*\~french
		*\brief
		*	Le point d'attache que cette structure décrit.
		*/
		uint32_t binding;
		/**
		*\~english
		*\brief
		*	The distance in bytes between two consecutives elements whithin the buffer.
		*\~french
		*\brief
		*	La distance en octets séparant deux éléments consécutifs dans le tampon.
		*/
		uint32_t stride;
		/**
		*\~english
		*\brief
		*	Specifies whether vertex attribute addressing is a function of the vertex index or of the instance index.
		*\~french
		*\brief
		*	Définit si l'adressage d'attribut de sommet est une fonction de l'indice de sommet ou de l'indice d'instance.
		*/
		VertexInputRate inputRate;
	};
}

#endif
