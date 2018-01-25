/*
This file belongs to Renderer.
See LICENSE file in root folder.
*/
#ifndef ___Renderer_ImageSubresourceRange_HPP___
#define ___Renderer_ImageSubresourceRange_HPP___
#pragma once

#include "RendererPrerequisites.hpp"

namespace renderer
{
	/**
	*\brief
	*	Spécifie l'intervalle de sous-ressource d'une image.
	*/
	class ImageSubresourceRange
	{
	public:
		/**
		*\brief
		*	Constructeur.
		*\param[in] aspectMask
		*	Masques de bits décrivant les aspects d'une image.
		*\param[in] baseMipLevel
		*	Le niveau de base de mipmap.
		*\param[in] levelCount
		*	Le nombre de niveaux.
		*\param[in] baseArrayLayer
		*	La couche de base du tableau.
		*\param[in] layerCount
		*	Le nombre de couches.
		*/
		ImageSubresourceRange( ImageAspectFlags aspectMask = 0u
			, uint32_t baseMipLevel = 0u
			, uint32_t levelCount = ~( 0u )
			, uint32_t baseArrayLayer = 0u
			, uint32_t layerCount = ~( 0u ) );
		/**
		*\return
		*	Masques de bits décrivant les aspects d'une image.
		*/
		inline ImageAspectFlags getAspectMask()const
		{
			return m_aspectMask;
		}
		/**
		*\return
		*	Le niveau de base de mipmap.
		*/
		inline uint32_t getBaseMipLevel()const
		{
			return m_baseMipLevel;
		}
		/**
		*\return
		*	Le nombre de niveaux.
		*/
		inline uint32_t getLevelCount()const
		{
			return m_levelCount;
		}
		/**
		*\return
		*	La couche de base du tableau.
		*/
		inline uint32_t getBaseArrayLayer()const
		{
			return m_baseArrayLayer;
		}
		/**
		*\return
		*	Le nombre de couches.
		*/
		inline uint32_t getLayerCount()const
		{
			return m_layerCount;
		}

	private:
		ImageAspectFlags m_aspectMask;
		uint32_t m_baseMipLevel;
		uint32_t m_levelCount;
		uint32_t m_baseArrayLayer;
		uint32_t m_layerCount;
	};
}

#endif
