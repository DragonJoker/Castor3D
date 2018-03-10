/**
*\file
*	Texture.h
*\author
*	Sylvain Doremus
*/
#ifndef ___GlRenderer_Texture_HPP___
#define ___GlRenderer_Texture_HPP___
#pragma once

#include "GlRendererPrerequisites.hpp"

#include <Image/Texture.hpp>

namespace gl_renderer
{
	/**
	*\brief
	*	Une texture, avec son image et son échantillonneur.
	*/
	class Texture
		: public renderer::Texture
	{
	public:
		/**
		*\brief
		*	Constructeur.
		*\param[in] device
		*	Le périphérique logique.
		*/
		Texture( Device const & device
			, renderer::Format format
			, renderer::Extent2D const & dimensions );
		/**
		*\brief
		*	Constructeur.
		*\param[in] device
		*	Le périphérique logique.
		*/
		Texture( Device const & device
			, renderer::ImageCreateInfo const & createInfo );
		/**
		*\brief
		*	Destructeur.
		*/
		~Texture();
		/**
		*\copydoc	renderer::Texture::getMemoryRequirements
		*/
		renderer::MemoryRequirements getMemoryRequirements()const override;
		/**
		*\copydoc	renderer::Texture::createView
		*/
		renderer::TextureViewPtr createView( renderer::ImageViewCreateInfo const & createInfo )const override;
		/**
		*\copydoc	renderer::Texture::generateMipmaps
		*/
		void generateMipmaps()const override;

		inline bool hasImage()const noexcept
		{
			return m_texture != GL_INVALID_INDEX;
		}
		/**
		*\return
		*	L'image OpenGL.
		*/
		inline GLuint getImage()const noexcept
		{
			assert( m_texture != GL_INVALID_INDEX );
			return m_texture;
		}
		/**
		*\return
		*	Le nombre d'échantillons.
		*/
		inline renderer::SampleCountFlag getSamplesCount()const noexcept
		{
			return m_createInfo.samples;
		}

	private:
		void doBindMemory()override;

	private:
		Device const & m_device;
		renderer::ImageCreateInfo m_createInfo;
		GlTextureType m_target;
		GLuint m_texture{ GL_INVALID_INDEX };
	};
}

#endif
