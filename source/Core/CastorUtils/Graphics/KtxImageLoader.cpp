#include "CastorUtils/Graphics/KtxImageLoader.hpp"

#include "CastorUtils/Data/LoaderException.hpp"
#include "CastorUtils/Data/Path.hpp"
#include "CastorUtils/Graphics/PixelBuffer.hpp"

#include <gli/gli.hpp>

namespace castor
{
	//************************************************************************************************

	namespace
	{
		PixelFormat convert( gli::format format )
		{
			return PixelFormat( format );
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

	void KtxImageLoader::registerLoader( ImageLoader & reg )
	{
		reg.registerLoader( listExtensions()
			, std::make_unique< KtxImageLoader >() );
	}

	void KtxImageLoader::unregisterLoader( ImageLoader & reg )
	{
		reg.unregisterLoader( listExtensions() );
	}

	ImageLayout KtxImageLoader::load( String const & imageFormat
		, uint8_t const * data
		, uint32_t size
		, PxBufferBaseSPtr & buffer )const
	{
		gli::texture texture;

		if ( string::upperCase( imageFormat ).find( cuT( "DDS" ) ) != String::npos )
		{
			texture = gli::load_dds( reinterpret_cast< char const * >( data ), size );
		}
		else if ( string::upperCase( imageFormat ).find( cuT( "KMG" ) ) != String::npos )
		{
			texture = gli::load_kmg( reinterpret_cast< char const * >( data ), size );
		}
		else if ( string::upperCase( imageFormat ).find( cuT( "KTX" ) ) != String::npos )
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
		buffer = PxBufferBase::create( result.dimensions()
			, result.layers
			, result.levels
			, result.format
			, static_cast< uint8_t const * >( texture.data() )
			, result.format );
		return result;
	}
}
