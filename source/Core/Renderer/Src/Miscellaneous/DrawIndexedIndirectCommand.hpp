/*
This file belongs to RendererLib.
See LICENSE file in root folder.
*/
#ifndef ___Renderer_DrawIndexedIndirectCommand_HPP___
#define ___Renderer_DrawIndexedIndirectCommand_HPP___
#pragma once

#include "RendererPrerequisites.hpp"

namespace renderer
{
	/**
	*\~english
	*\brief
	*	Structure specifying a draw indexed indirect command.
	*\~french
	*\brief
	*	Structure décrivant une commande de draw indirect indexée.
	*/
	struct DrawIndexedIndirectCommand
	{
		/**
		*\~english
		*\brief
		*	The number of vertices to draw.
		*\~french
		*\brief
		*	Le nombre de sommets à dessiner.
		*/
		uint32_t indexCount;
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
		*	The base index within the index buffer.
		*\~french
		*\brief
		*	L'indice de base dans le tampon d'indices.
		*/
		uint32_t firstIndex;
		/**
		*\~english
		*\brief
		*	The value added to the vertex index before indexing in the vertex buffer.
		*\~french
		*\brief
		*	La valeur ajoutée à l'indice du sommet avant d'indexer le tampon de sommets.
		*/
		int32_t vertexOffset;
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
