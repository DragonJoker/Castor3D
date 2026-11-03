#include "CastorUtils/Graphics/AvifImageLoader.hpp"

#include "CastorUtils/Data/LoaderException.hpp"
#include "CastorUtils/Graphics/PixelBufferBase.hpp"

#include "CastorUtils/Config/BeginExternHeaderGuard.hpp"
#include <libheif/heif_decoding.h>
#include "CastorUtils/Config/EndExternHeaderGuard.hpp"

namespace c3d
{
	//************************************************************************************************

	namespace avif
	{
		static StringArray const AvifExtensions
		{
			cuT( "avif" ),
		};

		static StringArray const & listExtensions()
		{
			return AvifExtensions;
		}
	}

	//************************************************************************************************

	void AvifImageLoader::registerLoader( ImageLoader & reg )
	{
		reg.registerLoader( avif::listExtensions()
			, c3d::makeRawUnique< AvifImageLoader >() );
	}

	void AvifImageLoader::unregisterLoader( ImageLoader & reg )
	{
		reg.unregisterLoader( avif::listExtensions() );
	}

	ImageMemoryLayout AvifImageLoader::doLoad( String const & imageFormat
		, uint8_t const * data
		, uint32_t size
		, PxBufferBaseUPtr & buffer )const
	{
		heif_context * context = heif_context_alloc();
		if ( !context )
			CU_LoaderError( "Can't initialise HEIF context" );

		heif_error err = heif_context_read_from_memory_without_copy( context, data, size, nullptr );
		if ( err.code != heif_error_Ok )
			CU_LoaderError( "Can't read HEIF file: " + makeString( err.message ) );

		heif_image_handle * handle{};
		err = heif_context_get_primary_image_handle( context, &handle );
		if ( err.code != heif_error_Ok )
			CU_LoaderError( "Can't get HEIF image handle: " + makeString( err.message ) );

		heif_image * decodedImage{};
		err = heif_decode_image( handle, &decodedImage, heif_colorspace_RGB, heif_chroma_interleaved_RGBA, nullptr );
		if ( err.code != heif_error_Ok )
			CU_LoaderError( "Can't decode HEIF image: " + makeString( err.message ) );

		auto format = PixelFormat::eR8G8B8A8_SRGB;
		int width = heif_image_get_primary_width( decodedImage );
		int height = heif_image_get_primary_height( decodedImage );
		u32 bpp = u32( getBytesPerPixel( format ) );

		ImageMemoryLayout result;
		result.type = ImageViewType::e2D;
		result.format = format;
		result.extent = { u32( width ), u32( height ), 1u };
		result.layers = 1u;
		result.levels = 1u;
		result.alignment = bpp;
		buffer = PxBufferBase::create( result.dimensions()
			, result.layers
			, result.levels
			, result.format );
		buffer->invertY();

		u32 lineWidth = u32( width ) * bpp;
		int stride{};
		const u8 * src = heif_image_get_plane_readonly( decodedImage, heif_channel_interleaved, &stride );
		u8 * dst = buffer->getPtr();

		for ( u32 y = 0u; y < u32( height ); ++y )
		{
			memcpy( dst, src, lineWidth );
			dst += lineWidth;
			src += stride;
		}

		heif_image_release( decodedImage );
		heif_context_free( context );

		return result;
	}
}
