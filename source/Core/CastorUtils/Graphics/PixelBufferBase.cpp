#include "CastorUtils/Graphics/PixelBuffer.hpp"

#include "CastorUtils/Miscellaneous/BitSize.hpp"

#include <ashes/common/Format.hpp>

namespace castor
{
	namespace details
	{
		VkDeviceSize getDataAt( VkFormat format
			, uint32_t x
			, uint32_t y
			, uint32_t index
			, uint32_t level
			, uint32_t levels
			, PxBufferBase const & buffer )
		{
			return VkDeviceSize( ( index * ashes::getLevelsSize( VkExtent2D{ x, y }, format, 0u, levels ) )
				+ ashes::getLevelsSize( VkExtent2D{ x, y }, format, 0u, level ) )
				+ ( size_t( ( y * buffer.getWidth() + x ) * ashes::getMinimalSize( format ) ) );
		}
	}

	PxBufferBase::PxBufferBase( Size const & size
		, PixelFormat format
		, uint32_t layers
		, uint32_t levels
		, uint8_t const * buffer
		, PixelFormat bufferFormat )
		: m_format{ format == PixelFormat::eUNDEFINED ? bufferFormat : format }
		, m_size{ size }
		, m_layers{ layers }
		, m_levels{ levels }
		, m_buffer{ 0 }
	{
		initialise( buffer, bufferFormat );
	}

	PxBufferBase::PxBufferBase( PxBufferBase const & rhs )
		: m_format{ rhs.m_format }
		, m_size{ rhs.m_size }
		, m_layers{ rhs.m_layers }
		, m_levels{ rhs.m_levels }
		, m_buffer{ 0 }
	{
		initialise( rhs.getConstPtr(), rhs.getFormat() );
	}

	PxBufferBase::~PxBufferBase()
	{
	}

	PxBufferBase & PxBufferBase::operator=( PxBufferBase const & rhs )
	{
		clear();
		m_size = rhs.m_size;
		m_format = rhs.m_format;
		m_layers = rhs.m_layers;
		m_levels = rhs.m_levels;
		initialise( rhs.m_buffer.data(), rhs.m_format );
		return * this;
	}

	void PxBufferBase::clear()
	{
		m_buffer.clear();
	}

	void PxBufferBase::initialise( uint8_t const * buffer
		, PixelFormat bufferFormat )
	{
		auto extent = VkExtent3D{ m_size.getWidth(), m_size.getHeight(), m_layers };
		auto newSize = ashes::getLevelsSize( extent
			, VkFormat( getFormat() )
			, 0u
			, 1u );
		m_buffer.resize( newSize );

		if ( !buffer )
		{
			memset( m_buffer.data(), 0, newSize );
		}
		else
		{
			if ( bufferFormat == m_format )
			{
				memcpy( m_buffer.data(), buffer, newSize );
			}
			else
			{
				PF::convertBuffer( bufferFormat
					, buffer
					, getCount() * PF::getBytesPerPixel( bufferFormat )
					, getFormat()
					, m_buffer.data()
					, uint32_t( getSize() ) );
			}
		}
	}

	void PxBufferBase::initialise( Size const & size )
	{
		m_size = size;
		initialise( nullptr, PixelFormat::eR8G8B8A8_UNORM );
	}

	void PxBufferBase::swap( PxBufferBase & pixelBuffer )
	{
		std::swap( m_size, pixelBuffer.m_size );
		std::swap( m_format, pixelBuffer.m_format );
		std::swap( m_buffer, pixelBuffer.m_buffer );
	}

	void PxBufferBase::assign( std::vector< uint8_t > const & buffer
		, PixelFormat pixelFormat )
	{
		uint8_t newSize = PF::getBytesPerPixel( m_format );
		uint32_t dstMax = getCount();
		uint32_t srcMax = uint32_t( buffer.size() / newSize );

		if ( buffer.size() > 0 && srcMax == dstMax )
		{
			PF::convertBuffer( m_format
				, buffer.data()
				, uint32_t( buffer.size() )
				, getFormat()
				, m_buffer.data()
				, uint32_t( getSize() ) );
		}
	}

	uint32_t getMipLevels( VkExtent3D const & extent )
	{
		auto min = std::min( extent.width, extent.height );
		return uint32_t( castor::getBitSize( min ) );
	}

	uint32_t getMinMipLevels( uint32_t mipLevels
		, VkExtent3D const & extent )
	{
		return std::min( getMipLevels( extent ), mipLevels );
	}

	void PxBufferBase::update( uint32_t layers
		, uint32_t levels )
	{
		auto extent = VkExtent3D{ m_size.getWidth(), m_size.getHeight(), 1u };
		levels = getMinMipLevels( levels, extent );

		if ( layers != m_layers
			|| levels != m_levels )
		{
			auto buffer = m_buffer;
			auto srcLayers = m_layers;
			auto srcLevels = m_levels;
			m_layers = layers;
			m_levels = levels;

			auto newSize = m_layers * ashes::getLevelsSize( extent
				, VkFormat( getFormat() )
				, 0u
				, m_levels );
			m_buffer.resize( newSize );

			auto srcBuffer = buffer.data();
			auto srcLayerSize = ashes::getLevelsSize( extent
				, VkFormat( getFormat() )
				, 0u
				, srcLevels );
			auto dstBuffer = m_buffer.data();
			auto dstLayerSize = ashes::getLevelsSize( extent
				, VkFormat( getFormat() )
				, 0u
				, m_levels );

			for ( uint32_t layer = 0u; layer < std::min( srcLevels, m_layers ); ++layer )
			{
				memcpy( dstBuffer, srcBuffer, std::min( srcLayerSize, dstLayerSize ) );
				srcBuffer += srcLayerSize;
				dstBuffer += dstLayerSize;
			}
		}
	}

	void PxBufferBase::flip()
	{
		m_flipped = !m_flipped;
	}

	PxBufferBase::PixelData PxBufferBase::getAt( uint32_t x
		, uint32_t y
		, uint32_t index
		, uint32_t level )
	{
		CU_Require( x < getWidth() && y < getHeight() );
		return m_buffer.begin()
			+ details::getDataAt( VkFormat( m_format ), x, y, index, level, m_levels, *this );
	}

	PxBufferBase::ConstPixelData PxBufferBase::getAt( uint32_t x
		, uint32_t y
		, uint32_t index
		, uint32_t level )const
	{
		CU_Require( x < getWidth() && y < getHeight() );
		return m_buffer.begin()
			+ details::getDataAt( VkFormat( m_format ), x, y, index, level, m_levels, *this );
	}


	PxBufferBaseSPtr PxBufferBase::create( Size const & size
		, uint32_t layers
		, uint32_t levels
		, PixelFormat wantedFormat
		, uint8_t const * buffer
		, PixelFormat bufferFormat )
	{
		return std::make_shared< PxBufferBase >( size
			, wantedFormat
			, layers
			, levels
			, buffer
			, bufferFormat );
	}
}
