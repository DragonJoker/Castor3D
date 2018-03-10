/*
This file belongs to RendererLib.
See LICENSE file in root folder.
*/
#ifndef ___Renderer_SpecialisationMapEntry_HPP___
#define ___Renderer_SpecialisationMapEntry_HPP___
#pragma once

#include "RendererPrerequisites.hpp"

namespace renderer
{
	/**
	*\~english
	*\brief
	*	Specifies a push constant range.
	*\~french
	*\brief
	*	Définit un intervalle de push constants.
	*/
	struct SpecialisationMapEntry
	{
		/**
		*\~english
		*\brief
		*	The ID of the specialization constant in SPIR-V.
		*\~french
		*\brief
		*	L'ID de la constante de spécialsation en SPIR-V.
		*/
		uint32_t constantID;
		/**
		*\~english
		*\brief
		*	The byte offset of the specialization constant value within the supplied data buffer.
		*\~french
		*\brief
		*	Le décalage de la valeur de la constante de spécialisation dans le tampon de données.
		*/
		uint32_t offset;
		/**
		*\~english
		*\brief
		*	The variable format, the size of the variable is deduced from that.
		*\~french
		*\brief
		*	Le format de la variable, la taille de la variable est déduite de là.
		*/
		ConstantFormat format;
		/**
		*\~english
		*\brief
		*	The dimensions of the array, if the constant is an array, used in OpenGL renderer.
		*\~french
		*\brief
		*	Les dimensions du tableau, si la constante est un tableau, utilisé dans le renderer OpenGL.
		*/
		uint32_t arraySize{ 1u };
	};
}

#endif
