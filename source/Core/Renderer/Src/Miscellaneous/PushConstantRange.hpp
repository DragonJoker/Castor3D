/*
This file belongs to Renderer.
See LICENSE file in root folder.
*/
#ifndef ___Renderer_PushConstantRange_HPP___
#define ___Renderer_PushConstantRange_HPP___
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
	struct PushConstantRange
	{
		/**
		*\~english
		*\brief
		*	A set of stage flags describing the shader stages that will access a range of push constants.
		*\remarks
		*	If a particular stage is not included in the range, then accessing members of that range of push constants from the corresponding shader stage will result in undefined data being read.
		*\~french
		*\brief
		*	Un ensemble d'indicateurs de niveaux shader décrivant les niveaux de shader qui accèderont à l'intervalle de push constants.
		*\remarks
		*	Si un niveau particulier n'est pas inclus dans l'intervalle, alors l'accès de cet intervalle de push constants dans le niveau de shader correspondant résultera en une lecture de données indéfinies.
		*/
		ShaderStageFlags stageFlags;
		/**
		*\~english
		*\brief
		*	The start offset of the range, in bytes. Must be a multiple of 4.
		*\~french
		*\brief
		*	Le décalage de départ de l'intervalle, en octets. Doit être un multiple de 4.
		*/
		uint32_t offset;
		/**
		*\~english
		*\brief
		*	The size of the range, in bytes. Must be a multiple or 4.
		*\~french
		*\brief
		*	La taille de l'intervalle, en octets. Doit être un multiple de 4.
		*/
		uint32_t size;
	};
}

#endif
