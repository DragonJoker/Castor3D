/*
This file belongs to Renderer.
See LICENSE file in root folder.
*/
#ifndef ___Renderer_StencilOpState_HPP___
#define ___Renderer_StencilOpState_HPP___
#pragma once

#include "RendererPrerequisites.hpp"

namespace renderer
{
	/**
	*\~french
	*\brief
	*	Etat puor une opération stencil.
	*/
	class StencilOpState
	{
	public:
		/**
		*\brief
		*	Constructeur
		*\param[in] failOp
		*	L'opération lors d'un échec du test de stencil.
		*\param[in] passOp
		*	L'opération lors de la réussite des tests stencil et profondeur.
		*\param[in] depthFailOp
		*	L'opération lors d'un échec du test de profondeur.
		*\param[in] compareOp
		*	L'opérateur de comparaison stencil.
		*\param[in] compareMask
		*	Le masque appliqué à la valeur lue pour comparaison.
		*\param[in] writeMask
		*	Le masque appliqué à la valeur écrite.
		*\param[in] reference
		*	La valeur de référence lors des comparaisons stencil.
		*/
		StencilOpState( StencilOp failOp = StencilOp::eKeep
			, StencilOp passOp = StencilOp::eKeep
			, StencilOp depthFailOp = StencilOp::eKeep
			, CompareOp compareOp = CompareOp::eAlways
			, uint32_t compareMask = 0xFFFFFFFFu
			, uint32_t writeMask = 0xFFFFFFFFu
			, uint32_t reference = 0u );
		/**
		*\return
		*	L'opération lors d'un échec du test de stencil.
		*/
		inline StencilOp getFailOp()const
		{
			return m_failOp;
		}
		/**
		*\return
		*	L'opération lors de la réussite des tests stencil et profondeur.
		*/
		inline StencilOp getPassOp()const
		{
			return m_passOp;
		}
		/**
		*\return
		*	L'opération lors d'un échec du test de profondeur.
		*/
		inline StencilOp getDepthFailOp()const
		{
			return m_depthFailOp;
		}
		/**
		*\return
		*	L'opérateur de comparaison stencil.
		*/
		inline CompareOp getCompareOp()const
		{
			return m_compareOp;
		}
		/**
		*\return
		*	Le masque appliqué à la valeur lue pour comparaison.
		*/
		inline uint32_t getCompareMask()const
		{
			return m_compareMask;
		}
		/**
		*\return
		*	Le masque appliqué à la valeur écrite.
		*/
		inline uint32_t getWriteMask()const
		{
			return m_writeMask;
		}
		/**
		*\return
		*	La valeur de référence lors des comparaisons stencil.
		*/
		inline uint32_t getReference()const
		{
			return m_reference;
		}

	private:
		StencilOp m_failOp;
		StencilOp m_passOp;
		StencilOp m_depthFailOp;
		CompareOp m_compareOp;
		uint32_t m_compareMask;
		uint32_t m_writeMask;
		uint32_t m_reference;
	};
}

#endif
