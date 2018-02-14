/*
This file belongs to Renderer.
See LICENSE file in root folder.
*/
#ifndef ___Renderer_InputAssemblyState_HPP___
#define ___Renderer_InputAssemblyState_HPP___
#pragma once

#include "RendererPrerequisites.hpp"

namespace renderer
{
	/**
	*\~english
	*\brief
	*	Specifies parameters of a pipeline input assembly state.
	*\~french
	*\brief
	*	Définit les paramètres d'un état d'assemblage des entrées.
	*/
	class InputAssemblyState
	{
	public:
		/**
		*\~english
		*\brief
		*	Constructor.
		*\param[in] topology
		*	The primitive topology.
		*\param[in] primitiveRestartEnable
		*	Controls wherher a special vertex index value is treated as restarting the assembly of primitives.
		*	This enable only applies to indexed draws.
		*	The special index value is 0xFFFFFFFF if index type is renderer::IndexType::eUInt32 or 0xFFFF if index type is renderer::IndexType::eUInt16.
		*	Doesn't apply to "list" primitive topologies.
		*\~french
		*\brief
		*	Constructeur.
		*\param[in] topology
		*	La topologie d'affichage des sommets affichés via ce pipeline.
		*\param[in] primitiveRestartEnable
		*	Dit si une valeur spéciale d'indice de sommet est traitée comme un redémarrage de l'assemblage des primitives.
		*	Cette activation s'applique uniquement aux dessins indicés.
		*	La valeur spéciale d'indice est 0xFFFFFFFF si le type d'index est renderer::IndexType::eUInt32 ou 0xFFFF si le type d'index est renderer::IndexType::eUInt32.
		*	Ne s'applique pas aux topologies de type "list".
		*/
		InputAssemblyState( PrimitiveTopology topology = PrimitiveTopology::eTriangleList
			, bool primitiveRestartEnable = false );
		/**
		*\~english
		*\return
		*	The primitive topology.
		*\~french
		*\return
		*	La topologie d'affichage des sommets affichés via ce pipeline.
		*/
		inline PrimitiveTopology getTopology()const
		{
			return m_topology;
		}
		/**
		*\~english
		*\return
		*	The primitive topology.
		*\~french
		*	Les indicateurs de l'état.
		*/
		inline bool isPrimitiveRestartEnabled()const
		{
			return m_primitiveRestartEnable;
		}

	private:
		PrimitiveTopology m_topology;
		bool m_primitiveRestartEnable;
		friend bool operator==( InputAssemblyState const & lhs, InputAssemblyState const & rhs );
	};

	inline bool operator==( InputAssemblyState const & lhs, InputAssemblyState const & rhs )
	{
		return lhs.m_topology == rhs.m_topology
			&& lhs.m_primitiveRestartEnable == rhs.m_primitiveRestartEnable;
	}

	inline bool operator!=( InputAssemblyState const & lhs, InputAssemblyState const & rhs )
	{
		return !( lhs == rhs );
	}
}

#endif
