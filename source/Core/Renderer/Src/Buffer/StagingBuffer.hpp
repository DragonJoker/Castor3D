/*
This file belongs to RendererLib.
See LICENSE file in root folder.
*/
#ifndef ___Renderer_StagingBuffer_HPP___
#define ___Renderer_StagingBuffer_HPP___
#pragma once

#include "Buffer/Buffer.hpp"
#include "Buffer/VertexBuffer.hpp"
#include "Buffer/UniformBuffer.hpp"

namespace renderer
{
	/**
	*\~english
	*\brief
	*	Class grouping the functions to transfer data from/to VRAM.
	*\~french
	*\brief
	*	Classe regroupant les fonctions nécessaires au transfert de données depuis/vers la VRAM.
	*/
	class StagingBuffer
	{
	public:
		/**
		*\~english
		*\brief
		*	Constructor.
		*\param[in] device
		*	The logical device.
		*\param[in] target
		*	The buffer targets.
		*\param[in] size
		*	The buffer size.
		*\~french
		*\brief
		*	Constructeur.
		*\param[in] device
		*	Le LogicalDevice parent.
		*\param[in] target
		*	Les cibles du tampon.
		*\param[in] size
		*	La taille du tampon.
		*/
		StagingBuffer( Device const & device
			, BufferTargets target
			, uint32_t size = 10000000u );
		/**
		*\name
		*	Upload.
		**/
		/**@{*/
		/**
		*\name
		*	Texture.
		**/
		/**@{*/
		void uploadTextureData( CommandBuffer const & commandBuffer
			, ImageSubresourceLayers const & subresourceLayers
			, Offset3D const & offset
			, Extent3D const & extent
			, uint8_t const * const data
			, uint32_t size
			, TextureView const & texture )const;
		inline void uploadTextureData( CommandBuffer const & commandBuffer
			, ImageSubresourceLayers const & subresourceLayers
			, Offset3D const & offset
			, Extent3D const & extent
			, ByteArray const & data
			, TextureView const & texture )const;
		void uploadTextureData( CommandBuffer const & commandBuffer
			, uint8_t const * const data
			, uint32_t size
			, TextureView const & texture )const;
		inline void uploadTextureData( CommandBuffer const & commandBuffer
			, ByteArray const & data
			, TextureView const & texture )const;
		/**@}*/
		/**
		*\name
		*	Buffer.
		**/
		/**@{*/
		template< typename T >
		inline void uploadBufferData( CommandBuffer const & commandBuffer
			, std::vector< T > const & data
			, Buffer< T > const & buffer )const;
		template< typename T >
		inline void uploadBufferData( CommandBuffer const & commandBuffer
			, ByteArray const & data
			, Buffer< T > const & buffer )const;
		template< typename T >
		inline void uploadBufferData( CommandBuffer const & commandBuffer
			, uint8_t const * const data
			, uint32_t size
			, Buffer< T > const & buffer )const;
		template< typename T >
		inline void uploadBufferData( CommandBuffer const & commandBuffer
			, std::vector< T > const & data
			, uint32_t offset
			, Buffer< T > const & buffer )const;
		template< typename T >
		inline void uploadBufferData( CommandBuffer const & commandBuffer
			, ByteArray const & data
			, uint32_t offset
			, Buffer< T > const & buffer )const;
		template< typename T >
		void uploadBufferData( CommandBuffer const & commandBuffer
			, uint8_t const * const data
			, uint32_t size
			, uint32_t offset
			, Buffer< T > const & buffer )const;
		/**@}*/
		/**
		*\name
		*	Vertex buffer.
		**/
		/**@{*/
		template< typename T >
		inline void uploadVertexData( CommandBuffer const & commandBuffer
			, std::vector< T > const & data
			, VertexBuffer< T > const & buffer
			, PipelineStageFlags const & flags )const;
		template< typename T >
		inline void uploadVertexData( CommandBuffer const & commandBuffer
			, ByteArray const & data
			, VertexBuffer< T > const & buffer
			, PipelineStageFlags const & flags )const;
		template< typename T >
		inline void uploadVertexData( CommandBuffer const & commandBuffer
			, uint8_t const * const data
			, uint32_t size
			, VertexBuffer< T > const & buffer
			, PipelineStageFlags const & flags )const;
		template< typename T >
		inline void uploadVertexData( CommandBuffer const & commandBuffer
			, std::vector< T > const & data
			, uint32_t offset
			, VertexBuffer< T > const & buffer
			, PipelineStageFlags const & flags )const;
		template< typename T >
		inline void uploadVertexData( CommandBuffer const & commandBuffer
			, ByteArray const & data
			, uint32_t offset
			, VertexBuffer< T > const & buffer
			, PipelineStageFlags const & flags )const;
		template< typename T >
		inline void uploadVertexData( CommandBuffer const & commandBuffer
			, uint8_t const * const data
			, uint32_t size
			, uint32_t offset
			, VertexBuffer< T > const & buffer
			, PipelineStageFlags const & flags )const;
		/**@}*/
		/**
		*\name
		*	Uniform buffer.
		**/
		/**@{*/
		template< typename T >
		inline void uploadUniformData( CommandBuffer const & commandBuffer
			, std::vector< T > const & data
			, UniformBuffer< T > const & buffer
			, PipelineStageFlags const & flags )const;
		template< typename T >
		inline void uploadUniformData( CommandBuffer const & commandBuffer
			, T const * const data
			, uint32_t count
			, UniformBuffer< T > const & buffer
			, PipelineStageFlags const & flags )const;
		template< typename T >
		inline void uploadUniformData( CommandBuffer const & commandBuffer
			, std::vector< T > const & data
			, uint32_t offset
			, UniformBuffer< T > const & buffer
			, PipelineStageFlags const & flags )const;
		template< typename T >
		inline void uploadUniformData( CommandBuffer const & commandBuffer
			, T const * const data
			, uint32_t count
			, uint32_t offset
			, UniformBuffer< T > const & buffer
			, PipelineStageFlags const & flags )const;
		/**@}*/
		/**
		*\name
		*	Download.
		**/
		/**@{*/
		/**
		*\name
		*	Texture.
		**/
		/**@{*/
		void downloadTextureData( CommandBuffer const & commandBuffer
			, ImageSubresourceLayers const & subresourceLayers
			, Offset3D const & offset
			, Extent3D const & extent
			, uint8_t * data
			, uint32_t size
			, TextureView const & texture )const;
		inline void downloadTextureData( CommandBuffer const & commandBuffer
			, ImageSubresourceLayers const & subresourceLayers
			, Offset3D const & offset
			, Extent3D const & extent
			, ByteArray & data
			, TextureView const & texture )const;
		void downloadTextureData( CommandBuffer const & commandBuffer
			, uint8_t * data
			, uint32_t size
			, TextureView const & texture )const;
		inline void downloadTextureData( CommandBuffer const & commandBuffer
			, ByteArray & data
			, TextureView const & texture )const;
		/**@}*/
		/**
		*\name
		*	Buffer.
		**/
		/**@{*/
		template< typename T >
		inline void downloadBufferData( CommandBuffer const & commandBuffer
			, std::vector< T > & data
			, Buffer< T > const & buffer )const;
		template< typename T >
		inline void downloadBufferData( CommandBuffer const & commandBuffer
			, ByteArray & data
			, Buffer< T > const & buffer )const;
		template< typename T >
		inline void downloadBufferData( CommandBuffer const & commandBuffer
			, uint8_t * data
			, uint32_t size
			, Buffer< T > const & buffer )const;
		template< typename T >
		inline void downloadBufferData( CommandBuffer const & commandBuffer
			, std::vector< T > & data
			, uint32_t offset
			, Buffer< T > const & buffer )const;
		template< typename T >
		inline void downloadBufferData( CommandBuffer const & commandBuffer
			, ByteArray & data
			, uint32_t offset
			, Buffer< T > const & buffer )const;
		template< typename T >
		void downloadBufferData( CommandBuffer const & commandBuffer
			, uint8_t * data
			, uint32_t size
			, uint32_t offset
			, Buffer< T > const & buffer )const;
		/**@}*/
		/**
		*\name
		*	Vertex buffer.
		**/
		/**@{*/
		template< typename T >
		inline void downloadVertexData( CommandBuffer const & commandBuffer
			, std::vector< T > & data
			, VertexBuffer< T > const & buffer
			, PipelineStageFlags const & flags )const;
		template< typename T >
		inline void downloadVertexData( CommandBuffer const & commandBuffer
			, ByteArray & data
			, VertexBuffer< T > const & buffer
			, PipelineStageFlags const & flags )const;
		template< typename T >
		inline void downloadVertexData( CommandBuffer const & commandBuffer
			, uint8_t * data
			, uint32_t size
			, VertexBuffer< T > const & buffer
			, PipelineStageFlags const & flags )const;
		template< typename T >
		inline void downloadVertexData( CommandBuffer const & commandBuffer
			, std::vector< T > & data
			, uint32_t offset
			, VertexBuffer< T > const & buffer
			, PipelineStageFlags const & flags )const;
		template< typename T >
		inline void downloadVertexData( CommandBuffer const & commandBuffer
			, ByteArray & data
			, uint32_t offset
			, VertexBuffer< T > const & buffer
			, PipelineStageFlags const & flags )const;
		template< typename T >
		inline void downloadVertexData( CommandBuffer const & commandBuffer
			, uint8_t * data
			, uint32_t size
			, uint32_t offset
			, VertexBuffer< T > const & buffer
			, PipelineStageFlags const & flags )const;
		/**@}*/
		/**
		*\name
		*	Uniform buffer.
		**/
		/**@{*/
		template< typename T >
		inline void downloadUniformData( CommandBuffer const & commandBuffer
			, std::vector< T > & data
			, UniformBuffer< T > const & buffer
			, PipelineStageFlags const & flags )const;
		template< typename T >
		inline void downloadUniformData( CommandBuffer const & commandBuffer
			, T * data
			, uint32_t count
			, UniformBuffer< T > const & buffer
			, PipelineStageFlags const & flags )const;
		template< typename T >
		inline void downloadUniformData( CommandBuffer const & commandBuffer
			, std::vector< T > & data
			, uint32_t offset
			, UniformBuffer< T > const & buffer
			, PipelineStageFlags const & flags )const;
		template< typename T >
		inline void downloadUniformData( CommandBuffer const & commandBuffer
			, T * data
			, uint32_t count
			, uint32_t offset
			, UniformBuffer< T > const & buffer
			, PipelineStageFlags const & flags )const;
		/**@}*/
		/**@}*/
		/**
		*\return
		*	Le tampon GPU.
		*/
		inline BufferBase const & getBuffer()const
		{
			return *m_buffer;
		}

	private:
		/**
		*\name
		*	Upload.
		**/
		/**@{*/
		template< typename T >
		inline void doCopyUniformDataToStagingBuffer( T const * const data
			, uint32_t count
			, uint32_t elemAlignedSize )const;
		void doCopyToStagingBuffer( uint8_t const * const data
			, uint32_t size )const;
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
		/**@{*/
		/**
		*\name
		*	Download.
		**/
		/**@{*/
		template< typename T >
		inline void doCopyUniformDataFromStagingBuffer( T * data
			, uint32_t count
			, uint32_t elemAlignedSize )const;
		void doCopyFromStagingBuffer( uint8_t * data
			, uint32_t size )const;
		void doCopyToStagingBuffer( CommandBuffer const & commandBuffer
			, uint32_t size
			, uint32_t offset
			, BufferBase const & buffer )const;
		void doCopyToStagingBuffer( CommandBuffer const & commandBuffer
			, uint32_t size
			, uint32_t offset
			, VertexBufferBase const & buffer
			, PipelineStageFlags const & flags )const;
		void doCopyToStagingBuffer( CommandBuffer const & commandBuffer
			, uint32_t size
			, uint32_t offset
			, UniformBufferBase const & buffer
			, PipelineStageFlags const & flags )const;
		/**@}*/

	protected:
		Device const & m_device;
		AccessFlags m_currentAccessMask{ AccessFlag::eMemoryWrite };
		BufferBasePtr m_buffer;
	};
}

#include "StagingBuffer.inl"

#endif
