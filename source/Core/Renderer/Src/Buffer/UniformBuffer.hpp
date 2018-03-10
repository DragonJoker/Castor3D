/*
This file belongs to RendererLib.
See LICENSE file in root folder.
*/
#ifndef ___Renderer_UniformBuffer_HPP___
#define ___Renderer_UniformBuffer_HPP___
#pragma once

#include "Buffer/Buffer.hpp"
#include "Core/RenderingResources.hpp"

namespace renderer
{
	/**
	*\~french
	*\brief
	*	A uniform variables buffer.
	*\~french
	*\brief
	*	Représente un tampon de variables uniformes.
	*/
	class UniformBufferBase
	{
	protected:
		/**
		*\~english
		*	Constructor.
		*\param[in] device
		*	The logical device.
		*\param[in] count
		*	The number of instances of the data.
		*\param[in] size
		*	The size of an instance, in bytes.
		*\param[in] target
		*	The buffer usage flags.
		*\param[in] flags
		*	The memory property flags.
		*\~french
		*\brief
		*	Constructeur.
		*\param[in] device
		*	Le périphérique logique.
		*\param[in] count
		*	Le nombre d'instance des données.
		*\param[in] size
		*	La taille d'une instance des données, en octets.
		*\param[in] target
		*	Les indicateurs d'utilisation du tampon.
		*\param[in] flags
		*	Les indicateurs de mémoire du tampon.
		*/
		UniformBufferBase( Device const & device
			, uint32_t count
			, uint32_t size
			, BufferTargets target
			, MemoryPropertyFlags flags );

	public:
		/**
		*\~english
		*\brief
		*	Destructor.
		*\~french
		*\brief
		*	Destructeur.
		*/
		virtual ~UniformBufferBase() = default;
		/**
		*\~english
		*\brief
		*	Retrieves the aligned size for an element.
		*\param[in] size
		*	The size of an element.
		*\return
		*	The aligned size.
		*\~french
		*\brief
		*	Récupère la taille alignée pour un élément.
		*\param[in] size
		*	La taille d'un élément.
		*\return
		*	La taille alignée.
		*/
		virtual uint32_t getAlignedSize( uint32_t size )const = 0;
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
		*\return
		*	The size of one element in the buffer.
		*\~french
		*\return
		*	La taille d'une instance des données du tampon.
		*/
		inline uint32_t getElementSize()const
		{
			return m_size;
		}

	protected:
		Device const & m_device;
		uint32_t m_count;
		uint32_t m_size;
		BufferBasePtr m_buffer;
	};
	/**
	*\~french
	*\brief
	*	A template class wrapping a UniformBufferBase.
	*\~french
	*\brief
	*	Classe template wrappant un UniformBufferBase.
	*/
	template< typename T >
	class UniformBuffer
	{
	public:
		/**
		*\~english
		*	Constructor.
		*\param[in] device
		*	The logical device.
		*\param[in] count
		*	The number of instances of the data.
		*\param[in] target
		*	The buffer usage flags.
		*\param[in] flags
		*	The memory property flags.
		*\~french
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
		inline UniformBuffer( Device const & device
			, uint32_t count
			, BufferTargets target
			, MemoryPropertyFlags flags );
		/**
		*\~english
		*\return
		*	The N-th instance of the data.
		*\~french
		*\return
		*	La n-ème instance des données.
		*/
		inline T const & getData( uint32_t index = 0 )const
		{
			return m_data[index];
		}
		/**
		*\~english
		*\return
		*	The N-th instance of the data.
		*\~french
		*\return
		*	La n-ème instance des données.
		*/
		inline T & getData( uint32_t index = 0 )
		{
			return m_data[index];
		}
		/**
		*\~english
		*\return
		*	The data.
		*\~french
		*\return
		*	Les données.
		*/
		inline std::vector< T > const & getDatas( uint32_t index = 0 )const
		{
			return m_data;
		}
		/**
		*\~english
		*\return
		*	The data.
		*\~french
		*\return
		*	Les données.
		*/
		inline std::vector< T > & getDatas( uint32_t index = 0 )
		{
			return m_data;
		}
		/**
		*\~english
		*\brief
		*	Retrieves the aligned size for an element.
		*\return
		*	The aligned size.
		*\~french
		*\brief
		*	Récupère la taille alignée pour un élément.
		*\return
		*	La taille alignée.
		*/
		inline uint32_t getAlignedSize()const
		{
			return m_ubo->getAlignedSize( uint32_t( sizeof( T ) ) );
		}
		/**
		*\~english
		*\return
		*	The GPU buffer.
		*\~french
		*\return
		*	Le tampon GPU.
		*/
		inline UniformBufferBase const & getUbo()const
		{
			return *m_ubo;
		}
		/**
		*\~english
		*\brief
		*	Uploads the buffer data to VRAM
		*\param[in] offset
		*	The offset in elements from which buffer memory is mapped.
		*\param[in] range
		*	The number of elements to map.
		*\~french
		*\brief
		*	Met en VRAM les données du tampon.
		*\param[in] offset
		*	L'offset à partir duquel la mémoire du tampon est mappée.
		*\param[in] range
		*	Le nombre d'éléments à mapper.
		*/
		inline void upload( uint32_t offset = 0u
			, uint32_t range = 1u )
		{
			assert( range + offset <= m_data.size() );
			auto size = getAlignedSize();

			if ( auto buffer = m_ubo->getBuffer().lock( offset * size
				, range * size
				, renderer::MemoryMapFlag::eWrite | renderer::MemoryMapFlag::eInvalidateRange ) )
			{
				for ( auto i = 0u; i < range; ++i )
				{
					std::memcpy( buffer, &m_data[offset + i], sizeof( T ) );
					buffer += size;
				}

				m_ubo->getBuffer().flush( offset * size, range * size );
				m_ubo->getBuffer().unlock();
			}
		}

	private:
		UniformBufferBasePtr m_ubo;
		std::vector< T > m_data;
	};
	/**
	*\~french
	*\brief
	*	UniformBuffer creation helper function.
	*\param[in] device
	*	The logical device.
	*\param[in] count
	*	The number of instances of the data.
	*\param[in] target
	*	The buffer usage flags.
	*\param[in] flags
	*	The memory property flags.
	*\return
	*	The created buffer.
	*\~french
	*\brief
	*	Fonction d'aide à la création d'un UniformBuffer.
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
	inline UniformBufferPtr< T > makeUniformBuffer( Device const & device
		, uint32_t count
		, BufferTargets target
		, MemoryPropertyFlags flags )
	{
		return std::make_unique< UniformBuffer< T > >( device
			, count
			, target
			, flags );
	}
}

#include "UniformBuffer.inl"

#endif
