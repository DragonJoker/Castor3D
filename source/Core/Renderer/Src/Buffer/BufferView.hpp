/*
This file belongs to Renderer.
See LICENSE file in root folder.
*/
#ifndef ___Renderer_BufferView_HPP___
#define ___Renderer_BufferView_HPP___
#pragma once

#include "Image/ImageSubresourceRange.hpp"

namespace renderer
{
	/**
	*\~french
	*\brief
	*	Description d'une vue sur un tampon.
	*\~english
	*\brief
	*	A view on a buffer.
	*/
	class BufferView
	{
	protected:
		/**
		*\~french
		*\brief
		*	Constructeur.
		*\param[in] device
		*	La connexion logique au GPU.
		*\param[in] buffer
		*	Le tampon sur lequel la vue est créée.
		*\param[in] format
		*	Le format des pixels de la vue.
		*\param[in] offset
		*	Le décalage dans le tampon.
		*\param[in] range
		*	Le nombre d'éléments dans le tampon.
		*\~english
		*\brief
		*	Constructor.
		*\param[in] device
		*	The logical connection to the GPU.
		*\param[in] buffer
		*	The buffer from which the view is created.
		*\param[in] format
		*	The view's pixels format.
		*\param[in] offset
		*	The offset in the buffer.
		*\param[in] range
		*	The number of elements from the buffer.
		*/
		BufferView( Device const & device
			, BufferBase const & buffer
			, PixelFormat format
			, uint32_t offset
			, uint32_t range );

	public:
		/**
		*\~french
		*\brief
		*	Destructeur.
		*\~english
		*\brief
		*	Destructor.
		*/
		virtual ~BufferView() = default;
		/**
		*\~french
		*\return
		*	Le format des pixels de la vue.
		*\~english
		*\return
		*	The view's pixels format.
		*/
		inline PixelFormat getFormat()const
		{
			return m_format;
		}
		/**
		*\~french
		*\return
		*	Le tampon de la vue.
		*\~english
		*\return
		*	The view's buffer.
		*/
		inline BufferBase const & getBuffer()const
		{
			return m_buffer;
		}
		/**
		*\~french
		*\return
		*	Le décalage dans le tampon.
		*\~english
		*\return
		*	The offset in the buffer.
		*/
		inline uint32_t getOffset()const
		{
			return m_offset;
		}
		/**
		*\~french
		*\return
		*	Le nombre d'éléments dans le tampon.
		*\~english
		*\return
		*	The number of elements from the buffer.
		*/
		inline uint32_t getRange()const
		{
			return m_range;
		}

	private:
		Device const & m_device;
		BufferBase const & m_buffer;
		PixelFormat m_format;
		uint32_t m_offset;
		uint32_t m_range;
	};
}

#endif
