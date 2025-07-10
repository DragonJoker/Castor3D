#include "CastorUtils/Graphics/ImageMemoryLayout.hpp"

#include "CastorUtils/Data/LoaderException.hpp"
#include "CastorUtils/Data/Path.hpp"
#include "CastorUtils/Graphics/ImageLoader.hpp"
#include "CastorUtils/Graphics/Rectangle.hpp"
#include "CastorUtils/Graphics/Size.hpp"
#include "CastorUtils/Log/Logger.hpp"

#include <ashes/common/Format.hpp>

namespace castor
{
	ImageMemoryLayout::SubresourceRange ImageMemoryLayout::range()const noexcept
	{
		auto off = offset();
		auto sze = size();
		return
		{
			off,
			( sze ? off + sze : off - 1u ),
		};
	}
	
	ImageMemoryLayout::SubresourceRange ImageMemoryLayout::slice( uint32_t index )const noexcept
	{
		auto offset = sliceOffset( index );
		auto size = sliceSize();
		return
		{
			offset,
			( size ? offset + size : offset - 1u ),
		};
	}

	ImageMemoryLayout::SubresourceRange ImageMemoryLayout::sliceMip( uint32_t index, uint32_t level )const noexcept
	{
		auto offset = sliceMipOffset( index, level );
		auto size = sliceMipSize( level );
		return
		{
			offset,
			( size ? offset + size : offset - 1u ),
		};
	}

	ImageMemoryLayout::DeviceSize ImageMemoryLayout::size()const noexcept
	{
		return depthLayers() * sliceSize();
	}

	ImageMemoryLayout::DeviceSize ImageMemoryLayout::sliceSize()const noexcept
	{
		return ashes::getLevelsSize( VkExtent2D{ extent->x, extent->y }
			, VkFormat( format )
			, baseLevel
			, levels
			, alignment );
	}

	ImageMemoryLayout::DeviceSize ImageMemoryLayout::sliceMipSize( uint32_t level )const noexcept
	{
		return ashes::getSize( VkExtent2D{ extent->x, extent->y }
			, VkFormat( format )
			, level );
	}

	ImageMemoryLayout::DeviceSize ImageMemoryLayout::offset()const noexcept
	{
		return sliceMipOffset( baseLayer, baseLevel );
	}

	ImageMemoryLayout::DeviceSize ImageMemoryLayout::sliceOffset( uint32_t index )const noexcept
	{
		return index * ashes::getLevelsSize( VkExtent2D{ extent->x, extent->y }
			, VkFormat( format )
			, 0u
			, levels
			, alignment );
	}
	
	ImageMemoryLayout::DeviceSize ImageMemoryLayout::sliceMipOffset( uint32_t index
		, uint32_t level )const noexcept
	{
		return sliceOffset( index )
			+ ashes::getLevelsSize( VkExtent2D{ extent->x, extent->y }
				, VkFormat( format )
				, 0u
				, level
				, alignment );
	}

	ImageMemoryLayout::Buffer ImageMemoryLayout::buffer( PxBufferBase & buffer )const noexcept
	{
		auto rng = range();
		return ImageMemoryLayout::Buffer
		{
			buffer.getPtr() + rng.getMin(),
			buffer.getPtr() + rng.getMax(),
		};
	}

	ImageMemoryLayout::Buffer ImageMemoryLayout::sliceBuffer( PxBufferBase & buffer
		, uint32_t index )const noexcept
	{
		auto range = slice( index );
		return ImageMemoryLayout::Buffer
		{
			buffer.getPtr() + range.getMin(),
			buffer.getPtr() + range.getMax(),
		};
	}

	ImageMemoryLayout::Buffer ImageMemoryLayout::sliceMipBuffer( PxBufferBase & buffer
		, uint32_t index
		, uint32_t level )const noexcept
	{
		auto range = sliceMip( index, level );
		return ImageMemoryLayout::Buffer
		{
			buffer.getPtr() + range.getMin(),
			buffer.getPtr() + range.getMax(),
		};
	}

	ImageMemoryLayout::ConstBuffer ImageMemoryLayout::buffer( PxBufferBase const & buffer )const noexcept
	{
		auto rng = range();
		return ImageMemoryLayout::ConstBuffer
		{
			buffer.getConstPtr() + rng.getMin(),
			buffer.getConstPtr() + rng.getMax(),
		};
	}

	ImageMemoryLayout::ConstBuffer ImageMemoryLayout::sliceBuffer( PxBufferBase const & buffer
		, uint32_t index )const noexcept
	{
		auto range = slice( index );
		return ImageMemoryLayout::ConstBuffer
		{
			buffer.getConstPtr() + range.getMin(),
			buffer.getConstPtr() + range.getMax(),
		};
	}

	ImageMemoryLayout::ConstBuffer ImageMemoryLayout::sliceMipBuffer( PxBufferBase const & buffer
		, uint32_t index
		, uint32_t level )const noexcept
	{
		auto range = sliceMip( index, level );
		return ImageMemoryLayout::ConstBuffer
		{
			buffer.getConstPtr() + range.getMin(),
			buffer.getConstPtr() + range.getMax(),
		};
	}

	bool ImageMemoryLayout::hasBuffer( PxBufferBase const & buffer )const noexcept
	{
		return buffer.getSize() >= range().getMax();
	}

	bool ImageMemoryLayout::hasSliceBuffer( PxBufferBase const & buffer, uint32_t index )const noexcept
	{
		return buffer.getSize() >= slice( index ).getMax();
	}

	bool ImageMemoryLayout::hasSliceMipBuffer( PxBufferBase const & buffer, uint32_t index, uint32_t level )const noexcept
	{
		return buffer.getSize() >= sliceMip( index, level ).getMax();
	}
}
