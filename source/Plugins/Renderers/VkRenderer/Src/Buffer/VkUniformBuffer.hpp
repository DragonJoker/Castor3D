/**
*\file
*	UniformBuffer.h
*\author
*	Sylvain Doremus
*/
#ifndef ___VkRenderer_UniformBuffer_HPP___
#define ___VkRenderer_UniformBuffer_HPP___
#pragma once

#include "VkRendererPrerequisites.hpp"

#include <Buffer/UniformBuffer.hpp>

namespace vk_renderer
{
	class UniformBuffer
		: public renderer::UniformBufferBase
	{
	public:
		/**
		*\brief
		*	Constructeur.
		*\param[in] device
		*	Le périphérique logique.
		*\param[in] count
		*	Le nombre d'instance des données.
		*\param[in] target
		*	Les indicateurs d'utilisation du tampon.
		*\param[in] flags
		*	Les indicateurs de mémoire du tampon.
		*/
		UniformBuffer( renderer::Device const & device
			, uint32_t count
			, uint32_t size
			, renderer::BufferTargets target
			, renderer::MemoryPropertyFlags flags );
		/**
		*\copydoc		renderer::UniformBuffer::getAlignedSize
		*/
		uint32_t getAlignedSize( uint32_t size )const override;
	};
}

#endif
