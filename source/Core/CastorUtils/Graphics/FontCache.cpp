#include "CastorUtils/Graphics/FontCache.hpp"

#include "CastorUtils/Design/Resource.hpp"
#include "CastorUtils/Graphics/Font.hpp"
#include "CastorUtils/Log/Logger.hpp"
#include "CastorUtils/Miscellaneous/StringUtils.hpp"

CU_ImplementSmartPtr( castor, FontCache )

namespace castor
{
	//*********************************************************************************************

	const String FontCacheTraits::Name = cuT( "Font" );

	ResourceCacheTraitsT< Font, String >::ElementPtrT ResourceCacheTraitsT< Font, String >::makeElement( ResourceCacheBaseT< Font, String, ResourceCacheTraitsT< Font, String > > const & cache
		, String const & name
		, uint32_t height
		, Path const & path )
	{
		auto & realCache = static_cast< ResourceCacheT< Font, String, ResourceCacheTraitsT< Font, String > > const & >( cache );
		auto realPath = realCache.getRealPath( path );

		if ( !File::fileExists( realPath ) )
		{
			CU_Exception( cuT( "Can't create the font, invalid name: " ) + name + cuT( ", path: " ) + path );
		}

		return makeResource< Font, String >( name, height, realPath );
	}

	ResourceCacheTraitsT< Font, String >::ElementPtrT ResourceCacheTraitsT< Font, String >::makeElement( ResourceCacheBaseT< Font, String, ResourceCacheTraitsT< Font, String > > const & cache
		, String const & name
		, Path const & path )
	{
		auto & realCache = static_cast< ResourceCacheT< Font, String, ResourceCacheTraitsT< Font, String > > const & >( cache );
		auto realPath = realCache.getRealPath( path );

		if ( !File::fileExists( realPath ) )
		{
			CU_Exception( cuT( "Can't create the font, invalid name: " ) + name + cuT( ", path: " ) + path );
		}

		return makeResource< Font, String >( name, realPath );
	}

	//*********************************************************************************************

	ResourceCacheT< Font, String, FontCacheTraits >::ResourceCacheT( LoggerInstance & logger )
		: ResourceCacheBaseT< Font, String, FontCacheTraits >{ logger
			, [this]( ElementT const & resource )
			{
				auto path = resource.getFilePath();
				auto nameExt = path.getFileName( true );

				if ( m_paths.find( nameExt ) == m_paths.end()
					&& File::fileExists( path ) )
				{
					m_paths.try_emplace( nameExt, path );
				}
			} }
	{
	}

	Path ResourceCacheT< Font, String, FontCacheTraits >::getRealPath( Path path )const
	{
		auto nameExt = path.getFileName( true );

		if ( !File::fileExists( path ) )
		{
			auto it = m_paths.find( nameExt );

			if ( it != m_paths.end() )
			{
				path = it->second;
			}
		}

		return path;
	}

	FontCacheTraits::ElementPtrT ResourceCacheT< Font, String, FontCacheTraits >::create( String const & name
		, uint32_t height
		, Path const & path )const
	{
		auto realPath = getRealPath( path );

		if ( !File::fileExists( realPath ) )
		{
			CU_Exception( cuT( "Can't create the font, invalid name: " ) + name + cuT( ", path: " ) + path );
		}

		reportCreation( name );
		return makeResource< Font, String >( name, height, realPath );
	}

	FontCacheTraits::ElementPtrT ResourceCacheT< Font, String, FontCacheTraits >::create( String const & name
		, Path const & path )const
	{
		auto realPath = getRealPath( path );

		if ( !File::fileExists( realPath ) )
		{
			CU_Exception( cuT( "Can't create the font, invalid name: " ) + name + cuT( ", path: " ) + path );
		}

		reportCreation( name );
		return makeResource< Font, String >( name, realPath );
	}

	FontCacheTraits::ElementObsT ResourceCacheT< Font, String, FontCacheTraits >::add( ElementKeyT const & name
		, ElementPtrT & element
		, bool initialise )
	{
		return ResourceCacheBaseT< Font, String, FontCacheTraits >::add( name
			, element
			, initialise );
	}

	FontCacheTraits::ElementObsT ResourceCacheT< Font, String, FontCacheTraits >::add( String const & name
		, uint32_t height
		, Path const & path )
	{
		ElementObsT created{};
		auto result = doTryAddNoLockT( name
			, true
			, created
			, height, path );

		if ( !ElementCacheTraitsT::areElementsEqual( result, created ) )
		{
			reportDuplicate( name );
		}
		else
		{
			reportCreation( name );
		}

		return result;
	}

	FontCacheTraits::ElementObsT ResourceCacheT< Font, String, FontCacheTraits >::add( String const & name
		, Path const & path )
	{
		ElementObsT created{};
		auto result = doTryAddNoLockT( name
			, true
			, created
			, path );

		if ( !ElementCacheTraitsT::areElementsEqual( result, created ) )
		{
			reportDuplicate( name );
		}
		else
		{
			reportCreation( name );
		}

		return result;
	}

	//*********************************************************************************************
}
