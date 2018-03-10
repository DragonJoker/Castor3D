/**
*\file
*	DescriptorSetPool.h
*\author
*	Sylvain Doremus
*/
#ifndef ___VkRenderer_DescriptorPool_HPP___
#define ___VkRenderer_DescriptorPool_HPP___
#pragma once

#include "VkRendererPrerequisites.hpp"

#include <Descriptor/DescriptorPool.hpp>

#include <vector>

namespace vk_renderer
{
	class DescriptorPool
		: public renderer::DescriptorPool
	{
	public:
		DescriptorPool( Device const & device
			, renderer::DescriptorPoolCreateFlags flags
			, uint32_t maxSets
			, renderer::DescriptorPoolSizeArray poolSizes );
		/**
		*\~french
		*\brief
		*	Destructeur.
		*\~english
		*\brief
		*	Destructor.
		*/
		~DescriptorPool();
		/**
		*\copydoc	renderer::DescriptorSetPool::createDescriptorSet
		*/
		renderer::DescriptorSetPtr createDescriptorSet( renderer::DescriptorSetLayout const & layout
			, uint32_t bindingPoint )const override;
		/**
		*\~french
		*\brief
		*	Conversion implicite vers VkDescriptorPool.
		*\~english
		*\brief
		*	VkDescriptorPool implicit cast operator.
		*/
		inline operator VkDescriptorPool const &( )const
		{
			return m_pool;
		}

	private:
		Device const & m_device;
		VkDescriptorPool m_pool{};
	};
}

#endif
