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
		*\param[in] device
		*	Le périphérique logique.
		*\param[in] initialLayout
		*	Le layout initial de l'image.
		*/
		Texture( Device const & device, renderer::ImageLayout initialLayout );
		/**
		*\brief
		*	Constructeur.
		*\param[in] device
		*	Le périphérique logique.
		*/
		Texture( Device const & device
			, renderer::PixelFormat format
			, renderer::UIVec2 const & dimensions
			, VkImage image );
		/**
		*\brief
		*	Constructeur.
		*\param[in] device
		*	Le périphérique logique.
		*/
		Texture( Device const & device
			, renderer::PixelFormat format
			, renderer::UIVec2 const & dimensions
			, renderer::ImageUsageFlags usageFlags
			, renderer::ImageTiling tiling
			, renderer::MemoryPropertyFlags memoryFlags );
		/**
		*\brief
		*	Destructeur.
		*/
		~Texture();
		/**
		*\~french
		*\brief
		*	Mappe la mémoire du tampon en RAM.
		*\param[in] offset
		*	L'offset à partir duquel la mémoire du tampon est mappée.
		*\param[in] size
		*	La taille en octets de la mémoire à mapper.
		*\param[in] flags
		*	Indicateurs de configuration du mapping.
		*\return
		*	\p nullptr si le mapping a échoué.
		*\~english
		*\brief
		*	Maps the buffer's memory in RAM.
		*\param[in] offset
		*	The memory mapping starting offset.
		*\param[in] size
		*	The memory mappping size.
		*\param[in] flags
		*	The memory mapping flags.
		*\return
		*	\p nullptr if the mapping failed.
		*/
		renderer::Texture::Mapped lock( uint32_t offset
			, uint32_t size
			, VkMemoryMapFlags flags )const;
		/**
		*\~french
		*\brief
		*	Unmappe la mémoire du tampon de la RAM.
		*\param[in] size
		*	La taille en octets de la mémoire mappée.
		*\param[in] modified
		*	Dit si le tampon a été modifié, et donc si la VRAM doit être mise à jour.
		*\~english
		*\brief
		*	Unmaps the buffer's memory from the RAM.
		*\param[in] size
		*	The mapped memory size.
		*\param[in] modified
		*	Tells it the buffer has been modified, and whether the VRAM must be updated or not.
		*/
		void unlock( uint32_t size
			, bool modified )const;
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
		/**
		*\~french
		*\return
		*	La layout actuel de l'image.
		*\~english
		*\brief
		*	The current image layout.
		*/
		inline renderer::ImageLayout getCurrentLayout()const
		{
			return m_currentLayout;
		}

	private:
		/**
		*\~french
		*\brief
		*	Prépare une barrière mémoire de transition de l'image vers un autre layout.
		*\param[in] layout
		*	Le layout vers lequel on fait la transition.
		*\param[in] queueFamily
		*	La file référençant l'image après la transition.
		*\param[in] dstAccessMask
		*	Les accès voulus, une fois que la transition est effectuée.
		*\return
		*	La barrière mémoire.
		*\~english
		*\brief
		*	Prepares a layout transition memory barrier.
		*\param[in] layout
		*	The destination layout.
		*\param[in] queueFamily
		*	The queue referencing the image after the transition.
		*\param[in] dstAccessMask
		*	The wanted access after the transition is done.
		*\return
		*	The memory barrier.
		*/
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
		VkImage m_image{};
		ImageStoragePtr m_storage;
		bool m_owner{};
		mutable renderer::AccessFlags m_currentAccessMask{};
		mutable renderer::ImageLayout m_currentLayout{};
		mutable uint32_t m_currentQueueFamily{ 0 };
	};
}

#endif
