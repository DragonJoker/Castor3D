#include "CastorUtils/Graphics/ImageLayout.hpp"

#include "CastorUtils/Data/LoaderException.hpp"
#include "CastorUtils/Data/Path.hpp"
#include "CastorUtils/Graphics/ImageLoader.hpp"
#include "CastorUtils/Graphics/Rectangle.hpp"
#include "CastorUtils/Graphics/Size.hpp"
#include "CastorUtils/Log/Logger.hpp"

#include <ashes/common/Format.hpp>

namespace castor
{
	ImageLayout::SubresourceRange ImageLayout::range()const noexcept
	{
		auto off = offset();
		auto sze = size();
		return
		{
			off,
			( sze ? off + sze : off - 1u ),
		};
	}
	
	ImageLayout::SubresourceRange ImageLayout::slice( uint32_t index )const noexcept
	{
		auto offset = sliceOffset( index );
		auto size = sliceSize();
		return
		{
			offset,
			( size ? offset + size : offset - 1u ),
		};
	}

	ImageLayout::SubresourceRange ImageLayout::sliceMip( uint32_t index, uint32_t level )const noexcept
	{
		auto offset = sliceMipOffset( index, level );
		auto size = sliceMipSize( level );
		return
		{
			offset,
			( size ? offset + size : offset - 1u ),
		};
	}

	ImageLayout::DeviceSize ImageLayout::size()const noexcept
	{
		return depthLayers() * sliceSize();
	}

	ImageLayout::DeviceSize ImageLayout::sliceSize()const noexcept
	{
		return ashes::getLevelsSize( VkExtent2D{ extent->x, extent->y }
			, VkFormat( format )
			, baseLevel
			, levels
			, alignment );
	}

	ImageLayout::DeviceSize ImageLayout::sliceMipSize( uint32_t level )const noexcept
	{
		return ashes::getSize( VkExtent2D{ extent->x, extent->y }
			, VkFormat( format )
			, level );
	}

	ImageLayout::DeviceSize ImageLayout::offset()const noexcept
	{
		return sliceMipOffset( baseLayer, baseLevel );
	}

	ImageLayout::DeviceSize ImageLayout::sliceOffset( uint32_t index )const noexcept
	{
		return index * ashes::getLevelsSize( VkExtent2D{ extent->x, extent->y }
			, VkFormat( format )
			, 0u
			, levels
			, alignment );
	}
	
	ImageLayout::DeviceSize ImageLayout::sliceMipOffset( uint32_t index
		, uint32_t level )const noexcept
	{
		return sliceOffset( index )
			+ ashes::getLevelsSize( VkExtent2D{ extent->x, extent->y }
				, VkFormat( format )
				, 0u
				, level
				, alignment );
	}

	ImageLayout::Buffer ImageLayout::buffer( PxBufferBase & buffer )const noexcept
	{
		auto rng = range();
		return ImageLayout::Buffer
		{
			buffer.getPtr() + rng.getMin(),
			buffer.getPtr() + rng.getMax(),
		};
	}

	ImageLayout::Buffer ImageLayout::sliceBuffer( PxBufferBase & buffer
		, uint32_t index )const noexcept
	{
		auto range = slice( index );
		return ImageLayout::Buffer
		{
			buffer.getPtr() + range.getMin(),
			buffer.getPtr() + range.getMax(),
		};
	}

	ImageLayout::Buffer ImageLayout::sliceMipBuffer( PxBufferBase & buffer
		, uint32_t index
		, uint32_t level )const noexcept
	{
		auto range = sliceMip( index, level );
		return ImageLayout::Buffer
		{
			buffer.getPtr() + range.getMin(),
			buffer.getPtr() + range.getMax(),
		};
	}

	ImageLayout::ConstBuffer ImageLayout::buffer( PxBufferBase const & buffer )const noexcept
	{
		auto rng = range();
		return ImageLayout::ConstBuffer
		{
			buffer.getConstPtr() + rng.getMin(),
			buffer.getConstPtr() + rng.getMax(),
		};
	}

	ImageLayout::ConstBuffer ImageLayout::sliceBuffer( PxBufferBase const & buffer
		, uint32_t index )const noexcept
	{
		auto range = slice( index );
		return ImageLayout::ConstBuffer
		{
			buffer.getConstPtr() + range.getMin(),
			buffer.getConstPtr() + range.getMax(),
		};
	}

	ImageLayout::ConstBuffer ImageLayout::sliceMipBuffer( PxBufferBase const & buffer
		, uint32_t index
		, uint32_t level )const noexcept
	{
		auto range = sliceMip( index, level );
		return ImageLayout::ConstBuffer
		{
			buffer.getConstPtr() + range.getMin(),
			buffer.getConstPtr() + range.getMax(),
		};
	}

	bool ImageLayout::hasBuffer( PxBufferBase const & buffer )const noexcept
	{
		return buffer.getSize() >= range().getMax();
	}

	bool ImageLayout::hasSliceBuffer( PxBufferBase const & buffer, uint32_t index )const noexcept
	{
		return buffer.getSize() >= slice( index ).getMax();
	}

	bool ImageLayout::hasSliceMipBuffer( PxBufferBase const & buffer, uint32_t index, uint32_t level )const noexcept
	{
		return buffer.getSize() >= sliceMip( index, level ).getMax();
	}
}
