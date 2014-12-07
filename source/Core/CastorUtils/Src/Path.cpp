#include "Path.hpp"

#include <algorithm>

namespace Castor
{
#if defined( _WIN32 )
	const xchar Path::Separator = cuT( '\\' );
#else
	const xchar Path::Separator = cuT( '/' );
#endif
	Path::Path()
	{
	}

	Path::Path( char const * p_data )
		:	String( str_utils::from_str( p_data ) )
	{
		DoNormalise();
	}

	Path::Path( wchar_t const * p_data )
		:	String( str_utils::from_wstr( p_data ) )
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
		:	String( std::move( p_data )	)
	{
	}

	Path & Path::operator =( Path const & p_data )
	{
		Path l_path( p_data );
		std::swap( *this, l_path );
		return *this;
	}

	Path & Path::operator =( Path && p_data )
	{
		String::operator =( std::move( p_data )	);
		return *this;
	}

	Path & Path::operator /=( Path const & p_path )
	{
		push_back( Separator );
		append( p_path );
		return *this;
	}

	Path & Path::operator /=( String const & p_string )
	{
		push_back( Separator );
		append( p_string );
		DoNormalise();
		return *this;
	}

	Path & Path::operator /=( char const * p_pBuffer )
	{
		push_back( Separator );
		String::operator+=( str_utils::from_str( p_pBuffer ) );
		DoNormalise();
		return *this;
	}

	Path & Path::operator /=( wchar_t const * p_pBuffer )
	{
		push_back( Separator );
		String::operator+=( str_utils::from_wstr( p_pBuffer ) );
		DoNormalise();
		return *this;
	}

	Path Path::GetPath()const
	{
		Path l_strReturn;
		std::size_t l_index = find_last_of( Separator );

		if ( l_index != String::npos )
		{
			l_strReturn = substr( 0, l_index );
		}

		return l_strReturn;
	}

	String Path::GetFileName()const
	{
		String l_strReturn = ( * this );
		std::size_t l_index = find_last_of( Separator );

		if ( l_index != String::npos )
		{
			l_strReturn = substr( l_index + 1, String::npos );
		}

		l_index = l_strReturn.find_last_of( cuT( "." ) );

		if ( l_index != String::npos )
		{
			l_strReturn = l_strReturn.substr( 0, l_index );
		}

		return l_strReturn;
	}

	String Path::GetFullFileName()const
	{
		String l_strReturn = ( * this );
		std::size_t l_index = find_last_of( Separator );

		if ( l_index != String::npos )
		{
			l_strReturn = substr( l_index + 1, String::npos );
		}

		return l_strReturn;
	}

	String Path::GetExtension()const
	{
		String l_strReturn = ( * this );
		std::size_t l_index = find_last_of( cuT( "." ) );

		if ( l_index != String::npos )
		{
			l_strReturn = substr( l_index + 1, String::npos );
		}

		return l_strReturn;
	}

	void Path::DoNormalise()
	{
		if ( !empty() )
		{
			String l_strBegin;
			String l_strEnd;

			if ( substr( 0, 2 ) == cuT( "\\\\" ) )
			{
				l_strBegin = cuT( "\\\\" );
				assign( substr( 2 ) );
			}
			else if ( substr( 0, 2 ) == cuT( "//" ) )
			{
				l_strBegin = cuT( "/" );
				assign( substr( 2 ) );
			}
			else if ( substr( 0, 1 ) == cuT( "/" ) )
			{
				l_strBegin = cuT( "/" );
				assign( substr( 1 ) );
			}

			xchar l_sep[3] = { Separator, 0 };
			String l_tmp( *this );
			str_utils::replace( l_tmp, cuT( "/" ), l_sep );
			str_utils::replace( l_tmp, cuT( "\\" ), l_sep );

			if ( this->at( this->size() - 1 ) == Separator )
			{
				l_strEnd = Separator;
			}

			StringArray l_folders = str_utils::split( l_tmp, l_sep, 1000, false );
			std::list< String > l_list( l_folders.begin(), l_folders.end() );
			l_tmp.clear();
			std::list< String >::iterator l_it = std::find( l_list.begin(), l_list.end(), cuT( ".." ) );

			while ( l_list.size() > 1 && l_it != l_list.end() )
			{
				if ( l_it == l_list.begin() )
				{
					// The folder looks like ../<something>, stop here.
					l_it = l_list.end();
				}
				else
				{
					// The folder looks like <something>/../<something else> remove the previous folder
					std::list< String >::iterator l_itPrv = l_it;
					l_itPrv--;
					l_list.erase( l_it );
					l_list.erase( l_itPrv );
					l_it = std::find( l_list.begin(), l_list.end(), cuT( ".." ) );
				}
			}

			l_tmp = l_strBegin;

			for ( std::list< String >::iterator l_it = l_list.begin(); l_it != l_list.end(); ++l_it )
			{
				if ( !l_tmp.empty() )
				{
					l_tmp += Separator;
				}

				l_tmp += *l_it;
			}

			assign( l_tmp + l_strEnd );
		}
	}

	Path operator /( Path const & p_pathA, Path const & p_pathB )
	{
		Path l_path( p_pathA );
		l_path /= p_pathB;
		return l_path;
	}

	Path operator /( Path const & p_path, String const & p_string )
	{
		Path l_path( p_path );
		l_path /= p_string;
		return l_path;
	}

	Path operator /( Path const & p_path, char const * p_pBuffer )
	{
		Path l_path( p_path );
		l_path /= p_pBuffer;
		return l_path;
	}

	Path operator /( Path const & p_path, wchar_t const * p_pBuffer )
	{
		Path l_path( p_path );
		l_path /= p_pBuffer;
		return l_path;
	}

	Path operator /( String const & p_string, Path const & p_path )
	{
		Path l_path( p_string );
		l_path /= p_path;
		return l_path;
	}

	Path operator /( char const * p_pBuffer, Path const & p_path )
	{
		Path l_path( p_pBuffer );
		l_path /= p_path;
		return l_path;
	}

	Path operator /( wchar_t const * p_pBuffer, Path const & p_path )
	{
		Path l_path( p_pBuffer );
		l_path /= p_path;
		return l_path;
	}
}
