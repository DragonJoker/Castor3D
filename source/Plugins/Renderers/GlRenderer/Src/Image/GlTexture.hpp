/**
*\file
*	Texture.h
*\author
*	Sylvain Doremus
*/
#ifndef ___VkRenderer_Texture_HPP___
#define ___VkRenderer_Texture_HPP___
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
		Texture( Device const & device );
		/**
		*\brief
		*	Destructeur.
		*/
		~Texture();
		/**
		*\copydoc	renderer::Texture::createView
		*/
		renderer::TextureViewPtr createView( renderer::TextureType type
			, renderer::PixelFormat format
			, uint32_t baseMipLevel
			, uint32_t levelCount
			, uint32_t baseArrayLayer
			, uint32_t layerCount )const override;
		/**
		*\copydoc	renderer::Texture::generateMipmaps
		*/
		void generateMipmaps()const override;
		/**
		*\copydoc	renderer::Texture::makeGeneralLayout
		*/
		renderer::ImageMemoryBarrier makeGeneralLayout( renderer::ImageSubresourceRange const & range
			, renderer::AccessFlags accessFlags )const override;
		/**
		*\copydoc	renderer::Texture::makeTransferDestination
		*/
		renderer::ImageMemoryBarrier makeTransferDestination( renderer::ImageSubresourceRange const & range )const override;
		/**
		*\copydoc	renderer::Texture::makeTransferSource
		*/
		renderer::ImageMemoryBarrier makeTransferSource( renderer::ImageSubresourceRange const & range )const override;
		/**
		*\copydoc	renderer::Texture::makeShaderInputResource
		*/
		renderer::ImageMemoryBarrier makeShaderInputResource( renderer::ImageSubresourceRange const & range )const override;
		/**
		*\copydoc	renderer::Texture::makeDepthStencilReadOnly
		*/
		renderer::ImageMemoryBarrier makeDepthStencilReadOnly( renderer::ImageSubresourceRange const & range )const override;
		/**
		*\copydoc	renderer::Texture::makeColourAttachment
		*/
		renderer::ImageMemoryBarrier makeColourAttachment( renderer::ImageSubresourceRange const & range )const override;
		/**
		*\copydoc	renderer::Texture::makeDepthStencilAttachment
		*/
		renderer::ImageMemoryBarrier makeDepthStencilAttachment( renderer::ImageSubresourceRange const & range )const override;
		/**
		*\copydoc	renderer::Texture::makePresentSource
		*/
		renderer::ImageMemoryBarrier makePresentSource( renderer::ImageSubresourceRange const & range )const override;
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
		renderer::ImageMemoryBarrier doMakeLayoutTransition( renderer::ImageLayout layout
			, uint32_t queueFamily
			, renderer::AccessFlags dstAccessMask
			, renderer::ImageSubresourceRange const & range )const;
		/**
		*\copydoc	renderer::Texture::doSetImage1D
		*/
		void doSetImage1D( renderer::ImageUsageFlags usageFlags
			, renderer::ImageTiling tiling
			, renderer::MemoryPropertyFlags memoryFlags )override;
		/**
		*\copydoc	renderer::Texture::doSetImage2D
		*/
		void doSetImage2D( renderer::ImageUsageFlags usageFlags
			, renderer::ImageTiling tiling
			, renderer::MemoryPropertyFlags memoryFlags )override;
		/**
		*\copydoc	renderer::Texture::doSetImage3D
		*/
		void doSetImage3D( renderer::ImageUsageFlags usageFlags
			, renderer::ImageTiling tiling
			, renderer::MemoryPropertyFlags memoryFlags )override;

	private:
		Device const & m_device;
		GlTextureType m_target;
		GLuint m_texture{ GL_INVALID_INDEX };
	};
}

#endif
