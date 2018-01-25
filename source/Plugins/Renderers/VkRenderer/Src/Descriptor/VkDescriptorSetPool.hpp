/**
*\file
*	DescriptorSetPool.h
*\author
*	Sylvain Doremus
*/
#ifndef ___VkRenderer_DescriptorSetPool_HPP___
#define ___VkRenderer_DescriptorSetPool_HPP___
#pragma once

#include "VkRendererPrerequisites.hpp"

#include <Descriptor/DescriptorSetPool.hpp>

#include <vector>

namespace vk_renderer
{
	/**
	*\~french
	*\brief
	*	Pool de descripteurs d'un layout défini.
	*\~english
	*\brief
	*	Pool to create descriptors of a defined layout.
	*/
	class DescriptorSetPool
		: public renderer::DescriptorSetPool
	{
	public:
		/**
		*\~french
		*\brief
		*	Constructeur.
		*\param[in] device
		*	Le device parent.
		*\param[in] layout
		*	Le layout à partir duquel sera créé le pool.
		*\param[in] maxSets
		*	Le nombre maximum de descripteurs que le pool peut créer.
		*\~english
		*\brief
		*	Constructor.
		*\param[in] device
		*	The logical connection to the GPU.
		*\param[in] layout
		*	The descriptors layout.
		*\param[in] maxSets
		*	The maximum sets count the pool can create.
		*/
		DescriptorSetPool( Device const & device
			, DescriptorSetLayout const & layout
			, uint32_t maxSets );
		/**
		*\~french
		*\brief
		*	Destructeur.
		*\~english
		*\brief
		*	Destructor.
		*/
		~DescriptorSetPool();
		/**
		*\~french
		*\brief
		*	Crée un descriptor set correspondant au layout défini pour ce pool.
		*\return
		*	Le descriptor set créé.
		*\~english
		*\brief
		*	Creates a descriptor set, matching the layout defined for this pool.
		*\return
		*	The created descriptor set.
		*/
		renderer::DescriptorSetPtr createDescriptorSet()const override;
		/**
		*\~french
		*\brief
		*	Dit si le pool désalloue automatiquement les descripteurs à sa propre destruction.
		*\~english
		*\return
		*	Tells if the pool automatically deallocates the descriptor sets during its on destruction.
		*/
		inline bool hasAutomaticFree()const
		{
			return true;
		}
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
		DescriptorSetLayout const & m_layout;
		VkDescriptorPool m_pool{};
	};
}

#endif
