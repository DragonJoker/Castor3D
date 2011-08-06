#include "CastorUtils/PrecompiledHeader.hpp"

#include "CastorUtils/Path.hpp"

using namespace Castor;
using namespace Castor::Utils;

//*************************************************************************************************

Path :: Path()
{
}

Path :: Path( char const * p_data)
	:	String( p_data)
{
	_normalise();
}

Path :: Path( wchar_t const * p_data)
	:	String( p_data)
{
	_normalise();
}

Path :: Path( String const & p_data)
	:	String( p_data)
{
	_normalise();
}

Path :: Path( Path const & p_data)
	:	String( p_data)
{
	_normalise();
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
	_normalise();
	return *this;
}

Path & Path :: operator /=( char const * p_pBuffer)
{
	push_back( Separator);
	String::operator+=( p_pBuffer);
	_normalise();
	return *this;
}

Path & Path :: operator /=( wchar_t const * p_pBuffer)
{
	push_back( Separator);
	String::operator+=( p_pBuffer);
	_normalise();
	return *this;
}

Path Path :: GetPath()const
{
	Path l_strReturn;

	size_t l_index = find_last_of( Separator);

	if (l_index != String::npos)
	{
		l_strReturn = substr( 0, l_index);
	}

	return l_strReturn;
}

String Path :: GetFileName()const
{
	String l_strReturn = ( * this);
	size_t l_index = find_last_of( Separator);

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
	size_t l_index = find_last_of( Separator);

	if (l_index != String::npos)
	{
		l_strReturn = substr( l_index + 1, String::npos);
	}

	return l_strReturn;
}

String Path :: GetExtension()const
{
	String l_strReturn = ( * this);
	size_t l_index = find_last_of( cuT( "."));

	if (l_index != String::npos)
	{
		l_strReturn = substr( l_index + 1, String::npos);
	}

	return l_strReturn;
}

void Path :: _normalise()
{
	replace( cuT( '\\'), Separator);
	replace( cuT( '/'), Separator);
	xchar l_tmp[3] = {Separator, Separator, 0};
	replace( l_tmp, Separator);
}

//*************************************************************************************************

Path Castor::Utils :: operator /( Path const & p_pathA, Path const & p_pathB)
{
	Path l_path( p_pathA);
	l_path /= p_pathB;
	return l_path;
}

Path Castor::Utils :: operator /( Path const & p_path, String const & p_string)
{
	Path l_path( p_path);
	l_path /= p_string;
	return l_path;
}

Path Castor::Utils :: operator /( Path const & p_path, char const * p_pBuffer)
{
	Path l_path( p_path);
	l_path /= p_pBuffer;
	return l_path;
}

Path Castor::Utils :: operator /( Path const & p_path, wchar_t const * p_pBuffer)
{
	Path l_path( p_path);
	l_path /= p_pBuffer;
	return l_path;
}

Path Castor::Utils :: operator /( String const & p_string, Path const & p_path)
{
	Path l_path( p_string);
	l_path /= p_path;
	return l_path;
}

Path Castor::Utils :: operator /( char const * p_pBuffer, Path const & p_path)
{
	Path l_path( p_pBuffer);
	l_path /= p_path;
	return l_path;
}

Path Castor::Utils :: operator /( wchar_t const * p_pBuffer, Path const & p_path)
{
	Path l_path( p_pBuffer);
	l_path /= p_path;
	return l_path;
}

//*************************************************************************************************