/*
This file belongs to Renderer.
See LICENSE file in root folder.
*/
#ifndef ___Renderer_DepthStencilState_HPP___
#define ___Renderer_DepthStencilState_HPP___
#pragma once

#include "StencilOpState.hpp"

namespace renderer
{
	/**
	*\brief
	*	L'état de stencil et profondeur.
	*/
	class DepthStencilState
	{
	public:
		/**
		*\brief
		*	Constructeur.
		*\param[in] flags
		*	Les indicateurs de l'état.
		*\param[in] depthTestEnable
		*	Le statut d'activation du test de profondeur.
		*\param[in] depthWriteEnable
		*	Le statut d'activation de l'écriture de profondeur.
		*\param[in] depthCompareOp
		*	L'opérateur de comparaison de profondeur.
		*\param[in] depthBoundsTestEnable
		*	Le statut d'activation du test des bornes de profondeur.
		*\param[in] stencilTestEnable
		*	Le statut d'activation du test de stencil.
		*\param[in] front
		*	L'état de stencil pour les faces avant.
		*\param[in] back
		*	L'état de stencil pour les faces arrière.
		*\param[in] minDepthBounds
		*	La borne profondeur minimale.
		*\param[in] maxDepthBounds
		*	La borne profondeur maximale.
		*/
		DepthStencilState( DepthStencilStateFlags flags = DepthStencilStateFlags{}
			, bool depthTestEnable = true
			, bool depthWriteEnable = true
			, CompareOp depthCompareOp = CompareOp::eLess
			, bool depthBoundsTestEnable = false
			, bool stencilTestEnable = false
			, StencilOpState const & front = StencilOpState{}
			, StencilOpState const & back = StencilOpState{}
			, float minDepthBounds = 0.0f
			, float maxDepthBounds = 1.0f );
		/**
		*\return
		*	Les indicateurs de l'état.
		*/
		inline DepthStencilStateFlags getFlags()const
		{
			return m_flags;
		}
		/**
		*\return
		*	Le statut d'activation du test de profondeur.
		*/
		inline bool isDepthTestEnabled()const
		{
			return m_depthTestEnable;
		}
		/**
		*\return
		*	Le statut d'activation de l'écriture de profondeur.
		*/
		inline bool isDepthWriteEnabled()const
		{
			return m_depthWriteEnable;
		}
		/**
		*\return
		*	L'opérateur de comparaison de profondeur.
		*/
		inline CompareOp getDepthCompareOp()const
		{
			return m_depthCompareOp;
		}
		/**
		*\return
		*	Le statut d'activation du test des bornes de profondeur.
		*/
		inline bool isDepthBoundsTestEnabled()const
		{
			return m_depthBoundsTestEnable;
		}
		/**
		*\return
		*	Le statut d'activation du test de stencil.
		*/
		inline bool isStencilTestEnabled()const
		{
			return m_stencilTestEnable;
		}
		/**
		*\return
		*	L'état de stencil pour les faces avant.
		*/
		inline StencilOpState getFrontStencilOp()const
		{
			return m_front;
		}
		/**
		*\return
		*	L'état de stencil pour les faces arrière.
		*/
		inline StencilOpState getBackStencilOp()const
		{
			return m_back;
		}
		/**
		*\return
		*	La borne profondeur minimale.
		*/
		inline float getMinDepthBounds()const
		{
			return m_minDepthBounds;
		}
		/**
		*\return
		*	La borne profondeur maximale.
		*/
		inline float getMaxDepthBounds()const
		{
			return m_maxDepthBounds;
		}

	private:
		DepthStencilStateFlags m_flags;
		bool m_depthTestEnable;
		bool m_depthWriteEnable;
		CompareOp m_depthCompareOp;
		bool m_depthBoundsTestEnable;
		bool m_stencilTestEnable;
		StencilOpState m_front;
		StencilOpState m_back;
		float m_minDepthBounds;
		float m_maxDepthBounds;
	};
}

#endif
