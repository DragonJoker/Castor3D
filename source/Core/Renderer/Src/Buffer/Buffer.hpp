/*
This file belongs to RendererLib.
See LICENSE file in root folder.
*/
#ifndef ___Renderer_Buffer_HPP___
#define ___Renderer_Buffer_HPP___
#pragma once

#include "Core/Device.hpp"
#include "Miscellaneous/DeviceMemory.hpp"

namespace renderer
{
	/**
	*\~english
	*\brief
	*	GPU buffer class.
	*\~french
	*\brief
	*	Classe de tampon GPU.
	*/
	class BufferBase
	{
	protected:
		/**
		*\~english
		*\brief
		*	Constructor.
		*\param[in] device
		*	The logical device.
		*\param[in] size
		*	The buffer size.
		*\param[in] target
		*	The buffer usage flags.
		*\~french
		*\brief
		*	Constructeur.
		*\param[in] device
		*	Le périphérique logique.
		*\param[in] size
		*	La taille du tampon.
		*\param[in] target
		*	Les indicateurs d'utilisation du tampon.
		*/
		BufferBase( Device const & device
			, uint32_t size
			, BufferTargets target );

	public:
		/**
		*\~english
		*\brief
		*	Destructor.
		*\~french
		*\brief
		*	Destructeur.
		*/
		virtual ~BufferBase() = default;
		/**
		*\~english
		*\brief
		*	Binds this buffer to given device memory object.
		*\param[in] memory
		*	The memory object.
		*\~french
		*\brief
		*	Lie ce tampon à l'objet mémoire donné.
		*\param[in] memory
		*	L'object mémoire de périphérique.
		*/
		void bindMemory( DeviceMemoryPtr memory );
		/**
		*\~english
		*\brief
		*	Maps a range of the buffer's memory in RAM.
		*\param[in] offset
		*	The range beginning offset.
		*\param[in] size
		*	The range size.
		*\param[in] flags
		*	The mapping flags.
		*\return
		*	\p nullptr if mapping failed.
		*\~french
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
		uint8_t * lock( uint32_t offset
			, uint32_t size
			, MemoryMapFlags flags )const;
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
		void invalidate( uint32_t offset
			, uint32_t size )const;
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
		void flush( uint32_t offset
			, uint32_t size )const;
		/**
		*\~english
		*\brief
		*	Unmaps the buffer's memory from RAM.
		*\~french
		*\brief
		*	Unmappe la mémoire du tampon de la RAM.
		*/
		void unlock()const;
		/**
		*\~english
		*\return
		*	The memory requirements for this buffer.
		*\~french
		*\return
		*	Les exigences mémoire pour ce tampon.
		*/
		virtual MemoryRequirements getMemoryRequirements()const = 0;
		/**
		*\~english
		*\brief
		*	Prepares a buffer memory barrier, to a transfer destination layout.
		*\return
		*	The memory barrier.
		*\~french
		*\brief
		*	Prépare une barrière mémoire de transition vers un layout de destination de transfert.
		*\return
		*	La barrière mémoire.
		*/
		virtual BufferMemoryBarrier makeTransferDestination()const = 0;
		/**
		*\~english
		*\brief
		*	Prepares a buffer memory barrier, to a transfer source layout.
		*\return
		*	The memory barrier.
		*\~french
		*\brief
		*	Prépare une barrière mémoire de transition vers un layout de source de transfert.
		*\return
		*	La barrière mémoire.
		*/
		virtual BufferMemoryBarrier makeTransferSource()const = 0;
		/**
		*\~english
		*\brief
		*	Prepares a buffer memory barrier, to a vertex shader input layout.
		*\return
		*	The memory barrier.
		*\~french
		*\brief
		*	Prépare une barrière mémoire de transition vers un layout de ressource d'entrée (lecture seule) d'un vertex shader.
		*\return
		*	La barrière mémoire.
		*/
		virtual BufferMemoryBarrier makeVertexShaderInputResource()const = 0;
		/**
		*\~english
		*\brief
		*	Prepares a buffer memory barrier, to a uniform buffer input layout.
		*\return
		*	The memory barrier.
		*\~french
		*\brief
		*	Prépare une barrière mémoire de transition vers un layout de ressource d'entrée (lecture seule) d'un shader.
		*\return
		*	La barrière mémoire.
		*/
		virtual BufferMemoryBarrier makeUniformBufferInput()const = 0;
		/**
		*\~english
		*\brief
		*	Prepares a buffer memory barrier, to a given memory layout.
		*\param[in] dstAccess
		*	The wanted access flags, after the transition.
		*\return
		*	The memory barrier.
		*\~french
		*\brief
		*	Prépare une barrière mémoire de transition vers un layout mémoire donné.
		*\param[in] dstAccess
		*	Les indicateurs d'accès voulus après la transition.
		*\return
		*	La barrière mémoire.
		*/
		virtual BufferMemoryBarrier makeMemoryTransitionBarrier( AccessFlags dstAccess )const = 0;
		/**
		*\~english
		*\return
		*	The buffer size.
		*\~french
		*\return
		*	La taille du tampon.
		*/
		inline uint32_t getSize()const
		{
			return m_size;
		}
		/**
		*\~english
		*\return
		*	The buffer usage flags.
		*\~french
		*\return
		*	Les cibles du tampon.
		*/
		inline BufferTargets getTargets()const
		{
			return m_target;
		}

	private:
		virtual void doBindMemory() = 0;

	protected:
		Device const & m_device;
		uint32_t m_size;
		BufferTargets m_target;
		DeviceMemoryPtr m_storage;
	};
	/**
	*\~english
	*\brief
	*	Template class wrapping a GPU buffer, to store structured Data.
	*\~french
	*\brief
	*	Classe template encapsulant un tampon GPU, pour stocker des données structurées.
	*/
	template< typename T >
	class Buffer
	{
	public:
		/**
		*\~english
		*\brief
		*	Constructor.
		*\param[in] device
		*	The logical device.
		*\param[in] count
		*	The buffer elements count.
		*\param[in] target
		*	The buffer usage flags.
		*\param[in] flags
		*	The buffer memory flags.
		*\~french
		*\brief
		*	Constructeur.
		*\param[in] device
		*	Le périphérique logique.
		*\param[in] count
		*	Le nombre d'éléments du tampon.
		*\param[in] target
		*	Les indicateurs d'utilisation du tampon.
		*\param[in] flags
		*	Les indicateurs de mémoire du tampon.
		*/
		Buffer( Device const & device
			, uint32_t count
			, BufferTargets target
			, MemoryPropertyFlags flags );
		/**
		*\~english
		*\return
		*	The elements count.
		*\~french
		*\return
		*	Le nombre d'éléments.
		*/
		inline uint32_t getCount()const
		{
			return uint32_t( m_buffer->getSize() / sizeof( T ) );
		}
		/**
		*\~english
		*\return
		*	The GPU buffer.
		*\~french
		*\return
		*	Le tampon GPU.
		*/
		inline BufferBase const & getBuffer()const
		{
			return *m_buffer;
		}
		/**
		*\~english
		*\brief
		*	Maps a range of the buffer's memory in RAM.
		*\param[in] offset
		*	The range beginning offset.
		*\param[in] size
		*	The range size.
		*\param[in] flags
		*	The mapping flags.
		*\return
		*	\p nullptr if mapping failed.
		*\~french
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

	private:
		BufferBasePtr m_buffer;
	};
	/**
	*\~english
	*\brief
	*	Helper function tor create a Buffer< T >.
	*\param[in] device
	*	The logical device.
	*\param[in] count
	*	The buffer elements count.
	*\param[in] target
	*	The buffer usage flags.
	*\param[in] flags
	*	The buffer memory flags.
	*\return
	*	The created buffer.
	*\~french
	*\brief
	*	Fonction d'aide à la création d'un Buffer< T >.
	*\param[in] device
	*	Les périphérique logique.
	*\param[in] count
	*	Le nombre d'éléments du tampon.
	*\param[in] target
	*	Les indicateurs d'utilisation du tampon.
	*\param[in] flags
	*	Les indicateurs de mémoire du tampon.
	*\return
	*	Le tampon créé.
	*/
	template< typename T >
	BufferPtr< T > makeBuffer( Device const & device
		, uint32_t count
		, BufferTargets target
		, MemoryPropertyFlags flags )
	{
		return std::make_unique< Buffer< T > >( device
			, count
			, target
			, flags );
	}
}

#include "Buffer.inl"

#endif
