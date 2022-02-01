#include "CastorUtils/Graphics/GliImageLoader.hpp"

#include "CastorUtils/Data/LoaderException.hpp"
#include "CastorUtils/Data/Path.hpp"
#include "CastorUtils/Graphics/PixelBuffer.hpp"

#pragma warning( push )
#pragma warning( disable: 4100 )
#pragma warning( disable: 4189 )
#pragma warning( disable: 4201 )
#pragma warning( disable: 4242 )
#pragma warning( disable: 4244 )
#pragma warning( disable: 4296 )
#pragma warning( disable: 4365 )
#pragma warning( disable: 4458 )
#pragma warning( disable: 4464 )
#pragma warning( disable: 5054 )
#pragma warning( disable: 5214 )
#pragma warning( disable: 5219 )
#include <gli/gli.hpp>
#pragma warning( pop )

namespace castor
{
	//************************************************************************************************

	namespace
	{
		PixelFormat convert( gli::format format )
		{
			if ( format < uint32_t( PixelFormat::eCount ) )
			{
				return PixelFormat( format );
			}

			switch ( format )
			{
			case gli::FORMAT_L8_UNORM_PACK8:
			case gli::FORMAT_A8_UNORM_PACK8:
				return PixelFormat::eR8_UNORM;
			case gli::FORMAT_L16_UNORM_PACK16:
			case gli::FORMAT_A16_UNORM_PACK16:
				return PixelFormat::eR16_UNORM;
			case gli::FORMAT_LA16_UNORM_PACK16:
				return PixelFormat::eR8G8_UNORM;
			case gli::FORMAT_BGR8_UNORM_PACK32:
				return PixelFormat::eB8G8R8A8_UNORM;
			case gli::FORMAT_BGR8_SRGB_PACK32:
				return PixelFormat::eB8G8R8A8_SRGB;
			case gli::FORMAT_RGB_PVRTC1_8X8_UNORM_BLOCK32:
			case gli::FORMAT_RGB_PVRTC1_8X8_SRGB_BLOCK32:
			case gli::FORMAT_RGB_PVRTC1_16X8_UNORM_BLOCK32:
			case gli::FORMAT_RGB_PVRTC1_16X8_SRGB_BLOCK32:
			case gli::FORMAT_RGBA_PVRTC1_8X8_UNORM_BLOCK32:
			case gli::FORMAT_RGBA_PVRTC1_8X8_SRGB_BLOCK32:
			case gli::FORMAT_RGBA_PVRTC1_16X8_UNORM_BLOCK32:
			case gli::FORMAT_RGBA_PVRTC1_16X8_SRGB_BLOCK32:
			case gli::FORMAT_RGBA_PVRTC2_4X4_UNORM_BLOCK8:
			case gli::FORMAT_RGBA_PVRTC2_4X4_SRGB_BLOCK8:
			case gli::FORMAT_RGBA_PVRTC2_8X4_UNORM_BLOCK8:
			case gli::FORMAT_RGBA_PVRTC2_8X4_SRGB_BLOCK8:
			case gli::FORMAT_RGB_ETC_UNORM_BLOCK8:
			case gli::FORMAT_RGB_ATC_UNORM_BLOCK8:
			case gli::FORMAT_RGBA_ATCA_UNORM_BLOCK16:
			case gli::FORMAT_RGBA_ATCI_UNORM_BLOCK16:
			case gli::FORMAT_LA8_UNORM_PACK8:
			case gli::FORMAT_RG3B2_UNORM_PACK8:
			default:
				CU_Exception( "Unsupported gli::format" );
			}
		}

		ImageLayout::Type convert( gli::target target )
		{
			switch ( target )
			{
			case gli::TARGET_1D:
				return ImageLayout::e1D;
			case gli::TARGET_1D_ARRAY:
				return ImageLayout::e1DArray;
			case gli::TARGET_2D:
				return ImageLayout::e2D;
			case gli::TARGET_2D_ARRAY:
				return ImageLayout::e2DArray;
			case gli::TARGET_3D:
				return ImageLayout::e3D;
			case gli::TARGET_CUBE:
				return ImageLayout::eCube;
			case gli::TARGET_CUBE_ARRAY:
				return ImageLayout::eCubeArray;
			default:
				CU_Failure( "Unsupported target type" );
				return ImageLayout::e2D;
			}
		}

		StringArray const & listExtensions()
		{
			static StringArray const list
			{
				cuT( "dds" ),
				cuT( "kmg" ),
				cuT( "ktx" ),
			};
			return list;
		}
	}

	//************************************************************************************************

	void GliImageLoader::registerLoader( ImageLoader & reg )
	{
		reg.registerLoader( listExtensions()
			, std::make_unique< GliImageLoader >() );
	}

	void GliImageLoader::unregisterLoader( ImageLoader & reg )
	{
		reg.unregisterLoader( listExtensions() );
	}

	ImageLayout GliImageLoader::load( String const & imageFormat
		, uint8_t const * data
		, uint32_t size
		, PxBufferBaseSPtr & buffer )const
	{
		gli::texture texture;
		bool flipped = false;

		if ( imageFormat.find( cuT( "dds" ) ) != String::npos )
		{
			texture = gli::load_dds( reinterpret_cast< char const * >( data ), size );
			flipped = true;
		}
		else if ( imageFormat.find( cuT( "kmg" ) ) != String::npos )
		{
			texture = gli::load_kmg( reinterpret_cast< char const * >( data ), size );
		}
		else if ( imageFormat.find( cuT( "ktx" ) ) != String::npos )
		{
			texture = gli::load_ktx( reinterpret_cast< char const * >( data ), size );
		}

		if ( texture.empty() )
		{
			CU_LoaderError( "Can't load image: Failed to read data" );
		}

		ImageLayout result;
		result.type = convert( texture.target() );
		result.format = convert( texture.format() );
		result.extent = { texture.extent().x, texture.extent().y, texture.extent().z };
		result.layers = uint32_t( texture.layers() );
		result.levels = uint32_t( texture.levels() );
		result.alignment = uint32_t( getBytesPerPixel( result.format ) );
		buffer = PxBufferBase::create( result.dimensions()
			, result.layers
			, result.levels
			, result.format
			, static_cast< uint8_t const * >( texture.data() )
			, result.format
			, result.alignment );

		if ( flipped )
		{
			buffer->flip();
		}

		return result;
	}
}
