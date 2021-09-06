#include "CastorUtils/Graphics/ImageCache.hpp"

#include "CastorUtils/Graphics/Image.hpp"
#include "CastorUtils/Graphics/ImageLoader.hpp"

namespace castor
{
	//*********************************************************************************************

	const String ResourceCacheTraitsT< Image, String >::Name = cuT( "Image" );

	ResourceCacheTraitsT< Image, String >::ElementPtrT ResourceCacheTraitsT< Image, String >::makeElement( ResourceCacheBaseT< Image, String, ResourceCacheTraitsT< Image, String > > const & cache
		, String const & name
		, ImageCreateParams const & params )
	{
		ImageSPtr result;

		if ( params.mode )
		{
			return makeResource< Image, String >( name
				, Path{}
				, params.size
				, params.format );
		}

		auto & realCache = static_cast< ResourceCacheT< Image, String, ResourceCacheTraitsT< Image, String > > const & >( cache );
		return makeResource< Image, String >( realCache.getLoader().load( name
			, params.path
			, params.allowCompression
			, params.generateMips ) );
	}

	//*********************************************************************************************

	ResourceCacheT< Image, String, ImageCacheTraits >::ResourceCacheT( LoggerInstance & logger
		, ImageLoader const & loader )
		: ResourceCacheBaseT< Image, String >{ logger }
		, m_loader{ loader }
	{
	}

	//*********************************************************************************************
}
