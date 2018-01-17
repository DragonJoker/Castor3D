/*
This file belongs to Renderer.
See LICENSE file in root folder.
*/
#ifndef ___Renderer_BufferMemoryBarrier_HPP___
#define ___Renderer_BufferMemoryBarrier_HPP___
#pragma once

#include "Buffer.hpp"
#include "StagingBuffer.hpp"
#include "UniformBuffer.hpp"
#include "VertexBuffer.hpp"

namespace renderer
{
	/**
	*\brief
	*	Encapsulation d'un VkBufferMemoryBarrier.
	*/
	class BufferMemoryBarrier
	{
	public:
		/**
		*\brief
		*	Constructeur.
		*\param[in] srcAccessMask
		*	Les indicateurs d'accès avant la barrière.
		*\param[in] dstAccessMask
		*	Les indicateurs d'accès après la barrière.
		*\param[in] srcQueueFamilyIndex
		*	La famille de file voulue avant la barrière.
		*\param[in] dstQueueFamilyIndex
		*	La famille de file voulue après la barrière.
		*\param[in] buffer
		*	Le tampon.
		*\param[in] offset
		*	L'offset dans le tampon.
		*\param[in] size
		*	La taille du tampon.
		*/
		BufferMemoryBarrier( AccessFlags srcAccessMask
			, AccessFlags dstAccessMask
			, uint32_t srcQueueFamilyIndex
			, uint32_t dstQueueFamilyIndex
			, BufferBase const & buffer
			, uint64_t offset
			, uint64_t size )
			: m_srcAccessMask{ srcAccessMask }
			, m_dstAccessMask{ dstAccessMask }
			, m_srcQueueFamilyIndex{ srcQueueFamilyIndex }
			, m_dstQueueFamilyIndex{ dstQueueFamilyIndex }
			, m_buffer{ buffer }
			, m_offset{ offset }
			, m_size{ size }
		{
		}
		/**
		*\return
		*	Les indicateurs d'accès avant la barrière.
		*/
		inline AccessFlags getSrcAccessMask()const
		{
			return m_srcAccessMask;
		}
		/**
		*\return
		*	Les indicateurs d'accès après la barrière.
		*/
		inline AccessFlags getDstAccessMask()const
		{
			return m_dstAccessMask;
		}
		/**
		*\return
		*	La famille de file voulue avant la barrière.
		*/
		inline uint32_t getSrcQueueFamilyIndex()const
		{
			return m_srcQueueFamilyIndex;
		}
		/**
		*\return
		*	La famille de file voulue après la barrière.
		*/
		inline uint32_t getDstQueueFamilyIndex()const
		{
			return m_dstQueueFamilyIndex;
		}
		/**
		*\return
		*	L'image.
		*/
		inline BufferBase const & getBuffer()const
		{
			return m_buffer;
		}
		/**
		*\return
		*	L'offset dans le tampon.
		*/
		inline uint64_t getOffset()const
		{
			return m_offset;
		}
		/**
		*\return
		*	La taille du tampon.
		*/
		inline uint64_t getSize()const
		{
			return m_size;
		}

	private:
		AccessFlags m_srcAccessMask;
		AccessFlags m_dstAccessMask;
		uint32_t m_srcQueueFamilyIndex;
		uint32_t m_dstQueueFamilyIndex;
		BufferBase const & m_buffer;
		uint64_t m_offset;
		uint64_t m_size;
	};
}

#endif
