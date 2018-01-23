/*
This file belongs to Renderer.
See LICENSE file in root folder.
*/
#ifndef ___Renderer_ColourBlendStateAttachment_HPP___
#define ___Renderer_ColourBlendStateAttachment_HPP___
#pragma once

#include "RendererPrerequisites.hpp"

namespace renderer
{
	/**
	*\brief
	*	Wrapper de ColourBlendStateAttachment.
	*\~english
	*\brief
	*	ColourBlendStateAttachment wrapper.
	*/
	class ColourBlendStateAttachment
	{
	public:
		/**
		*\brief
		*	Constructeur
		*\param[in] blendEnable
		*	Le statut d'activation du mélange.
		*\param[in] srcColorBlendFactor
		*	L'opérateur de mélange couleur sur la source.
		*\param[in] dstColorBlendFactor
		*	L'opérateur de mélange couleur sur la destination.
		*\param[in] colorBlendOp
		*	L'opération de mélange couleur.
		*\param[in] srcAlphaBlendFactor
		*	L'opérateur de mélange alpha sur la source.
		*\param[in] dstAlphaBlendFactor
		*	L'opérateur de mélange alpha sur la destination.
		*\param[in] alphaBlendOp
		*	L'opération de mélange alpha.
		*\param[in] colorWriteMask
		*	Le masque d'écriture de couleur.
		*/
		ColourBlendStateAttachment( bool blendEnable = false
			, BlendFactor srcColorBlendFactor = BlendFactor::eOne
			, BlendFactor dstColorBlendFactor = BlendFactor::eZero
			, BlendOp colorBlendOp = BlendOp::eAdd
			, BlendFactor srcAlphaBlendFactor = BlendFactor::eOne
			, BlendFactor dstAlphaBlendFactor = BlendFactor::eZero
			, BlendOp alphaBlendOp = BlendOp::eAdd
			, ColourComponentFlags colorWriteMask = ColourComponentFlag::eR
				| ColourComponentFlag::eG
				| ColourComponentFlag::eB
				| ColourComponentFlag::eA );
		/**
		*\return
		*	Le statut d'activation du mélange.
		*/
		inline bool isBlendEnabled()const
		{
			return m_blendEnable;
		}
		/**
		*\return
		*	L'opérateur de mélange couleur sur la source.
		*/
		inline BlendFactor getSrcColourBlendFactor()const
		{
			return m_srcColorBlendFactor;
		}
		/**
		*\return
		*	L'opérateur de mélange couleur sur la destination.
		*/
		inline BlendFactor getDstColourBlendFactor()const
		{
			return m_dstColorBlendFactor;
		}
		/**
		*\return
		*	L'opération de mélange couleur.
		*/
		inline BlendOp getColourBlendOp()const
		{
			return m_colorBlendOp;
		}
		/**
		*\return
		*	L'opérateur de mélange alpha sur la source.
		*/
		inline BlendFactor getSrcAlphaBlendFactor()const
		{
			return m_srcAlphaBlendFactor;
		}
		/**
		*\return
		*	L'opérateur de mélange alpha sur la destination.
		*/
		inline BlendFactor getDstAlphaBlendFactor()const
		{
			return m_dstAlphaBlendFactor;
		}
		/**
		*\return
		*	L'opération de mélange alpha.
		*/
		inline BlendOp getAlphaBlendOp()const
		{
			return m_alphaBlendOp;
		}
		/**
		*\return
		*	Le masque d'écriture de couleur.
		*/
		inline ColourComponentFlags getColourWriteMask()const
		{
			return m_colorWriteMask;
		}

	private:
		bool m_blendEnable;
		BlendFactor m_srcColorBlendFactor;
		BlendFactor m_dstColorBlendFactor;
		BlendOp m_colorBlendOp;
		BlendFactor m_srcAlphaBlendFactor;
		BlendFactor m_dstAlphaBlendFactor;
		BlendOp m_alphaBlendOp;
		ColourComponentFlags m_colorWriteMask;
	};
}

#endif
