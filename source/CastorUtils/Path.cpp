#include "CastorUtils/PrecompiledHeader.hpp"

#include "CastorUtils/Path.hpp"

using namespace Castor;

//*************************************************************************************************

Path :: Path()
{
}

Path :: Path( char const * p_data )
	:	String( str_utils::from_str( p_data ) )
{
	DoNormalise();
}

Path :: Path( wchar_t const * p_data )
	:	String( str_utils::from_wstr( p_data ) )
{
	DoNormalise();
}

Path :: Path( String const & p_data )
	:	String( p_data )
{
	DoNormalise();
}

Path :: Path( Path const & p_data )
	:	String( p_data )
{
	DoNormalise();
}

Path :: Path( Path && p_data )
	:	String( std::move( p_data )	)
{
}

Path & Path :: operator =( Path const & p_data )
{
	Path l_path( p_data );
	std::swap( *this, l_path );
	return *this;
}

Path & Path :: operator =( Path && p_data )
{
	String::operator =( std::move( p_data )	);
	return *this;
}

Path & Path :: operator /=( Path const & p_path)
{
	push_back( Separator);
	append( p_path);
	return *this;
}

Path & Path :: operator /=( String const & p_string)
{
	push_back( Separator);
	append( p_string);
	DoNormalise();
	return *this;
}

Path & Path :: operator /=( char const * p_pBuffer)
{
	push_back( Separator);
	String::operator+=( str_utils::from_str( p_pBuffer ) );
	DoNormalise();
	return *this;
}

Path & Path :: operator /=( wchar_t const * p_pBuffer)
{
	push_back( Separator);
	String::operator+=( str_utils::from_wstr( p_pBuffer ) );
	DoNormalise();
	return *this;
}

Path Path :: GetPath()const
{
	Path l_strReturn;

	std::size_t l_index = find_last_of( Separator);

	if (l_index != String::npos)
	{
		l_strReturn = substr( 0, l_index);
	}

	return l_strReturn;
}

String Path :: GetFileName()const
{
	String l_strReturn = ( * this);
	std::size_t l_index = find_last_of( Separator);

	if (l_index != String::npos)
	{
		l_strReturn = substr( l_index + 1, String::npos);
	}

	l_index = l_strReturn.find_last_of( cuT( "."));

	if (l_index != String::npos)
	{
		l_strReturn = l_strReturn.substr( 0, l_index);
	}

	return l_strReturn;
}

String Path :: GetFullFileName()const
{
	String l_strReturn = ( * this);
	std::size_t l_index = find_last_of( Separator);

	if (l_index != String::npos)
	{
		l_strReturn = substr( l_index + 1, String::npos);
	}

	return l_strReturn;
}

String Path :: GetExtension()const
{
	String l_strReturn = ( * this);
	std::size_t l_index = find_last_of( cuT( "."));

	if (l_index != String::npos)
	{
		l_strReturn = substr( l_index + 1, String::npos);
	}

	return l_strReturn;
}

void Path :: DoNormalise()
{
	String l_strSave;
	if( substr( 0, 2 ) == cuT( "\\\\" ) )
	{
		l_strSave = cuT( "\\\\" );
		assign( substr( 2 ) );
	}

	str_utils::replace( *this, cuT( '\\' ), Separator );
	str_utils::replace( *this, cuT( '/'), Separator );
	xchar l_tmp[3] = { Separator, Separator, 0 };
	str_utils::replace( *this, l_tmp, Separator );
	str_utils::replace( *this, l_tmp, Separator );
	str_utils::replace( *this, l_tmp, Separator );

	assign( l_strSave + *this );
}

//*************************************************************************************************

Path Castor :: operator /( Path const & p_pathA, Path const & p_pathB)
{
	Path l_path( p_pathA);
	l_path /= p_pathB;
	return l_path;
}

Path Castor :: operator /( Path const & p_path, String const & p_string)
{
	Path l_path( p_path);
	l_path /= p_string;
	return l_path;
}

Path Castor :: operator /( Path const & p_path, char const * p_pBuffer)
{
	Path l_path( p_path);
	l_path /= p_pBuffer;
	return l_path;
}

Path Castor :: operator /( Path const & p_path, wchar_t const * p_pBuffer)
{
	Path l_path( p_path);
	l_path /= p_pBuffer;
	return l_path;
}

Path Castor :: operator /( String const & p_string, Path const & p_path)
{
	Path l_path( p_string);
	l_path /= p_path;
	return l_path;
}

Path Castor :: operator /( char const * p_pBuffer, Path const & p_path)
{
	Path l_path( p_pBuffer);
	l_path /= p_path;
	return l_path;
}

Path Castor :: operator /( wchar_t const * p_pBuffer, Path const & p_path)
{
	Path l_path( p_pBuffer);
	l_path /= p_path;
	return l_path;
}

//*************************************************************************************************
