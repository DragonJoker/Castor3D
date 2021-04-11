#include "CastorUtils/Graphics/PixelFormat.hpp"
#include "CastorUtils/Graphics/PixelBuffer.hpp"

#include <ashes/common/Format.hpp>

namespace castor
{
	namespace PF
	{
		//*****************************************************************************************

		PixelFormat getPFWithoutAlpha( PixelFormat format )
		{
			PixelFormat result = PixelFormat::eCount;

			switch ( format )
			{
			case PixelFormat::eR8A8_UNORM:
				result = PixelDefinitions< PixelFormat::eR8A8_UNORM >::NoAlphaPF;
				break;

			case PixelFormat::eR16A16_SFLOAT:
				result = PixelDefinitions< PixelFormat::eR16A16_SFLOAT >::NoAlphaPF;
				break;

			case PixelFormat::eR32A32_SFLOAT:
				result = PixelDefinitions< PixelFormat::eR32A32_SFLOAT >::NoAlphaPF;
				break;

			case PixelFormat::eR5G5B5A1_UNORM:
				result = PixelDefinitions< PixelFormat::eR5G5B5A1_UNORM >::NoAlphaPF;
				break;

			case PixelFormat::eR8G8B8A8_UNORM:
				result = PixelDefinitions< PixelFormat::eR8G8B8A8_UNORM >::NoAlphaPF;
				break;

			case PixelFormat::eA8B8G8R8_UNORM:
				result = PixelDefinitions< PixelFormat::eA8B8G8R8_UNORM >::NoAlphaPF;
				break;

			case PixelFormat::eR8G8B8A8_SRGB:
				result = PixelDefinitions< PixelFormat::eR8G8B8A8_UNORM >::NoAlphaPF;
				break;

			case PixelFormat::eA8B8G8R8_SRGB:
				result = PixelDefinitions< PixelFormat::eA8B8G8R8_UNORM >::NoAlphaPF;
				break;

			case PixelFormat::eR32G32B32A32_SFLOAT:
				result = PixelDefinitions< PixelFormat::eR32G32B32A32_SFLOAT >::NoAlphaPF;
				break;

			case PixelFormat::eBC3_UNORM_BLOCK:
				result = PixelDefinitions< PixelFormat::eBC3_UNORM_BLOCK >::NoAlphaPF;
				break;

			case PixelFormat::eBC5_UNORM_BLOCK:
				result = PixelDefinitions< PixelFormat::eBC5_UNORM_BLOCK >::NoAlphaPF;
				break;

			default:
				result = format;
				break;
			}

			return result;
		}

		bool hasComponent( PixelFormat format
			, PixelComponent component )
		{
			bool result = false;

			switch ( format )
			{
			case PixelFormat::eR8_UNORM:
				result = component == PixelComponent::eRed;
				break;

			case PixelFormat::eR16_SFLOAT:
				result = component == PixelComponent::eRed;
				break;

			case PixelFormat::eR32_SFLOAT:
				result = component == PixelComponent::eRed;
				break;

			case PixelFormat::eR8A8_UNORM:
				result = component == PixelComponent::eRed
					|| component == PixelComponent::eAlpha;
				break;

			case PixelFormat::eR32A32_SFLOAT:
				result = component == PixelComponent::eRed
					|| component == PixelComponent::eAlpha;
				break;

			case PixelFormat::eR5G5B5A1_UNORM:
				result = component == PixelComponent::eRed
					|| component == PixelComponent::eGreen
					|| component == PixelComponent::eBlue
					|| component == PixelComponent::eAlpha;
				break;

			case PixelFormat::eR5G6B5_UNORM:
			case PixelFormat::eR8G8B8_UNORM:
			case PixelFormat::eB8G8R8_UNORM:
			case PixelFormat::eR8G8B8_SRGB:
			case PixelFormat::eB8G8R8_SRGB:
				result = component == PixelComponent::eRed
					|| component == PixelComponent::eGreen
					|| component == PixelComponent::eBlue;
				break;

			case PixelFormat::eR8G8B8A8_UNORM:
			case PixelFormat::eA8B8G8R8_UNORM:
			case PixelFormat::eB8G8R8A8_UNORM:
			case PixelFormat::eR8G8B8A8_SRGB:
			case PixelFormat::eA8B8G8R8_SRGB:
				result = component == PixelComponent::eRed
					|| component == PixelComponent::eGreen
					|| component == PixelComponent::eBlue
					|| component == PixelComponent::eAlpha;
				break;

			case PixelFormat::eR16G16B16_SFLOAT:
				result = component == PixelComponent::eRed
					|| component == PixelComponent::eGreen
					|| component == PixelComponent::eBlue;
				break;

			case PixelFormat::eR16G16B16A16_SFLOAT:
				result = component == PixelComponent::eRed
					|| component == PixelComponent::eGreen
					|| component == PixelComponent::eBlue
					|| component == PixelComponent::eAlpha;
				break;

			case PixelFormat::eR32G32B32_SFLOAT:
				result = component == PixelComponent::eRed
					|| component == PixelComponent::eGreen
					|| component == PixelComponent::eBlue;
				break;

			case PixelFormat::eR32G32B32A32_SFLOAT:
				result = component == PixelComponent::eRed
					|| component == PixelComponent::eGreen
					|| component == PixelComponent::eBlue
					|| component == PixelComponent::eAlpha;
				break;

			case PixelFormat::eBC1_RGB_UNORM_BLOCK:
				result = component == PixelComponent::eRed
					|| component == PixelComponent::eGreen
					|| component == PixelComponent::eBlue;
				break;

			case PixelFormat::eBC3_UNORM_BLOCK:
				result = component == PixelComponent::eRed
					|| component == PixelComponent::eGreen
					|| component == PixelComponent::eBlue
					|| component == PixelComponent::eAlpha;
				break;

			case PixelFormat::eBC5_UNORM_BLOCK:
				result = component == PixelComponent::eRed
					|| component == PixelComponent::eGreen;
				break;

			case PixelFormat::eD16_UNORM:
				result = component == PixelComponent::eDepth;
				break;

			case PixelFormat::eD24_UNORM_S8_UINT:
				result = component == PixelComponent::eStencil
					|| component == PixelComponent::eDepth;
				break;

			case PixelFormat::eD32_UNORM:
				result = component == PixelComponent::eDepth;
				break;

			case PixelFormat::eD32_SFLOAT:
				result = component == PixelComponent::eDepth;
				break;

			case PixelFormat::eD32_SFLOAT_S8_UINT:
				result = component == PixelComponent::eStencil
					|| component == PixelComponent::eDepth;
				break;

			case PixelFormat::eS8_UINT:
				result = component == PixelComponent::eStencil;
				break;

			default:
				result = false;
				break;
			}

			return result;
		}

		bool hasAlpha( PixelFormat format )
		{
			bool result = false;

			switch ( format )
			{
			case PixelFormat::eR8_UNORM:
				result = PixelDefinitions< PixelFormat::eR8_UNORM >::Alpha;
				break;

			case PixelFormat::eR16_SFLOAT:
				result = PixelDefinitions< PixelFormat::eR16_SFLOAT >::Alpha;
				break;

			case PixelFormat::eR32_SFLOAT:
				result = PixelDefinitions< PixelFormat::eR32_SFLOAT >::Alpha;
				break;

			case PixelFormat::eR8A8_UNORM:
				result = PixelDefinitions< PixelFormat::eR8A8_UNORM >::Alpha;
				break;

			case PixelFormat::eR32A32_SFLOAT:
				result = PixelDefinitions< PixelFormat::eR32A32_SFLOAT >::Alpha;
				break;

			case PixelFormat::eR5G5B5A1_UNORM:
				result = PixelDefinitions< PixelFormat::eR5G5B5A1_UNORM >::Alpha;
				break;

			case PixelFormat::eR5G6B5_UNORM:
				result = PixelDefinitions< PixelFormat::eR5G6B5_UNORM >::Alpha;
				break;

			case PixelFormat::eR8G8B8_UNORM:
				result = PixelDefinitions< PixelFormat::eR8G8B8_UNORM >::Alpha;
				break;

			case PixelFormat::eB8G8R8_UNORM:
				result = PixelDefinitions< PixelFormat::eB8G8R8_UNORM >::Alpha;
				break;

			case PixelFormat::eB8G8R8A8_UNORM:
				result = PixelDefinitions< PixelFormat::eB8G8R8A8_UNORM >::Alpha;
				break;

			case PixelFormat::eR8G8B8_SRGB:
				result = PixelDefinitions< PixelFormat::eR8G8B8_SRGB >::Alpha;
				break;

			case PixelFormat::eB8G8R8_SRGB:
				result = PixelDefinitions< PixelFormat::eB8G8R8_SRGB >::Alpha;
				break;

			case PixelFormat::eR8G8B8A8_UNORM:
				result = PixelDefinitions< PixelFormat::eR8G8B8A8_UNORM >::Alpha;
				break;

			case PixelFormat::eA8B8G8R8_UNORM:
				result = PixelDefinitions< PixelFormat::eA8B8G8R8_UNORM >::Alpha;
				break;

			case PixelFormat::eR8G8B8A8_SRGB:
				result = PixelDefinitions< PixelFormat::eR8G8B8A8_SRGB >::Alpha;
				break;

			case PixelFormat::eA8B8G8R8_SRGB:
				result = PixelDefinitions< PixelFormat::eA8B8G8R8_SRGB >::Alpha;
				break;

			case PixelFormat::eR16G16B16_SFLOAT:
				result = PixelDefinitions< PixelFormat::eR16G16B16_SFLOAT >::Alpha;
				break;

			case PixelFormat::eR16G16B16A16_SFLOAT:
				result = PixelDefinitions< PixelFormat::eR16G16B16A16_SFLOAT >::Alpha;
				break;

			case PixelFormat::eR32G32B32_SFLOAT:
				result = PixelDefinitions< PixelFormat::eR32G32B32_SFLOAT >::Alpha;
				break;

			case PixelFormat::eR32G32B32A32_SFLOAT:
				result = PixelDefinitions< PixelFormat::eR32G32B32A32_SFLOAT >::Alpha;
				break;

			case PixelFormat::eBC1_RGB_UNORM_BLOCK:
				result = PixelDefinitions< PixelFormat::eBC1_RGB_UNORM_BLOCK >::Alpha;
				break;

			case PixelFormat::eBC3_UNORM_BLOCK:
				result = PixelDefinitions< PixelFormat::eBC3_UNORM_BLOCK >::Alpha;
				break;

			case PixelFormat::eBC5_UNORM_BLOCK:
				result = PixelDefinitions< PixelFormat::eBC5_UNORM_BLOCK >::Alpha;
				break;

			case PixelFormat::eD16_UNORM:
				result = PixelDefinitions< PixelFormat::eD16_UNORM >::Alpha;
				break;

			case PixelFormat::eD24_UNORM_S8_UINT:
				result = PixelDefinitions< PixelFormat::eD24_UNORM_S8_UINT >::Alpha;
				break;

			case PixelFormat::eD32_UNORM:
				result = PixelDefinitions< PixelFormat::eD32_UNORM >::Alpha;
				break;

			case PixelFormat::eD32_SFLOAT:
				result = PixelDefinitions< PixelFormat::eD32_SFLOAT >::Alpha;
				break;

			case PixelFormat::eD32_SFLOAT_S8_UINT:
				result = PixelDefinitions< PixelFormat::eD32_SFLOAT_S8_UINT >::Alpha;
				break;

			case PixelFormat::eS8_UINT:
				result = PixelDefinitions< PixelFormat::eS8_UINT >::Alpha;
				break;

			default:
				result = false;
				break;
			}

			return result;
		}

		bool isFloatingPoint( PixelFormat format )
		{
			return format == PixelFormat::eR32G32B32A32_SFLOAT
				|| format == PixelFormat::eR32G32B32_SFLOAT
				|| format == PixelFormat::eR32A32_SFLOAT
				|| format == PixelFormat::eR32_SFLOAT
				|| format == PixelFormat::eR16G16B16A16_SFLOAT
				|| format == PixelFormat::eR16G16B16_SFLOAT
				|| format == PixelFormat::eR16A16_SFLOAT
				|| format == PixelFormat::eR16_SFLOAT
				|| format == PixelFormat::eD32_SFLOAT
				|| format == PixelFormat::eD32_SFLOAT_S8_UINT;
		}

		bool isCompressed( PixelFormat format )
		{
			return format >= PixelFormat::eBC1_RGB_UNORM_BLOCK
				&& format <= PixelFormat::eASTC_12x12_SRGB_BLOCK;
		}

		PxBufferBaseSPtr decompressBuffer( PxBufferBaseSPtr src )
		{
			PxBufferBaseSPtr result = src;

			if ( isCompressed( src->getFormat() ) )
			{
				using PFNDecompressBlock = bool( *)( uint8_t const * data, uint8_t * pixelBuffer );
				PFNDecompressBlock decompressBlock = nullptr;

				switch ( src->getFormat() )
				{
				case PixelFormat::eBC1_RGB_UNORM_BLOCK:
				case PixelFormat::eBC1_RGB_SRGB_BLOCK:
					decompressBlock = decompressBC1Block;
					break;
				case PixelFormat::eBC3_UNORM_BLOCK:
				case PixelFormat::eBC3_SRGB_BLOCK:
					decompressBlock = decompressBC3Block;
					break;
				case PixelFormat::eBC5_UNORM_BLOCK:
				case PixelFormat::eBC5_SNORM_BLOCK:
					decompressBlock = decompressBC5Block;
					break;
				default:
					CU_Failure( "Unsupported compression format" );
					return result;
				}

				result = PxBufferBase::create( src->getDimensions()
					, PixelFormat::eR8G8B8A8_UNORM );
				uint8_t * pixelBuffer = result->getPtr();
				uint8_t blockBuffer[16 * 4u];
				uint8_t const * data = src->getConstPtr();
				uint32_t pixelSize = PF::getBytesPerPixel( result->getFormat() );
				uint32_t height = src->getHeight();
				uint32_t width = src->getWidth();
				uint32_t heightInBlocks = height / 4u;
				uint32_t widthInBlocks = width / 4u;

				for ( uint32_t y = 0u; y < heightInBlocks; ++y )
				{
					uint32_t newRows;

					if ( y * 4 + 3 >= height )
					{
						newRows = height - y * 4u;
					}
					else
					{
						newRows = 4u;
					}

					for ( uint32_t x = 0u; x < widthInBlocks; ++x )
					{
						bool r = decompressBlock( data, blockBuffer );

						if ( !r )
						{
							return src;
						}

						uint32_t blockSize = 8u;
						uint8_t * pixelp = pixelBuffer +
							y * 4u * width * pixelSize +
							x * 4u * pixelSize;
						uint32_t newColumns;

						if ( x * 4 + 3 >= width )
						{
							newColumns = width - x * 4;
						}
						else
						{
							newColumns = 4u;
						}

						for ( uint32_t row = 0u; row < newRows; ++row )
						{
							memcpy( pixelp + row * width * pixelSize
								, blockBuffer + row * 4u * pixelSize
								, newColumns * pixelSize );
						}

						data += blockSize;
					}
				}

				if ( src->getFormat() == PixelFormat::eBC1_RGB_UNORM_BLOCK )
				{
					// Remove alpha channel for BC1 compressed buffer.
					extractAlpha( result );
				}
			}

			return result;
		}

		PxBufferBaseSPtr extractComponent( PxBufferBaseSPtr src
			, PixelComponent component )
		{
			src = decompressBuffer( src );
			PxBufferBaseSPtr result;

			if ( hasComponent( src->getFormat(), component ) )
			{
				auto index = uint32_t( component );

				if ( isFloatingPoint( src->getFormat() ) )
				{
					result = PxBufferBase::create( src->getDimensions(), PixelFormat::eR32_SFLOAT );
					auto srcBuffer = reinterpret_cast< float const * >( src->getConstPtr() );
					auto dstBuffer = reinterpret_cast< float * >( result->getPtr() );
					auto srcPixelSize = PF::getBytesPerPixel( src->getFormat() ) / sizeof( float );
					auto dstPixelSize = PF::getBytesPerPixel( result->getFormat() ) / sizeof( float );

					for ( size_t i = 0; i < result->getCount(); ++i )
					{
						*dstBuffer = srcBuffer[index];
						dstBuffer += dstPixelSize;
						srcBuffer += srcPixelSize;
					}
				}
				else
				{
					result = PxBufferBase::create( src->getDimensions(), PixelFormat::eR8_UNORM );
					auto srcBuffer = src->getConstPtr();
					auto dstBuffer = result->getPtr();
					auto srcPixelSize = PF::getBytesPerPixel( src->getFormat() );
					auto dstPixelSize = PF::getBytesPerPixel( result->getFormat() );

					for ( size_t i = 0; i < result->getCount(); ++i )
					{
						*dstBuffer = srcBuffer[index];
						dstBuffer += dstPixelSize;
						srcBuffer += srcPixelSize;
					}
				}
			}

			return result;
		}

		PxBufferBaseSPtr extractAlpha( PxBufferBaseSPtr & srcBuffer )
		{
			PxBufferBaseSPtr result;

			if ( hasAlpha( srcBuffer->getFormat() ) )
			{
				result = PxBufferBase::create( srcBuffer->getDimensions()
					, PixelFormat::eR8A8_UNORM
					, srcBuffer->getConstPtr()
					, srcBuffer->getFormat() );
				uint8_t * pBuffer = result->getPtr();

				for ( uint32_t i = 0; i < result->getSize(); i += 2 )
				{
					pBuffer[0] = pBuffer[1];
					pBuffer++;
					pBuffer++;
				}

				result = PxBufferBase::create( srcBuffer->getDimensions()
					, PixelFormat::eR8_UNORM
					, result->getConstPtr()
					, result->getFormat() );
				srcBuffer = PxBufferBase::create( srcBuffer->getDimensions()
					, getPFWithoutAlpha( srcBuffer->getFormat() )
					, srcBuffer->getConstPtr()
					, srcBuffer->getFormat() );
			}

			return result;
		}

		void reduceToAlpha( PxBufferBaseSPtr & srcBuffer )
		{
			if ( hasAlpha( srcBuffer->getFormat() ) )
			{
				if ( srcBuffer->getFormat() != PixelFormat::eR8A8_UNORM )
				{
					srcBuffer = PxBufferBase::create( srcBuffer->getDimensions()
						, PixelFormat::eR8A8_UNORM
						, srcBuffer->getConstPtr()
						, srcBuffer->getFormat() );
				}

				uint8_t * pBuffer = srcBuffer->getPtr();

				for ( uint32_t i = 0; i < srcBuffer->getSize(); i += 2 )
				{
					pBuffer[0] = pBuffer[1];
					pBuffer++;
					pBuffer++;
				}
			}

			srcBuffer = PxBufferBase::create( srcBuffer->getDimensions()
				, PixelFormat::eR8_UNORM
				, srcBuffer->getConstPtr()
				, srcBuffer->getFormat() );
		}

		uint8_t getBytesPerPixel( PixelFormat format )
		{
			return uint8_t( ashes::getSize( ashes::getMinimalExtent2D( VkFormat( format ) ), VkFormat( format ) ) );
		}

		uint8_t getComponentsCount( PixelFormat format )
		{
			return uint8_t( ashes::getCount( VkFormat( format ) ) );
		}

		void convertPixel( PixelFormat srcFormat
			, uint8_t const *& srcBuffer
			, PixelFormat dstFormat
			, uint8_t *& dstBuffer )
		{
			switch ( srcFormat )
			{
			case PixelFormat::eR8_UNORM:
				PixelDefinitions< PixelFormat::eR8_UNORM >::convert( srcBuffer, dstBuffer, dstFormat );
				break;

			case PixelFormat::eR16_SFLOAT:
				PixelDefinitions< PixelFormat::eR16_SFLOAT >::convert( srcBuffer, dstBuffer, dstFormat );
				break;

			case PixelFormat::eR32_SFLOAT:
				PixelDefinitions< PixelFormat::eR32_SFLOAT >::convert( srcBuffer, dstBuffer, dstFormat );
				break;

			case PixelFormat::eR8A8_UNORM:
				PixelDefinitions< PixelFormat::eR8A8_UNORM >::convert( srcBuffer, dstBuffer, dstFormat );
				break;

			case PixelFormat::eR32A32_SFLOAT:
				PixelDefinitions< PixelFormat::eR32A32_SFLOAT >::convert( srcBuffer, dstBuffer, dstFormat );
				break;

			case PixelFormat::eR5G5B5A1_UNORM:
				PixelDefinitions< PixelFormat::eR5G5B5A1_UNORM >::convert( srcBuffer, dstBuffer, dstFormat );
				break;

			case PixelFormat::eR5G6B5_UNORM:
				PixelDefinitions< PixelFormat::eR5G6B5_UNORM >::convert( srcBuffer, dstBuffer, dstFormat );
				break;

			case PixelFormat::eR8G8B8_UNORM:
				PixelDefinitions< PixelFormat::eR8G8B8_UNORM >::convert( srcBuffer, dstBuffer, dstFormat );
				break;

			case PixelFormat::eB8G8R8_UNORM:
				PixelDefinitions< PixelFormat::eB8G8R8_UNORM >::convert( srcBuffer, dstBuffer, dstFormat );
				break;

			case PixelFormat::eB8G8R8A8_UNORM:
				PixelDefinitions< PixelFormat::eB8G8R8A8_UNORM >::convert( srcBuffer, dstBuffer, dstFormat );
				break;

			case PixelFormat::eR8G8B8_SRGB:
				PixelDefinitions< PixelFormat::eR8G8B8_SRGB >::convert( srcBuffer, dstBuffer, dstFormat );
				break;

			case PixelFormat::eB8G8R8_SRGB:
				PixelDefinitions< PixelFormat::eB8G8R8_SRGB >::convert( srcBuffer, dstBuffer, dstFormat );
				break;

			case PixelFormat::eR8G8B8A8_UNORM:
				PixelDefinitions< PixelFormat::eR8G8B8A8_UNORM >::convert( srcBuffer, dstBuffer, dstFormat );
				break;

			case PixelFormat::eA8B8G8R8_UNORM:
				PixelDefinitions< PixelFormat::eA8B8G8R8_UNORM >::convert( srcBuffer, dstBuffer, dstFormat );
				break;

			case PixelFormat::eR8G8B8A8_SRGB:
				PixelDefinitions< PixelFormat::eR8G8B8A8_SRGB >::convert( srcBuffer, dstBuffer, dstFormat );
				break;

			case PixelFormat::eA8B8G8R8_SRGB:
				PixelDefinitions< PixelFormat::eA8B8G8R8_SRGB >::convert( srcBuffer, dstBuffer, dstFormat );
				break;

			case PixelFormat::eR16G16B16_SFLOAT:
				PixelDefinitions< PixelFormat::eR16G16B16_SFLOAT >::convert( srcBuffer, dstBuffer, dstFormat );
				break;

			case PixelFormat::eR16G16B16A16_SFLOAT:
				PixelDefinitions< PixelFormat::eR16G16B16A16_SFLOAT >::convert( srcBuffer, dstBuffer, dstFormat );
				break;

			case PixelFormat::eR32G32B32_SFLOAT:
				PixelDefinitions< PixelFormat::eR32G32B32_SFLOAT >::convert( srcBuffer, dstBuffer, dstFormat );
				break;

			case PixelFormat::eR32G32B32A32_SFLOAT:
				PixelDefinitions< PixelFormat::eR32G32B32A32_SFLOAT >::convert( srcBuffer, dstBuffer, dstFormat );
				break;

			case PixelFormat::eBC1_RGB_UNORM_BLOCK:
				PixelDefinitions< PixelFormat::eBC1_RGB_UNORM_BLOCK >::convert( srcBuffer, dstBuffer, dstFormat );
				break;

			case PixelFormat::eBC3_UNORM_BLOCK:
				PixelDefinitions< PixelFormat::eBC3_UNORM_BLOCK >::convert( srcBuffer, dstBuffer, dstFormat );
				break;

			case PixelFormat::eBC5_UNORM_BLOCK:
				PixelDefinitions< PixelFormat::eBC5_UNORM_BLOCK >::convert( srcBuffer, dstBuffer, dstFormat );
				break;

			case PixelFormat::eD16_UNORM:
				PixelDefinitions< PixelFormat::eD16_UNORM >::convert( srcBuffer, dstBuffer, dstFormat );
				break;

			case PixelFormat::eD24_UNORM_S8_UINT:
				PixelDefinitions< PixelFormat::eD24_UNORM_S8_UINT >::convert( srcBuffer, dstBuffer, dstFormat );
				break;

			case PixelFormat::eD32_UNORM:
				PixelDefinitions< PixelFormat::eD32_UNORM >::convert( srcBuffer, dstBuffer, dstFormat );
				break;

			case PixelFormat::eD32_SFLOAT:
				PixelDefinitions< PixelFormat::eD32_SFLOAT >::convert( srcBuffer, dstBuffer, dstFormat );
				break;

			case PixelFormat::eD32_SFLOAT_S8_UINT:
				PixelDefinitions< PixelFormat::eD32_SFLOAT_S8_UINT >::convert( srcBuffer, dstBuffer, dstFormat );
				break;

			case PixelFormat::eS8_UINT:
				PixelDefinitions< PixelFormat::eS8_UINT >::convert( srcBuffer, dstBuffer, dstFormat );
				break;

			default:
				CU_Failure( "Unsupported pixel format" );
				break;
			}
		}

		void convertBuffer( Size const & srcDimensions
			, Size const & dstDimensions
			, PixelFormat srcFormat
			, uint8_t const * srcBuffer
			, uint32_t srcSize
			, PixelFormat dstFormat
			, uint8_t * dstBuffer
			, uint32_t dstSize )
		{
			switch ( srcFormat )
			{
			case PixelFormat::eR8_UNORM:
				PixelDefinitions< PixelFormat::eR8_UNORM >::convert( srcDimensions, dstDimensions, srcBuffer, srcSize, dstFormat, dstBuffer, dstSize );
				break;

			case PixelFormat::eR16_SFLOAT:
				PixelDefinitions< PixelFormat::eR16_SFLOAT >::convert( srcDimensions, dstDimensions, srcBuffer, srcSize, dstFormat, dstBuffer, dstSize );
				break;

			case PixelFormat::eR32_SFLOAT:
				PixelDefinitions< PixelFormat::eR32_SFLOAT >::convert( srcDimensions, dstDimensions, srcBuffer, srcSize, dstFormat, dstBuffer, dstSize );
				break;

			case PixelFormat::eR8A8_UNORM:
				PixelDefinitions< PixelFormat::eR8A8_UNORM >::convert( srcDimensions, dstDimensions, srcBuffer, srcSize, dstFormat, dstBuffer, dstSize );
				break;

			case PixelFormat::eR32A32_SFLOAT:
				PixelDefinitions< PixelFormat::eR32A32_SFLOAT >::convert( srcDimensions, dstDimensions, srcBuffer, srcSize, dstFormat, dstBuffer, dstSize );
				break;

			case PixelFormat::eR5G5B5A1_UNORM:
				PixelDefinitions< PixelFormat::eR5G5B5A1_UNORM >::convert( srcDimensions, dstDimensions, srcBuffer, srcSize, dstFormat, dstBuffer, dstSize );
				break;

			case PixelFormat::eR5G6B5_UNORM:
				PixelDefinitions< PixelFormat::eR5G6B5_UNORM >::convert( srcDimensions, dstDimensions, srcBuffer, srcSize, dstFormat, dstBuffer, dstSize );
				break;

			case PixelFormat::eR8G8B8_UNORM:
				PixelDefinitions< PixelFormat::eR8G8B8_UNORM >::convert( srcDimensions, dstDimensions, srcBuffer, srcSize, dstFormat, dstBuffer, dstSize );
				break;

			case PixelFormat::eB8G8R8_UNORM:
				PixelDefinitions< PixelFormat::eB8G8R8_UNORM >::convert( srcDimensions, dstDimensions, srcBuffer, srcSize, dstFormat, dstBuffer, dstSize );
				break;

			case PixelFormat::eB8G8R8A8_UNORM:
				PixelDefinitions< PixelFormat::eB8G8R8A8_UNORM >::convert( srcDimensions, dstDimensions, srcBuffer, srcSize, dstFormat, dstBuffer, dstSize );
				break;

			case PixelFormat::eR8G8B8_SRGB:
				PixelDefinitions< PixelFormat::eR8G8B8_SRGB >::convert( srcDimensions, dstDimensions, srcBuffer, srcSize, dstFormat, dstBuffer, dstSize );
				break;

			case PixelFormat::eB8G8R8_SRGB:
				PixelDefinitions< PixelFormat::eB8G8R8_SRGB >::convert( srcDimensions, dstDimensions, srcBuffer, srcSize, dstFormat, dstBuffer, dstSize );
				break;

			case PixelFormat::eR8G8B8A8_UNORM:
				PixelDefinitions< PixelFormat::eR8G8B8A8_UNORM >::convert( srcDimensions, dstDimensions, srcBuffer, srcSize, dstFormat, dstBuffer, dstSize );
				break;

			case PixelFormat::eA8B8G8R8_UNORM:
				PixelDefinitions< PixelFormat::eA8B8G8R8_UNORM >::convert( srcDimensions, dstDimensions, srcBuffer, srcSize, dstFormat, dstBuffer, dstSize );
				break;

			case PixelFormat::eR8G8B8A8_SRGB:
				PixelDefinitions< PixelFormat::eR8G8B8A8_SRGB >::convert( srcDimensions, dstDimensions, srcBuffer, srcSize, dstFormat, dstBuffer, dstSize );
				break;

			case PixelFormat::eA8B8G8R8_SRGB:
				PixelDefinitions< PixelFormat::eA8B8G8R8_SRGB >::convert( srcDimensions, dstDimensions, srcBuffer, srcSize, dstFormat, dstBuffer, dstSize );
				break;

			case PixelFormat::eR16G16B16_SFLOAT:
				PixelDefinitions< PixelFormat::eR16G16B16_SFLOAT >::convert( srcDimensions, dstDimensions, srcBuffer, srcSize, dstFormat, dstBuffer, dstSize );
				break;

			case PixelFormat::eR16G16B16A16_SFLOAT:
				PixelDefinitions< PixelFormat::eR16G16B16A16_SFLOAT >::convert( srcDimensions, dstDimensions, srcBuffer, srcSize, dstFormat, dstBuffer, dstSize );
				break;

			case PixelFormat::eR32G32B32_SFLOAT:
				PixelDefinitions< PixelFormat::eR32G32B32_SFLOAT >::convert( srcDimensions, dstDimensions, srcBuffer, srcSize, dstFormat, dstBuffer, dstSize );
				break;

			case PixelFormat::eR32G32B32A32_SFLOAT:
				PixelDefinitions< PixelFormat::eR32G32B32A32_SFLOAT >::convert( srcDimensions, dstDimensions, srcBuffer, srcSize, dstFormat, dstBuffer, dstSize );
				break;

			case PixelFormat::eBC1_RGB_UNORM_BLOCK:
				PixelDefinitions< PixelFormat::eBC1_RGB_UNORM_BLOCK >::convert( srcDimensions, dstDimensions, srcBuffer, srcSize, dstFormat, dstBuffer, dstSize );
				break;

			case PixelFormat::eBC1_RGBA_UNORM_BLOCK:
				PixelDefinitions< PixelFormat::eBC1_RGBA_UNORM_BLOCK >::convert( srcDimensions, dstDimensions, srcBuffer, srcSize, dstFormat, dstBuffer, dstSize );
				break;

			case PixelFormat::eBC3_UNORM_BLOCK:
				PixelDefinitions< PixelFormat::eBC3_UNORM_BLOCK >::convert( srcDimensions, dstDimensions, srcBuffer, srcSize, dstFormat, dstBuffer, dstSize );
				break;

			case PixelFormat::eBC5_UNORM_BLOCK:
				PixelDefinitions< PixelFormat::eBC5_UNORM_BLOCK >::convert( srcDimensions, dstDimensions, srcBuffer, srcSize, dstFormat, dstBuffer, dstSize );
				break;

			case PixelFormat::eD16_UNORM:
				PixelDefinitions< PixelFormat::eD16_UNORM >::convert( srcDimensions, dstDimensions, srcBuffer, srcSize, dstFormat, dstBuffer, dstSize );
				break;

			case PixelFormat::eD24_UNORM_S8_UINT:
				PixelDefinitions< PixelFormat::eD24_UNORM_S8_UINT >::convert( srcDimensions, dstDimensions, srcBuffer, srcSize, dstFormat, dstBuffer, dstSize );
				break;

			case PixelFormat::eD32_UNORM:
				PixelDefinitions< PixelFormat::eD32_UNORM >::convert( srcDimensions, dstDimensions, srcBuffer, srcSize, dstFormat, dstBuffer, dstSize );
				break;

			case PixelFormat::eD32_SFLOAT:
				PixelDefinitions< PixelFormat::eD32_SFLOAT >::convert( srcDimensions, dstDimensions, srcBuffer, srcSize, dstFormat, dstBuffer, dstSize );
				break;

			case PixelFormat::eD32_SFLOAT_S8_UINT:
				PixelDefinitions< PixelFormat::eD32_SFLOAT_S8_UINT >::convert( srcDimensions, dstDimensions, srcBuffer, srcSize, dstFormat, dstBuffer, dstSize );
				break;

			case PixelFormat::eS8_UINT:
				PixelDefinitions< PixelFormat::eS8_UINT >::convert( srcDimensions, dstDimensions, srcBuffer, srcSize, dstFormat, dstBuffer, dstSize );
				break;

			default:
				CU_Failure( "Unsupported pixel format" );
				break;
			}
		}

		PixelFormat getFormatByName( String const & formatName )
		{
			PixelFormat result = PixelFormat::eCount;

			for ( int i = 0u; i < int( result ) && result == PixelFormat::eCount; ++i )
			{
				switch ( PixelFormat( i ) )
				{
				case PixelFormat::eR8_UNORM:
					result = ( formatName == PixelDefinitions< PixelFormat::eR8_UNORM >::toStr() ? PixelFormat( i ) : PixelFormat::eCount );
					break;
				case PixelFormat::eR16_SFLOAT:
					result = ( formatName == PixelDefinitions< PixelFormat::eR16_SFLOAT >::toStr() ? PixelFormat( i ) : PixelFormat::eCount );
					break;
				case PixelFormat::eR32_SFLOAT:
					result = ( formatName == PixelDefinitions< PixelFormat::eR32_SFLOAT >::toStr() ? PixelFormat( i ) : PixelFormat::eCount );
					break;
				case PixelFormat::eR8A8_UNORM:
					result = ( formatName == PixelDefinitions< PixelFormat::eR8A8_UNORM >::toStr() ? PixelFormat( i ) : PixelFormat::eCount );
					break;
				case PixelFormat::eR16A16_SFLOAT:
					result = ( formatName == PixelDefinitions< PixelFormat::eR16_SFLOAT >::toStr() ? PixelFormat( i ) : PixelFormat::eCount );
					break;
				case PixelFormat::eR32A32_SFLOAT:
					result = ( formatName == PixelDefinitions< PixelFormat::eR32A32_SFLOAT >::toStr() ? PixelFormat( i ) : PixelFormat::eCount );
					break;
				case PixelFormat::eR5G5B5A1_UNORM:
					result = ( formatName == PixelDefinitions< PixelFormat::eR5G5B5A1_UNORM >::toStr() ? PixelFormat( i ) : PixelFormat::eCount );
					break;
				case PixelFormat::eR5G6B5_UNORM:
					result = ( formatName == PixelDefinitions< PixelFormat::eR5G6B5_UNORM >::toStr() ? PixelFormat( i ) : PixelFormat::eCount );
					break;
				case PixelFormat::eR8G8B8_UNORM:
					result = ( formatName == PixelDefinitions< PixelFormat::eR8G8B8_UNORM >::toStr() ? PixelFormat( i ) : PixelFormat::eCount );
					break;
				case PixelFormat::eB8G8R8_UNORM:
					result = ( formatName == PixelDefinitions< PixelFormat::eB8G8R8_UNORM >::toStr() ? PixelFormat( i ) : PixelFormat::eCount );
					break;
				case PixelFormat::eB8G8R8A8_UNORM:
					result = ( formatName == PixelDefinitions< PixelFormat::eB8G8R8A8_UNORM >::toStr() ? PixelFormat( i ) : PixelFormat::eCount );
					break;
				case PixelFormat::eR8G8B8_SRGB:
					result = ( formatName == PixelDefinitions< PixelFormat::eR8G8B8_SRGB >::toStr() ? PixelFormat( i ) : PixelFormat::eCount );
					break;
				case PixelFormat::eB8G8R8_SRGB:
					result = ( formatName == PixelDefinitions< PixelFormat::eB8G8R8_SRGB >::toStr() ? PixelFormat( i ) : PixelFormat::eCount );
					break;
				case PixelFormat::eR8G8B8A8_UNORM:
					result = ( formatName == PixelDefinitions< PixelFormat::eR8G8B8A8_UNORM >::toStr() ? PixelFormat( i ) : PixelFormat::eCount );
					break;
				case PixelFormat::eA8B8G8R8_UNORM:
					result = ( formatName == PixelDefinitions< PixelFormat::eA8B8G8R8_UNORM >::toStr() ? PixelFormat( i ) : PixelFormat::eCount );
					break;
				case PixelFormat::eR8G8B8A8_SRGB:
					result = ( formatName == PixelDefinitions< PixelFormat::eR8G8B8A8_SRGB >::toStr() ? PixelFormat( i ) : PixelFormat::eCount );
					break;
				case PixelFormat::eA8B8G8R8_SRGB:
					result = ( formatName == PixelDefinitions< PixelFormat::eA8B8G8R8_SRGB >::toStr() ? PixelFormat( i ) : PixelFormat::eCount );
					break;
				case PixelFormat::eR16G16B16_SFLOAT:
					result = ( formatName == PixelDefinitions< PixelFormat::eR16G16B16_SFLOAT >::toStr() ? PixelFormat( i ) : PixelFormat::eCount );
					break;
				case PixelFormat::eR16G16B16A16_SFLOAT:
					result = ( formatName == PixelDefinitions< PixelFormat::eR16G16B16A16_SFLOAT >::toStr() ? PixelFormat( i ) : PixelFormat::eCount );
					break;
				case PixelFormat::eR32G32B32_SFLOAT:
					result = ( formatName == PixelDefinitions< PixelFormat::eR32G32B32_SFLOAT >::toStr() ? PixelFormat( i ) : PixelFormat::eCount );
					break;
				case PixelFormat::eR32G32B32A32_SFLOAT:
					result = ( formatName == PixelDefinitions< PixelFormat::eR32G32B32A32_SFLOAT >::toStr() ? PixelFormat( i ) : PixelFormat::eCount );
					break;
				case PixelFormat::eBC1_RGB_UNORM_BLOCK:
					result = ( formatName == PixelDefinitions< PixelFormat::eBC1_RGB_UNORM_BLOCK >::toStr() ? PixelFormat( i ) : PixelFormat::eCount );
					break;
				case PixelFormat::eBC3_UNORM_BLOCK:
					result = ( formatName == PixelDefinitions< PixelFormat::eBC3_UNORM_BLOCK >::toStr() ? PixelFormat( i ) : PixelFormat::eCount );
					break;
				case PixelFormat::eBC5_UNORM_BLOCK:
					result = ( formatName == PixelDefinitions< PixelFormat::eBC5_UNORM_BLOCK >::toStr() ? PixelFormat( i ) : PixelFormat::eCount );
					break;
				case PixelFormat::eD16_UNORM:
					result = ( formatName == PixelDefinitions< PixelFormat::eD16_UNORM >::toStr() ? PixelFormat( i ) : PixelFormat::eCount );
					break;
				case PixelFormat::eD24_UNORM_S8_UINT:
					result = ( formatName == PixelDefinitions< PixelFormat::eD24_UNORM_S8_UINT >::toStr() ? PixelFormat( i ) : PixelFormat::eCount );
					break;
				case PixelFormat::eD32_UNORM:
					result = ( formatName == PixelDefinitions< PixelFormat::eD32_UNORM >::toStr() ? PixelFormat( i ) : PixelFormat::eCount );
					break;
				case PixelFormat::eD32_SFLOAT:
					result = ( formatName == PixelDefinitions< PixelFormat::eD32_SFLOAT >::toStr() ? PixelFormat( i ) : PixelFormat::eCount );
					break;
				case PixelFormat::eD32_SFLOAT_S8_UINT:
					result = ( formatName == PixelDefinitions< PixelFormat::eD32_SFLOAT_S8_UINT >::toStr() ? PixelFormat( i ) : PixelFormat::eCount );
					break;
				case PixelFormat::eS8_UINT:
					result = ( formatName == PixelDefinitions< PixelFormat::eS8_UINT >::toStr() ? PixelFormat( i ) : PixelFormat::eCount );
					break;
				default:
					break;
				}
			}

			if ( result == PixelFormat::eCount )
			{
				CU_Failure( "Unsupported pixel format" );
			}

			return result;
		}

		String getFormatName( PixelFormat format )
		{
			String result;

			switch ( format )
			{
			case PixelFormat::eR8_UNORM:
				result = PixelDefinitions< PixelFormat::eR8_UNORM >::toStr();
				break;
			case PixelFormat::eR16_SFLOAT:
				result = PixelDefinitions< PixelFormat::eR32_SFLOAT >::toStr();
				break;
			case PixelFormat::eR32_SFLOAT:
				result = PixelDefinitions< PixelFormat::eR32_SFLOAT >::toStr();
				break;
			case PixelFormat::eR8A8_UNORM:
				result = PixelDefinitions< PixelFormat::eR8A8_UNORM >::toStr();
				break;
			case PixelFormat::eR32A32_SFLOAT:
				result = PixelDefinitions< PixelFormat::eR32A32_SFLOAT >::toStr();
				break;
			case PixelFormat::eR5G5B5A1_UNORM:
				result = PixelDefinitions< PixelFormat::eR5G5B5A1_UNORM >::toStr();
				break;
			case PixelFormat::eR5G6B5_UNORM:
				result = PixelDefinitions< PixelFormat::eR5G6B5_UNORM >::toStr();
				break;
			case PixelFormat::eR8G8B8_UNORM:
				result = PixelDefinitions< PixelFormat::eR8G8B8_UNORM >::toStr();
				break;
			case PixelFormat::eB8G8R8_UNORM:
				result = PixelDefinitions< PixelFormat::eB8G8R8_UNORM >::toStr();
				break;
			case PixelFormat::eB8G8R8A8_UNORM:
				result = PixelDefinitions< PixelFormat::eB8G8R8A8_UNORM >::toStr();
				break;
			case PixelFormat::eR8G8B8_SRGB:
				result = PixelDefinitions< PixelFormat::eR8G8B8_SRGB >::toStr();
				break;
			case PixelFormat::eB8G8R8_SRGB:
				result = PixelDefinitions< PixelFormat::eB8G8R8_SRGB >::toStr();
				break;
			case PixelFormat::eR8G8B8A8_UNORM:
				result = PixelDefinitions< PixelFormat::eR8G8B8A8_UNORM >::toStr();
				break;
			case PixelFormat::eA8B8G8R8_UNORM:
				result = PixelDefinitions< PixelFormat::eA8B8G8R8_UNORM >::toStr();
				break;
			case PixelFormat::eR8G8B8A8_SRGB:
				result = PixelDefinitions< PixelFormat::eR8G8B8A8_SRGB >::toStr();
				break;
			case PixelFormat::eA8B8G8R8_SRGB:
				result = PixelDefinitions< PixelFormat::eA8B8G8R8_SRGB >::toStr();
				break;
			case PixelFormat::eR16G16B16_SFLOAT:
				result = PixelDefinitions< PixelFormat::eR16G16B16_SFLOAT >::toStr();
				break;
			case PixelFormat::eR16G16B16A16_SFLOAT:
				result = PixelDefinitions< PixelFormat::eR16G16B16A16_SFLOAT >::toStr();
				break;
			case PixelFormat::eR32G32B32_SFLOAT:
				result = PixelDefinitions< PixelFormat::eR32G32B32_SFLOAT >::toStr();
				break;
			case PixelFormat::eR32G32B32A32_SFLOAT:
				result = PixelDefinitions< PixelFormat::eR32G32B32A32_SFLOAT >::toStr();
				break;
			case PixelFormat::eBC1_RGB_UNORM_BLOCK:
				result = PixelDefinitions< PixelFormat::eBC1_RGB_UNORM_BLOCK >::toStr();
				break;
			case PixelFormat::eBC3_UNORM_BLOCK:
				result = PixelDefinitions< PixelFormat::eBC3_UNORM_BLOCK >::toStr();
				break;
			case PixelFormat::eBC5_UNORM_BLOCK:
				result = PixelDefinitions< PixelFormat::eBC5_UNORM_BLOCK >::toStr();
				break;
			case PixelFormat::eD16_UNORM:
				result = PixelDefinitions< PixelFormat::eD16_UNORM >::toStr();
				break;
			case PixelFormat::eD24_UNORM_S8_UINT:
				result = PixelDefinitions< PixelFormat::eD24_UNORM_S8_UINT >::toStr();
				break;
			case PixelFormat::eD32_UNORM:
				result = PixelDefinitions< PixelFormat::eD32_UNORM >::toStr();
				break;
			case PixelFormat::eD32_SFLOAT:
				result = PixelDefinitions< PixelFormat::eD32_SFLOAT >::toStr();
				break;
			case PixelFormat::eD32_SFLOAT_S8_UINT:
				result = PixelDefinitions< PixelFormat::eD32_SFLOAT_S8_UINT >::toStr();
				break;
			case PixelFormat::eS8_UINT:
				result = PixelDefinitions< PixelFormat::eS8_UINT >::toStr();
				break;
			default:
				CU_Failure( "Unsupported pixel format" );
				break;
			}

			return result;
		}

		PixelFormat getCompressed( PixelFormat format )
		{
			PixelFormat result;

			switch ( format )
			{
			case PixelFormat::eR8G8B8_UNORM:
			case PixelFormat::eB8G8R8_UNORM:
				result = PixelFormat::eBC1_RGB_UNORM_BLOCK;
				break;
			case PixelFormat::eB8G8R8A8_UNORM:
			case PixelFormat::eR8G8B8A8_UNORM:
			case PixelFormat::eA8B8G8R8_UNORM:
				result = PixelFormat::eBC3_UNORM_BLOCK;
				break;
			case PixelFormat::eR8G8B8_SRGB:
			case PixelFormat::eB8G8R8_SRGB:
				result = PixelFormat::eBC1_RGB_SRGB_BLOCK;
				break;
			case PixelFormat::eR8G8B8A8_SRGB:
			case PixelFormat::eA8B8G8R8_SRGB:
				result = PixelFormat::eBC3_SRGB_BLOCK;
				break;
			default:
				result = format;
				break;
			}

			return result;
		}

		//*****************************************************************************************
	}
}
