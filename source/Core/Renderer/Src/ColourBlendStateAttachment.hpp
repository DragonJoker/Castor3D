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
		*	Le statut d'activation du m�lange.
		*\param[in] srcColorBlendFactor
		*	L'op�rateur de m�lange couleur sur la source.
		*\param[in] dstColorBlendFactor
		*	L'op�rateur de m�lange couleur sur la destination.
		*\param[in] colorBlendOp
		*	L'op�ration de m�lange couleur.
		*\param[in] srcAlphaBlendFactor
		*	L'op�rateur de m�lange alpha sur la source.
		*\param[in] dstAlphaBlendFactor
		*	L'op�rateur de m�lange alpha sur la destination.
		*\param[in] alphaBlendOp
		*	L'op�ration de m�lange alpha.
		*\param[in] colorWriteMask
		*	Le masque d'�criture de couleur.
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
		*	Le statut d'activation du m�lange.
		*/
		inline bool isBlendEnabled()const
		{
			return m_blendEnable;
		}
		/**
		*\return
		*	L'op�rateur de m�lange couleur sur la source.
		*/
		inline BlendFactor getSrcColourBlendFactor()const
		{
			return m_srcColorBlendFactor;
		}
		/**
		*\return
		*	L'op�rateur de m�lange couleur sur la destination.
		*/
		inline BlendFactor getDstColourBlendFactor()const
		{
			return m_dstColorBlendFactor;
		}
		/**
		*\return
		*	L'op�ration de m�lange couleur.
		*/
		inline BlendOp getColourBlendOp()const
		{
			return m_colorBlendOp;
		}
		/**
		*\return
		*	L'op�rateur de m�lange alpha sur la source.
		*/
		inline BlendFactor getSrcAlphaBlendFactor()const
		{
			return m_srcAlphaBlendFactor;
		}
		/**
		*\return
		*	L'op�rateur de m�lange alpha sur la destination.
		*/
		inline BlendFactor getDstAlphaBlendFactor()const
		{
			return m_dstAlphaBlendFactor;
		}
		/**
		*\return
		*	L'op�ration de m�lange alpha.
		*/
		inline BlendOp getAlphaBlendOp()const
		{
			return m_alphaBlendOp;
		}
		/**
		*\return
		*	Le masque d'�criture de couleur.
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
