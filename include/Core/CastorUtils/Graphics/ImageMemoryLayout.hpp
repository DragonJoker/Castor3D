/*
See LICENSE file in root folder
*/
#ifndef ___CU_ImageMemoryLayout_HPP___
#define ___CU_ImageMemoryLayout_HPP___

#include "CastorUtils/Math/Point.hpp"
#include "CastorUtils/Math/Range.hpp"
#include "CastorUtils/Graphics/Colour.hpp"
#include "CastorUtils/Graphics/PixelBuffer.hpp"

namespace castor
{
	struct ImageMemoryLayout
	{
		enum Type
			: uint8_t
		{
#pragma clang push
#pragma clang diagnostic ignored "-Wduplicate-enum"
			e1D = 0,
			e2D = 1,
			e3D = 2,
			eCube = 3,
			e1DArray = 4,
			e2DArray = 5,
			eCubeArray = 6,
			CU_EnumBounds( Type, e1D, eCubeArray ),
#pragma clang pop
		};
		static StringView getName( Type t )
		{
			switch ( t )
			{
			case Type::e1D:
				return cuT( "1D" );
			case Type::e2D:
				return cuT( "2D" );
			case Type::e3D:
				return cuT( "3D" );
			case Type::eCube:
				return cuT( "Cube" );
			case Type::e1DArray:
				return cuT( "1DArray" );
			case Type::e2DArray:
				return cuT( "2DArray" );
			case Type::eCubeArray:
				return cuT( "CubeArray" );
			default:
				return cuT( "UnknowImageType" );
			}
		}

		using Buffer = ByteArrayView;
		using ConstBuffer = ConstByteArrayView;
		using DeviceSize = uint64_t;
		using SubresourceRange = Range< DeviceSize >;

		explicit ImageMemoryLayout( Type type = e2D
			, PixelFormat format = PixelFormat::eR8G8B8A8_UNORM
			, Point3ui extent = { 1u, 1u, 1u }
			, uint32_t baseLayer = 0u
			, uint32_t layers = 1u
			, uint32_t baseLevel = 0u
			, uint32_t levels = 1u
			, uint32_t alignment = 1u )
			: type{ type }
			, format{ format }
			, extent{ extent }
			, baseLayer{ baseLayer }
			, layers{ layers }
			, baseLevel{ baseLevel }
			, levels{ levels }
			, alignment{ alignment }
		{
		}

		explicit ImageMemoryLayout( PxBufferBase const & buffer )
			: ImageMemoryLayout{ getType( buffer ), buffer }
		{
		}

		explicit ImageMemoryLayout( Type type
			, PxBufferBase const & buffer )
			: ImageMemoryLayout{ type
				, buffer.getFormat()
				, Point3ui{ buffer.getWidth(), buffer.getHeight(), 1u }
				, 0u
				, buffer.getLayers()
				, 0u
				, buffer.getLevels()
				, buffer.getAlign() }
		{
		}

		static Type getType( PxBufferBase const & buffer )
		{
			return ( buffer.getLayers() > 1
				? ( buffer.getHeight() <= 1 && buffer.getWidth() > 1 ? e1DArray : e2DArray )
				: ( buffer.getHeight() <= 1 && buffer.getWidth() > 1 ? e1D : e2D ) );
		}

		Size dimensions()const noexcept
		{
			return { extent->x, extent->y };
		}

		Size dimensions( uint32_t level )const noexcept
		{
			return dimensions() >> level;
		}

		uint32_t depthLayers()const noexcept
		{
			return std::max( extent->z, layers );
		}

		CU_API SubresourceRange range()const noexcept;
		CU_API SubresourceRange slice( uint32_t index )const noexcept;
		CU_API SubresourceRange sliceMip( uint32_t index, uint32_t level )const noexcept;

		CU_API DeviceSize size()const noexcept;
		CU_API DeviceSize sliceSize()const noexcept;
		CU_API DeviceSize sliceMipSize( uint32_t level )const noexcept;

		CU_API DeviceSize offset()const noexcept;
		CU_API DeviceSize sliceOffset( uint32_t index )const noexcept;
		CU_API DeviceSize sliceMipOffset( uint32_t index, uint32_t level )const noexcept;

		CU_API Buffer buffer( PxBufferBase & buffer )const noexcept;
		CU_API Buffer sliceBuffer( PxBufferBase & buffer, uint32_t index )const noexcept;
		CU_API Buffer sliceMipBuffer( PxBufferBase & buffer, uint32_t index, uint32_t level )const noexcept;

		CU_API ConstBuffer buffer( PxBufferBase const & buffer )const noexcept;
		CU_API ConstBuffer sliceBuffer( PxBufferBase const & buffer, uint32_t index )const noexcept;
		CU_API ConstBuffer sliceMipBuffer( PxBufferBase const & buffer, uint32_t index, uint32_t level )const noexcept;

		CU_API bool hasBuffer( PxBufferBase const & buffer )const noexcept;
		CU_API bool hasSliceBuffer( PxBufferBase const & buffer, uint32_t index )const noexcept;
		CU_API bool hasSliceMipBuffer( PxBufferBase const & buffer, uint32_t index, uint32_t level )const noexcept;

		SubresourceRange layer( uint32_t index )const noexcept
		{
			return slice( index );
		}

		SubresourceRange layerMip( uint32_t index
			, uint32_t level )const noexcept
		{
			return sliceMip( index, level );
		}

		DeviceSize layerSize()const noexcept
		{
			return sliceSize();
		}

		DeviceSize layerMipSize( uint32_t level )const noexcept
		{
			return sliceMipSize( level );
		}

		DeviceSize layerOffset( uint32_t index )const noexcept
		{
			return sliceOffset( index );
		}

		DeviceSize layerMipOffset( uint32_t index
			, uint32_t level )const noexcept
		{
			return sliceMipOffset( index, level );
		}

		Buffer layerBuffer( PxBufferBase & buffer
			, uint32_t index )const noexcept
		{
			return sliceBuffer( buffer, index );
		}

		Buffer layerMipBuffer( PxBufferBase & buffer
			, uint32_t index
			, uint32_t level )const noexcept
		{
			return sliceMipBuffer( buffer, index, level );
		}

		ConstBuffer layerBuffer( PxBufferBase const & buffer
			, uint32_t index )const noexcept
		{
			return sliceBuffer( buffer, index );
		}

		ConstBuffer layerMipBuffer( PxBufferBase const & buffer
			, uint32_t index
			, uint32_t level )const noexcept
		{
			return sliceMipBuffer( buffer, index, level );
		}

		bool hasLayerBuffer( PxBufferBase const & buffer
			, uint32_t index )const noexcept
		{
			return hasSliceBuffer( buffer, index );
		}

		bool hasLayerMipBuffer( PxBufferBase const & buffer
			, uint32_t index
			, uint32_t level )const noexcept
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
		uint32_t alignment;
	};

	inline ImageMemoryLayout::DeviceSize getSliceSize( ImageMemoryLayout const & layout )
	{
		return layout.sliceSize();
	}

	inline ImageMemoryLayout::DeviceSize getSliceMipSize( ImageMemoryLayout const & layout
		, uint32_t level )
	{
		return layout.sliceMipSize( level );
	}

	inline ImageMemoryLayout::DeviceSize getLayerSize( ImageMemoryLayout const & layout )
	{
		return layout.layerSize();
	}

	inline ImageMemoryLayout::DeviceSize getLayerMipSize( ImageMemoryLayout const & layout
		, uint32_t level )
	{
		return layout.layerMipSize( level );
	}

	inline ImageMemoryLayout::DeviceSize getSliceOffset( ImageMemoryLayout const & layout
		, uint32_t index )
	{
		return layout.sliceOffset( index );
	}

	inline ImageMemoryLayout::DeviceSize getSliceMipOffset( ImageMemoryLayout const & layout
		, uint32_t index
		, uint32_t level )
	{
		return layout.sliceMipOffset( index, level );
	}

	inline ImageMemoryLayout::DeviceSize getLayerOffset( ImageMemoryLayout const & layout
		, uint32_t index )
	{
		return layout.layerOffset( index );
	}

	inline ImageMemoryLayout::DeviceSize getLayerMipOffset( ImageMemoryLayout const & layout
		, uint32_t index
		, uint32_t level )
	{
		return layout.layerMipOffset( index, level );
	}

	inline ImageMemoryLayout::Buffer getBuffer( ImageMemoryLayout const & layout
		, PxBufferBase & buffer )
	{
		return layout.buffer( buffer );
	}

	inline ImageMemoryLayout::ConstBuffer getBuffer( ImageMemoryLayout const & layout
		, PxBufferBase const & buffer )
	{
		return layout.buffer( buffer );
	}

	inline ImageMemoryLayout::Buffer getLayerBuffer( ImageMemoryLayout const & layout
		, PxBufferBase & buffer
		, uint32_t index )
	{
		return layout.layerBuffer( buffer, index );
	}

	inline ImageMemoryLayout::Buffer getLayerMipBuffer( ImageMemoryLayout const & layout
		, PxBufferBase & buffer
		, uint32_t index
		, uint32_t level )
	{
		return layout.layerMipBuffer( buffer, index, level );
	}

	inline ImageMemoryLayout::ConstBuffer getLayerBuffer( ImageMemoryLayout const & layout
		, PxBufferBase const & buffer
		, uint32_t index )
	{
		return layout.layerBuffer( buffer, index );
	}

	inline ImageMemoryLayout::ConstBuffer getLayerMipBuffer( ImageMemoryLayout const & layout
		, PxBufferBase const & buffer
		, uint32_t index
		, uint32_t level )
	{
		return layout.layerMipBuffer( buffer, index, level );
	}

	inline bool hasLayerBuffer( ImageMemoryLayout const & layout
		, PxBufferBase const & buffer
		, uint32_t index )
	{
		return layout.hasLayerBuffer( buffer, index );
	}

	inline bool hasLayerMipBuffer( ImageMemoryLayout const & layout
		, PxBufferBase const & buffer
		, uint32_t index
		, uint32_t level )
	{
		return layout.hasLayerMipBuffer( buffer, index, level );
	}

	inline ImageMemoryLayout::Buffer getSliceBuffer( ImageMemoryLayout const & layout
		, PxBufferBase & buffer
		, uint32_t index )
	{
		return layout.sliceBuffer( buffer, index );
	}

	inline ImageMemoryLayout::Buffer getSliceMipBuffer( ImageMemoryLayout const & layout
		, PxBufferBase & buffer
		, uint32_t index
		, uint32_t level )
	{
		return layout.sliceMipBuffer( buffer, index, level );
	}

	inline ImageMemoryLayout::ConstBuffer getSliceBuffer( ImageMemoryLayout const & layout
		, PxBufferBase const & buffer
		, uint32_t index )
	{
		return layout.sliceBuffer( buffer, index );
	}

	inline ImageMemoryLayout::ConstBuffer getSliceMipBuffer( ImageMemoryLayout const & layout
		, PxBufferBase const & buffer
		, uint32_t index
		, uint32_t level )
	{
		return layout.sliceMipBuffer( buffer, index, level );
	}

	inline bool hasSliceBuffer( ImageMemoryLayout const & layout
		, PxBufferBase const & buffer
		, uint32_t index )
	{
		return layout.hasSliceBuffer( buffer, index );
	}

	inline bool hasSliceMipBuffer( ImageMemoryLayout const & layout
		, PxBufferBase const & buffer
		, uint32_t index
		, uint32_t level )
	{
		return layout.hasSliceMipBuffer( buffer, index, level );
	}
}

#endif
