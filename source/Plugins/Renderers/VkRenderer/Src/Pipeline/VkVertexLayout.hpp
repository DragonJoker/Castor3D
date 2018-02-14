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
			, uint32_t stride
			, renderer::VertexInputRate inputRate );
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

	private:
		VkVertexInputBindingDescription m_bindingDescription{};
	};
}

#endif
