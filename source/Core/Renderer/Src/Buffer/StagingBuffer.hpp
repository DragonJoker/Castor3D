/*
This file belongs to Renderer.
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
		*\name
		*	Upload.
		**/
		/**@{*/
		/**
		*\~french
		*\brief
		*	Copie les données d'une image dans une texture, en passant par le tampon de transfert.
		*\param[in] data
		*	Les données à copier.
		*\param[out] texture
		*	La texture de destination.
		*/
		void uploadTextureData( CommandBuffer const & commandBuffer
			, ImageSubresourceLayers const & subresourceLayers
			, IVec3 const & offset
			, UIVec3 const & extent
			, uint8_t const * const data
			, uint32_t size
			, TextureView const & texture )const;
		/**
		*\~french
		*\brief
		*	Copie les données d'une image dans une texture, en passant par le tampon de transfert.
		*\param[in] data
		*	Les données à copier.
		*\param[out] texture
		*	La texture de destination.
		*/
		inline void uploadTextureData( CommandBuffer const & commandBuffer
			, ImageSubresourceLayers const & subresourceLayers
			, IVec3 const & offset
			, UIVec3 const & extent
			, ByteArray const & data
			, TextureView const & texture )const;
		/**
		*\~french
		*\brief
		*	Copie les données d'une image dans une texture, en passant par le tampon de transfert.
		*\param[in] data
		*	Les données à copier.
		*\param[out] texture
		*	La texture de destination.
		*/
		void uploadTextureData( CommandBuffer const & commandBuffer
			, uint8_t const * const data
			, uint32_t size
			, TextureView const & texture )const;
		/**
		*\~french
		*\brief
		*	Copie les données d'une image dans une texture, en passant par le tampon de transfert.
		*\param[in] data
		*	Les données à copier.
		*\param[out] texture
		*	La texture de destination.
		*/
		inline void uploadTextureData( CommandBuffer const & commandBuffer
			, ByteArray const & data
			, TextureView const & texture )const;
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
		inline void uploadBufferData( CommandBuffer const & commandBuffer
			, std::vector< T > const & data
			, Buffer< T > const & buffer )const;
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
		inline void uploadBufferData( CommandBuffer const & commandBuffer
			, ByteArray const & data
			, Buffer< T > const & buffer )const;
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
		inline void uploadBufferData( CommandBuffer const & commandBuffer
			, uint8_t const * const data
			, uint32_t size
			, Buffer< T > const & buffer )const;
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
		inline void uploadBufferData( CommandBuffer const & commandBuffer
			, std::vector< T > const & data
			, uint32_t offset
			, Buffer< T > const & buffer )const;
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
		inline void uploadBufferData( CommandBuffer const & commandBuffer
			, ByteArray const & data
			, uint32_t offset
			, Buffer< T > const & buffer )const;
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
		void uploadBufferData( CommandBuffer const & commandBuffer
			, uint8_t const * const data
			, uint32_t size
			, uint32_t offset
			, Buffer< T > const & buffer )const;
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
		inline void uploadVertexData( CommandBuffer const & commandBuffer
			, std::vector< T > const & data
			, VertexBuffer< T > const & buffer
			, PipelineStageFlags const & flags )const;
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
		inline void uploadVertexData( CommandBuffer const & commandBuffer
			, ByteArray const & data
			, VertexBuffer< T > const & buffer
			, PipelineStageFlags const & flags )const;
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
		inline void uploadVertexData( CommandBuffer const & commandBuffer
			, uint8_t const * const data
			, uint32_t size
			, VertexBuffer< T > const & buffer
			, PipelineStageFlags const & flags )const;
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
		inline void uploadVertexData( CommandBuffer const & commandBuffer
			, std::vector< T > const & data
			, uint32_t offset
			, VertexBuffer< T > const & buffer
			, PipelineStageFlags const & flags )const;
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
		inline void uploadVertexData( CommandBuffer const & commandBuffer
			, ByteArray const & data
			, uint32_t offset
			, VertexBuffer< T > const & buffer
			, PipelineStageFlags const & flags )const;
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
		inline void uploadVertexData( CommandBuffer const & commandBuffer
			, uint8_t const * const data
			, uint32_t size
			, uint32_t offset
			, VertexBuffer< T > const & buffer
			, PipelineStageFlags const & flags )const;
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
		inline void uploadUniformData( CommandBuffer const & commandBuffer
			, std::vector< T > const & data
			, UniformBuffer< T > const & buffer
			, PipelineStageFlags const & flags )const;
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
		inline void uploadUniformData( CommandBuffer const & commandBuffer
			, T const * const data
			, uint32_t count
			, UniformBuffer< T > const & buffer
			, PipelineStageFlags const & flags )const;
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
		inline void uploadUniformData( CommandBuffer const & commandBuffer
			, std::vector< T > const & data
			, uint32_t offset
			, UniformBuffer< T > const & buffer
			, PipelineStageFlags const & flags )const;
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
		inline void uploadUniformData( CommandBuffer const & commandBuffer
			, T const * const data
			, uint32_t count
			, uint32_t offset
			, UniformBuffer< T > const & buffer
			, PipelineStageFlags const & flags )const;
		/**@}*/
		/**
		*\name
		*	Upload.
		**/
		/**@{*/
		/**
		*\~french
		*\brief
		*	Copie les données d'une image dans une texture, en passant par le tampon de transfert.
		*\param[in] data
		*	Les données à copier.
		*\param[out] texture
		*	La texture de destination.
		*/
		void downloadTextureData( CommandBuffer const & commandBuffer
			, ImageSubresourceLayers const & subresourceLayers
			, IVec3 const & offset
			, UIVec3 const & extent
			, uint8_t * data
			, uint32_t size
			, TextureView const & texture )const;
		/**
		*\~french
		*\brief
		*	Copie les données d'une image dans une texture, en passant par le tampon de transfert.
		*\param[in] data
		*	Les données à copier.
		*\param[out] texture
		*	La texture de destination.
		*/
		inline void downloadTextureData( CommandBuffer const & commandBuffer
			, ImageSubresourceLayers const & subresourceLayers
			, IVec3 const & offset
			, UIVec3 const & extent
			, ByteArray & data
			, TextureView const & texture )const;
		/**
		*\~french
		*\brief
		*	Copie les données d'une image dans une texture, en passant par le tampon de transfert.
		*\param[in] data
		*	Les données à copier.
		*\param[out] texture
		*	La texture de destination.
		*/
		void downloadTextureData( CommandBuffer const & commandBuffer
			, uint8_t * data
			, uint32_t size
			, TextureView const & texture )const;
		/**
		*\~french
		*\brief
		*	Copie les données d'une image dans une texture, en passant par le tampon de transfert.
		*\param[in] data
		*	Les données à copier.
		*\param[out] texture
		*	La texture de destination.
		*/
		inline void downloadTextureData( CommandBuffer const & commandBuffer
			, ByteArray & data
			, TextureView const & texture )const;
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
		inline void downloadBufferData( CommandBuffer const & commandBuffer
			, std::vector< T > & data
			, Buffer< T > const & buffer )const;
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
		inline void downloadBufferData( CommandBuffer const & commandBuffer
			, ByteArray & data
			, Buffer< T > const & buffer )const;
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
		inline void downloadBufferData( CommandBuffer const & commandBuffer
			, uint8_t * data
			, uint32_t size
			, Buffer< T > const & buffer )const;
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
		inline void downloadBufferData( CommandBuffer const & commandBuffer
			, std::vector< T > & data
			, uint32_t offset
			, Buffer< T > const & buffer )const;
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
		inline void downloadBufferData( CommandBuffer const & commandBuffer
			, ByteArray & data
			, uint32_t offset
			, Buffer< T > const & buffer )const;
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
		void downloadBufferData( CommandBuffer const & commandBuffer
			, uint8_t * data
			, uint32_t size
			, uint32_t offset
			, Buffer< T > const & buffer )const;
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
		inline void downloadVertexData( CommandBuffer const & commandBuffer
			, std::vector< T > & data
			, VertexBuffer< T > const & buffer
			, PipelineStageFlags const & flags )const;
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
		inline void downloadVertexData( CommandBuffer const & commandBuffer
			, ByteArray & data
			, VertexBuffer< T > const & buffer
			, PipelineStageFlags const & flags )const;
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
		inline void downloadVertexData( CommandBuffer const & commandBuffer
			, uint8_t * data
			, uint32_t size
			, VertexBuffer< T > const & buffer
			, PipelineStageFlags const & flags )const;
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
		inline void downloadVertexData( CommandBuffer const & commandBuffer
			, std::vector< T > & data
			, uint32_t offset
			, VertexBuffer< T > const & buffer
			, PipelineStageFlags const & flags )const;
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
		inline void downloadVertexData( CommandBuffer const & commandBuffer
			, ByteArray & data
			, uint32_t offset
			, VertexBuffer< T > const & buffer
			, PipelineStageFlags const & flags )const;
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
		inline void downloadVertexData( CommandBuffer const & commandBuffer
			, uint8_t * data
			, uint32_t size
			, uint32_t offset
			, VertexBuffer< T > const & buffer
			, PipelineStageFlags const & flags )const;
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
		inline void downloadUniformData( CommandBuffer const & commandBuffer
			, std::vector< T > & data
			, UniformBuffer< T > const & buffer
			, PipelineStageFlags const & flags )const;
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
		inline void downloadUniformData( CommandBuffer const & commandBuffer
			, T * data
			, uint32_t count
			, UniformBuffer< T > const & buffer
			, PipelineStageFlags const & flags )const;
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
		inline void downloadUniformData( CommandBuffer const & commandBuffer
			, std::vector< T > & data
			, uint32_t offset
			, UniformBuffer< T > const & buffer
			, PipelineStageFlags const & flags )const;
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
		inline void downloadUniformData( CommandBuffer const & commandBuffer
			, T * data
			, uint32_t count
			, uint32_t offset
			, UniformBuffer< T > const & buffer
			, PipelineStageFlags const & flags )const;
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
