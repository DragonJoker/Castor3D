/*
This file belongs to Renderer.
See LICENSE file in root folder
*/
#pragma once

#include "GlCommandBase.hpp"

namespace gl_renderer
{
	/**
	*\brief
	*	Commande de dessin index�.
	*/
	class DrawIndexedCommand
		: public CommandBase
	{
	public:
	public:
		/**
		*\brief
		*	Constructeur.
		*\param[in] indexCount
		*	Nombre d'indices.
		*\param[in] instCount
		*	Nombre d'instances.
		*\param[in] firstIndex
		*	Index du premier indice.
		*\param[in] vertexOffset
		*	La valeur ajout�e � l'indice du sommet avant d'indexer le tampon de sommets.
		*\param[in] firstInstance
		*	Index de la premi�re instance.
		*/
		DrawIndexedCommand( uint32_t indexCount
			, uint32_t instCount
			, uint32_t firstIndex
			, uint32_t vertexOffset
			, uint32_t firstInstance
			, renderer::PrimitiveTopology mode
			, renderer::IndexType type );

		void apply()const override;
		CommandPtr clone()const override;

	private:
		uint32_t m_indexCount;
		uint32_t m_instCount;
		size_t m_firstIndex;
		uint32_t m_vertexOffset;
		uint32_t m_firstInstance;
		GlPrimitiveTopology m_mode;
		GlIndexType m_type;
	};
}
