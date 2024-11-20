#include "CastorUtils/Graphics/Ktx2ImageLoader.hpp"

#include "CastorUtils/Data/LoaderException.hpp"
#include "CastorUtils/Data/Path.hpp"
#include "CastorUtils/Graphics/PixelBuffer.hpp"

#include "CastorUtils/Config/BeginExternHeaderGuard.hpp"
#include <ktx.h>
#include "CastorUtils/Config/EndExternHeaderGuard.hpp"

namespace castor
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
			, castor::make_unique< Ktx2ImageLoader >() );
	}

	void Ktx2ImageLoader::unregisterLoader( ImageLoader & reg )
	{
		reg.unregisterLoader( ktx2::listExtensions() );
	}

	ImageLayout Ktx2ImageLoader::load( String const & imageFormat
		, uint8_t const * data
		, uint32_t size
		, PxBufferBaseUPtr & buffer )const
	{
		ktxTexture * texture{};

		if ( ktx_error_code_e err = ktxTexture_CreateFromMemory( data, size, KTX_TEXTURE_CREATE_LOAD_IMAGE_DATA_BIT, &texture );
			err != KTX_SUCCESS )
		{
			CU_LoaderError( ktxErrorString( err ) );
		}

		if ( texture->classId != ktxTexture2_c )
		{
			CU_LoaderError( "Not a KTX2 texture" );
		}

		auto texture2 = reinterpret_cast< ktxTexture2 * >( texture );
		auto format = PixelFormat( PixelFormat( texture2->vkFormat ) );

		if ( format == PixelFormat::eUNDEFINED )
		{
			CU_LoaderError( "Undefined pixel format" );
		}

		ImageLayout result;
		result.type = texture->baseDepth > 1u
			? ImageLayout::Type::e3D
			: ( texture->isCubemap
				? ImageLayout::Type::eCube
				: ( texture->isArray
					? ImageLayout::Type::e2DArray
					: ImageLayout::Type::e2D ) );
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
		ktxTexture_Destroy( texture );

		return result;
	}
}
