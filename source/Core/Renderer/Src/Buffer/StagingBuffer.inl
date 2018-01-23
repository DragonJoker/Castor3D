/*
This file belongs to Renderer.
See LICENSE file in root folder.
*/
namespace renderer
{
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
}
