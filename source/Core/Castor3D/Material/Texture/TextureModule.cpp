#include "Castor3D/Material/Texture/TextureModule.hpp"

#include "Castor3D/Engine.hpp"
#include "Castor3D/Material/Texture/Sampler.hpp"
#include "Castor3D/Material/Texture/TextureLayout.hpp"

CU_ImplementSmartPtr( c3d, SamplerCache )

namespace c3d
{
	//*********************************************************************************************

	namespace texmod
	{
		static Image & getImage( Engine & engine
			, String const & name
			, ImageCreateParams createParams )
		{
			auto image = engine.tryFindImage( name );

			if ( !image )
			{
				image = engine.addNewImage( name, createParams );
			}

			if ( !image )
			{
				CU_LoaderError( "Couldn't load image." );
			}

			if ( !image->hasBuffer() )
			{
				**image = **engine.createImage( name, createParams );

				if ( image )
				{
					log::info << "Loaded image [" << name << "] (" << *image << ")" << std::endl;
				}
			}

			if ( !image->hasBuffer() )
			{
				CU_LoaderError( "Couldn't load image." );
			}

			return *image;
		}
	}

	//*********************************************************************************************

	const String PtrCacheTraitsT< Sampler, String >::Name = cuT( "Sampler" );

	//*********************************************************************************************

	String getName( BorderColour v )
	{
		return ashes::getName( convert( v ) );
	}

	VkBorderColor convert( BorderColour v )
	{
		return VkBorderColor( v );
	}

	BorderColour convert( VkBorderColor v )
	{
		return BorderColour( v );
	}

	//*********************************************************************************************

	String getName( TextureSpace value )
	{
		switch ( value )
		{
		case TextureSpace::eNormalised:
			return cuT( "Normalised" );
		case TextureSpace::eYInverted:
			return cuT( "YInverted" );
		case TextureSpace::eColour:
			return cuT( "Colour" );
		case TextureSpace::eDepth:
			return cuT( "Depth" );
		case TextureSpace::eTangentSpace:
			return cuT( "TangentSpace" );
		case TextureSpace::eObjectSpace:
			return cuT( "ObjectSpace" );
		case TextureSpace::eWorldSpace:
			return cuT( "WorldSpace" );
		case TextureSpace::eViewSpace:
			return cuT( "ViewSpace" );
		case TextureSpace::eClipSpace:
			return cuT( "ClipSpace" );
		case TextureSpace::eStencil:
			return cuT( "Stencil" );
		default:
			CU_Failure( "Unsupported TextureSpace" );
			return cuEmptyString;
		}
	}

	String getName( TextureSpaces values )
	{
		String result;
		String sep;

		for ( auto value : values )
		{
			result += sep + getName( value );
			sep = cuT( "|" );
		}

		return result;
	}

	//*********************************************************************************************

	String getName( CubeMapFace value )
	{
		switch ( value )
		{
		case CubeMapFace::ePositiveX:
			return cuT( "PositiveX" );
		case CubeMapFace::eNegativeX:
			return cuT( "NegativeX" );
		case CubeMapFace::ePositiveY:
			return cuT( "PositiveY" );
		case CubeMapFace::eNegativeY:
			return cuT( "NegativeY" );
		case CubeMapFace::ePositiveZ:
			return cuT( "PositiveZ" );
		case CubeMapFace::eNegativeZ:
			return cuT( "NegativeZ" );
		default:
			CU_Failure( "Unsupported CubeMapFace" );
			return cuEmptyString;
		}
	}

	//*********************************************************************************************

	Image & getBufferImage( Engine & engine
		, String const & name
		, String const & type
		, ByteArray const & data )
	{
		return texmod::getImage( engine
			, name
			, ImageCreateParams{ type
				, data
				, { false, false, false } } );
	}

	Image & getFileImage( Engine & engine
		, String const & name
		, Path const & folder
		, Path const & relative )
	{
		return texmod::getImage( engine
			, name
			, ImageCreateParams{ folder / relative
				, { false, false, false } } );
	}

	//*********************************************************************************************

	TextureLayoutUPtr createTextureLayout( Engine const & engine
		, Path const & relative
		, Path const & folder )
	{
		ImageCreateInfo createInfo{ ImageCreateFlags::eNone
			, ImageType::e2D
			, PixelFormat::eUNDEFINED
			, { 1u, 1u, 1u }
			, 20u
			, 1u
			, SampleCount::e1
			, ImageTiling::eOptimal
			, ImageUsageFlags::eSampled | ImageUsageFlags::eTransferDst };
		auto texture = makeUnique< TextureLayout >( *engine.getRenderSystem()
			, createInfo
			, MemoryPropertyFlags::eDeviceLocal
			, relative );
		texture->setSource( folder, relative );
		return texture;
	}

	TextureLayoutUPtr createTextureLayout( Engine const & engine
		, String const & name
		, PxBufferBaseUPtr buffer
		, bool isStatic )
	{
		ImageCreateInfo createInfo{ ImageCreateFlags::eNone
			, ( buffer->getHeight() <= 1u && buffer->getWidth() > 1u
				? ImageType::e1D
				: ImageType::e2D )
			, PixelFormat::eUNDEFINED
			, { buffer->getWidth(), buffer->getHeight(), 1u }
			, uint32_t( getBitSize( std::min( buffer->getWidth(), buffer->getHeight() ) ) )
			, 1u// TODO: Support array layers: buffer->getLayers()
			, SampleCount::e1
			, ImageTiling::eOptimal
			, ImageUsageFlags::eSampled | ImageUsageFlags::eTransferDst };
		log::debug << ( cuT( "Creating " ) + name + cuT( " texture layout.\n" ) );
		auto texture = makeUnique< TextureLayout >( *engine.getRenderSystem()
			, createInfo
			, MemoryPropertyFlags::eDeviceLocal
			, name );
		texture->setSource( c3d::move( buffer ), isStatic );
		return texture;
	}

	//*********************************************************************************************

	uint32_t getMipLevels( Extent3D const & extent
		, PixelFormat format )
	{
		auto blockSize = ashes::getBlockSize( convert( format ) );
		auto min = std::min( extent.width / blockSize.extent.width, extent.height / blockSize.extent.height );
		return uint32_t( getBitSize( min ) );
	}

	//*********************************************************************************************
}
