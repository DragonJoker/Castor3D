/*
This file belongs to Renderer.
See LICENSE file in root folder.
*/
#ifndef ___Renderer_Texture_HPP___
#define ___Renderer_Texture_HPP___
#pragma once

#include "RendererPrerequisites.hpp"

namespace renderer
{
	/**
	*\~english
	*\brief
	*	A texture image.
	*\~french
	*\brief
	*	L'image d'une texture.
	*/
	class Texture
	{
	public:
		/**
		*\~french
		*\brief
		*	Contient les informations d'une image mappée en RAM.
		*\~english
		*\brief
		*	Contains an image mapped in RAM informations.
		*/
		struct Mapped
		{
			uint8_t * data;
			uint64_t size;
			uint64_t rowPitch;
			uint64_t arrayPitch;
			uint64_t depthPitch;
		};

	protected:
		/**
		*\~french
		*\brief
		*	Constructeur.
		*\param[in] device
		*	Le périphérique logique.
		*\~english
		*\brief
		*	Constructor.
		*\param[in] device
		*	The logical device.
		*/
		Texture( Device const & device );

	public:
		Texture( Texture const & ) = delete;
		Texture & operator=( Texture const & ) = delete;
		Texture( Texture && rhs );
		Texture & operator=( Texture && rhs );
		/**
		*\~english
		*\brief
		*	Destructor.
		*\~french
		*\brief
		*	Destructeur.
		*/
		virtual ~Texture() = default;
		/**
		*\~french
		*\brief
		*	Charge l'image de la texture.
		*\param[in] format
		*	Le format de l'image.
		*\param[in] size
		*	Les dimensions de l'image.
		*\param[in] usageFlags
		*	Les indicateurs d'utilisation de l'image.
		*\param[in] tiling
		*	Le mode de tiling de l'image.
		*\~english
		*\brief
		*	Loads the texture image.
		*\param[in] format
		*	The pixel format.
		*\param[in] size
		*	The image dimensions.
		*\param[in] usageFlags
		*	The image usage flags.
		*\param[in] tiling
		*	The image tiling mode.
		*/
		void setImage( PixelFormat format
			, uint32_t size
			, ImageUsageFlags usageFlags = ImageUsageFlag::eTransferDst | ImageUsageFlag::eSampled
			, ImageTiling tiling = ImageTiling::eOptimal );
		/**
		*\~french
		*\brief
		*	Charge l'image de la texture.
		*\param[in] format
		*	Le format de l'image.
		*\param[in] size
		*	Les dimensions de l'image.
		*\param[in] usageFlags
		*	Les indicateurs d'utilisation de l'image.
		*\param[in] tiling
		*	Le mode de tiling de l'image.
		*\~english
		*\brief
		*	Loads the texture image.
		*\param[in] format
		*	The pixel format.
		*\param[in] size
		*	The image dimensions.
		*\param[in] usageFlags
		*	The image usage flags.
		*\param[in] tiling
		*	The image tiling mode.
		*/
		void setImage( PixelFormat format
			, UIVec2 const & size
			, ImageUsageFlags usageFlags = ImageUsageFlag::eTransferDst | ImageUsageFlag::eSampled
			, ImageTiling tiling = ImageTiling::eOptimal
			, SampleCountFlag samples = SampleCountFlag::e1 );
		/**
		*\~french
		*\brief
		*	Charge l'image de la texture.
		*\param[in] format
		*	Le format de l'image.
		*\param[in] size
		*	Les dimensions de l'image.
		*\param[in] usageFlags
		*	Les indicateurs d'utilisation de l'image.
		*\param[in] tiling
		*	Le mode de tiling de l'image.
		*\~english
		*\brief
		*	Loads the texture image.
		*\param[in] format
		*	The pixel format.
		*\param[in] size
		*	The image dimensions.
		*\param[in] usageFlags
		*	The image usage flags.
		*\param[in] tiling
		*	The image tiling mode.
		*/
		void setImage( PixelFormat format
			, UIVec3 const & size
			, ImageUsageFlags usageFlags = ImageUsageFlag::eTransferDst | ImageUsageFlag::eSampled
			, ImageTiling tiling = ImageTiling::eOptimal
			, SampleCountFlag samples = SampleCountFlag::e1 );
		/**
		*\~french
		*\brief
		*	Charge l'image de la texture.
		*\param[in] format
		*	Le format de l'image.
		*\param[in] size
		*	Les dimensions de l'image.
		*\param[in] layerCount
		*	Le nombre de couches.
		*\param[in] usageFlags
		*	Les indicateurs d'utilisation de l'image.
		*\param[in] tiling
		*	Le mode de tiling de l'image.
		*\~english
		*\brief
		*	Loads the texture image.
		*\param[in] format
		*	The pixel format.
		*\param[in] size
		*	The image dimensions.
		*\param[in] layerCount
		*	The number of layers.
		*\param[in] usageFlags
		*	The image usage flags.
		*\param[in] tiling
		*	The image tiling mode.
		*/
		void setImage( PixelFormat format
			, uint32_t size
			, uint32_t layerCount
			, ImageUsageFlags usageFlags = ImageUsageFlag::eTransferDst | ImageUsageFlag::eSampled
			, ImageTiling tiling = ImageTiling::eOptimal );
		/**
		*\~french
		*\brief
		*	Charge l'image de la texture.
		*\param[in] format
		*	Le format de l'image.
		*\param[in] size
		*	Les dimensions de l'image.
		*\param[in] layerCount
		*	Le nombre de couches.
		*\param[in] usageFlags
		*	Les indicateurs d'utilisation de l'image.
		*\param[in] tiling
		*	Le mode de tiling de l'image.
		*\~english
		*\brief
		*	Loads the texture image.
		*\param[in] format
		*	The pixel format.
		*\param[in] size
		*	The image dimensions.
		*\param[in] layerCount
		*	The number of layers.
		*\param[in] usageFlags
		*	The image usage flags.
		*\param[in] tiling
		*	The image tiling mode.
		*/
		void setImage( PixelFormat format
			, UIVec2 const & size
			, uint32_t layerCount
			, ImageUsageFlags usageFlags = ImageUsageFlag::eTransferDst | ImageUsageFlag::eSampled
			, ImageTiling tiling = ImageTiling::eOptimal );
		/**
		*\~french
		*\brief
		*	Crée une vue sur la texture.
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
		*	Creates a view to the texture.
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
		virtual TextureViewPtr createView( TextureType type
			, PixelFormat format
			, uint32_t baseMipLevel
			, uint32_t levelCount
			, uint32_t baseArrayLayer
			, uint32_t layerCount )const = 0;
		/**
		*\~french
		*\brief
		*	Génère les mipmaps de la texture.
		*\~english
		*\brief
		*	Generates the texture mipmaps.
		*/
		virtual void generateMipmaps()const = 0;
		/**
		*\~french
		*\brief
		*	Prépare une barrière mémoire de transition vers un layout général.
		*\param[in] accessFlags
		*	Les accès voulus, une fois que la transition est effectuée.
		*\return
		*	La barrière mémoire.
		*\~english
		*\brief
		*	Prepares a memory barrier, for a general layout.
		*\param[in] accessFlags
		*	The wanted access flags, once the transition is done.
		*\return
		*	The memory barrier.
		*/
		virtual ImageMemoryBarrier makeGeneralLayout( ImageSubresourceRange const & range
			, AccessFlags accessFlags )const = 0;
		/**
		*\~french
		*\brief
		*	Prépare une barrière mémoire de transition vers un layout de destination de transfert.
		*\return
		*	La barrière mémoire.
		*\~english
		*\brief
		*	Prepares a memory barrier, for a transfer destination layout.
		*\return
		*	The memory barrier.
		*/
		virtual ImageMemoryBarrier makeTransferDestination( ImageSubresourceRange const & range )const = 0;
		/**
		*\~french
		*\brief
		*	Prépare une barrière mémoire de transition vers un layout de source de transfert.
		*\return
		*	La barrière mémoire.
		*\~english
		*\brief
		*	Prepares a memory barrier, for a transfer source layout.
		*\return
		*	The memory barrier.
		*/
		virtual ImageMemoryBarrier makeTransferSource( ImageSubresourceRange const & range )const = 0;
		/**
		*\~french
		*\brief
		*	Prépare une barrière mémoire de transition vers un layout de ressource d'entrée (lecture seule) d'un shader.
		*\return
		*	La barrière mémoire.
		*\~english
		*\brief
		*	Prepares a memory barrier, for an input shader resource layout.
		*\return
		*	The memory barrier.
		*/
		virtual ImageMemoryBarrier makeShaderInputResource( ImageSubresourceRange const & range )const = 0;
		/**
		*\~french
		*\brief
		*	Prépare une barrière mémoire de transition vers un layout de ressource de lecture de profondeur et/ou stencil.
		*\return
		*	La barrière mémoire.
		*\~english
		*\brief
		*	Prepares a memory barrier, for a depth ant/or stencil read only layout.
		*\return
		*	The memory barrier.
		*/
		virtual ImageMemoryBarrier makeDepthStencilReadOnly( ImageSubresourceRange const & range )const = 0;
		/**
		*\~french
		*\brief
		*	Prépare une barrière mémoire de transition vers un layout d'attache couleur.
		*\return
		*	La barrière mémoire.
		*\~english
		*\brief
		*	Prepares a memory barrier, for a colour attachment layout.
		*\return
		*	The memory barrier.
		*/
		virtual ImageMemoryBarrier makeColourAttachment( ImageSubresourceRange const & range )const = 0;
		/**
		*\~french
		*\brief
		*	Prépare une barrière mémoire de transition vers un layout d'attache profondeur et/ou stencil.
		*\return
		*	La barrière mémoire.
		*\~english
		*\brief
		*	Prepares a memory barrier, for a depth and/or stencil attachment layout.
		*\return
		*	The memory barrier.
		*/
		virtual ImageMemoryBarrier makeDepthStencilAttachment( ImageSubresourceRange const & range )const = 0;
		/**
		*\~french
		*\brief
		*	Prépare une barrière mémoire de transition vers un layout de source de presentation.
		*\return
		*	La barrière mémoire.
		*\~english
		*\brief
		*	Prepares a memory barrier, for a presentation source layout.
		*\return
		*	The memory barrier.
		*/
		virtual ImageMemoryBarrier makePresentSource( ImageSubresourceRange const & range )const = 0;
		/**
		*\~french
		*\return
		*	Le format des pixels de la texture.
		*\~english
		*\return
		*	The texture pixel format.
		*/
		inline PixelFormat getFormat()const noexcept
		{
			return m_format;
		}
		/**
		*\~french
		*\return
		*	Les dimensions de la texture.
		*\~english
		*\return
		*	The texture dimensions.
		*/
		inline UIVec3 const & getDimensions()const noexcept
		{
			return m_size;
		}
		/**
		*\~french
		*\return
		*	Le type de texture.
		*\~english
		*\return
		*	The texture type.
		*/
		inline TextureType getType()const
		{
			return m_type;
		}

	private:
		/**
		*\~french
		*\brief
		*	Charge l'image de la texture.
		*\param[in] usageFlags
		*	Les indicateurs d'utilisation de l'image.
		*\param[in] tiling
		*	Le mode de tiling de l'image.
		*\param[in] memoryFlags
		*	Les indicateurs de mémoire de l'image.
		*\~english
		*\brief
		*	Loads the texture image.
		*\param[in] usageFlags
		*	The image usage flags.
		*\param[in] tiling
		*	The image tiling mode.
		*\param[in] memoryFlags
		*	The image memory flags.
		*/
		virtual void doSetImage1D( ImageUsageFlags usageFlags
			, ImageTiling tiling
			, MemoryPropertyFlags memoryFlags ) = 0;
		/**
		*\~french
		*\brief
		*	Charge l'image de la texture.
		*\param[in] usageFlags
		*	Les indicateurs d'utilisation de l'image.
		*\param[in] tiling
		*	Le mode de tiling de l'image.
		*\param[in] memoryFlags
		*	Les indicateurs de mémoire de l'image.
		*\~english
		*\brief
		*	Loads the texture image.
		*\param[in] usageFlags
		*	The image usage flags.
		*\param[in] tiling
		*	The image tiling mode.
		*\param[in] memoryFlags
		*	The image memory flags.
		*/
		virtual void doSetImage2D( ImageUsageFlags usageFlags
			, ImageTiling tiling
			, MemoryPropertyFlags memoryFlags ) = 0;
		/**
		*\~french
		*\brief
		*	Charge l'image de la texture.
		*\param[in] usageFlags
		*	Les indicateurs d'utilisation de l'image.
		*\param[in] tiling
		*	Le mode de tiling de l'image.
		*\param[in] memoryFlags
		*	Les indicateurs de mémoire de l'image.
		*\~english
		*\brief
		*	Loads the texture image.
		*\param[in] usageFlags
		*	The image usage flags.
		*\param[in] tiling
		*	The image tiling mode.
		*\param[in] memoryFlags
		*	The image memory flags.
		*/
		virtual void doSetImage3D( ImageUsageFlags usageFlags
			, ImageTiling tiling
			, MemoryPropertyFlags memoryFlags ) = 0;

	protected:
		Device const & m_device;
		TextureType m_type{ TextureType::eCount };
		UIVec3 m_size;
		uint32_t m_layerCount{ 0u };
		PixelFormat m_format{ PixelFormat::eR8G8B8 };
		SampleCountFlag m_samples{ SampleCountFlag::e1 };
	};
}

#endif
