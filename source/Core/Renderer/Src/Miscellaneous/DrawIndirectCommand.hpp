/*
This file belongs to RendererLib.
See LICENSE file in root folder.
*/
#ifndef ___Renderer_DrawIndirectCommand_HPP___
#define ___Renderer_DrawIndirectCommand_HPP___
#pragma once

#include "RendererPrerequisites.hpp"

namespace renderer
{
	/**
	*\~english
	*\brief
	*	Structure specifying a draw indirect command.
	*\~french
	*\brief
	*	Structure décrivant une commande de draw indirect.
	*/
	struct DrawIndirectCommand
	{
		/**
		*\~english
		*\brief
		*	The number of vertices to draw.
		*\~french
		*\brief
		*	Le nombre de sommets à dessiner.
		*/
		uint32_t vertexCount;
		/**
		*\~english
		*\brief
		*	The number of instances to draw.
		*\~french
		*\brief
		*	Le nombre d'instances à dessiner.
		*/
		uint32_t instanceCount;
		/**
		*\~english
		*\brief
		*	The index of the first vertex to draw.
		*\~french
		*\brief
		*	L'indice du premier sommet à dessiner.
		*/
		uint32_t firstVertex;
		/**
		*\~english
		*\brief
		*	The instance ID of the first instance to draw.
		*\~french
		*\brief
		*	L'ID de la première instance à dessiner.
		*/
		uint32_t firstInstance;
	};
}

#endif
