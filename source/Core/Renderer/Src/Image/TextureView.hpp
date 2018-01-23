/*
This file belongs to Renderer.
See LICENSE file in root folder.
*/
#ifndef ___Renderer_TextureView_HPP___
#define ___Renderer_TextureView_HPP___
#pragma once

#include "RendererPrerequisites.hpp"

#include "Image/ImageSubresourceRange.hpp"

namespace renderer
{
	/**
	*\~french
	*\brief
	*	Description d'une vue sur une image.
	*\~english
	*\brief
	*	Image view wrapper.
	*/
	class TextureView
	{
	protected:
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
			, TextureType type
			, PixelFormat format
			, uint32_t baseMipLevel
			, uint32_t levelCount
			, uint32_t baseArrayLayer
			, uint32_t layerCount );

	public:
		/**
		*\~french
		*\brief
		*	Destructeur.
		*\~english
		*\brief
		*	Destructor.
		*/
		virtual ~TextureView() = default;
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
		/**
		*\~french
		*\return
		*	Le format de l'image de la vue.
		*\~english
		*\return
		*	The view's image's pixels format.
		*/
		inline PixelFormat getFormat()const
		{
			return m_format;
		}
		/**
		*\~french
		*\return
		*	L'image de la vue.
		*\~english
		*\return
		*	The view's image.
		*/
		inline Texture const & getTexture()const
		{
			return m_image;
		}
		/**
		*\~french
		*\return
		*	Les informations de la ressource liée à l'image.
		*\~english
		*\return
		*	The image's resource information.
		*/
		inline ImageSubresourceRange const & getSubResourceRange()const
		{
			return m_subResourceRange;
		}

	private:
		Device const & m_device;
		Texture const & m_image;
		TextureType m_type{ TextureType::eCount };
		PixelFormat m_format{};
		ImageSubresourceRange m_subResourceRange{};
	};
}

#endif
