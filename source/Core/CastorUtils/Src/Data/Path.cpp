#include "Path.hpp"

#include <algorithm>

namespace Castor
{
#if defined( CASTOR_PLATFORM_WINDOWS )
	const xchar Path::Separator = cuT( '\\' );
#else
	const xchar Path::Separator = cuT( '/' );
#endif

	Path::Path()
	{
	}

	Path::Path( char const * p_data )
		:	String( string::string_cast< xchar >( p_data ) )
	{
		DoNormalise();
	}

	Path::Path( wchar_t const * p_data )
		:	String( string::string_cast< xchar >( p_data ) )
	{
		DoNormalise();
	}

	Path::Path( String const & p_data )
		:	String( p_data )
	{
		DoNormalise();
	}

	Path::Path( Path const & p_data )
		:	String( p_data )
	{
		DoNormalise();
	}

	Path::Path( Path && p_data )
		:	String( std::move( p_data ) )
	{
	}

	Path & Path::operator=( Path const & p_data )
	{
		Path path( p_data );
		std::swap( *this, path );
		return *this;
	}

	Path & Path::operator=( Path && p_data )
	{
		String::operator=( std::move( p_data ) );
		return *this;
	}

	Path & Path::operator/=( Path const & p_path )
	{
		push_back( Separator );
		append( p_path );
		DoNormalise();
		return *this;
	}

	Path & Path::operator/=( String const & p_string )
	{
		push_back( Separator );
		append( p_string );
		DoNormalise();
		return *this;
	}

	Path & Path::operator/=( char const * p_buffer )
	{
		push_back( Separator );
		String::operator+=( string::string_cast< xchar >( p_buffer ) );
		DoNormalise();
		return *this;
	}

	Path & Path::operator/=( wchar_t const * p_buffer )
	{
		push_back( Separator );
		String::operator+=( string::string_cast< xchar >( p_buffer ) );
		DoNormalise();
		return *this;
	}

	Path & Path::operator+=( Path const & p_path )
	{
		String::operator+=( p_path );
		return *this;
	}

	Path & Path::operator+=( String const & p_string )
	{
		String::operator+=( p_string );
		DoNormalise();
		return *this;
	}

	Path & Path::operator+=( char const * p_buffer )
	{
		String::operator+=( string::string_cast< xchar >( p_buffer ) );
		DoNormalise();
		return *this;
	}

	Path & Path::operator+=( wchar_t const * p_buffer )
	{
		String::operator+=( string::string_cast< xchar >( p_buffer ) );
		DoNormalise();
		return *this;
	}

	Path Path::GetPath()const
	{
		Path result;
		std::size_t index = find_last_of( Separator );

		if ( index != String::npos )
		{
			result = Path{ substr( 0, index ) };
		}

		return result;
	}

	Path Path::GetFileName( bool p_withExtension )const
	{
		Path result = ( * this );
		std::size_t index = find_last_of( Separator );

		if ( index != String::npos )
		{
			result = Path{ substr( index + 1, String::npos ) };
		}

		if ( !p_withExtension )
		{
			index = result.find_last_of( cuT( "." ) );

			if ( index != String::npos )
			{
				result = Path{ result.substr( 0, index ) };
			}
		}

		return result;
	}

	Path Path::GetFullFileName()const
	{
		Path result = ( * this );
		std::size_t index = find_last_of( Separator );

		if ( index != String::npos )
		{
			result = Path{ substr( index + 1, String::npos ) };
		}

		return result;
	}

	String Path::GetExtension()const
	{
		String result = ( * this );
		std::size_t index = find_last_of( cuT( "." ) );

		if ( index != String::npos )
		{
			result = substr( index + 1, String::npos );
		}

		return result;
	}

	void Path::DoNormalise()
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

			xchar sep[3] = { Separator, 0 };
			String tmp( *this );
			string::replace( tmp, cuT( "/" ), sep );
			string::replace( tmp, cuT( "\\" ), sep );

			if ( this->at( this->size() - 1 ) == Separator )
			{
				end = Separator;
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
					itPrv--;
					list.erase( it );
					list.erase( itPrv );
					it = std::find( list.begin(), list.end(), cuT( ".." ) );
				}
			}

			tmp = begin;

			for ( auto folder : list )
			{
				if ( !tmp.empty() )
				{
					tmp += Separator;
				}

				tmp += folder;
			}

			assign( tmp + end );
		}
	}

	Path operator/( Path const & p_lhs, Path const & p_rhs )
	{
		Path path{ p_lhs };
		path /= p_rhs;
		return path;
	}

	Path operator/( Path const & p_lhs, String const & p_rhs )
	{
		Path path{ p_lhs };
		path /= p_rhs;
		return path;
	}

	Path operator/( Path const & p_lhs, char const * p_rhs )
	{
		Path path{ p_lhs };
		path /= p_rhs;
		return path;
	}

	Path operator/( Path const & p_lhs, wchar_t const * p_rhs )
	{
		Path path{ p_lhs };
		path /= p_rhs;
		return path;
	}

	Path operator/( String const & p_lhs, Path const & p_rhs )
	{
		Path path{ p_lhs };
		path /= p_rhs;
		return path;
	}

	Path operator/( char const * p_lhs, Path const & p_rhs )
	{
		Path path{ p_lhs };
		path /= p_rhs;
		return path;
	}

	Path operator/( wchar_t const * p_lhs, Path const & p_rhs )
	{
		Path path{ p_lhs };
		path /= p_rhs;
		return path;
	}
}
