/**
*\file
*	VertexBuffer.h
*\author
*	Sylvain Doremus
*/
#ifndef ___VkRenderer_DescriptorSetLayout_HPP___
#define ___VkRenderer_DescriptorSetLayout_HPP___
#pragma once

#include "VkRendererPrerequisites.hpp"

#include <Descriptor/DescriptorSetLayout.hpp>

namespace vk_renderer
{
	/**
	*\brief
	*	Classe wrappant un VkDescriptorSetLayout.
	*/
	class DescriptorSetLayout
		: public renderer::DescriptorSetLayout
	{
	public:
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
		*	The logical connection to the GPU.
		*\param[in] bindings
		*	The bindings.
		*/
		DescriptorSetLayout( Device const & device
			, renderer::DescriptorSetLayoutBindingArray && bindings );
		/**
		*\~french
		*\brief
		*	Destructeur.
		*\~english
		*\brief
		*	Destructor.
		*/
		~DescriptorSetLayout();
		/**
		*\~french
		*\brief
		*	Conversion implicite vers VkDescriptorSetLayout.
		*\~english
		*\brief
		*	VkDescriptorSetLayout implicit cast operator.
		*/
		inline operator VkDescriptorSetLayout const &( )const
		{
			return m_layout;
		}

	private:
		Device const & m_device;
		VkDescriptorSetLayout m_layout{};
	};
}

#endif
