/*
This file belongs to Renderer.
See LICENSE file in root folder.
*/
namespace renderer
{
	/**
	*name
	*	Upload.
	**/
	/**@{*/
	inline void StagingBuffer::uploadTextureData( CommandBuffer const & commandBuffer
		, ImageSubresourceLayers const & subresourceLayers
		, IVec3 const & offset
		, UIVec3 const & extent
		, ByteArray const & data
		, TextureView const & texture )const
	{
		uploadTextureData( commandBuffer
			, subresourceLayers
			, offset
			, extent
			, data.data()
			, uint32_t( data.size() )
			, texture );
	}

	inline void StagingBuffer::uploadTextureData( CommandBuffer const & commandBuffer
		, ByteArray const & data
		, TextureView const & texture )const
	{
		uploadTextureData( commandBuffer
			, data.data()
			, uint32_t( data.size() )
			, texture );
	}

	template< typename T >
	inline void StagingBuffer::uploadBufferData( CommandBuffer const & commandBuffer
		, std::vector< T > const & data
		, Buffer< T > const & buffer )const
	{
		uploadBufferData( commandBuffer
			, reinterpret_cast< uint8_t const * const >( data.data() )
			, uint32_t( data.size() * sizeof( T ) )
			, 0u
			, buffer );
	}

	template< typename T >
	inline void StagingBuffer::uploadBufferData( CommandBuffer const & commandBuffer
		, ByteArray const & data
		, Buffer< T > const & buffer )const
	{
		uploadBufferData( commandBuffer
			, data.data()
			, uint32_t( data.size() )
			, 0u
			, buffer );
	}

	template< typename T >
	inline void StagingBuffer::uploadBufferData( CommandBuffer const & commandBuffer
		, uint8_t const * const data
		, uint32_t size
		, Buffer< T > const & buffer )const
	{
		uploadBufferData( commandBuffer
			, data
			, size
			, 0u
			, buffer );
	}

	template< typename T >
	inline void StagingBuffer::uploadBufferData( CommandBuffer const & commandBuffer
		, std::vector< T > const & data
		, uint32_t offset
		, Buffer< T > const & buffer )const
	{
		uploadBufferData( commandBuffer
			, reinterpret_cast< uint8_t const * const >( data.data() )
			, uint32_t( data.size() * sizeof( T ) )
			, uint32_t( offset * sizeof( T ) )
			, buffer );
	}

	template< typename T >
	inline void StagingBuffer::uploadBufferData( CommandBuffer const & commandBuffer
		, ByteArray const & data
		, uint32_t offset
		, Buffer< T > const & buffer )const
	{
		uploadBufferData( commandBuffer
			, data.data()
			, uint32_t( data.size() )
			, offset
			, buffer );
	}

	template< typename T >
	void StagingBuffer::uploadBufferData( CommandBuffer const & commandBuffer
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

	template< typename T >
	inline void StagingBuffer::uploadVertexData( CommandBuffer const & commandBuffer
		, std::vector< T > const & data
		, VertexBuffer< T > const & buffer
		, PipelineStageFlags const & flags )const
	{
		uploadVertexData( commandBuffer
			, reinterpret_cast< uint8_t const * const >( data.data() )
			, uint32_t( data.size() * sizeof( T ) )
			, 0u
			, buffer
			, flags );
	}

	template< typename T >
	inline void StagingBuffer::uploadVertexData( CommandBuffer const & commandBuffer
		, ByteArray const & data
		, VertexBuffer< T > const & buffer
		, PipelineStageFlags const & flags )const
	{
		uploadVertexData( commandBuffer
			, data.data()
			, uint32_t( data.size() )
			, 0u
			, buffer
			, flags );
	}

	template< typename T >
	inline void StagingBuffer::uploadVertexData( CommandBuffer const & commandBuffer
		, uint8_t const * const data
		, uint32_t size
		, VertexBuffer< T > const & buffer
		, PipelineStageFlags const & flags )const
	{
		uploadVertexData( commandBuffer
			, data
			, size
			, 0u
			, buffer
			, flags );
	}

	template< typename T >
	inline void StagingBuffer::uploadVertexData( CommandBuffer const & commandBuffer
		, std::vector< T > const & data
		, uint32_t offset
		, VertexBuffer< T > const & buffer
		, PipelineStageFlags const & flags )const
	{
		uploadVertexData( commandBuffer
			, reinterpret_cast< uint8_t const * const >( data.data() )
			, uint32_t( data.size() * sizeof( T ) )
			, uint32_t( offset * sizeof( T ) )
			, buffer
			, flags );
	}

	template< typename T >
	inline void StagingBuffer::uploadVertexData( CommandBuffer const & commandBuffer
		, ByteArray const & data
		, uint32_t offset
		, VertexBuffer< T > const & buffer
		, PipelineStageFlags const & flags )const
	{
		uploadVertexData( commandBuffer
			, data.data()
			, uint32_t( data.size() )
			, offset
			, buffer
			, flags );
	}

	template< typename T >
	inline void StagingBuffer::uploadVertexData( CommandBuffer const & commandBuffer
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

	template< typename T >
	inline void StagingBuffer::uploadUniformData( CommandBuffer const & commandBuffer
		, ByteArray const & data
		, UniformBuffer< T > const & buffer
		, PipelineStageFlags const & flags )const
	{
		uploadUniformData( commandBuffer
			, data.data()
			, uint32_t( data.size() )
			, 0u
			, buffer
			, flags );
	}

	template< typename T >
	inline void StagingBuffer::uploadUniformData( CommandBuffer const & commandBuffer
		, uint8_t const * const data
		, uint32_t size
		, UniformBuffer< T > const & buffer
		, PipelineStageFlags const & flags )const
	{
		uploadUniformData( commandBuffer
			, data
			, size
			, 0u
			, buffer
			, flags );
	}

	template< typename T >
	inline void StagingBuffer::uploadUniformData( CommandBuffer const & commandBuffer
		, ByteArray const & data
		, uint32_t offset
		, UniformBuffer< T > const & buffer
		, PipelineStageFlags const & flags )const
	{
		uploadUniformData( commandBuffer
			, data.data()
			, uint32_t( data.size() )
			, offset
			, buffer
			, flags );
	}

	template< typename T >
	inline void StagingBuffer::uploadUniformData( CommandBuffer const & commandBuffer
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

	template< typename T >
	inline void StagingBuffer::uploadUniformData( CommandBuffer const & commandBuffer
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

	template< typename T >
	inline void StagingBuffer::uploadUniformData( CommandBuffer const & commandBuffer
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

	template< typename T >
	inline void StagingBuffer::uploadUniformData( CommandBuffer const & commandBuffer
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

	template< typename T >
	inline void StagingBuffer::uploadUniformData( CommandBuffer const & commandBuffer
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
	/**@}*/
	/**
	*name
	*	Upload.
	**/
	/**@{*/
	inline void StagingBuffer::downloadTextureData( CommandBuffer const & commandBuffer
		, ImageSubresourceLayers const & subresourceLayers
		, IVec3 const & offset
		, UIVec3 const & extent
		, ByteArray & data
		, TextureView const & texture )const
	{
		downloadTextureData( commandBuffer
			, subresourceLayers
			, offset
			, extent
			, data.data()
			, uint32_t( data.size() )
			, texture );
	}

	inline void StagingBuffer::downloadTextureData( CommandBuffer const & commandBuffer
		, ByteArray & data
		, TextureView const & texture )const
	{
		downloadTextureData( commandBuffer
			, data.data()
			, uint32_t( data.size() )
			, texture );
	}

	template< typename T >
	inline void StagingBuffer::downloadBufferData( CommandBuffer const & commandBuffer
		, std::vector< T > const & data
		, Buffer< T > const & buffer )const
	{
		downloadBufferData( commandBuffer
			, reinterpret_cast< uint8_t const * const >( data.data() )
			, uint32_t( data.size() * sizeof( T ) )
			, 0u
			, buffer );
	}

	template< typename T >
	inline void StagingBuffer::downloadBufferData( CommandBuffer const & commandBuffer
		, ByteArray const & data
		, Buffer< T > const & buffer )const
	{
		downloadBufferData( commandBuffer
			, data.data()
			, uint32_t( data.size() )
			, 0u
			, buffer );
	}

	template< typename T >
	inline void StagingBuffer::downloadBufferData( CommandBuffer const & commandBuffer
		, uint8_t const * const data
		, uint32_t size
		, Buffer< T > const & buffer )const
	{
		downloadBufferData( commandBuffer
			, data
			, size
			, 0u
			, buffer );
	}

	template< typename T >
	inline void StagingBuffer::downloadBufferData( CommandBuffer const & commandBuffer
		, std::vector< T > const & data
		, uint32_t offset
		, Buffer< T > const & buffer )const
	{
		downloadBufferData( commandBuffer
			, reinterpret_cast< uint8_t const * const >( data.data() )
			, uint32_t( data.size() * sizeof( T ) )
			, uint32_t( offset * sizeof( T ) )
			, buffer );
	}

	template< typename T >
	inline void StagingBuffer::downloadBufferData( CommandBuffer const & commandBuffer
		, ByteArray const & data
		, uint32_t offset
		, Buffer< T > const & buffer )const
	{
		downloadBufferData( commandBuffer
			, data.data()
			, uint32_t( data.size() )
			, offset
			, buffer );
	}

	template< typename T >
	void StagingBuffer::downloadBufferData( CommandBuffer const & commandBuffer
		, uint8_t const * const data
		, uint32_t size
		, uint32_t offset
		, Buffer< T > const & buffer )const
	{
		doCopyToStagingBuffer( commandBuffer
			, size
			, offset
			, buffer.getBuffer() );
		doCopyFromStagingBuffer( data
			, size );
	}

	template< typename T >
	inline void StagingBuffer::downloadVertexData( CommandBuffer const & commandBuffer
		, std::vector< T > const & data
		, VertexBuffer< T > const & buffer
		, PipelineStageFlags const & flags )const
	{
		downloadVertexData( commandBuffer
			, reinterpret_cast< uint8_t const * const >( data.data() )
			, uint32_t( data.size() * sizeof( T ) )
			, 0u
			, buffer
			, flags );
	}

	template< typename T >
	inline void StagingBuffer::downloadVertexData( CommandBuffer const & commandBuffer
		, ByteArray const & data
		, VertexBuffer< T > const & buffer
		, PipelineStageFlags const & flags )const
	{
		downloadVertexData( commandBuffer
			, data.data()
			, uint32_t( data.size() )
			, 0u
			, buffer
			, flags );
	}

	template< typename T >
	inline void StagingBuffer::downloadVertexData( CommandBuffer const & commandBuffer
		, uint8_t const * const data
		, uint32_t size
		, VertexBuffer< T > const & buffer
		, PipelineStageFlags const & flags )const
	{
		downloadVertexData( commandBuffer
			, data
			, size
			, 0u
			, buffer
			, flags );
	}

	template< typename T >
	inline void StagingBuffer::downloadVertexData( CommandBuffer const & commandBuffer
		, std::vector< T > const & data
		, uint32_t offset
		, VertexBuffer< T > const & buffer
		, PipelineStageFlags const & flags )const
	{
		downloadVertexData( commandBuffer
			, reinterpret_cast< uint8_t const * const >( data.data() )
			, uint32_t( data.size() * sizeof( T ) )
			, uint32_t( offset * sizeof( T ) )
			, buffer
			, flags );
	}

	template< typename T >
	inline void StagingBuffer::downloadVertexData( CommandBuffer const & commandBuffer
		, ByteArray const & data
		, uint32_t offset
		, VertexBuffer< T > const & buffer
		, PipelineStageFlags const & flags )const
	{
		downloadVertexData( commandBuffer
			, data.data()
			, uint32_t( data.size() )
			, offset
			, buffer
			, flags );
	}

	template< typename T >
	inline void StagingBuffer::downloadVertexData( CommandBuffer const & commandBuffer
		, uint8_t const * const data
		, uint32_t size
		, uint32_t offset
		, VertexBuffer< T > const & buffer
		, PipelineStageFlags const & flags )const
	{
		doCopyToStagingBuffer( commandBuffer
			, size
			, offset
			, buffer
			, flags );
		doCopyFromStagingBuffer( data
			, size );
	}

	template< typename T >
	inline void StagingBuffer::downloadUniformData( CommandBuffer const & commandBuffer
		, ByteArray const & data
		, UniformBuffer< T > const & buffer
		, PipelineStageFlags const & flags )const
	{
		downloadUniformData( commandBuffer
			, data.data()
			, uint32_t( data.size() )
			, 0u
			, buffer
			, flags );
	}

	template< typename T >
	inline void StagingBuffer::downloadUniformData( CommandBuffer const & commandBuffer
		, uint8_t const * const data
		, uint32_t size
		, UniformBuffer< T > const & buffer
		, PipelineStageFlags const & flags )const
	{
		downloadUniformData( commandBuffer
			, data
			, size
			, 0u
			, buffer
			, flags );
	}

	template< typename T >
	inline void StagingBuffer::downloadUniformData( CommandBuffer const & commandBuffer
		, ByteArray const & data
		, uint32_t offset
		, UniformBuffer< T > const & buffer
		, PipelineStageFlags const & flags )const
	{
		downloadUniformData( commandBuffer
			, data.data()
			, uint32_t( data.size() )
			, offset
			, buffer
			, flags );
	}

	template< typename T >
	inline void StagingBuffer::downloadUniformData( CommandBuffer const & commandBuffer
		, std::vector< T > const & data
		, UniformBuffer< T > const & buffer
		, PipelineStageFlags const & flags )const
	{
		doCopyToStagingBuffer( commandBuffer
			, buffer.getOffset( uint32_t( data.size() ) )
			, 0u
			, buffer.getUbo()
			, flags );
		doCopyUniformDataFromStagingBuffer( data.data()
			, uint32_t( data.size() )
			, buffer.getOffset( 1u ) );
	}

	template< typename T >
	inline void StagingBuffer::downloadUniformData( CommandBuffer const & commandBuffer
		, T const * const data
		, uint32_t count
		, UniformBuffer< T > const & buffer
		, PipelineStageFlags const & flags )const
	{
		doCopyToStagingBuffer( commandBuffer
			, buffer.getOffset( uint32_t( data.size() ) )
			, 0u
			, buffer.getUbo()
			, flags );
		doCopyUniformDataFromStagingBuffer( data.data()
			, count
			, buffer.getOffset( 1u ) );
	}

	template< typename T >
	inline void StagingBuffer::downloadUniformData( CommandBuffer const & commandBuffer
		, std::vector< T > const & data
		, uint32_t offset
		, UniformBuffer< T > const & buffer
		, PipelineStageFlags const & flags )const
	{
		doCopyToStagingBuffer( commandBuffer
			, buffer.getOffset( uint32_t( data.size() ) )
			, buffer.getOffset( offset )
			, buffer.getUbo()
			, flags );
		doCopyUniformDataFromStagingBuffer( data.data()
			, uint32_t( data.size() )
			, buffer.getOffset( 1u ) );
	}

	template< typename T >
	inline void StagingBuffer::downloadUniformData( CommandBuffer const & commandBuffer
		, T const * const data
		, uint32_t count
		, uint32_t offset
		, UniformBuffer< T > const & buffer
		, PipelineStageFlags const & flags )const
	{
		doCopyToStagingBuffer( commandBuffer
			, buffer.getOffset( uint32_t( data.size() ) )
			, buffer.getOffset( offset )
			, buffer.getUbo()
			, flags );
		doCopyUniformDataFromStagingBuffer( data.data()
			, count
			, buffer.getOffset( 1u ) );
	}

	template< typename T >
	inline void StagingBuffer::downloadUniformData( CommandBuffer const & commandBuffer
		, uint8_t const * const data
		, uint32_t size
		, uint32_t offset
		, UniformBuffer< T > const & buffer
		, PipelineStageFlags const & flags )const
	{
		doCopyToStagingBuffer( commandBuffer
			, size
			, offset
			, buffer.getUbo()
			, flags );
		doCopyFromStagingBuffer( data
			, size );
	}
	/**@}*/
	/**
	*name
	*	Upload.
	**/
	/**@{*/
	template< typename T >
	inline void StagingBuffer::doCopyUniformDataToStagingBuffer( T const * const datas
		, uint32_t count
		, uint32_t offset )const
	{
		auto size = count * offset;
		ByteArray data( size_t( size ), uint8_t{} );
		auto buffer = data.data();

		for ( uint32_t i = 0; i < count; ++i )
		{
			std::memcpy( buffer, &datas[i], sizeof( T ) );
			buffer += offset;
		}

		doCopyToStagingBuffer( data.data(), size );
	}
	/**@}*/
	/**
	*name
	*	Download.
	**/
	/**@{*/
	template< typename T >
	inline void StagingBuffer::doCopyUniformDataFromStagingBuffer( T * datas
		, uint32_t count
		, uint32_t offset )const
	{
		auto size = count * offset;
		ByteArray data( size_t( size ), uint8_t{} );
		doCopyFromStagingBuffer( data.data(), size );
		auto buffer = data.data();

		for ( uint32_t i = 0; i < count; ++i )
		{
			std::memcpy( &datas[i], buffer, sizeof( T ) );
			buffer += offset;
		}
	}
	/**@}*/
}
