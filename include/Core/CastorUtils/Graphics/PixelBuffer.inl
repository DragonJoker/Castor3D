namespace castor
{
	template< PixelFormat FT >
	PxBuffer< FT >::PxBuffer( Size const & size
		, uint32_t layers
		, uint32_t levels
		, uint8_t const * buffer
		, PixelFormat bufferFormat )
		: PxBufferBase{ size, PixelFormat( FT ), layers, levels, buffer, bufferFormat }
	{
	}

	template< PixelFormat FT >
	Pixel< FT > PxBuffer< FT >::at( uint32_t x, uint32_t y )const
	{
		CU_Require( x < getWidth() && y < getHeight() );
		return *( begin() + doConvert( x, y ) );
	}
}
