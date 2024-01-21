#include "Castor3D/Material/Texture/TextureModule.hpp"

#include "Castor3D/Engine.hpp"
#include "Castor3D/Material/Texture/Sampler.hpp"
#include "Castor3D/Material/Texture/TextureLayout.hpp"
#include "Castor3D/Render/RenderSystem.hpp"

CU_ImplementSmartPtr( castor3d, SamplerCache )

namespace castor3d
{
	namespace texmod
	{
		static castor::Image & getImage( Engine & engine
			, castor::String const & name
			, castor::ImageCreateParams createParams )
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
				**image = *engine.createImage( name, createParams );

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

	const castor::String PtrCacheTraitsT< castor3d::Sampler, castor::String >::Name = cuT( "Sampler" );

	castor::String getName( TextureSpace value )
	{
		switch ( value )
		{
		case castor3d::TextureSpace::eNormalised:
			return cuT( "Normalised" );
		case castor3d::TextureSpace::eYInverted:
			return cuT( "YInverted" );
		case castor3d::TextureSpace::eColour:
			return cuT( "Colour" );
		case castor3d::TextureSpace::eDepth:
			return cuT( "Depth" );
		case castor3d::TextureSpace::eTangentSpace:
			return cuT( "TangentSpace" );
		case castor3d::TextureSpace::eObjectSpace:
			return cuT( "ObjectSpace" );
		case castor3d::TextureSpace::eWorldSpace:
			return cuT( "WorldSpace" );
		case castor3d::TextureSpace::eViewSpace:
			return cuT( "ViewSpace" );
		case castor3d::TextureSpace::eClipSpace:
			return cuT( "ClipSpace" );
		case castor3d::TextureSpace::eStencil:
			return cuT( "Stencil" );
		default:
			CU_Failure( "Unsupported TextureSpace" );
			return castor::cuEmptyString;
		}
	}

	castor::String getName( TextureSpaces values )
	{
		castor::String result;
		castor::String sep;

		for ( auto value : values )
		{
			result += sep + getName( value );
			sep = "|";
		}

		return result;
	}

	castor::String getName( CubeMapFace value )
	{
		switch ( value )
		{
		case castor3d::CubeMapFace::ePositiveX:
			return cuT( "PositiveX" );
		case castor3d::CubeMapFace::eNegativeX:
			return cuT( "NegativeX" );
		case castor3d::CubeMapFace::ePositiveY:
			return cuT( "PositiveY" );
		case castor3d::CubeMapFace::eNegativeY:
			return cuT( "NegativeY" );
		case castor3d::CubeMapFace::ePositiveZ:
			return cuT( "PositiveZ" );
		case castor3d::CubeMapFace::eNegativeZ:
			return cuT( "NegativeZ" );
		default:
			CU_Failure( "Unsupported CubeMapFace" );
			return castor::cuEmptyString;
		}
	}

	std::ostream & operator<<( std::ostream & stream, castor::ImageLayout const & rhs )
	{
		stream << castor::ImageLayout::getName( rhs.type )
			<< ", " << ashes::getName( convert( rhs.format ) )
			<< ", " << rhs.extent->x
			<< "x" << rhs.extent->y;

		if ( rhs.extent->z > 1 )
		{
			stream << ", " << rhs.extent << " slices";
		}
		else
		{
			stream << ", " << rhs.layers << " layers";
		}

		stream << ", " << rhs.levels << " miplevels";
		return stream;
	}

	std::ostream & operator<<( std::ostream & stream, castor::Image const & rhs )
	{
		stream << rhs.getLayout();
		return stream;
	}

	std::ostream & operator<<( std::ostream & stream, TextureLayout const & rhs )
	{
		stream << rhs.getImage();
		return stream;
	}

	castor::Image & getBufferImage( Engine & engine
		, castor::String const & name
		, castor::String const & type
		, castor::ByteArray const & data )
	{
		return texmod::getImage( engine
			, name
			, castor::ImageCreateParams{ type
				, data
				, { false, false, false } } );
	}

	castor::Image & getFileImage( Engine & engine
		, castor::String const & name
		, castor::Path const & folder
		, castor::Path const & relative )
	{
		return texmod::getImage( engine
			, name
			, castor::ImageCreateParams{ folder / relative
				, { false, false, false } } );
	}

	TextureLayoutUPtr createTextureLayout( Engine const & engine
		, castor::Path const & relative
		, castor::Path const & folder )
	{
		ashes::ImageCreateInfo createInfo
		{
			0u,
			VK_IMAGE_TYPE_2D,
			VK_FORMAT_UNDEFINED,
			{ 1u, 1u, 1u },
			20u,
			1u,
			VK_SAMPLE_COUNT_1_BIT,
			VK_IMAGE_TILING_OPTIMAL,
			VK_IMAGE_USAGE_SAMPLED_BIT | VK_IMAGE_USAGE_TRANSFER_DST_BIT,
		};
		auto texture = castor::makeUnique< TextureLayout >( *engine.getRenderSystem()
			, createInfo
			, VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT
			, relative );
		texture->setSource( folder, relative );
		return texture;
	}

	TextureLayoutUPtr createTextureLayout( Engine const & engine
		, castor::String const & name
		, castor::PxBufferBaseUPtr buffer
		, bool isStatic )
	{
		ashes::ImageCreateInfo createInfo{ 0u
			, ( buffer->getHeight() <= 1u && buffer->getWidth() > 1u
				? VK_IMAGE_TYPE_1D
				: VK_IMAGE_TYPE_2D )
			, VK_FORMAT_UNDEFINED
			, { buffer->getWidth(), buffer->getHeight(), 1u }
			, uint32_t( castor::getBitSize( std::min( buffer->getWidth(), buffer->getHeight() ) ) )
			, 1u// TODO: Support array layers: buffer->getLayers()
			, VK_SAMPLE_COUNT_1_BIT
			, VK_IMAGE_TILING_OPTIMAL
			, VK_IMAGE_USAGE_SAMPLED_BIT | VK_IMAGE_USAGE_TRANSFER_DST_BIT };
		log::debug << ( cuT( "Creating " ) + name + cuT( " texture layout.\n" ) );
		auto texture = castor::makeUnique< TextureLayout >( *engine.getRenderSystem()
			, createInfo
			, VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT
			, name );
		texture->setSource( std::move( buffer ), isStatic );
		return texture;
	}

	uint32_t getMipLevels( VkExtent3D const & extent
		, VkFormat format )
	{
		auto blockSize = ashes::getBlockSize( format );
		auto min = std::min( extent.width / blockSize.extent.width, extent.height / blockSize.extent.height );
		return uint32_t( castor::getBitSize( min ) );
	}
}
