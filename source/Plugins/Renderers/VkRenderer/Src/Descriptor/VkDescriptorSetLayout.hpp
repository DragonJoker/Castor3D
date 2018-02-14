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
		*	Cr&e un pool pour les descripteurs qui utiliseront ce layout.
		*\param[in] maxSets
		*	Le nombre maximum de sets que le pool peut cr&er.
		*\return
		*	Le pool créé.
		*\~english
		*\brief
		*	Creates a descriptor pool.
		*\param[in] maxSets
		*	The maximum sets count the pool can create.
		*\return
		*	The created pool.
		*/
		renderer::DescriptorSetPoolPtr createPool( uint32_t maxSets
			, bool automaticFree )const override;
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
