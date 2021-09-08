#include "CastorUtils/Graphics/GliImageWriter.hpp"

#include "CastorUtils/Data/LoaderException.hpp"
#include "CastorUtils/Data/Path.hpp"
#include "CastorUtils/Graphics/ImageLayout.hpp"
#include "CastorUtils/Graphics/PixelBuffer.hpp"
#include "CastorUtils/Graphics/Rectangle.hpp"
#include "CastorUtils/Miscellaneous/StringUtils.hpp"
#include "CastorUtils/Log/Logger.hpp"

#pragma warning( push )
#pragma warning( disable: 4100 )
#pragma warning( disable: 4201 )
#pragma warning( disable: 4296 )
#pragma warning( disable: 4458 )
#pragma warning( disable: 4464 )
#pragma warning( disable: 5219 )
#include <gli/gli.hpp>
#pragma warning( pop )

namespace castor
{
	//************************************************************************************************

	namespace
	{
		gli::target getTarget( Size const & dimensions, uint32_t layers )
		{
			if ( dimensions.getHeight() == 1u )
			{
				return layers > 1u
					? gli::TARGET_1D_ARRAY
					: gli::TARGET_1D;
			}

			return layers > 1u
				? gli::TARGET_2D_ARRAY
				: gli::TARGET_2D;
		}

		gli::format getFormat( PixelFormat format )
		{
			return gli::format( format );
		}

		gli::extent3d getExtent( Size const & dimensions )
		{
			return { int32_t( dimensions.getWidth() )
				, int32_t( dimensions.getHeight() )
				, 1 };
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

		gli::texture convert( PxBufferBase const & buffer )
		{
			gli::texture result{ getTarget( buffer.getDimensions(), buffer.getLayers() )
				, getFormat( buffer.getFormat() )
				, getExtent( buffer.getDimensions() )
				, buffer.getLayers()
				, 1u
				, buffer.getLevels() };
			auto dstLayer = reinterpret_cast< uint8_t * >( result.data() );
			auto srcLayer = buffer.getConstPtr();
			auto srcLayerSize = ashes::getLevelsSize( { buffer.getWidth(), buffer.getHeight(), 1u }
				, VkFormat( buffer.getFormat() )
				, 0u
				, buffer.getLevels()
				, buffer.getAlign() );
			auto dstLayerSize = ashes::getLevelsSize( { buffer.getWidth(), buffer.getHeight(), 1u }
				, VkFormat( buffer.getFormat() )
				, 0u
				, buffer.getLevels()
				, 1u );

			for ( auto layer = 0u; layer < buffer.getLayers(); ++layer )
			{
				auto srcLevel = srcLayer;
				auto dstLevel = dstLayer;

				for ( auto level = 0u; level < buffer.getLevels(); ++level )
				{
					auto srcLevelSize = ashes::getSize( { buffer.getWidth(), buffer.getHeight(), 1u }
						, VkFormat( buffer.getFormat() )
						, level
						, buffer.getAlign() );
					auto dstLevelSize = ashes::getSize( { buffer.getWidth(), buffer.getHeight(), 1u }
						, VkFormat( buffer.getFormat() )
						, level );
					std::memcpy( dstLevel, srcLevel, std::min( srcLevelSize, dstLevelSize ) );
					srcLevel += srcLevelSize;
					dstLevel += dstLevelSize;
				}

				srcLayer += srcLayerSize;
				dstLayer += dstLayerSize;
			}

			return result;
		}
	}

	//************************************************************************************************

	void GliImageWriter::registerWriter( ImageWriter & reg )
	{
		reg.registerWriter( listExtensions()
			, std::make_unique< GliImageWriter >() );
	}

	void GliImageWriter::unregisterWriter( ImageWriter & reg )
	{
		reg.unregisterWriter( listExtensions() );
	}

	bool GliImageWriter::write( Path const & path
		, PxBufferBase const & buffer )const
	{
		auto imageFormat = string::lowerCase( path.getExtension() );
		bool result = false;
		auto texture = convert( buffer );

		if ( imageFormat.find( cuT( "dds" ) ) != String::npos )
		{
			result = gli::save_dds( texture, path );
		}
		else if ( imageFormat.find( cuT( "kmg" ) ) != String::npos )
		{
			result = gli::save_kmg( texture, path );
		}
		else if ( imageFormat.find( cuT( "ktx" ) ) != String::npos )
		{
			result = gli::save_ktx( texture, path );
		}

		return result;
	}
}
