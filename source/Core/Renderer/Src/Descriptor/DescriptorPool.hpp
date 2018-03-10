/*
This file belongs to RendererLib.
See LICENSE file in root folder.
*/
#ifndef ___Renderer_DescriptorPool_HPP___
#define ___Renderer_DescriptorPool_HPP___
#pragma once

#include "DescriptorPoolSize.hpp"

#include <vector>

namespace renderer
{
	/**
	*\brief
	*	Pool de descripteurs.
	*/
	class DescriptorPool
	{
	protected:
		/**
		*\~english
		*\brief
		*	Constructor.
		*\~french
		*\brief
		*	Constructeur.
		*/
		DescriptorPool( DescriptorPoolCreateFlags flags );

	public:
		/**
		*\~english
		*\brief
		*	Destructor.
		*\~french
		*\brief
		*	Destructeur.
		*/
		virtual ~DescriptorPool() = default;
		/**
		*\~english
		*\brief
		*	Creates a descriptor set matching the given layout.
		*\param[in] layout
		*	The layout describing the set.
		*\param[in] bindingPoint
		*	The binding point for the set.
		*\return
		*	The created descriptor set.
		*\~french
		*\brief
		*	Crée un descriptor set correspondant au layout donné.
		*\param[in] layout
		*	Le layout décrivant l'ensemble.
		*\param[in] bindingPoint
		*	Le point d'attache du set.
		*\return
		*	Le descriptor set créé.
		*/
		virtual DescriptorSetPtr createDescriptorSet( renderer::DescriptorSetLayout const & layout
			, uint32_t bindingPoint = 0u )const = 0;
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
			return m_automaticFree;
		}

	private:
		bool m_automaticFree;
	};
}

#endif
