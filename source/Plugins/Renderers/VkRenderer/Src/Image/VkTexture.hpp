/**
*\file
*	Texture.h
*\author
*	Sylvain Doremus
*/
#ifndef ___VkRenderer_Texture_HPP___
#define ___VkRenderer_Texture_HPP___
#pragma once

#include "VkRendererPrerequisites.hpp"

#include <Image/Texture.hpp>

namespace vk_renderer
{
	/**
	*\~french
	*\brief
	*	Classe encapsulant le concept d'image Vulkan.
	*\remarks
	*	Gère la transition de layouts.
	*	Dépendant du fait que l'image provienne de la swap chain
	*	ou d'une ressource, la VkImage sera détruite par le parent
	*	correspondant.
	*\~english
	*\brief
	*	Class wrapping the Vulkan image concept.
	*\remarks
	*	Handles the layouts transition.
	*	Depending on wheter the image comes from a resource or a swap chain,
	*	The VkImage will be destroyed or not.
	*/
	class Texture
		: public renderer::Texture
	{
	public:
		Texture( Texture const & ) = delete;
		Texture & operator=( Texture const & ) = delete;
		Texture( Texture && rhs );
		Texture & operator=( Texture && rhs );
		/**
		*\brief
		*	Constructeur.
		*/
		Texture( Device const & device
			, renderer::ImageCreateInfo const & createInfo );
		/**
		*\brief
		*	Constructeur.
		*\param[in] device
		*	Le périphérique logique.
		*/
		Texture( Device const & device
			, renderer::Format format
			, renderer::Extent2D const & dimensions
			, VkImage image );
		/**
		*\brief
		*	Constructeur.
		*\param[in] device
		*	Le périphérique logique.
		*/
		Texture( Device const & device
			, renderer::Format format
			, renderer::Extent2D const & dimensions
			, renderer::ImageUsageFlags usageFlags
			, renderer::ImageTiling tiling
			, renderer::MemoryPropertyFlags memoryFlags );
		/**
		*\brief
		*	Destructeur.
		*/
		~Texture();
		/**
		*\copydoc	renderer::Texture::createView
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
		/**
		*\~french
		*\brief
		*	Opérateur de conversion implicite vers VkImage.
		*\~english
		*\brief
		*	VkImage implicit cast operator.
		*/
		inline operator VkImage const &( )const
		{
			return m_image;
		}

	private:
		void doBindMemory()override;

	private:
		Device const & m_device;
		VkImage m_image{};
		bool m_owner{};
	};
}

#endif
