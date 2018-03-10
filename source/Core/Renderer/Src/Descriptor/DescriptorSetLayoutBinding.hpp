/*
This file belongs to RendererLib.
See LICENSE file in root folder.
*/
#ifndef ___Renderer_DescriptorSetLayoutBinding_HPP___
#define ___Renderer_DescriptorSetLayoutBinding_HPP___
#pragma once

#include "RendererPrerequisites.hpp"

namespace renderer
{
	/**
	*\brief
	*	Attache d'un layout de descripteur de ressources shader.
	*/
	class DescriptorSetLayoutBinding
	{
	public:
		/**
		*\brief
		*	Constructeur
		*\param[in] index
		*	L'index d'attache.
		*\param[in] type
		*	Le type d'attache.
		*\param[in] flags
		*	Les indicateurs d'étape shader.
		*\param[in] count
		*	Le nombre de descripteurs (pour les tableaux).
		*\~english
		*\brief
		*	Constructor.
		*\param[in] index
		*	The binding index.
		*\param[in] type
		*	The binding type.
		*\param[in] flags
		*	The shader stage flags.
		*\param[in] count
		*	The descriptors count (for arrays).
		*/ 
		DescriptorSetLayoutBinding( uint32_t index
			, DescriptorType type
			, ShaderStageFlags flags
			, uint32_t count = 1u );
		/**
		*\~english
		*\return
		*	The binding point.
		*\~french
		*\return
		*	Le point d'attache.
		*/
		inline uint32_t getBindingPoint()const
		{
			return m_index;
		}
		/**
		*\~english
		*\return
		*	The descriptor type.
		*\~french
		*\return
		*	Le Le type de descripteur.
		*/
		inline DescriptorType getDescriptorType()const
		{
			return m_type;
		}
		/**
		*\~english
		*\return
		*	The shader stage flags.
		*\~french
		*\return
		*	Les indicateurs de niveau de shader.
		*/
		inline ShaderStageFlags getShaderStageFlags()const
		{
			return m_flags;
		}
		/**
		*\~english
		*\return
		*	The descriptors count.
		*\~french
		*\return
		*	Le nombre de descripteurs.
		*/
		inline uint32_t getDescriptorsCount()const
		{
			return m_count;
		}

	private:
		uint32_t m_index;
		DescriptorType m_type;
		ShaderStageFlags m_flags;
		uint32_t m_count;
	};
}

#endif
