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
		Texture( Texture const & ) = delete;
		Texture( Texture && rhs );
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
		Texture & operator=( Texture const & ) = delete;
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
		void setImageArray( PixelFormat format
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
		void setImageArray( PixelFormat format
			, UIVec2 const & size
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
			, uint32_t mipmapLevels
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
			, uint32_t mipmapLevels
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
			, uint32_t mipmapLevels
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
		void setImageArray( PixelFormat format
			, uint32_t size
			, uint32_t layerCount
			, uint32_t mipmapLevels
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
		void setImageArray( PixelFormat format
			, UIVec2 const & size
			, uint32_t layerCount
			, uint32_t mipmapLevels
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
			, uint32_t baseMipLevel = 0u
			, uint32_t levelCount = 1u
			, uint32_t baseArrayLayer = 0u
			, uint32_t layerCount=  1u )const = 0;
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
		*	Le nombre de niveaux de mipmaps.
		*\~english
		*\return
		*	The mipmap levels count.
		*/
		inline uint32_t getMipmapLevels()const noexcept
		{
			return m_mipmapLevels;
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
		UIVec3 m_size;
		TextureType m_type{ TextureType::eCount };
		uint32_t m_layerCount{ 1u };
		uint32_t m_mipmapLevels{ 1u };
		PixelFormat m_format{ PixelFormat::eR8G8B8 };
		SampleCountFlag m_samples{ SampleCountFlag::e1 };
	};
}

#endif
