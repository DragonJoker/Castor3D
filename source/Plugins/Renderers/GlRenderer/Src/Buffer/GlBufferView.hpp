/**
*\file
*	Texture.h
*\author
*	Sylvain Doremus
*/
#ifndef ___GlRenderer_BufferView_HPP___
#define ___GlRenderer_BufferView_HPP___
#pragma once

#include "GlRendererPrerequisites.hpp"

#include <Buffer/BufferView.hpp>

namespace gl_renderer
{
	/**
	*\~french
	*\brief
	*	Une vue de texture sur un tampon.
	*\~english
	*\brief
	*	A texture view to a buffer.
	*/
	class BufferView
		: public renderer::BufferView
	{
	public:
		/**
		*\~french
		*\brief
		*	Constructeur.
		*\param[in] device
		*	Le périphérique logique.
		*\param[in] buffer
		*	Le tampon.
		*\param[in] format
		*	Le format des pixels de la vue.
		*\param[in] offset
		*	Le décalage dans le tampon.
		*\param[in] range
		*	La taille occupée dans le tampon.
		*\~english
		*\brief
		*	Constructor.
		*\param[in] device
		*	The logical device.
		*\param[in] buffer
		*	The buffer.
		*\param[in] format
		*	The view's pixel format.
		*\param[in] offset
		*	The offset in the buffer.
		*\param[in] range
		*	The range from the buffer.
		*/
		BufferView( renderer::Device const & device
			, Buffer const & buffer
			, renderer::PixelFormat format
			, uint32_t offset
			, uint32_t range );
		/**
		*\~french
		*\brief
		*	Destructeur.
		*\~english
		*\brief
		*	Destructor.
		*/
		~BufferView();
		/**
		*\~french
		*\return
		*	Le nom de la texture.
		*\~english
		*\return
		*	The texture name.
		*/
		inline GLuint getImage()const
		{
			return m_name;
		}

	private:
		GLuint m_name{ GL_INVALID_INDEX };
	};
}

#endif
