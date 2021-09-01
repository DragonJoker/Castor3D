#include "CastorUtils/Graphics/FontCache.hpp"

#include "CastorUtils/Graphics/Font.hpp"
#include "CastorUtils/Log/Logger.hpp"
#include "CastorUtils/Miscellaneous/StringUtils.hpp"

namespace castor
{
	template<>
	const String ResourceCacheTraitsT< Font, String >::Name = cuT( "Font" );

	ResourceCacheT< Font, String >::ResourceCacheT( LoggerInstance & logger )
		: ResourceCacheBaseT< Font, String >{ logger
			, [this]( String const & name, uint32_t height, Path const & path )
			{
				return doProduce( name, height, path );
			}
			, [this]( FontSPtr resource )
			{
				doInitialise( resource );
			} }
	{
	}

	FontSPtr ResourceCacheT< Font, String >::doProduce( String const & name
		, uint32_t height
		, Path path )
	{
		auto nameExt = path.getFileName( true );
		FontSPtr result;
		auto realPath = path;

		if ( !File::fileExists( realPath ) )
		{
			auto it = m_paths.find( nameExt );

			if ( it != m_paths.end() )
			{
				realPath = it->second;
			}
		}

		if ( File::fileExists( realPath ) )
		{
			result = std::make_shared< Font >( name, height, realPath );
		}
		else
		{
			CU_Exception( "Can't create the font, invalid name: " + string::stringCast< char >( name ) + ", path: " + string::stringCast< char >( path ) );
		}

		return result;
	}

	void ResourceCacheT< Font, String >::doInitialise( FontSPtr resource )
	{
		auto path = resource->getFilePath();
		auto nameExt = path.getFileName( true );

		if ( m_paths.find( nameExt ) == m_paths.end()
			&& File::fileExists( path ) )
		{
			m_paths.insert( std::make_pair( nameExt, path ) );
		}
	}
}
