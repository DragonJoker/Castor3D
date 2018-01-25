/**
*\file
*	Texture.h
*\author
*	Sylvain Doremus
*/
#ifndef ___GlRenderer_TextureView_HPP___
#define ___GlRenderer_TextureView_HPP___
#pragma once

#include "GlRendererPrerequisites.hpp"

#include <Image/TextureView.hpp>

namespace gl_renderer
{
	/**
	*\~french
	*\brief
	*	Description d'une vue sur une image Vulkan.
	*\~english
	*\brief
	*	Vulkan image view wrapper.
	*/
	class TextureView
		: public renderer::TextureView
	{
	public:
		/**
		*\~french
		*\brief
		*	Constructeur.
		*\param[in] device
		*	La connexion logique au GPU.
		*\param[in] image
		*	L'image sur laquelle la vue est créée.
		*\param[in] type
		*	Le type de texture de la vue.
		*\param[in] format
		*	Le format des pixels de la vue.
		*\param[in] baseMipLevel
		*	Le premier niveau de mipmap accessible à la vue.
		*\param[in] levelCount
		*	Le nombre de niveaux de mipmap (à partir de \p baseMipLevel) accessibles à la vue.
		*\param[in] baseArrayLayer
		*	La première couche de tableau accessible à la vue.
		*\param[in] layerCount
		*	Le nombre de couches de tableau (à partir de \p baseArrayLayer) accessibles à la vue.
		*\~english
		*\brief
		*	Constructor.
		*\param[in] device
		*	The logical connection to the GPU.
		*\param[in] image
		*	The image from which the view is created.
		*\param[in] type
		*	The view's texture type.
		*\param[in] format
		*	The view's pixels format.
		*\param[in] baseMipLevel
		*	The first mipmap level accessible to the view.
		*\param[in] levelCount
		*	The number of mipmap levels (starting from \p baseMipLevel) accessible to the view.
		*\param[in] baseArrayLayer
		*	The first array layer accessible to the view.
		*\param[in] layerCount
		*	The number of array layers (starting from \p baseArrayLayer) accessible to the view.
		*/
		TextureView( Device const & device
			, Texture const & image
			, renderer::TextureType type
			, renderer::PixelFormat format
			, uint32_t baseMipLevel
			, uint32_t levelCount
			, uint32_t baseArrayLayer
			, uint32_t layerCount );
		/**
		*\~french
		*\brief
		*	Destructeur.
		*\~english
		*\brief
		*	Destructor.
		*/
		~TextureView();
		/**
		*\return
		*	L'image OpenGL.
		*/
		inline GLuint getImage()const noexcept
		{
			assert( m_texture != GL_INVALID_INDEX );
			return m_texture;
		}

	private:
		Device const & m_device;
		GlTextureType m_target;
		GLuint m_texture{ GL_INVALID_INDEX };
	};
}

#endif
