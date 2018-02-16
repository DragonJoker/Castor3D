/*
This file belongs to Renderer.
See LICENSE file in root folder.
*/
#ifndef ___Renderer_TextureAttachment_HPP___
#define ___Renderer_TextureAttachment_HPP___
#pragma once

#include "RendererPrerequisites.hpp"

namespace renderer
{
	/**
	*\~english
	*\brief
	*	An attach to a render pass.
	*\~french
	*\brief
	*	Une attache à une passe de rendu.
	*/
	class RenderPassAttachment
	{
	private:
		inline RenderPassAttachment( uint32_t index, PixelFormat format, bool clear )
			: m_index{ index }
			, m_format{ format }
			, m_clear{ clear }
		{
		}

	public:
		/**
		*\~english
		*\brief
		*	Creates a depth and/or stencil attachment.
		*\param[in] format
		*	The attach pixel format.
		*\param[in] clear
		*	Tells if the attach is cleared when the render pass is started.
		*\~french
		*\brief
		*	Crée une attache de type profondeur et/ou stencil.
		*\param[in] format
		*	Le format des pixels de l'attache.
		*\param[in] clear
		*	Dit si l'attache est vidée lors du démarrage de la passe de rendu.
		*/
		static inline RenderPassAttachment createDepthStencilAttachment( PixelFormat format, bool clear )
		{
			return RenderPassAttachment{ 0u, format, clear };
		}
		/**
		*\~english
		*\brief
		*	Creates a colour attachment.
		*\param[in] index
		*	The attach index.
		*\param[in] format
		*	The attach pixel format.
		*\param[in] clear
		*	Tells if the attach is cleared when the render pass is started.
		*\~french
		*\brief
		*	Crée une attache de type couleur.
		*\param[in] index
		*	L'indice de l'attache.
		*\param[in] format
		*	Le format des pixels de l'attache.
		*\param[in] clear
		*	Dit si l'attache est vidée lors du démarrage de la passe de rendu.
		*/
		static inline RenderPassAttachment createColourAttachment( uint32_t index, PixelFormat format, bool clear )
		{
			return RenderPassAttachment{ index, format, clear };
		}
		/**
		*\~english
		*\return
		*	The attach index.
		*\~french
		*\return
		*	L'indice de l'attache.
		*/
		inline uint32_t getIndex()const
		{
			return m_index;
		}
		/**
		*\~english
		*\return
		*	The attach pixel format.
		*\~french
		*\return
		*	Le format des pixels de l'attache.
		*/
		inline PixelFormat getFormat()const
		{
			return m_format;
		}
		/**
		*\~english
		*\return
		*	Tells if the attach is cleared when the render pass is started.
		*\~french
		*\return
		*	Dit si l'attache est vidée lors du démarrage de la passe de rendu.
		*/
		inline bool getClear()const
		{
			return m_clear;
		}

	private:
		uint32_t m_index;
		PixelFormat m_format;
		bool m_clear;
	};
}

#endif
