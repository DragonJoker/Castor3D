/**
*\file
*	VertexBuffer.h
*\author
*	Sylvain Doremus
*/
#ifndef ___VkRenderer_DescriptorSet_HPP___
#define ___VkRenderer_DescriptorSet_HPP___
#pragma once

#include "VkRendererPrerequisites.hpp"

#include <Descriptor/DescriptorSet.hpp>

#include <vector>

namespace vk_renderer
{
	/**
	*\brief
	*	Classe wrappant un VkDescriptorSet.
	*/
	class DescriptorSet
		: public renderer::DescriptorSet
	{
	public:
		/**
		*\~french
		*\brief
		*	Constructeur.
		*\param[in] device
		*	Le périphérique logique.
		*\param[in] pool
		*	Le pool parent.
		*\param[in] bindingPoint
		*	Le point d'attache du set.
		*\~english
		*\brief
		*	Constructor.
		*\param[in] device
		*	The logical connection to the GPU.
		*\param[in] pool
		*	The parent pool.
		*\param[in] bindingPoint
		*	The binding point for the set.
		*/
		DescriptorSet( Device const & device
			, DescriptorPool const & pool
			, DescriptorSetLayout const & layout
			, uint32_t bindingPoint );
		/**
		*\~french
		*\brief
		*	Destructeur.
		*\~english
		*\brief
		*	Destructor.
		*/
		~DescriptorSet();
		/**
		*\copydoc	renderer::DescriptorSet::update
		*/
		void update()const override;
		/**
		*\~french
		*\brief
		*	Conversion implicite vers VkDescriptorSet.
		*\~english
		*\brief
		*	VkDescriptorSet implicit cast operator.
		*/
		inline operator VkDescriptorSet const &( )const
		{
			return m_descriptorSet;
		}

	private:
		Device const & m_device;
		DescriptorPool const & m_pool;
		DescriptorSetLayout const & m_layout;
		VkDescriptorSet m_descriptorSet{};
	};
}

#endif
