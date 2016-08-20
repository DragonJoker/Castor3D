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
		Path l_path( p_data );
		std::swap( *this, l_path );
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
		Path l_return;
		std::size_t l_index = find_last_of( Separator );

		if ( l_index != String::npos )
		{
			l_return = Path{ substr( 0, l_index ) };
		}

		return l_return;
	}

	Path Path::GetFileName( bool p_withExtension )const
	{
		Path l_return = ( * this );
		std::size_t l_index = find_last_of( Separator );

		if ( l_index != String::npos )
		{
			l_return = Path{ substr( l_index + 1, String::npos ) };
		}

		if ( !p_withExtension )
		{
			l_index = l_return.find_last_of( cuT( "." ) );

			if ( l_index != String::npos )
			{
				l_return = Path{ l_return.substr( 0, l_index ) };
			}
		}

		return l_return;
	}

	Path Path::GetFullFileName()const
	{
		Path l_return = ( * this );
		std::size_t l_index = find_last_of( Separator );

		if ( l_index != String::npos )
		{
			l_return = Path{ substr( l_index + 1, String::npos ) };
		}

		return l_return;
	}

	String Path::GetExtension()const
	{
		String l_return = ( * this );
		std::size_t l_index = find_last_of( cuT( "." ) );

		if ( l_index != String::npos )
		{
			l_return = substr( l_index + 1, String::npos );
		}

		return l_return;
	}

	void Path::DoNormalise()
	{
		if ( !empty() )
		{
			String l_begin;
			String l_end;

			if ( substr( 0, 2 ) == cuT( "\\\\" ) )
			{
				l_begin = cuT( "\\\\" );
				assign( substr( 2 ) );
			}
			else if ( substr( 0, 2 ) == cuT( "//" ) )
			{
				l_begin = cuT( "/" );
				assign( substr( 2 ) );
			}
			else if ( substr( 0, 1 ) == cuT( "/" ) )
			{
				l_begin = cuT( "/" );
				assign( substr( 1 ) );
			}

			xchar l_sep[3] = { Separator, 0 };
			String l_tmp( *this );
			string::replace( l_tmp, cuT( "/" ), l_sep );
			string::replace( l_tmp, cuT( "\\" ), l_sep );

			if ( this->at( this->size() - 1 ) == Separator )
			{
				l_end = Separator;
			}

			StringArray l_folders = string::split( l_tmp, l_sep, 1000, false );
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

			l_tmp = l_begin;

			for ( auto l_folder : l_list )
			{
				if ( !l_tmp.empty() )
				{
					l_tmp += Separator;
				}

				l_tmp += l_folder;
			}

			assign( l_tmp + l_end );
		}
	}

	Path operator/( Path const & p_lhs, Path const & p_rhs )
	{
		Path l_path{ p_lhs };
		l_path /= p_rhs;
		return l_path;
	}

	Path operator/( Path const & p_lhs, String const & p_rhs )
	{
		Path l_path{ p_lhs };
		l_path /= p_rhs;
		return l_path;
	}

	Path operator/( Path const & p_lhs, char const * p_rhs )
	{
		Path l_path{ p_lhs };
		l_path /= p_rhs;
		return l_path;
	}

	Path operator/( Path const & p_lhs, wchar_t const * p_rhs )
	{
		Path l_path{ p_lhs };
		l_path /= p_rhs;
		return l_path;
	}

	Path operator/( String const & p_lhs, Path const & p_rhs )
	{
		Path l_path{ p_lhs };
		l_path /= p_rhs;
		return l_path;
	}

	Path operator/( char const * p_lhs, Path const & p_rhs )
	{
		Path l_path{ p_lhs };
		l_path /= p_rhs;
		return l_path;
	}

	Path operator/( wchar_t const * p_lhs, Path const & p_rhs )
	{
		Path l_path{ p_lhs };
		l_path /= p_rhs;
		return l_path;
	}
}
