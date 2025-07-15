#include "CastorUtils/Graphics/Ktx2ImageLoader.hpp"

#include "CastorUtils/Data/LoaderException.hpp"
#include "CastorUtils/Data/Path.hpp"
#include "CastorUtils/Design/BlockGuard.hpp"
#include "CastorUtils/Graphics/PixelBuffer.hpp"

#include "CastorUtils/Config/BeginExternHeaderGuard.hpp"
#include <ktx.h>
#include "CastorUtils/Config/EndExternHeaderGuard.hpp"

namespace c3d
{
	//************************************************************************************************

	namespace ktx2
	{
		static StringArray const Ktx2Extensions
		{
			cuT( "ktx2" ),
		};

		static StringArray const & listExtensions()
		{
			return Ktx2Extensions;
		}
	}

	//************************************************************************************************

	void Ktx2ImageLoader::registerLoader( ImageLoader & reg )
	{
		reg.registerLoader( ktx2::listExtensions()
			, c3d::makeRawUnique< Ktx2ImageLoader >() );
	}

	void Ktx2ImageLoader::unregisterLoader( ImageLoader & reg )
	{
		reg.unregisterLoader( ktx2::listExtensions() );
	}

	ImageMemoryLayout Ktx2ImageLoader::load( String const & imageFormat
		, uint8_t const * data
		, uint32_t size
		, PxBufferBaseUPtr & buffer )const
	{
		ktxTexture * texture{};
		ktx_error_code_e err = ktxTexture_CreateFromMemory( data, size, KTX_TEXTURE_CREATE_LOAD_IMAGE_DATA_BIT, &texture );

		if ( err != KTX_SUCCESS || !texture )
		{
			CU_LoaderError( ktxErrorString( err ) );
		}

		auto guard = makeBlockGuard( [texture](){ ktxTexture_Destroy( texture ); } );

		if ( texture->classId != ktxTexture2_c )
		{
			CU_LoaderError( "Not a KTX2 texture" );
		}

		auto texture2 = reinterpret_cast< ktxTexture2 * >( texture );

		if ( ktxTexture2_NeedsTranscoding( texture2 ) )
		{
			err = ktxTexture2_TranscodeBasis( texture2, KTX_TTF_RGBA32, 0 );

			if ( err != KTX_SUCCESS )
			{
				CU_LoaderError( ktxErrorString( err ) );
			}
		}

		auto format = PixelFormat( PixelFormat( texture2->vkFormat ) );

		if ( format == PixelFormat::eUNDEFINED )
		{
			CU_LoaderError( "Undefined pixel format" );
		}

		ImageMemoryLayout result;
		result.type = texture->baseDepth > 1u
			? ImageViewType::e3D
			: ( texture->isCubemap
				? ImageViewType::eCube
				: ( texture->isArray
					? ImageViewType::e2DArray
					: ImageViewType::e2D ) );
		result.format = format;
		result.extent = { texture->baseWidth, texture->baseHeight, texture->baseDepth };
		result.layers = texture->numLayers;
		result.levels = texture->numLevels;
		result.alignment = uint32_t( getBytesPerPixel( result.format ) );
		buffer = PxBufferBase::create( result.dimensions()
			, result.layers
			, result.levels
			, result.format
			, ktxTexture_GetData( texture )
			, result.format );

		if ( texture->orientation.x == KTX_ORIENT_X_LEFT )
		{
			buffer->invertX();
		}

		if ( texture->orientation.y == KTX_ORIENT_Y_DOWN )
		{
			buffer->invertY();
		}

		if ( texture->orientation.z == KTX_ORIENT_Z_IN )
		{
			buffer->invertZ();
		}

		return result;
	}
}
