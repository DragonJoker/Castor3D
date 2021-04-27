#include "CastorUtils/Graphics/PixelBuffer.hpp"

#include "CastorUtils/Miscellaneous/BitSize.hpp"
#include "CastorUtils/Graphics/PxBufferCompression.hpp"

#include <ashes/common/Format.hpp>

#include "stb_image_resize.h"

namespace castor
{
	//*********************************************************************************************

	namespace
	{
		VkDeviceSize getDataAt( VkFormat format
			, uint32_t x
			, uint32_t y
			, uint32_t index
			, uint32_t level
			, uint32_t levels
			, uint32_t align
			, PxBufferBase const & buffer )
		{
			return VkDeviceSize( ( index * ashes::getLevelsSize( VkExtent2D{ x, y }, format, 0u, levels, align ) )
				+ ashes::getLevelsSize( VkExtent2D{ x, y }, format, 0u, level, 1u ) )
				+ ( size_t( ( y * buffer.getWidth() + x ) * ashes::getMinimalSize( format ) ) );
		}

		template< typename TypeT >
		struct LargerTyperT;

		template<>
		struct LargerTyperT< uint8_t >
		{
			using Type = uint16_t;
		};

		template<>
		struct LargerTyperT< int8_t >
		{
			using Type = int16_t;
		};

		template<>
		struct LargerTyperT< uint16_t >
		{
			using Type = uint32_t;
		};

		template<>
		struct LargerTyperT< int16_t >
		{
			using Type = int32_t;
		};

		template<>
		struct LargerTyperT< uint32_t >
		{
			using Type = uint64_t;
		};

		template<>
		struct LargerTyperT< int32_t >
		{
			using Type = int64_t;
		};

		template<>
		struct LargerTyperT< uint64_t >
		{
			using Type = uint64_t;
		};

		template<>
		struct LargerTyperT< int64_t >
		{
			using Type = int64_t;
		};

		template<>
		struct LargerTyperT< float >
		{
			using Type = double;
		};

		template<>
		struct LargerTyperT< double >
		{
			using Type = long double;
		};

		template< typename TypeT >
		using LargerTypeT = typename LargerTyperT< TypeT >::Type;

		template< PixelFormat PFT >
		void generateMipPixelT( uint8_t const * srcPixel1
			, uint8_t const * srcPixel2
			, uint8_t const * srcPixel3
			, uint8_t const * srcPixel4
			, uint8_t * dstPixel )
		{
			using MyPixelComponentsT = PixelComponentsT< PFT >;
			using MyTypeT = typename MyPixelComponentsT::Type;
			using MyLargerTypeT = LargerTypeT< MyTypeT >;

			MyPixelComponentsT::R( dstPixel
				, MyTypeT( ( MyLargerTypeT( MyPixelComponentsT::R( srcPixel1 ) )
						+ MyLargerTypeT( MyPixelComponentsT::R( srcPixel2 ) )
						+ MyLargerTypeT( MyPixelComponentsT::R( srcPixel3 ) )
						+ MyLargerTypeT( MyPixelComponentsT::R( srcPixel4 ) ) )
					/ 4u ) );

			if constexpr ( getComponentsCount( PFT ) >= 2 )
			{
				MyPixelComponentsT::G( dstPixel
					, MyTypeT( ( MyLargerTypeT( MyPixelComponentsT::G( srcPixel1 ) )
							+ MyLargerTypeT( MyPixelComponentsT::G( srcPixel2 ) )
							+ MyLargerTypeT( MyPixelComponentsT::G( srcPixel3 ) )
							+ MyLargerTypeT( MyPixelComponentsT::G( srcPixel4 ) ) )
						/ 4u ) );

				if constexpr ( getComponentsCount( PFT ) >= 3 )
				{
					MyPixelComponentsT::B( dstPixel
						, MyTypeT( ( MyLargerTypeT( MyPixelComponentsT::B( srcPixel1 ) )
								+ MyLargerTypeT( MyPixelComponentsT::B( srcPixel2 ) )
								+ MyLargerTypeT( MyPixelComponentsT::B( srcPixel3 ) )
								+ MyLargerTypeT( MyPixelComponentsT::B( srcPixel4 ) ) )
							/ 4u ) );

					if constexpr ( getComponentsCount( PFT ) >= 4 )
					{
						MyPixelComponentsT::A( dstPixel
							, MyTypeT( ( MyLargerTypeT( MyPixelComponentsT::A( srcPixel1 ) )
									+ MyLargerTypeT( MyPixelComponentsT::A( srcPixel2 ) )
									+ MyLargerTypeT( MyPixelComponentsT::A( srcPixel3 ) )
									+ MyLargerTypeT( MyPixelComponentsT::A( srcPixel4 ) ) )
								/ 4u ) );
					}
				}
			}
		}

		template< PixelFormat PFT >
		void generateMipLevelT( VkExtent2D const & extent
			, uint8_t const * srcBuffer
			, uint8_t * dstBuffer
			, uint32_t level
			, uint32_t levelSize )
		{
			auto pixelSize = getBytesPerPixel( PFT );
			auto srcLevelExtent = ashes::getSubresourceDimensions( extent, level - 1u, VkFormat( PFT ) );
			auto dstLevelExtent = ashes::getSubresourceDimensions( extent, level, VkFormat( PFT ) );
			auto srcLineSize = pixelSize * srcLevelExtent.width;
			auto dstLineSize = pixelSize * dstLevelExtent.width;

			for ( auto y = 0u; y < dstLevelExtent.height; ++y )
			{
				auto srcLine0 = srcBuffer;
				auto srcLine1 = srcBuffer + srcLineSize;
				auto dstLine = dstBuffer;

				for ( auto x = 0u; x < dstLevelExtent.width; ++x )
				{
					generateMipPixelT< PFT >( srcLine0
						, srcLine0 + pixelSize
						, srcLine1
						, srcLine1 + pixelSize
						, dstLine );
					srcLine0 += 2 * pixelSize;
					srcLine1 += 2 * pixelSize;
					dstLine += pixelSize;
				}

				srcBuffer += 2 * srcLineSize;
				dstBuffer += dstLineSize;
			}
		}

		template< PixelFormat PFT >
		ByteArray generateMipmapsT( VkExtent3D const & extent
			, uint8_t const * buffer
			, uint32_t align
			, uint32_t dstLevels )
		{
			ByteArray result;
			auto vkfmt = VkFormat( PFT );
			VkExtent2D dim{ extent.width, extent.height };
			auto srcLayerSize = ashes::getLevelsSize( dim
				, vkfmt
				, 0u
				, 1u
				, 0u );
			auto dstLayerSize = ashes::getLevelsSize( dim
				, vkfmt
				, 0u
				, dstLevels
				, 0u );
			result.resize( dstLayerSize * extent.depth );
			auto srcLayer = buffer;
			auto dstLayer = result.data();

			for ( auto layer = 0u; layer < extent.depth; ++layer )
			{
				auto levelSize = ashes::getSize( dim
					, vkfmt
					, 0u
					, align );

				// Copy level 0 to dst layer buffer
				std::memcpy( dstLayer
					, srcLayer
					, levelSize );

				auto dstLevel = dstLayer;

				for ( auto i = 1u; i < dstLevels; ++i )
				{
					auto srcLevel = dstLevel;
					dstLevel += levelSize;
					levelSize = ashes::getSize( dim
						, vkfmt
						, i );
					generateMipLevelT< PFT >( dim
						, srcLevel
						, dstLevel
						, i
						, levelSize );
				}

				srcLayer += srcLayerSize;
				dstLayer += dstLayerSize;
			}

			return result;
		}

		ByteArray resample( VkExtent3D const & srcDimensions
			, VkExtent3D const & dstDimensions
			, PixelFormat format
			, uint8_t const * src )
		{
			int channels = int( getComponentsCount( format ) );
			int alpha{ hasAlpha( format )
				? 1
				: STBIR_ALPHA_CHANNEL_NONE };
			stbir_datatype dataType{ isFloatingPoint( format )
				? STBIR_TYPE_FLOAT
				: STBIR_TYPE_UINT8 };
			stbir_colorspace colorSpace{ isSRGBFormat( format )
				? STBIR_COLORSPACE_SRGB
				: STBIR_COLORSPACE_LINEAR };
			auto dstLayerSize = ashes::getSize( VkExtent2D{ dstDimensions.width, dstDimensions.height }
				, VkFormat( format ) );
			ByteArray result;
			result.resize( dstLayerSize );
			auto dst = result.data();

			auto ret = stbir_resize( src, int( srcDimensions.width ), int( srcDimensions.height ), 0
				, dst, int( dstDimensions.width ), int( dstDimensions.height ), 0
				, dataType
				, channels, alpha, 0
				, STBIR_EDGE_CLAMP, STBIR_EDGE_CLAMP
				, STBIR_FILTER_CATMULLROM, STBIR_FILTER_CATMULLROM
				, colorSpace, nullptr );

			if ( !ret )
			{
				CU_LoaderError( "Image couldn't be resized" );
			}

			return result;
		}

		ByteArray generateMipmaps( VkExtent3D const & extent
			, uint8_t const * buffer
			, PixelFormat format
			, uint32_t align
			, uint32_t dstLevels )
		{
			switch ( format )
			{
#define CUPF_ENUM_VALUE_COLOR( name, value, components, alpha )\
			case PixelFormat::e##name:\
				return generateMipmapsT< PixelFormat::e##name >( extent, buffer, align, dstLevels );
				break;
#include "CastorUtils/Graphics/PixelFormat.enum"
			default:
				CU_Failure( "Unsupported format type for CPU mipmaps generation" );
				return ByteArray{};
			}
		}

		ByteArray prepareForCompression( VkExtent3D & extent
			, uint8_t const * buffer
			, PixelFormat bufferFormat
			, uint32_t bufferAlign
			, PixelFormat compressed
			, uint32_t & dstLevels )
		{
			ByteArray result;
			auto blockSize = ashes::getBlockSize( VkFormat( compressed ) );

			if ( ( extent.width % blockSize.extent.width ) != 0
				|| ( extent.height % blockSize.extent.height ) != 0 )
			{
				auto resized = extent;
				resized.width = uint32_t( ashes::getAlignedSize( extent.width, blockSize.extent.width ) );
				resized.height = uint32_t( ashes::getAlignedSize( extent.height, blockSize.extent.height ) );
				result = resample( extent
					, resized
					, bufferFormat
					, buffer );
				extent = resized;
				buffer = result.data();
				dstLevels = 1u;
			}

			auto requiredLevels = ashes::getMaxMipCount( extent );

			if ( dstLevels <= 1u
				&& requiredLevels > 1u )
			{
				// Since blitting to compressed formats may not be supported by graphics API,
				// we need to generate them on CPU.
				dstLevels = requiredLevels;
				result = generateMipmaps( extent
					, buffer
					, bufferFormat
					, bufferAlign
					, dstLevels );
			}

			return result;
		}

		void copyBuffer( PxBufferConvertOptions const * options
			, Size const & dimensions
			, uint8_t const * srcBuffer
			, PixelFormat srcFormat
			, uint32_t srcAlign
			, uint8_t * dstBuffer
			, PixelFormat dstFormat
			, uint32_t dstAlign
			, VkExtent3D const & extent
			, uint32_t layers
			, uint32_t levels )
		{
			srcAlign = ( srcAlign
				? srcAlign
				: uint32_t( getBytesPerPixel( srcFormat ) ) );
			dstAlign = ( dstAlign
				? dstAlign
				: uint32_t( getBytesPerPixel( dstFormat ) ) );
			auto srcLayerStart = srcBuffer;
			auto dstLayerStart = dstBuffer;
			auto srcBlockSize = ashes::getBlockSize( VkFormat( srcFormat ) );
			auto dstBlockSize = ashes::getBlockSize( VkFormat( dstFormat ) );

			for ( uint32_t layer = 0u; layer < layers; ++layer )
			{
				auto srcLevelStart = srcLayerStart;
				auto dstLevelStart = dstLayerStart;

				for ( uint32_t level = 0u; level < levels; ++level )
				{
					auto srcLevel = srcLevelStart;
					auto dstLevel = dstLevelStart;
					auto srcLevelSize = uint32_t( ashes::getSize( VkFormat( srcFormat )
						, extent
						, srcBlockSize
						, level
						, srcAlign ) );
					auto dstLevelSize = uint32_t( ashes::getSize( VkFormat( dstFormat )
						, extent
						, dstBlockSize
						, level
						, dstAlign ) );
					auto srcLevelExtent = ashes::getSubresourceDimensions( VkExtent2D{ dimensions.getWidth(), dimensions.getHeight() }
						, level
						, VkFormat( srcFormat ) );
					auto dstLevelExtent = ashes::getSubresourceDimensions( VkExtent2D{ dimensions.getWidth(), dimensions.getHeight() }
						, level
						, VkFormat( dstFormat ) );

					convertBuffer( { srcLevelExtent.width, srcLevelExtent.height }
						, { dstLevelExtent.width, dstLevelExtent.height }
						, srcFormat
						, srcLevel
						, srcLevelSize
						, dstFormat
						, dstLevel
						, dstLevelSize );
					srcLevelStart += srcLevelSize;
					dstLevelStart += dstLevelSize;
				}

				srcLayerStart = srcLevelStart;
				dstLayerStart = dstLevelStart;
			}
		}

		void compressBuffer( PxBufferConvertOptions const * options
			, Size const & dimensions
			, uint8_t const * srcBuffer
			, PixelFormat srcFormat
			, uint32_t srcAlign
			, uint8_t * dstBuffer
			, PixelFormat dstFormat
			, uint32_t dstAlign
			, VkExtent3D const & extent
			, uint32_t layers
			, uint32_t levels )
		{
			srcAlign = ( srcAlign
				? srcAlign
				: uint32_t( getBytesPerPixel( srcFormat ) ) );
			dstAlign = ( dstAlign
				? dstAlign
				: uint32_t( getBytesPerPixel( dstFormat ) ) );
			auto srcLayerStart = srcBuffer;
			auto dstLayerStart = dstBuffer;
			auto srcBlockSize = ashes::getBlockSize( VkFormat( srcFormat ) );
			auto dstBlockSize = ashes::getBlockSize( VkFormat( dstFormat ) );

			for ( uint32_t layer = 0u; layer < layers; ++layer )
			{
				auto srcLevelStart = srcLayerStart;
				auto dstLevelStart = dstLayerStart;

				for ( uint32_t level = 0u; level < levels; ++level )
				{
					auto srcLevel = srcLevelStart;
					auto dstLevel = dstLevelStart;
					auto srcLevelSize = uint32_t( ashes::getSize( VkFormat( srcFormat )
						, extent
						, srcBlockSize
						, level
						, srcAlign ) );
					auto dstLevelSize = uint32_t( ashes::getSize( VkFormat( dstFormat )
						, extent
						, dstBlockSize
						, level
						, dstAlign ) );
					auto srcLevelExtent = ashes::getSubresourceDimensions( VkExtent2D{ dimensions.getWidth(), dimensions.getHeight() }
						, level
						, VkFormat( srcFormat ) );
					auto dstLevelExtent = ashes::getSubresourceDimensions( VkExtent2D{ dimensions.getWidth(), dimensions.getHeight() }
						, level
						, VkFormat( srcFormat ) );

					compressBuffer( options
						, { srcLevelExtent.width, srcLevelExtent.height }
						, { dstLevelExtent.width, dstLevelExtent.height }
						, srcFormat
						, srcLevel
						, srcLevelSize
						, dstFormat
						, dstLevel
						, dstLevelSize );
					srcLevelStart += srcLevelSize;
					dstLevelStart += dstLevelSize;
				}

				srcLayerStart = srcLevelStart;
				dstLayerStart = dstLevelStart;
			}
		}
	}

	//*********************************************************************************************

	PxBufferConvertOptions::PxBufferConvertOptions( PxCompressionSupport support )
		: support{ std::move( support ) }
#if CU_UseCVTT
		, additionalOptions{ new CVTTOptions{} }
#endif
	{
	}

	PxBufferConvertOptions::~PxBufferConvertOptions()
	{
#if CU_UseCVTT
		auto options = reinterpret_cast< CVTTOptions * >( additionalOptions );
		delete options;
#endif
	}

	PixelFormat PxBufferConvertOptions::getCompressed( PixelFormat format )const
	{
		PixelFormat result = format;

		switch ( format )
		{
		case PixelFormat::eR8G8B8_UNORM:
		case PixelFormat::eB8G8R8_UNORM:
			if ( support.supportBC1 )
			{
				result = PixelFormat::eBC1_RGB_UNORM_BLOCK;
			}
			break;
		case PixelFormat::eR8G8B8_SRGB:
		case PixelFormat::eB8G8R8_SRGB:
			if ( support.supportBC1 )
			{
				result = PixelFormat::eBC1_RGB_SRGB_BLOCK;
			}
			break;
		case PixelFormat::eB8G8R8A8_UNORM:
		case PixelFormat::eR8G8B8A8_UNORM:
		case PixelFormat::eA8B8G8R8_UNORM:
			if ( support.supportBC3 )
			{
				result = PixelFormat::eBC3_UNORM_BLOCK;
			}
			break;
		case PixelFormat::eR8G8B8A8_SRGB:
		case PixelFormat::eA8B8G8R8_SRGB:
			if ( support.supportBC3 )
			{
				result = PixelFormat::eBC3_SRGB_BLOCK;
			}
			break;
		default:
			break;
		}

		return result;
	}

	uint32_t PxBufferConvertOptions::getAdditionalAlign( PixelFormat format )const
	{
		switch ( format )
		{
#if CU_UseCVTT
		case castor::PixelFormat::eBC1_RGB_UNORM_BLOCK:
		case castor::PixelFormat::eBC1_RGB_SRGB_BLOCK:
		case castor::PixelFormat::eBC1_RGBA_UNORM_BLOCK:
		case castor::PixelFormat::eBC1_RGBA_SRGB_BLOCK:
		case castor::PixelFormat::eBC4_UNORM_BLOCK:
		case castor::PixelFormat::eBC4_SNORM_BLOCK:
		case castor::PixelFormat::eBC2_UNORM_BLOCK:
		case castor::PixelFormat::eBC2_SRGB_BLOCK:
		case castor::PixelFormat::eBC3_UNORM_BLOCK:
		case castor::PixelFormat::eBC3_SRGB_BLOCK:
		case castor::PixelFormat::eBC5_UNORM_BLOCK:
		case castor::PixelFormat::eBC5_SNORM_BLOCK:
		case castor::PixelFormat::eBC6H_UFLOAT_BLOCK:
		case castor::PixelFormat::eBC6H_SFLOAT_BLOCK:
		case castor::PixelFormat::eBC7_UNORM_BLOCK:
		case castor::PixelFormat::eBC7_SRGB_BLOCK:
		case castor::PixelFormat::eETC2_R8G8B8_UNORM_BLOCK:
		case castor::PixelFormat::eETC2_R8G8B8_SRGB_BLOCK:
		case castor::PixelFormat::eETC2_R8G8B8A1_UNORM_BLOCK:
		case castor::PixelFormat::eETC2_R8G8B8A1_SRGB_BLOCK:
		case castor::PixelFormat::eETC2_R8G8B8A8_UNORM_BLOCK:
		case castor::PixelFormat::eETC2_R8G8B8A8_SRGB_BLOCK:
			return cvtt::NumParallelBlocks * getBytesPerPixel( format );
#endif
		default:
			return 1u;
		}
	}

	//*********************************************************************************************

	PxBufferBase::PxBufferBase( PxBufferConvertOptions const * options
		, Size const & size
		, PixelFormat format
		, uint32_t layers
		, uint32_t levels
		, uint8_t const * buffer
		, PixelFormat bufferFormat
		, uint32_t bufferAlign )
		: m_format{ format == PixelFormat::eUNDEFINED ? bufferFormat : format }
		, m_size{ size }
		, m_layers{ layers }
		, m_levels{ levels }
		, m_buffer{ 0 }
	{
		initialise( options, buffer, bufferFormat, bufferAlign );
	}

	PxBufferBase::PxBufferBase( Size const & size
		, PixelFormat format
		, uint32_t layers
		, uint32_t levels
		, uint8_t const * buffer
		, PixelFormat bufferFormat
		, uint32_t bufferAlign )
		: PxBufferBase{ nullptr
			, size
			, format
			, layers
			, levels
			, buffer
			, bufferFormat
			, bufferAlign }
	{
	}

	PxBufferBase::PxBufferBase( PxBufferBase const & rhs )
		: m_format{ rhs.m_format }
		, m_size{ rhs.m_size }
		, m_layers{ rhs.m_layers }
		, m_levels{ rhs.m_levels }
		, m_buffer{ 0 }
	{
		initialise( rhs.getConstPtr(), rhs.getFormat(), rhs.getAlign() );
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

	void PxBufferBase::initialise( PxBufferConvertOptions const * options
		, uint8_t const * buffer
		, PixelFormat bufferFormat
		, uint32_t bufferAlign )
	{
		auto extent = VkExtent3D{ m_size.getWidth(), m_size.getHeight(), m_layers };

		if ( isCompressed( getFormat() )
			&& !isCompressed( bufferFormat )
			&& buffer )
		{
			ByteArray mips;
			mips = prepareForCompression( extent
				, buffer
				, bufferFormat
				, bufferAlign
				, getFormat()
				, m_levels );
			m_size = { extent.width, extent.height };
			buffer = mips.empty() ? buffer : mips.data();
			m_align = ( mips.empty() || !options )
				? getBytesPerPixel( getFormat() )
				: std::max( VkDeviceSize( options->getAdditionalAlign( getFormat() ) ), getBytesPerPixel( getFormat() ) );
			VkDeviceSize newSize = ashes::getLevelsSize( extent
				, VkFormat( getFormat() )
				, 0u
				, m_levels
				, m_align );
			m_buffer.resize( newSize );
			compressBuffer( options
				, m_size
				, buffer
				, bufferFormat
				, bufferAlign
				, m_buffer.data()
				, getFormat()
				, m_align
				, extent
				, m_layers
				, m_levels );
		}
		else
		{
			m_align = isCompressed( bufferFormat )
				? bufferAlign
				: getBytesPerPixel( getFormat() );
			VkDeviceSize newSize = ashes::getLevelsSize( extent
				, VkFormat( getFormat() )
				, 0u
				, m_levels
				, m_align );
			m_buffer.resize( newSize );

			if ( !buffer )
			{
				memset( m_buffer.data(), 0, newSize );
			}
			else
			{
				copyBuffer( options
					, m_size
					, buffer
					, bufferFormat
					, bufferAlign
					, m_buffer.data()
					, getFormat()
					, m_align
					, extent
					, m_layers
					, m_levels );
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
		std::swap( m_format, pixelBuffer.m_format );
		std::swap( m_flipped, pixelBuffer.m_flipped );
		std::swap( m_size, pixelBuffer.m_size );
		std::swap( m_layers, pixelBuffer.m_layers );
		std::swap( m_levels, pixelBuffer.m_levels );
		std::swap( m_buffer, pixelBuffer.m_buffer );
	}

	uint32_t getMipLevels( VkExtent3D const & extent )
	{
		return uint32_t( ashes::getMaxMipCount( extent ) );
	}

	uint32_t getMinMipLevels( uint32_t mipLevels
		, VkExtent3D const & extent )
	{
		return std::min( getMipLevels( extent ), mipLevels );
	}

	void PxBufferBase::generateMips()
	{
		auto levels = getMipLevels( { m_size.getWidth(), m_size.getHeight(), 1u } );

		if ( m_levels == 1u
			&& levels > m_levels )
		{
			m_levels = levels;
			auto buffer = generateMipmaps( { m_size.getWidth(), m_size.getHeight(), m_layers }
				, m_buffer.data()
				, m_format
				, m_align
				, m_levels );
			m_buffer = buffer;
		}
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
			auto srcLevels = m_levels;
			m_layers = layers;
			m_levels = levels;

			auto newSize = m_layers * ashes::getLevelsSize( extent
				, VkFormat( getFormat() )
				, 0u
				, m_levels
				, m_align );
			m_buffer.resize( newSize );

			auto srcBuffer = buffer.data();
			auto srcLayerSize = ashes::getLevelsSize( extent
				, VkFormat( getFormat() )
				, 0u
				, srcLevels
				, m_align );
			auto dstBuffer = m_buffer.data();
			auto dstLayerSize = ashes::getLevelsSize( extent
				, VkFormat( getFormat() )
				, 0u
				, m_levels
				, m_align );

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
			+ getDataAt( VkFormat( m_format ), x, y, index, level, m_levels, m_align, *this );
	}

	PxBufferBase::ConstPixelData PxBufferBase::getAt( uint32_t x
		, uint32_t y
		, uint32_t index
		, uint32_t level )const
	{
		CU_Require( x < getWidth() && y < getHeight() );
		return m_buffer.begin()
			+ getDataAt( VkFormat( m_format ), x, y, index, level, m_levels, m_align, *this );
	}

	PxBufferBaseSPtr PxBufferBase::create( PxBufferConvertOptions const * options
		, Size const & size
		, uint32_t layers
		, uint32_t levels
		, PixelFormat wantedFormat
		, uint8_t const * buffer
		, PixelFormat bufferFormat
		, uint32_t bufferAlign )
	{
		return std::make_shared< PxBufferBase >( options
			, size
			, wantedFormat
			, layers
			, levels
			, buffer
			, bufferFormat
			, bufferAlign );
	}

	PxBufferBaseUPtr PxBufferBase::createUnique( PxBufferConvertOptions const * options
		, Size const & size
		, uint32_t layers
		, uint32_t levels
		, PixelFormat wantedFormat
		, uint8_t const * buffer
		, PixelFormat bufferFormat
		, uint32_t bufferAlign )
	{
		return std::make_unique< PxBufferBase >( options
			, size
			, wantedFormat
			, layers
			, levels
			, buffer
			, bufferFormat
			, bufferAlign );
	}

	//*********************************************************************************************
}
