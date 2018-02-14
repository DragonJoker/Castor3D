/*
This file belongs to Renderer.
See LICENSE file in root folder.
*/
#ifndef ___Renderer_DescriptorSetLayout_HPP___
#define ___Renderer_DescriptorSetLayout_HPP___
#pragma once

#include "Descriptor/DescriptorSetLayoutBinding.hpp"

namespace renderer
{
	/**
	*\~english
	*\brief
	*	A descriptor set layout.
	*\~french
	*\brief
	*	Un layout de set de descripteurs.
	*/
	class DescriptorSetLayout
	{
	protected:
		/**
		*\~english
		*\brief
		*	Constructor.
		*\param[in] device
		*	The logical device.
		*\param[in] bindings
		*	The layout bindings.
		*\~french
		*\brief
		*	Constructeur.
		*\param[in] device
		*	Le périphérique logique.
		*\param[in] bindings
		*	Les attaches du layout.
		*/
		DescriptorSetLayout( Device const & device
			, DescriptorSetLayoutBindingArray && bindings );

	public:
		/**
		*\~english
		*\brief
		*	Destructor.
		*\~french
		*\brief
		*	Destructeur.
		*/
		virtual ~DescriptorSetLayout() = default;
		/**
		*\~english
		*\return
		*	The descriptor set attach at given binding point and index.
		*\param[in] point
		*	The binding point.
		*\param[in] index
		*	The binding index, for array descriptors.
		*\~french
		*\return
		*	L'attache de descripteur au point d'attache et à l'index donnés.
		*\param[in] point
		*	Le point d'attache.
		*\param[in] index
		*	L'index d'attache, pour les descipteurs tableau.
		*/
		DescriptorSetLayoutBinding const & getBinding( uint32_t point
			, uint32_t index = 0u )const;
		/**
		*\~english
		*\return
		*	The descriptor set attaches.
		*\~french
		*\return
		*	Les attaches de descripteurs.
		*/
		inline DescriptorSetLayoutBindingArray const & getBindings()const
		{
			return m_bindings;
		}
		/**
		*\~english
		*\brief
		*	Creates a pool for the descriptor sets using this layout.
		*\param[in] maxSets
		*	The maximum descriptor sets taht can be created from the pool.
		*\param[in] automaticFree
		*	Tells if the pool will destroy all allocated sets at its destruction (\p true) or not (\p false).
		*\return
		*	The created pool.
		*\~french
		*\brief
		*	Crée un pool pour les descripteurs qui utiliseront ce layout.
		*\param[in] maxSets
		*	Le nombre maximum de sets que le pool peut créer.
		*\param[in] automaticFree
		*	Dit si le pool détruira automatiquement les sets qu'il a alloués à sa destruction (\p true) ou pas (\p false).
		*\return
		*	Le pool créé.
		*/
		virtual DescriptorSetPoolPtr createPool( uint32_t maxSets
			, bool automaticFree = true )const = 0;

	protected:
		DescriptorSetLayoutBindingArray m_bindings;
	};
}

#endif
