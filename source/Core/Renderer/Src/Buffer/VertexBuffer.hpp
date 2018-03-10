/*
This file belongs to RendererLib.
See LICENSE file in root folder.
*/
#ifndef ___Renderer_VertexBuffer_HPP___
#define ___Renderer_VertexBuffer_HPP___
#pragma once

#include "Buffer/Buffer.hpp"

#include <vector>

namespace renderer
{
	/**
	*\brief
	*	Tampon de sommets.
	*/
	class VertexBufferBase
	{
	public:
		/**
		*\brief
		*	Constructeur.
		*\param[in] device
		*	Le périphérique logique.
		*\param[in] size
		*	La taille du tampon.
		*\param[in] target
		*	Les indicateurs d'utilisation du tampon.
		*\param[in] flags
		*	Les indicateurs de mémoire du tampon.
		*/
		VertexBufferBase( Device const & device
			, uint32_t size
			, BufferTargets target
			, MemoryPropertyFlags flags );
		/**
		*\return
		*	La taille du tampon.
		*/
		inline uint32_t getSize()const
		{
			return m_size;
		}
		/**
		*\return
		*	Le tampon GPU.
		*/
		inline BufferBase const & getBuffer()const
		{
			return *m_buffer;
		}
		/**
		*\return
		*	Le périphérique logique.
		*/
		inline Device const & getDevice()const
		{
			return m_device;
		}

	protected:
		Device const & m_device;
		uint32_t m_size;
		BufferBasePtr m_buffer;
	};
	/**
	*\brief
	*	Tampon de sommets, typé.
	*/
	template< typename T >
	class VertexBuffer
		: public VertexBufferBase
	{
	public:
		/**
		*\brief
		*	Constructeur.
		*\param[in] device
		*	Le périphérique logique.
		*\param[in] count
		*	La nombre d'éléments dans le tampon.
		*\param[in] target
		*	Les indicateurs d'utilisation du tampon.
		*\param[in] flags
		*	Les indicateurs de mémoire du tampon.
		*/
		inline VertexBuffer( Device const & device
			, uint32_t count
			, BufferTargets target
			, MemoryPropertyFlags flags );
		/**
		*\return
		*	Le nombre d'éléments contenus dans ce tampon.
		*/
		inline uint32_t getCount()const
		{
			return m_buffer->getSize() / sizeof( T );
		}
		/**
		*\brief
		*	Mappe la mémoire du tampon en RAM.
		*\param[in] offset
		*	L'offset à partir duquel la mémoire du tampon est mappée.
		*\param[in] size
		*	La taille en octets de la mémoire à mapper.
		*\param[in] flags
		*	Indicateurs de configuration du mapping.
		*\return
		*	\p nullptr si le mapping a échoué.
		*/
		inline T * lock( uint32_t offset
			, uint32_t size
			, MemoryMapFlags flags )const
		{
			return reinterpret_cast< T * >( m_buffer->lock( uint32_t( offset * sizeof( T ) )
				, uint32_t( size * sizeof( T ) )
				, flags ) );
		}
		/**
		*\~english
		*\brief
		*	Updates the VRAM.
		*\param[in] offset
		*	The mapped memory starting offset.
		*\param[in] size
		*	The range size.
		*\~french
		*\brief
		*	Met à jour la VRAM.
		*\param[in] offset
		*	L'offset de la mémoire mappée.
		*\param[in] size
		*	La taille en octets de la mémoire mappée.
		*/
		inline void flush( uint32_t offset
			, uint32_t size )const
		{
			m_buffer->flush( uint32_t( offset * sizeof( T ) )
				, uint32_t( size * sizeof( T ) ) );
		}
		/**
		*\~english
		*\brief
		*	Invalidates the buffer content.
		*\param[in] offset
		*	The mapped memory starting offset.
		*\param[in] size
		*	The range size.
		*\~french
		*\brief
		*	Invalide le contenu du tampon.
		*\param[in] offset
		*	L'offset de la mémoire mappée.
		*\param[in] size
		*	La taille en octets de la mémoire mappée.
		*/
		inline void invalidate( uint32_t offset
			, uint32_t size )const
		{
			m_buffer->invalidate( uint32_t( offset * sizeof( T ) )
				, uint32_t( size * sizeof( T ) ) );
		}
		/**
		*\~english
		*\brief
		*	Unmaps the buffer's memory from RAM.
		*\~french
		*\brief
		*	Unmappe la mémoire du tampon de la RAM.
		*/
		virtual void unlock()const
		{
			m_buffer->unlock();
		}
	};
	/**
	*\brief
	*	Fonction d'aide à la création d'un Buffer.
	*\param[in] device
	*	Le périphérique logique.
	*\param[in] count
	*	La nombre d'éléments dans le tampon.
	*\param[in] target
	*	Les indicateurs d'utilisation du tampon.
	*\param[in] flags
	*	Les indicateurs de mémoire du tampon.
	*\return
	*	Le tampon créé.
	*/
	template< typename T >
	inline VertexBufferPtr< T > makeVertexBuffer( Device const & device
		, uint32_t count
		, BufferTargets target
		, MemoryPropertyFlags flags )
	{
		return std::make_unique< VertexBuffer< T > >( device
			, count
			, target
			, flags );
	}
}

#include "VertexBuffer.inl"

#endif
