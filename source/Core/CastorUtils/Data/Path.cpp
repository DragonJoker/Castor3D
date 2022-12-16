#include "CastorUtils/Data/Path.hpp"

#include <algorithm>

// Both for std::u8path
#pragma warning( disable: 4996 )
#pragma GCC diagnostic ignored "-Wdeprecated-declarations"

namespace castor
{
	const xchar Path::GenericSeparator = cuT( '/' );
#if defined( CU_PlatformWindows )
	const xchar Path::NativeSeparator = cuT( '\\' );
#else
	const xchar Path::NativeSeparator = cuT( '/' );
#endif

	Path::Path()
	{
	}

	Path::Path( char const * rhs )
		:	String( string::stringCast< xchar >( rhs ) )
	{
		doNormalise();
	}

	Path::Path( wchar_t const * rhs )
		:	String( string::stringCast< xchar >( rhs ) )
	{
		doNormalise();
	}

	Path::Path( String const & rhs )
		:	String( rhs )
	{
		doNormalise();
	}

	Path::Path( Path const & rhs )
		:	String( rhs )
	{
		doNormalise();
	}

	Path::Path( Path && rhs )
		:	String( std::move( rhs ) )
	{
	}

	Path & Path::operator=( Path const & rhs )
	{
		Path path( rhs );
		std::swap( *this, path );
		return *this;
	}

	Path & Path::operator=( Path && rhs )
	{
		String::operator=( std::move( rhs ) );
		return *this;
	}

	Path & Path::operator/=( Path const & rhs )
	{
		push_back( NativeSeparator );
		append( rhs );
		doNormalise();
		return *this;
	}

	Path & Path::operator/=( String const & rhs )
	{
		push_back( NativeSeparator );
		append( rhs );
		doNormalise();
		return *this;
	}

	Path & Path::operator/=( char const * rhs )
	{
		push_back( NativeSeparator );
		String::operator+=( string::stringCast< xchar >( rhs ) );
		doNormalise();
		return *this;
	}

	Path & Path::operator/=( wchar_t const * rhs )
	{
		push_back( NativeSeparator );
		String::operator+=( string::stringCast< xchar >( rhs ) );
		doNormalise();
		return *this;
	}

	Path & Path::operator+=( Path const & rhs )
	{
		String::operator+=( rhs );
		return *this;
	}

	Path & Path::operator+=( String const & rhs )
	{
		String::operator+=( rhs );
		doNormalise();
		return *this;
	}

	Path & Path::operator+=( char const * rhs )
	{
		String::operator+=( string::stringCast< xchar >( rhs ) );
		doNormalise();
		return *this;
	}

	Path & Path::operator+=( wchar_t const * rhs )
	{
		String::operator+=( string::stringCast< xchar >( rhs ) );
		doNormalise();
		return *this;
	}

	Path Path::getPath()const
	{
		Path result;
		std::size_t index = find_last_of( NativeSeparator );

		if ( index != String::npos )
		{
			result = Path{ substr( 0, index ) };
		}

		return result;
	}

	Path Path::getFileName( bool withExtension )const
	{
		Path result = ( * this );
		std::size_t index = find_last_of( NativeSeparator );

		if ( index != String::npos )
		{
			result = Path{ substr( index + 1, String::npos ) };
		}

		if ( !withExtension )
		{
			index = result.find_last_of( cuT( "." ) );

			if ( index != String::npos )
			{
				result = Path{ result.substr( 0, index ) };
			}
		}

		return result;
	}

	Path Path::getFullFileName()const
	{
		Path result = ( * this );
		std::size_t index = find_last_of( NativeSeparator );

		if ( index != String::npos )
		{
			result = Path{ substr( index + 1, String::npos ) };
		}
		else
		{
			result.clear();
		}

		return result;
	}

	String Path::getExtension()const
	{
		String result = ( * this );
		std::size_t index = find_last_of( cuT( "." ) );

		if ( index != String::npos )
		{
			result = substr( index + 1, String::npos );
		}
		else
		{
			result.clear();
		}

		return result;
	}

	String Path::toGeneric()const
	{
#if defined( CU_PlatformWindows )
		String copy{ *this };
		return string::replace( copy, String{ NativeSeparator }, String{ GenericSeparator } );
#else
		return *this;
#endif
	}

	void Path::doNormalise()
	{
		if ( !empty() )
		{
			String begin;
			String end;

			if ( substr( 0, 2 ) == cuT( "\\\\" ) )
			{
				begin = cuT( "\\\\" );
				assign( substr( 2 ) );
			}
			else if ( substr( 0, 2 ) == cuT( "//" ) )
			{
				begin = cuT( "/" );
				assign( substr( 2 ) );
			}
			else if ( substr( 0, 1 ) == cuT( "/" ) )
			{
				begin = cuT( "/" );
				assign( substr( 1 ) );
			}

			xchar sep[3] = { NativeSeparator, 0 };
			String tmp( *this );
			string::replace( tmp, cuT( "/" ), sep );
			string::replace( tmp, cuT( "\\" ), sep );

			if ( this->at( this->size() - 1 ) == NativeSeparator )
			{
				end = NativeSeparator;
			}

			StringArray folders = string::split( tmp, sep, 1000, false );
			std::list< String > list( folders.begin(), folders.end() );
			tmp.clear();
			std::list< String >::iterator it = std::find( list.begin(), list.end(), cuT( ".." ) );

			while ( list.size() > 1 && it != list.end() )
			{
				if ( it == list.begin() )
				{
					// The folder looks like ../<something>, stop here.
					it = list.end();
				}
				else
				{
					// The folder looks like <something>/../<something else> remove the previous folder
					std::list< String >::iterator itPrv = it;
					--itPrv;
					list.erase( it );
					list.erase( itPrv );
					it = std::find( list.begin(), list.end(), cuT( ".." ) );
				}
			}

			tmp = begin;

			for ( auto folder : list )
			{
				if ( !tmp.empty()
					&& ( tmp.size() > 1 || tmp != "/" ) )
				{
					tmp += NativeSeparator;
				}

				tmp += folder;
			}

			assign( tmp + end );
		}
	}

	Path operator/( Path const & lhs, Path const & rhs )
	{
		Path path{ lhs };
		path /= rhs;
		return path;
	}

	Path operator/( Path const & lhs, String const & rhs )
	{
		Path path{ lhs };
		path /= rhs;
		return path;
	}

	Path operator/( Path const & lhs, char const * rhs )
	{
		Path path{ lhs };
		path /= rhs;
		return path;
	}

	Path operator/( Path const & lhs, wchar_t const * rhs )
	{
		Path path{ lhs };
		path /= rhs;
		return path;
	}

	Path operator/( String const & lhs, Path const & rhs )
	{
		Path path{ lhs };
		path /= rhs;
		return path;
	}

	Path operator/( char const * lhs, Path const & rhs )
	{
		Path path{ lhs };
		path /= rhs;
		return path;
	}

	Path operator/( wchar_t const * lhs, Path const & rhs )
	{
		Path path{ lhs };
		path /= rhs;
		return path;
	}

	std::filesystem::path makePath( castor::String const & str )
	{
		return std::filesystem::u8path( str.begin(), str.end() );
	}
}
