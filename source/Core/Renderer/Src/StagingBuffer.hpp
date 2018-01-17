/*
This file belongs to Renderer.
See LICENSE file in root folder.
*/
#ifndef ___Renderer_StagingBuffer_HPP___
#define ___Renderer_StagingBuffer_HPP___
#pragma once

#include "Buffer.hpp"
#include "VertexBuffer.hpp"
#include "UniformBuffer.hpp"

namespace renderer
{
	/**
	*\~french
	*\brief
	*	Classe regroupant les ressources de rendu nécessaires au dessin d'une image.
	*/
	class StagingBuffer
	{
	public:
		/**
		*\~french
		*\brief
		*	Constructeur.
		*\param[in] device
		*	Le LogicalDevice parent.
		*\param[in] size
		*	La taille du tampon.
		*/
		StagingBuffer( Device const & device
			, BufferTargets target
			, uint32_t size = 10000000u );
		/**
		*\~french
		*\brief
		*	Copie les données d'une image dans une texture, en passant par le tampon de transfert.
		*\param[in] data
		*	Les données à copier.
		*\param[out] texture
		*	La texture de destination.
		*/
		void copyTextureData( CommandBuffer const & commandBuffer
			, ByteArray const & data
			, Texture const & texture )const;
		/**
		*\~french
		*\brief
		*	Copie les données de sommets dans un tampon, en passant par le tampon de transfert.
		*\param[in] data
		*	Les données à copier.
		*\param[out] buffer
		*	Le tampon de destination.
		*/
		template< typename T >
		inline void copyBufferData( CommandBuffer const & commandBuffer
			, std::vector< T > const & data
			, Buffer< T > const & buffer )const
		{
			copyBufferData( commandBuffer
				, reinterpret_cast< uint8_t const * const >( data.data() )
				, uint32_t( data.size() * sizeof( T ) )
				, 0u
				, buffer );
		}
		/**
		*\~french
		*\brief
		*	Copie les données de sommets dans un tampon, en passant par le tampon de transfert.
		*\param[in] data
		*	Les données à copier.
		*\param[out] buffer
		*	Le tampon de destination.
		*/
		template< typename T >
		inline void copyBufferData( CommandBuffer const & commandBuffer
			, ByteArray const & data
			, Buffer< T > const & buffer )const
		{
			copyBufferData( commandBuffer
				, data.data()
				, uint32_t( data.size() )
				, 0u
				, buffer );
		}
		/**
		*\~french
		*\brief
		*	Copie les données de sommets dans un tampon, en passant par le tampon de transfert.
		*\param[in] data
		*	Les données à copier.
		*\param[in] size
		*	La taille des données à copier.
		*\param[out] buffer
		*	Le tampon de destination.
		*/
		template< typename T >
		inline void copyBufferData( CommandBuffer const & commandBuffer
			, uint8_t const * const data
			, uint32_t size
			, Buffer< T > const & buffer )const
		{
			copyBufferData( commandBuffer
				, data
				, size
				, 0u
				, buffer );
		}
		/**
		*\~french
		*\brief
		*	Copie les données de sommets dans un tampon, en passant par le tampon de transfert.
		*\param[in] data
		*	Les données à copier.
		*\param[in] offset
		*	L'offset dans le tampon.
		*\param[out] buffer
		*	Le tampon de destination.
		*/
		template< typename T >
		inline void copyBufferData( CommandBuffer const & commandBuffer
			, std::vector< T > const & data
			, uint32_t offset
			, Buffer< T > const & buffer )const
		{
			copyBufferData( commandBuffer
				, reinterpret_cast< uint8_t const * const >( data.data() )
				, uint32_t( data.size() * sizeof( T ) )
				, uint32_t( offset * sizeof( T ) )
				, buffer );
		}
		/**
		*\~french
		*\brief
		*	Copie les données de sommets dans un tampon, en passant par le tampon de transfert.
		*\param[in] data
		*	Les données à copier.
		*\param[in] offset
		*	L'offset dans le tampon.
		*\param[out] buffer
		*	Le tampon de destination.
		*/
		template< typename T >
		inline void copyBufferData( CommandBuffer const & commandBuffer
			, ByteArray const & data
			, uint32_t offset
			, Buffer< T > const & buffer )const
		{
			copyBufferData( commandBuffer
				, data.data()
				, uint32_t( data.size() )
				, offset
				, buffer );
		}
		/**
		*\~french
		*\brief
		*	Copie les données de sommets dans un tampon, en passant par le tampon de transfert.
		*\param[in] data
		*	Les données à copier.
		*\param[in] size
		*	La taille des données à copier.
		*\param[in] offset
		*	L'offset dans le tampon.
		*\param[out] buffer
		*	Le tampon de destination.
		*/
		template< typename T >
		void copyBufferData( CommandBuffer const & commandBuffer
			, uint8_t const * const data
			, uint32_t size
			, uint32_t offset
			, Buffer< T > const & buffer )const
		{
			doCopyToStagingBuffer( data
				, size );
			doCopyFromStagingBuffer( commandBuffer
				, size
				, offset
				, buffer.getBuffer() );
		}
		/**
		*\~french
		*\brief
		*	Copie les données de sommets dans un tampon, en passant par le tampon de transfert.
		*\param[in] data
		*	Les données à copier.
		*\param[out] buffer
		*	Le tampon de destination.
		*/
		template< typename T >
		inline void copyVertexData( CommandBuffer const & commandBuffer
			, std::vector< T > const & data
			, VertexBuffer< T > const & buffer
			, PipelineStageFlags const & flags )const
		{
			copyVertexData( commandBuffer
				, reinterpret_cast< uint8_t const * const >( data.data() )
				, uint32_t( data.size() * sizeof( T ) )
				, 0u
				, buffer
				, flags );
		}
		/**
		*\~french
		*\brief
		*	Copie les données de sommets dans un tampon, en passant par le tampon de transfert.
		*\param[in] data
		*	Les données à copier.
		*\param[out] buffer
		*	Le tampon de destination.
		*/
		template< typename T >
		inline void copyVertexData( CommandBuffer const & commandBuffer
			, ByteArray const & data
			, VertexBuffer< T > const & buffer
			, PipelineStageFlags const & flags )const
		{
			copyVertexData( commandBuffer
				, data.data()
				, uint32_t( data.size() )
				, 0u
				, buffer
				, flags );
		}
		/**
		*\~french
		*\brief
		*	Copie les données de sommets dans un tampon, en passant par le tampon de transfert.
		*\param[in] data
		*	Les données à copier.
		*\param[in] size
		*	La taille des données à copier.
		*\param[out] buffer
		*	Le tampon de destination.
		*/
		template< typename T >
		inline void copyVertexData( CommandBuffer const & commandBuffer
			, uint8_t const * const data
			, uint32_t size
			, VertexBuffer< T > const & buffer
			, PipelineStageFlags const & flags )const
		{
			copyVertexData( commandBuffer
				, data
				, size
				, 0u
				, buffer
				, flags );
		}
		/**
		*\~french
		*\brief
		*	Copie les données de sommets dans un tampon, en passant par le tampon de transfert.
		*\param[in] data
		*	Les données à copier.
		*\param[in] offset
		*	L'offset dans le tampon.
		*\param[out] buffer
		*	Le tampon de destination.
		*/
		template< typename T >
		inline void copyVertexData( CommandBuffer const & commandBuffer
			, std::vector< T > const & data
			, uint32_t offset
			, VertexBuffer< T > const & buffer
			, PipelineStageFlags const & flags )const
		{
			copyVertexData( commandBuffer
				, reinterpret_cast< uint8_t const * const >( data.data() )
				, uint32_t( data.size() * sizeof( T ) )
				, uint32_t( offset * sizeof( T ) )
				, buffer
				, flags );
		}
		/**
		*\~french
		*\brief
		*	Copie les données de sommets dans un tampon, en passant par le tampon de transfert.
		*\param[in] data
		*	Les données à copier.
		*\param[in] offset
		*	L'offset dans le tampon.
		*\param[out] buffer
		*	Le tampon de destination.
		*/
		template< typename T >
		inline void copyVertexData( CommandBuffer const & commandBuffer
			, ByteArray const & data
			, uint32_t offset
			, VertexBuffer< T > const & buffer
			, PipelineStageFlags const & flags )const
		{
			copyVertexData( commandBuffer
				, data.data()
				, uint32_t( data.size() )
				, offset
				, buffer
				, flags );
		}
		/**
		*\~french
		*\brief
		*	Copie les données de sommets dans un tampon, en passant par le tampon de transfert.
		*\param[in] data
		*	Les données à copier.
		*\param[in] size
		*	La taille des données à copier.
		*\param[in] offset
		*	L'offset dans le tampon.
		*\param[out] buffer
		*	Le tampon de destination.
		*/
		template< typename T >
		void copyVertexData( CommandBuffer const & commandBuffer
			, uint8_t const * const data
			, uint32_t size
			, uint32_t offset
			, VertexBuffer< T > const & buffer
			, PipelineStageFlags const & flags )const
		{
			doCopyToStagingBuffer( data
				, size );
			doCopyFromStagingBuffer( commandBuffer
				, size
				, offset
				, buffer
				, flags );
		}
		/**
		*\~french
		*\brief
		*	Copie les données d'un tampon d'uniformes dans un tampon, en passant par le tampon de transfert.
		*\param[in] data
		*	Les données à copier.
		*\param[out] buffer
		*	Le tampon de destination.
		*/
		template< typename T >
		inline void copyUniformData( CommandBuffer const & commandBuffer
			, ByteArray const & data
			, UniformBuffer< T > const & buffer
			, PipelineStageFlags const & flags )const
		{
			copyUniformData( commandBuffer
				, data.data()
				, uint32_t( data.size() )
				, 0u
				, buffer
				, flags );
		}
		/**
		*\~french
		*\brief
		*	Copie les données d'un tampon d'uniformes dans un tampon, en passant par le tampon de transfert.
		*\param[in] data
		*	Les données à copier.
		*\param[in] size
		*	La taille des données à copier.
		*\param[out] buffer
		*	Le tampon de destination.
		*/
		template< typename T >
		inline void copyUniformData( CommandBuffer const & commandBuffer
			, uint8_t const * const data
			, uint32_t size
			, UniformBuffer< T > const & buffer
			, PipelineStageFlags const & flags )const
		{
			copyUniformData( commandBuffer
				, data
				, size
				, 0u
				, buffer
				, flags );
		}
		/**
		*\~french
		*\brief
		*	Copie les données d'un tampon d'uniformes dans un tampon, en passant par le tampon de transfert.
		*\param[in] data
		*	Les données à copier.
		*\param[in] offset
		*	L'offset dans le tampon.
		*\param[out] buffer
		*	Le tampon de destination.
		*/
		template< typename T >
		void copyUniformData( CommandBuffer const & commandBuffer
			, ByteArray const & data
			, uint32_t offset
			, UniformBuffer< T > const & buffer
			, PipelineStageFlags const & flags )const
		{
			copyUniformData( commandBuffer
				, data.data()
				, uint32_t( data.size() )
				, offset
				, buffer
				, flags );
		}
		/**
		*\~french
		*\brief
		*	Copie les données d'un tampon d'uniformes dans un tampon, en passant par le tampon de transfert.
		*\param[in] data
		*	Les données à copier.
		*\param[out] buffer
		*	Le tampon de destination.
		*/
		template< typename T >
		void copyUniformData( CommandBuffer const & commandBuffer
			, std::vector< T > const & data
			, UniformBuffer< T > const & buffer
			, PipelineStageFlags const & flags )const
		{
			doCopyUniformDataToStagingBuffer( data.data()
				, uint32_t( data.size() )
				, buffer.getOffset( 1u ) );
			doCopyFromStagingBuffer( commandBuffer
				, buffer.getOffset( uint32_t( data.size() ) )
				, 0u
				, buffer.getUbo()
				, flags );
		}
		/**
		*\~french
		*\brief
		*	Copie les données d'un tampon d'uniformes dans un tampon, en passant par le tampon de transfert.
		*\param[in] data
		*	Les données à copier.
		*\param[in] count
		*	Le nombre d'éléments à copier.
		*\param[out] buffer
		*	Le tampon de destination.
		*/
		template< typename T >
		void copyUniformData( CommandBuffer const & commandBuffer
			, T const * const data
			, uint32_t count
			, UniformBuffer< T > const & buffer
			, PipelineStageFlags const & flags )const
		{
			doCopyUniformDataToStagingBuffer( data.data()
				, count
				, buffer.getOffset( 1u ) );
			doCopyFromStagingBuffer( commandBuffer
				, buffer.getOffset( uint32_t( data.size() ) )
				, 0u
				, buffer.getUbo()
				, flags );
		}
		/**
		*\~french
		*\brief
		*	Copie les données d'un tampon d'uniformes dans un tampon, en passant par le tampon de transfert.
		*\param[in] data
		*	Les données à copier.
		*\param[in] offset
		*	L'offset dans le tampon.
		*\param[out] buffer
		*	Le tampon de destination.
		*/
		template< typename T >
		void copyUniformData( CommandBuffer const & commandBuffer
			, std::vector< T > const & data
			, uint32_t offset
			, UniformBuffer< T > const & buffer
			, PipelineStageFlags const & flags )const
		{
			doCopyUniformDataToStagingBuffer( data.data()
				, uint32_t( data.size() )
				, buffer.getOffset( 1u ) );
			doCopyFromStagingBuffer( commandBuffer
				, buffer.getOffset( uint32_t( data.size() ) )
				, buffer.getOffset( offset )
				, buffer.getUbo()
				, flags );
		}
		/**
		*\~french
		*\brief
		*	Copie les données d'un tampon d'uniformes dans un tampon, en passant par le tampon de transfert.
		*\param[in] data
		*	Les données à copier.
		*\param[in] count
		*	Le nombre d'éléments à copier.
		*\param[in] offset
		*	L'offset dans le tampon.
		*\param[out] buffer
		*	Le tampon de destination.
		*/
		template< typename T >
		void copyUniformData( CommandBuffer const & commandBuffer
			, T const * const data
			, uint32_t count
			, uint32_t offset
			, UniformBuffer< T > const & buffer
			, PipelineStageFlags const & flags )const
		{
			doCopyUniformDataToStagingBuffer( data.data()
				, count
				, buffer.getOffset( 1u ) );
			doCopyFromStagingBuffer( commandBuffer
				, buffer.getOffset( uint32_t( data.size() ) )
				, buffer.getOffset( offset )
				, buffer.getUbo()
				, flags );
		}
		/**
		*\~french
		*\brief
		*	Copie les données d'un tampon d'uniformes dans un tampon, en passant par le tampon de transfert.
		*\param[in] data
		*	Les données à copier.
		*\param[in] size
		*	La taille des données à copier.
		*\param[in] offset
		*	L'offset dans le tampon.
		*\param[out] buffer
		*	Le tampon de destination.
		*/
		template< typename T >
		void copyUniformData( CommandBuffer const & commandBuffer
			, uint8_t const * const data
			, uint32_t size
			, uint32_t offset
			, UniformBuffer< T > const & buffer
			, PipelineStageFlags const & flags )const
		{
			doCopyToStagingBuffer( data
				, size );
			doCopyFromStagingBuffer( commandBuffer
				, size
				, offset
				, buffer.getUbo()
				, flags );
		}
		/**
		*\return
		*	Le tampon GPU.
		*/
		inline BufferBase const & getBuffer()const
		{
			return *m_buffer;
		}

	protected:
		virtual void doCopyToStagingBuffer( uint8_t const * const data
			, uint32_t size )const;

	private:
		template< typename T >
		inline void doCopyUniformDataToStagingBuffer( T const * const data
			, uint32_t count
			, uint32_t offset )const;
		void doCopyFromStagingBuffer( CommandBuffer const & commandBuffer
			, uint32_t size
			, uint32_t offset
			, BufferBase const & buffer )const;
		void doCopyFromStagingBuffer( CommandBuffer const & commandBuffer
			, uint32_t size
			, uint32_t offset
			, VertexBufferBase const & buffer
			, PipelineStageFlags const & flags )const;
		void doCopyFromStagingBuffer( CommandBuffer const & commandBuffer
			, uint32_t size
			, uint32_t offset
			, UniformBufferBase const & buffer
			, PipelineStageFlags const & flags )const;

	protected:
		Device const & m_device;
		AccessFlags m_currentAccessMask{ AccessFlag::eMemoryWrite };
		BufferBasePtr m_buffer;
	};
}

#include "StagingBuffer.inl"

#endif
