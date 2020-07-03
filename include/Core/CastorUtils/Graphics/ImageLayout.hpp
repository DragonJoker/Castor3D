/*
See LICENSE file in root folder
*/
#ifndef ___CU_ImageLayout_HPP___
#define ___CU_ImageLayout_HPP___

#include "CastorUtils/Math/Point.hpp"
#include "CastorUtils/Math/Range.hpp"
#include "CastorUtils/Graphics/Colour.hpp"
#include "CastorUtils/Graphics/PixelBuffer.hpp"

namespace castor
{
	struct ImageLayout
	{
		enum Type
			: uint8_t
		{
			e1D,
			e2D,
			e3D,
			eCube,
			e1DArray,
			e2DArray,
			eCubeArray,
			CU_EnumBounds( Type, e1D ),
		};

		using Buffer = castor::ArrayView< uint8_t >;
		using ConstBuffer = castor::ArrayView< uint8_t const >;
		using DeviceSize = uint64_t;
		using Range = castor::Range< DeviceSize >;

		CU_API ImageLayout( ImageLayout const & rhs ) = default;
		CU_API ImageLayout( ImageLayout && rhs ) = default;
		CU_API ImageLayout & operator=( ImageLayout const & rhs ) = default;
		CU_API ImageLayout & operator=( ImageLayout && rhs ) = default;

		inline ImageLayout( Type type = e2D
			, PixelFormat format = PixelFormat::eR8G8B8A8_UNORM
			, Point3ui extent = { 1u, 1u, 1u }
			, uint32_t baseLayer = 0u
			, uint32_t layers = 1u
			, uint32_t baseLevel = 0u
			, uint32_t levels = 1u )
			: type{ type }
			, format{ format }
			, extent{ extent }
			, baseLayer{ baseLayer }
			, layers{ layers }
			, baseLevel{ baseLevel }
			, levels{ levels }
		{
		}

		inline explicit ImageLayout( PxBufferBase const & buffer )
			: ImageLayout{ getType( buffer ), buffer }
		{
		}

		inline explicit ImageLayout( Type type
			, PxBufferBase const & buffer )
			: ImageLayout{ type
				, buffer.getFormat()
				, Point3ui{ buffer.getWidth(), buffer.getHeight(), 1u }
				, 0u
				, buffer.getLayers()
				, 0u
				, buffer.getLevels() }
		{
		}

		static inline Type getType( PxBufferBase const & buffer )
		{
			return ( buffer.getLayers() > 1
				? ( buffer.getHeight() > 1
					? e2DArray
					: e1DArray )
				: ( buffer.getHeight() > 1
					? e2D
					: e1D ) );
		}

		inline Size dimensions()const
		{
			return { extent->x, extent->y };
		}

		inline Size dimensions( uint32_t level )const
		{
			return dimensions() >> level;
		}

		inline uint32_t depthLayers()const
		{
			return std::max( extent->z, layers );
		}

		CU_API Range range()const;
		CU_API Range slice( uint32_t index )const;
		CU_API Range sliceMip( uint32_t index, uint32_t level )const;

		CU_API DeviceSize size()const;
		CU_API DeviceSize sliceSize()const;
		CU_API DeviceSize sliceMipSize( uint32_t level )const;

		CU_API DeviceSize offset()const;
		CU_API DeviceSize sliceOffset( uint32_t index )const;
		CU_API DeviceSize sliceMipOffset( uint32_t index, uint32_t level )const;

		CU_API Buffer buffer( PxBufferBase & buffer )const;
		CU_API Buffer sliceBuffer( PxBufferBase & buffer, uint32_t index )const;
		CU_API Buffer sliceMipBuffer( PxBufferBase & buffer, uint32_t index, uint32_t level )const;

		CU_API ConstBuffer buffer( PxBufferBase const & buffer )const;
		CU_API ConstBuffer sliceBuffer( PxBufferBase const & buffer, uint32_t index )const;
		CU_API ConstBuffer sliceMipBuffer( PxBufferBase const & buffer, uint32_t index, uint32_t level )const;

		CU_API bool hasBuffer( PxBufferBase const & buffer )const;
		CU_API bool hasSliceBuffer( PxBufferBase const & buffer, uint32_t index )const;
		CU_API bool hasSliceMipBuffer( PxBufferBase const & buffer, uint32_t index, uint32_t level )const;

		Range layer( uint32_t index )const
		{
			return slice( index );
		}

		Range layerMip( uint32_t index
			, uint32_t level )const
		{
			return sliceMip( index, level );
		}

		DeviceSize layerSize()const
		{
			return sliceSize();
		}

		DeviceSize layerMipSize( uint32_t level )const
		{
			return sliceMipSize( level );
		}

		DeviceSize layerOffset( uint32_t index )const
		{
			return sliceOffset( index );
		}

		DeviceSize layerMipOffset( uint32_t index
			, uint32_t level )const
		{
			return sliceMipOffset( index, level );
		}

		Buffer layerBuffer( PxBufferBase & buffer
			, uint32_t index )const
		{
			return sliceBuffer( buffer, index );
		}

		Buffer layerMipBuffer( PxBufferBase & buffer
			, uint32_t index
			, uint32_t level )const
		{
			return sliceMipBuffer( buffer, index, level );
		}

		ConstBuffer layerBuffer( PxBufferBase const & buffer
			, uint32_t index )const
		{
			return sliceBuffer( buffer, index );
		}

		ConstBuffer layerMipBuffer( PxBufferBase const & buffer
			, uint32_t index
			, uint32_t level )const
		{
			return sliceMipBuffer( buffer, index, level );
		}

		bool hasLayerBuffer( PxBufferBase const & buffer
			, uint32_t index )const
		{
			return hasSliceBuffer( buffer, index );
		}

		bool hasLayerMipBuffer( PxBufferBase const & buffer
			, uint32_t index
			, uint32_t level )const
		{
			return hasSliceMipBuffer( buffer, index, level );
		}

		Type type;
		PixelFormat format;
		Point3ui extent;
		uint32_t baseLayer;
		uint32_t layers;
		uint32_t baseLevel;
		uint32_t levels;
	};

	inline ImageLayout::DeviceSize getSliceSize( ImageLayout const & layout )
	{
		return layout.sliceSize();
	}

	inline ImageLayout::DeviceSize getSliceMipSize( ImageLayout const & layout
		, uint32_t level )
	{
		return layout.sliceMipSize( level );
	}

	inline ImageLayout::DeviceSize getLayerSize( ImageLayout const & layout )
	{
		return layout.layerSize();
	}

	inline ImageLayout::DeviceSize getLayerMipSize( ImageLayout const & layout
		, uint32_t level )
	{
		return layout.layerMipSize( level );
	}

	inline ImageLayout::DeviceSize getSliceOffset( ImageLayout const & layout
		, uint32_t index )
	{
		return layout.sliceOffset( index );
	}

	inline ImageLayout::DeviceSize getSliceMipOffset( ImageLayout const & layout
		, uint32_t index
		, uint32_t level )
	{
		return layout.sliceMipOffset( index, level );
	}

	inline ImageLayout::DeviceSize getLayerOffset( ImageLayout const & layout
		, uint32_t index )
	{
		return layout.layerOffset( index );
	}

	inline ImageLayout::DeviceSize getLayerMipOffset( ImageLayout const & layout
		, uint32_t index
		, uint32_t level )
	{
		return layout.layerMipOffset( index, level );
	}

	inline ImageLayout::Buffer getBuffer( ImageLayout const & layout
		, PxBufferBase & buffer )
	{
		return layout.buffer( buffer );
	}

	inline ImageLayout::ConstBuffer getBuffer( ImageLayout const & layout
		, PxBufferBase const & buffer )
	{
		return layout.buffer( buffer );
	}

	inline ImageLayout::Buffer getLayerBuffer( ImageLayout const & layout
		, PxBufferBase & buffer
		, uint32_t index )
	{
		return layout.layerBuffer( buffer, index );
	}

	inline ImageLayout::Buffer getLayerMipBuffer( ImageLayout const & layout
		, PxBufferBase & buffer
		, uint32_t index
		, uint32_t level )
	{
		return layout.layerMipBuffer( buffer, index, level );
	}

	inline ImageLayout::ConstBuffer getLayerBuffer( ImageLayout const & layout
		, PxBufferBase const & buffer
		, uint32_t index )
	{
		return layout.layerBuffer( buffer, index );
	}

	inline ImageLayout::ConstBuffer getLayerMipBuffer( ImageLayout const & layout
		, PxBufferBase const & buffer
		, uint32_t index
		, uint32_t level )
	{
		return layout.layerMipBuffer( buffer, index, level );
	}

	inline bool hasLayerBuffer( ImageLayout const & layout
		, PxBufferBase const & buffer
		, uint32_t index )
	{
		return layout.hasLayerBuffer( buffer, index );
	}

	inline bool hasLayerMipBuffer( ImageLayout const & layout
		, PxBufferBase const & buffer
		, uint32_t index
		, uint32_t level )
	{
		return layout.hasLayerMipBuffer( buffer, index, level );
	}

	inline ImageLayout::Buffer getSliceBuffer( ImageLayout const & layout
		, PxBufferBase & buffer
		, uint32_t index )
	{
		return layout.sliceBuffer( buffer, index );
	}

	inline ImageLayout::Buffer getSliceMipBuffer( ImageLayout const & layout
		, PxBufferBase & buffer
		, uint32_t index
		, uint32_t level )
	{
		return layout.sliceMipBuffer( buffer, index, level );
	}

	inline ImageLayout::ConstBuffer getSliceBuffer( ImageLayout const & layout
		, PxBufferBase const & buffer
		, uint32_t index )
	{
		return layout.sliceBuffer( buffer, index );
	}

	inline ImageLayout::ConstBuffer getSliceMipBuffer( ImageLayout const & layout
		, PxBufferBase const & buffer
		, uint32_t index
		, uint32_t level )
	{
		return layout.sliceMipBuffer( buffer, index, level );
	}

	inline bool hasSliceBuffer( ImageLayout const & layout
		, PxBufferBase const & buffer
		, uint32_t index )
	{
		return layout.hasSliceBuffer( buffer, index );
	}

	inline bool hasSliceMipBuffer( ImageLayout const & layout
		, PxBufferBase const & buffer
		, uint32_t index
		, uint32_t level )
	{
		return layout.hasSliceMipBuffer( buffer, index, level );
	}
}

#endif
