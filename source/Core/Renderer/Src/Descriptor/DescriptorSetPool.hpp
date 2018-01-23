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
	protected:
		/**
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
		*\brief
		*	Alloue des descripteurs.
		*\param[in] count
		*	Le nombre de descripteurs.
		*/
		virtual void allocate( uint32_t count )const;
		/**
		*\brief
		*	Crée un descriptor set correspondant au layout défini pour ce pool.
		*\return
		*	Le descriptor set créé.
		*/
		virtual DescriptorSetPtr createDescriptorSet()const = 0;
		/**
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
		mutable uint32_t m_allocated{ 0u };
	};
}

#endif
