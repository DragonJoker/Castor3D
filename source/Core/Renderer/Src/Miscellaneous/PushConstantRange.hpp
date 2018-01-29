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
	*	D�finit un intervalle de push constants.
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
		*	Un ensemble d'indicateurs de niveaux shader d�crivant les niveaux de shader qui acc�deront � l'intervalle de push constants.
		*\remarks
		*	Si un niveau particulier n'est pas inclus dans l'intervalle, alors l'acc�s de cet intervalle de push constants dans le niveau de shader correspondant r�sultera en une lecture de donn�es ind�finies.
		*/
		ShaderStageFlags stageFlags;
		/**
		*\~english
		*\brief
		*	The start offset of the range, in bytes. Must be a multiple of 4.
		*\~french
		*\brief
		*	Le d�calage de d�part de l'intervalle, en octets. Doit �tre un multiple de 4.
		*/
		uint32_t offset;
		/**
		*\~english
		*\brief
		*	The size of the range, in bytes. Must be a multiple or 4.
		*\~french
		*\brief
		*	La taille de l'intervalle, en octets. Doit �tre un multiple de 4.
		*/
		uint32_t size;
	};
}

#endif
