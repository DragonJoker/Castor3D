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

	Path::Path( String data, bool normalise )
		: String{ castor::move( data ) }
	{
		if ( normalise )
		{
			doNormalise();
		}

		doUpdateInternal();
	}

	Path::Path( char const * rhs )
		: Path{ makeString( rhs ), true }
	{
	}

	Path::Path( wchar_t const * rhs )
		: Path{ makeString( rhs ), true }
	{
	}

	Path::Path( String const & rhs )
		: Path{ rhs, true }
	{
	}

	Path::Path( StringView rhs )
		: Path{ String{ rhs }, true }
	{
	}

	Path::Path( Path const & rhs )
		: String{ rhs }
	{
		doUpdateInternal();
	}

	Path::Path( Path && rhs )noexcept
		: String{ rhs }
	{
		doUpdateInternal();
	}

	Path & Path::operator=( Path const & rhs )
	{
		Path path( rhs );
		castor::swap( *this, path );
		return *this;
	}

	Path & Path::operator=( Path && rhs )noexcept
	{
		String::operator=( rhs );
		doUpdateInternal();
		return *this;
	}

	Path & Path::operator/=( Path const & rhs )
	{
		push_back( NativeSeparator );
		append( rhs );
		doNormalise();
		doUpdateInternal();
		return *this;
	}

	Path & Path::operator/=( StringView rhs )
	{
		push_back( NativeSeparator );
		append( rhs );
		doNormalise();
		doUpdateInternal();
		return *this;
	}

	Path & Path::operator/=( char const * rhs )
	{
		push_back( NativeSeparator );
		String::operator+=( makeString( rhs ) );
		doNormalise();
		doUpdateInternal();
		return *this;
	}

	Path & Path::operator/=( wchar_t const * rhs )
	{
		push_back( NativeSeparator );
		String::operator+=( makeString( rhs ) );
		doNormalise();
		doUpdateInternal();
		return *this;
	}

	Path & Path::operator+=( Path const & rhs )
	{
		String::operator+=( rhs );
		return *this;
	}

	Path & Path::operator+=( StringView rhs )
	{
		String::operator+=( rhs );
		doNormalise();
		doUpdateInternal();
		return *this;
	}

	Path & Path::operator+=( char const * rhs )
	{
		String::operator+=( makeString( rhs ) );
		doNormalise();
		doUpdateInternal();
		return *this;
	}

	Path & Path::operator+=( wchar_t const * rhs )
	{
		String::operator+=( makeString( rhs ) );
		doNormalise();
		doUpdateInternal();
		return *this;
	}

	Path Path::getPath()const
	{
		return Path{ String{ m_path }, false };
	}

	Path Path::getFileName( bool withExtension )const
	{
		if ( !withExtension )
		{
			return Path{ String{ m_fileName }, false };
		}

		return Path{ String{ m_fileName } + cuT( "." ) + getExtension(), false };
	}

	String Path::getExtension()const
	{
		return String{ m_extension };
	}

	String Path::toGeneric()const
	{
#if defined( CU_PlatformWindows )
		String copy{ *this };
		return string::replace( copy, NativeSeparator, GenericSeparator );
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

			if ( StringView{ data(), 2U } == cuT( "\\\\"_sv ) )
			{
				begin = cuT( "\\\\" );
				assign( substr( 2U ) );
			}
			else if ( StringView{ data(), 2U } == cuT( "//" ) )
			{
				begin = cuT( "/" );
				assign( substr( 2U ) );
			}
			else if ( StringView{ data(), 2U } == cuT( "/" ) )
			{
				begin = cuT( "/" );
				assign( substr( 1U ) );
			}

			Array< xchar, 3 > sep{ NativeSeparator, 0, 0 };
			String tmp( *this );
			string::replace( tmp, cuT( "/" ), sep.data() );
			string::replace( tmp, cuT( "\\" ), sep.data() );

			if ( this->at( this->size() - 1 ) == NativeSeparator )
			{
				end = NativeSeparator;
			}

			StringArray folders = string::split( tmp, sep.data(), 1000, false );
			List< String > list( folders.begin(), folders.end() );
			tmp.clear();
			auto it = std::find( list.begin(), list.end(), cuT( ".." ) );

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
					auto itPrv = it;
					--itPrv;
					list.erase( it );
					list.erase( itPrv );
					it = std::find( list.begin(), list.end(), cuT( ".." ) );
				}
			}

			tmp = begin;

			for ( auto const & folder : list )
			{
				if ( !tmp.empty()
					&& ( tmp.size() > 1 || tmp != cuT( "/" ) ) )
				{
					tmp += NativeSeparator;
				}

				tmp += folder;
			}

			assign( tmp + end );
		}
	}

	void Path::doUpdateInternal()
	{
		m_fileName = *this;
		m_path = {};
		m_extension = {};

		if ( std::size_t index = find_last_of( NativeSeparator );
			index != String::npos )
		{
			m_path = { data(), index };
			m_fileName = { std::next( begin(), ptrdiff_t( index + 1 ) ), end() };
		}

		if ( std::size_t index = m_fileName.find_last_of( cuT( "." ) );
			index != String::npos )
		{
			m_extension = { std::next( m_fileName.begin(), ptrdiff_t( index + 1 ) ), m_fileName.end() };
			m_fileName = { m_fileName.data(), index };
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

	std::filesystem::path makePath( StringView str )
	{
		if constexpr ( std::is_same_v< xchar, wchar_t > )
		{
			return std::filesystem::path{ str };
		}
		else
		{
			return std::filesystem::u8path( str.begin(), str.end() );
		}
	}
}
