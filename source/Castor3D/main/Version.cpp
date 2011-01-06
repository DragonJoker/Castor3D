#include "Castor3D/PrecompiledHeader.h"

#include "Castor3D/main/Version.h"

using namespace Castor3D;
using namespace Castor::Utils;

//*************************************************************************************************

VersionException :: VersionException( const Version & p_given, const Version & p_expected, const String & p_strFile, const String & p_strFunction, unsigned int p_uiLine)
	:	Exception( "", p_strFile, p_strFunction, p_uiLine)
{
	m_description = "Expected version [";
	m_description << p_expected.m_iMajor << "." << p_expected.m_iMinor << "] but got version [";
	m_description << p_given.m_iMajor << "." << p_given.m_iMinor << "] - Incompatible versions";
}

//*************************************************************************************************

Version :: Version( int p_iMajor, int p_iMinor)
	:	m_iMajor	( p_iMajor)
	,	m_iMinor	( p_iMinor)
{
}

Version :: Version( const Version & p_version)
	:	m_iMajor	( p_version.m_iMajor)
	,	m_iMinor	( p_version.m_iMinor)
{
}

Version :: ~Version()
{
}

Version & Version :: operator =( const Version & p_version)
{
	m_iMajor	= p_version.m_iMajor;
	m_iMinor	= p_version.m_iMinor;

	return * this;
}

bool Version :: operator ==( const Version & p_version)
{
	return m_iMajor	== p_version.m_iMajor
		&& m_iMinor	== p_version.m_iMinor;
}

bool Version :: operator !=( const Version & p_version)
{
	return m_iMajor	!= p_version.m_iMajor
		|| m_iMinor	!= p_version.m_iMinor;
}

bool Version :: operator < ( const Version & p_version)
{
	return	m_iMajor < p_version.m_iMajor
		|| (m_iMajor == p_version.m_iMajor && m_iMinor < p_version.m_iMinor);
}

bool Version :: operator > ( const Version & p_version)
{
	return	m_iMajor > p_version.m_iMajor
		|| (m_iMajor == p_version.m_iMajor && m_iMinor > p_version.m_iMinor);
}

bool Version :: operator <=( const Version & p_version)
{
	return	m_iMajor <= p_version.m_iMajor
		|| (m_iMajor == p_version.m_iMajor && m_iMinor <= p_version.m_iMinor);
}

bool Version :: operator >=( const Version & p_version)
{
	return	m_iMajor >= p_version.m_iMajor
		|| (m_iMajor == p_version.m_iMajor && m_iMinor >= p_version.m_iMinor);
}


//*************************************************************************************************