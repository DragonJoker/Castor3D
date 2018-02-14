/**
*\file
*	VertexBuffer.h
*\author
*	Sylvain Doremus
*/
#ifndef ___VkRenderer_VertexLayout_HPP___
#define ___VkRenderer_VertexLayout_HPP___
#pragma once

#include "VkRendererPrerequisites.hpp"

#include <Pipeline/VertexLayout.hpp>

#include <vector>

namespace vk_renderer
{
	class VertexLayout
		: public renderer::VertexLayout
	{
	public:
		/**
		*\brief
		*	Constructeur.
		*\param[in] bindingSlot
		*	Le point d'attache du tampon associé.
		*\param[in] stride
		*	La taille en octets séparant un élément du suivant, dans le tampon.
		*/
		VertexLayout( uint32_t bindingSlot
			, uint32_t stride );
		/**
		*\brief
		*	Crée un attribut de sommet.
		*\param[in] location
		*	La position de l'attribut dans le shader.
		*\param[in] offset
		*	La position de l'attribut dans le tampon.
		*/
		renderer::AttributeBase createAttribute( uint32_t location
			, renderer::AttributeFormat format
			, uint32_t offset )override;
		/**
		*\~french
		*\return
		*	La description des données du tampon.
		*\~english
		*\return
		*	The buffer's data description.
		*/
		inline VkVertexInputBindingDescription getDescription()const
		{
			return m_bindingDescription;
		}
		/**
		*\~french
		*\return
		*	Le début du tableau d'attributs.
		*\~english
		*\return
		*	The attributes array beginning.
		*/
		inline auto begin()const
		{
			return m_attributes.begin();
		}
		/**
		*\~french
		*\return
		*	Le début du tableau d'attributs.
		*\~english
		*\return
		*	The attributes array beginning.
		*/
		inline auto begin()
		{
			return m_attributes.begin();
		}
		/**
		*\~french
		*\return
		*	La fin du tableau d'attributs.
		*\~english
		*\return
		*	The attributes array end.
		*/
		inline auto end()const
		{
			return m_attributes.end();
		}
		/**
		*\~french
		*\return
		*	La fin du tableau d'attributs.
		*\~english
		*\return
		*	The attributes array end.
		*/
		inline auto end()
		{
			return m_attributes.end();
		}

	private:
		VkVertexInputBindingDescription m_bindingDescription{};
		AttributeArray m_attributes;
	};
}

#endif
