/*
This file belongs to RendererLib.
See LICENSE file in root folder.
*/
#ifndef ___Renderer_DescriptorSetPool_HPP___
#define ___Renderer_DescriptorSetPool_HPP___
#pragma once

#include "Descriptor/DescriptorPool.hpp"

#include <vector>

namespace renderer
{
	/**
	*\brief
	*	Pool de sets de descripteurs.
	*/
	class DescriptorSetPool
	{
	public:
		/**
		*\~english
		*\brief
		*	Constructor.
		*\param[in] layout
		*	The layout from which the pool will be created.
		*\param[in] maxSets
		*	The maximum sets count that can be created from this pool.
		*\~french
		*\brief
		*	Constructeur.
		*\param[in] layout
		*	Le layout à partir duquel sera créé le pool.
		*\param[in] maxSets
		*	Le nombre maximum de sets que le pool peut créer.
		*/
		DescriptorSetPool( Device const & device
			, DescriptorSetLayout const & layout
			, uint32_t maxSets
			, bool automaticFree );
		/**
		*\~english
		*\brief
		*	Destructor.
		*\~french
		*\brief
		*	Destructeur.
		*/
		virtual ~DescriptorSetPool() = default;
		/**
		*\~english
		*\brief
		*	Creates a descriptor set matching the layout defined for this pool.
		*\param[in] bindingPoint
		*	The binding point for the set.
		*\return
		*	The created descriptor set.
		*\~french
		*\brief
		*	Crée un descriptor set correspondant au layout défini pour ce pool.
		*\param[in] bindingPoint
		*	Le point d'attache du set.
		*\return
		*	Le descriptor set créé.
		*/
		DescriptorSetPtr createDescriptorSet( uint32_t bindingPoint = 0u )const;
		/**
		*\~english
		*\return
		*	The descriptor set layout.
		*\~french
		*\return
		*	Le layout de descriptor set.
		*/
		inline DescriptorSetLayout const & getLayout()const
		{
			return m_layout;
		}

	private:
		DescriptorSetLayout const & m_layout;
		uint32_t m_maxSets;
		DescriptorPoolPtr m_pool;
	};
}

#endif
