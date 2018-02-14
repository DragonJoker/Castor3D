/*
This file belongs to Renderer.
See LICENSE file in root folder.
*/
#ifndef ___Renderer_DescriptorSetPool_HPP___
#define ___Renderer_DescriptorSetPool_HPP___
#pragma once

#include "RendererPrerequisites.hpp"

#include <vector>

namespace renderer
{
	/**
	*\brief
	*	Pool de sets de descripteurs.
	*/
	class DescriptorSetPool
	{
		friend class DescriptorSet;

	protected:
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
		DescriptorSetPool( DescriptorSetLayout const & layout, uint32_t maxSets );

	public:
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
		virtual DescriptorSetPtr createDescriptorSet( uint32_t bindingPoint = 0u )const = 0;
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
		/**
		*\~english
		*\brief
		*	Allocates a descriptor set.
		*\param[in] count
		*	The number of sets.
		*\~french
		*\brief
		*	Alloue des descripteurs.
		*\param[in] count
		*	Le nombre de descripteurs.
		*/
		void allocate( uint32_t count )const;
		/**
		*\~english
		*\brief
		*	Allocates a descriptor set.
		*\param[in] count
		*	The number of sets.
		*\~french
		*\brief
		*	Alloue des descripteurs.
		*\param[in] count
		*	Le nombre de descripteurs.
		*/
		void deallocate( uint32_t count )const;

	private:
		DescriptorSetLayout const & m_layout;
		uint32_t m_maxSets;
		mutable uint32_t m_allocated{ 0u };
	};
}

#endif
