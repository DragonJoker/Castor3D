/*
This file belongs to Renderer.
See LICENSE file in root folder.
*/
#ifndef ___Renderer_DescriptorSetLayout_HPP___
#define ___Renderer_DescriptorSetLayout_HPP___
#pragma once

#include "DescriptorSetLayoutBinding.hpp"

namespace renderer
{
	/**
	*\brief
	*	Un layout de set de descripteurs.
	*/
	class DescriptorSetLayout
	{
	protected:
		/**
		*\brief
		*	Constructeur.
		*\param[in] device
		*	Le p�riph�rique logique.
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
		*\return
		*	L'attache de descripteur au point d'attache donn�.
		*/
		DescriptorSetLayoutBinding const & getBinding( uint32_t point )const;
		/**
		*\return
		*	Les attaches de descripteurs.
		*/
		inline DescriptorSetLayoutBindingArray const & getBindings()const
		{
			return m_bindings;
		}
		/**
		*\brief
		*	Cr�e un pool pour les descripteurs qui utiliseront ce layout.
		*\param[in] maxSets
		*	Le nombre maximum de sets que le pool peut cr�er.
		*\return
		*	Le pool.
		*/
		virtual DescriptorSetPoolPtr createPool( uint32_t maxSets )const = 0;

	protected:
		DescriptorSetLayoutBindingArray m_bindings;
	};
}

#endif
