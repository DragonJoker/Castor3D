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
	ImageLayout::Range ImageLayout::range()const
	{
		auto off = offset();
		auto sze = size();
		return
		{
			off,
			( sze ? off + sze : off - 1u ),
		};
	}
	
	ImageLayout::Range ImageLayout::slice( uint32_t index )const
	{
		auto offset = sliceOffset( index );
		auto size = sliceSize();
		return
		{
			offset,
			( size ? offset + size : offset - 1u ),
		};
	}

	ImageLayout::Range ImageLayout::sliceMip( uint32_t index, uint32_t level )const
	{
		auto offset = sliceMipOffset( index, level );
		auto size = sliceMipSize( level );
		return
		{
			offset,
			( size ? offset + size : offset - 1u ),
		};
	}

	ImageLayout::DeviceSize ImageLayout::size()const
	{
		return depthLayers() * sliceSize();
	}

	ImageLayout::DeviceSize ImageLayout::sliceSize()const
	{
		return ashes::getLevelsSize( VkExtent2D{ extent->x, extent->y }
			, VkFormat( format )
			, baseLevel
			, levels
			, alignment );
	}

	ImageLayout::DeviceSize ImageLayout::sliceMipSize( uint32_t level )const
	{
		return ashes::getSize( VkExtent2D{ extent->x, extent->y }
			, VkFormat( format )
			, level );
	}

	ImageLayout::DeviceSize ImageLayout::offset()const
	{
		return sliceMipOffset( baseLayer, baseLevel );
	}

	ImageLayout::DeviceSize ImageLayout::sliceOffset( uint32_t index )const
	{
		return index * ashes::getLevelsSize( VkExtent2D{ extent->x, extent->y }
			, VkFormat( format )
			, 0u
			, levels
			, alignment );
	}
	
	ImageLayout::DeviceSize ImageLayout::sliceMipOffset( uint32_t index
		, uint32_t level )const
	{
		return sliceOffset( index )
			+ ashes::getLevelsSize( VkExtent2D{ extent->x, extent->y }
				, VkFormat( format )
				, 0u
				, level
				, alignment );
	}

	ImageLayout::Buffer ImageLayout::buffer( PxBufferBase & buffer )const
	{
		auto rng = range();
		return ImageLayout::Buffer
		{
			buffer.getPtr() + rng.getMin(),
			buffer.getPtr() + rng.getMax(),
		};
	}

	ImageLayout::Buffer ImageLayout::sliceBuffer( PxBufferBase & buffer
		, uint32_t index )const
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
		, uint32_t level )const
	{
		auto range = sliceMip( index, level );
		return ImageLayout::Buffer
		{
			buffer.getPtr() + range.getMin(),
			buffer.getPtr() + range.getMax(),
		};
	}

	ImageLayout::ConstBuffer ImageLayout::buffer( PxBufferBase const & buffer )const
	{
		auto rng = range();
		return ImageLayout::ConstBuffer
		{
			buffer.getConstPtr() + rng.getMin(),
			buffer.getConstPtr() + rng.getMax(),
		};
	}

	ImageLayout::ConstBuffer ImageLayout::sliceBuffer( PxBufferBase const & buffer
		, uint32_t index )const
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
		, uint32_t level )const
	{
		auto range = sliceMip( index, level );
		return ImageLayout::ConstBuffer
		{
			buffer.getConstPtr() + range.getMin(),
			buffer.getConstPtr() + range.getMax(),
		};
	}

	bool ImageLayout::hasBuffer( PxBufferBase const & buffer )const
	{
		return buffer.getSize() >= range().getMax();
	}

	bool ImageLayout::hasSliceBuffer( PxBufferBase const & buffer, uint32_t index )const
	{
		return buffer.getSize() >= slice( index ).getMax();
	}

	bool ImageLayout::hasSliceMipBuffer( PxBufferBase const & buffer, uint32_t index, uint32_t level )const
	{
		return buffer.getSize() >= sliceMip( index, level ).getMax();
	}
}
