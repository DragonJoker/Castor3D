#include "CastorUtils/Graphics/GliImageLoader.hpp"

#include "CastorUtils/Data/LoaderException.hpp"
#include "CastorUtils/Data/Path.hpp"
#include "CastorUtils/Graphics/PixelBuffer.hpp"

#include "CastorUtils/Config/BeginExternHeaderGuard.hpp"
#include <gli/gli.hpp>
#include "CastorUtils/Config/EndExternHeaderGuard.hpp"

namespace castor
{
	//************************************************************************************************

	namespace glil
	{
		static PixelFormat convert( gli::format format )
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
			default:
				CU_Exception( "Unsupported gli::format" );
			}
		}

		static ImageLayout::Type convert( gli::target target )
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

		static StringArray const GliExtensions
		{
			cuT( "dds" ),
			cuT( "kmg" ),
			cuT( "ktx" ),
		};

		static StringArray const & listExtensions()
		{
			return GliExtensions;
		}
	}

	//************************************************************************************************

	void GliImageLoader::registerLoader( ImageLoader & reg )
	{
		reg.registerLoader( glil::listExtensions()
			, castor::make_unique< GliImageLoader >() );
	}

	void GliImageLoader::unregisterLoader( ImageLoader & reg )
	{
		reg.unregisterLoader( glil::listExtensions() );
	}

	ImageLayout GliImageLoader::load( String const & imageFormat
		, uint8_t const * data
		, uint32_t size
		, PxBufferBaseUPtr & buffer )const
	{
		using CharCPtr = char const *;
		gli::texture texture;
		bool flipped = false;

		if ( imageFormat.find( cuT( "dds" ) ) != String::npos )
		{
			texture = gli::load_dds( CharCPtr( data ), size );
			flipped = true;
		}
		else if ( imageFormat.find( cuT( "kmg" ) ) != String::npos )
		{
			texture = gli::load_kmg( CharCPtr( data ), size );
		}
		else if ( imageFormat.find( cuT( "ktx" ) ) != String::npos )
		{
			texture = gli::load_ktx( CharCPtr( data ), size );
		}

		if ( texture.empty() )
		{
			CU_LoaderError( "Can't load image: Failed to read data" );
		}

		ImageLayout result;
		result.type = glil::convert( texture.target() );
		result.format = glil::convert( texture.format() );
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
