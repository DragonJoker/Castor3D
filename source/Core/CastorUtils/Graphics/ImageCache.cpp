#include "CastorUtils/Graphics/ImageCache.hpp"

#include "CastorUtils/Graphics/Image.hpp"
#include "CastorUtils/Graphics/ImageLoader.hpp"

namespace castor
{
	template<>
	const String ResourceCacheTraitsT< Image, String >::Name = cuT( "Image" );

	ResourceCacheT< Image, String >::ResourceCacheT( LoggerInstance & logger
		, ImageLoader const & loader )
		: ResourceCacheBaseT< Image, String >{ logger
			, [this]( String const & name, ImageCreateParams const & params )
			{
				return doProduce( name, params );
			}
			, [this]( ImageSPtr resource )
			{
				doInitialise( resource );
			} }
		, m_loader{ loader }
	{
	}

	ImageSPtr ResourceCacheT< Image, String >::doProduce( String const & name
		, ImageCreateParams const & params )
	{
		ImageSPtr result;

		if ( params.mode )
		{
			result = std::make_shared< Image >( name
				, Path{}
				, params.size
				, params.format );
		}
		else
		{
			result = std::make_shared< Image >( m_loader.load( name
				, params.path
				, params.allowCompression
				, params.generateMips ) );
		}

		return result;
	}

	void ResourceCacheT< Image, String >::doInitialise( ImageSPtr resource )
	{
		if ( !resource->hasBuffer()
			&& File::fileExists( resource->getPath() ) )
		{
			*resource = m_loader.load( resource->getName()
				, resource->getPath()
				, ashes::isCompressedFormat( VkFormat( resource->getPixelFormat() ) )
				, resource->getLevels() > 1 );
		}
	}
}
