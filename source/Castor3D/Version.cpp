#include "Castor3D/PrecompiledHeader.hpp"

#include "Castor3D/Version.hpp"

using namespace Castor3D;
using namespace Castor::Utils;

//*************************************************************************************************

VersionException :: VersionException( const Version & p_given, const Version & p_expected, String const & p_strFile, String const & p_strFunction, unsigned int p_uiLine)
	:	Exception( "", p_strFile, p_strFunction, p_uiLine)
{
	m_description = "Expected version [";
	m_description << p_expected.m_iMajor << "." << p_expected.m_iAnnex << "." << p_expected.m_iCorrect << "." << p_expected.m_iPoint << "] but got version [";
	m_description << p_given.m_iMajor << "." << p_given.m_iAnnex << "." << p_given.m_iCorrect << "." << p_given.m_iPoint << "] - Incompatible versions";
}

//*************************************************************************************************

Version :: Version( int p_iMajor, int p_iAnnex, int p_iCorrect, int p_iPoint)
	:	m_iMajor	( p_iMajor)
	,	m_iAnnex	( p_iAnnex)
	,	m_iCorrect	( p_iCorrect)
	,	m_iPoint	( p_iPoint)
{
}

Version :: Version( const Version & p_version)
	:	m_iMajor	( p_version.m_iMajor)
	,	m_iAnnex	( p_version.m_iAnnex)
	,	m_iCorrect	( p_version.m_iCorrect)
	,	m_iPoint	( p_version.m_iPoint)
{
}

Version :: ~Version()
{
}

Version & Version :: operator =( const Version & p_version)
{
	m_iMajor	= p_version.m_iMajor;
	m_iAnnex	= p_version.m_iAnnex;
	m_iCorrect	= p_version.m_iCorrect;
	m_iPoint	= p_version.m_iPoint;

	return * this;
}

bool Version :: operator ==( const Version & p_version)
{
	return	m_iMajor	== p_version.m_iMajor
		&&	m_iAnnex	== p_version.m_iAnnex
		&&	m_iCorrect	== p_version.m_iCorrect
		&&	m_iPoint	== p_version.m_iPoint;
}

bool Version :: operator !=( const Version & p_version)
{
	return	m_iMajor	!= p_version.m_iMajor
		||	m_iAnnex	!= p_version.m_iAnnex
		||	m_iCorrect	!= p_version.m_iCorrect
		||	m_iPoint	!= p_version.m_iPoint;
}

bool Version :: operator < ( const Version & p_version)
{
	return	m_iMajor < p_version.m_iMajor
		|| (m_iMajor == p_version.m_iMajor && m_iAnnex < p_version.m_iAnnex)
		|| (m_iMajor == p_version.m_iMajor && m_iAnnex == p_version.m_iAnnex && m_iCorrect < p_version.m_iCorrect)
		|| (m_iMajor == p_version.m_iMajor && m_iAnnex == p_version.m_iAnnex && m_iCorrect == p_version.m_iCorrect && m_iPoint < p_version.m_iPoint);
}

bool Version :: operator > ( const Version & p_version)
{
	return	m_iMajor > p_version.m_iMajor
		|| (m_iMajor == p_version.m_iMajor && m_iAnnex > p_version.m_iAnnex)
		|| (m_iMajor == p_version.m_iMajor && m_iAnnex == p_version.m_iAnnex && m_iCorrect < p_version.m_iCorrect)
		|| (m_iMajor == p_version.m_iMajor && m_iAnnex == p_version.m_iAnnex && m_iCorrect == p_version.m_iCorrect && m_iPoint > p_version.m_iPoint);
}

bool Version :: operator <=( const Version & p_version)
{
	return ! operator >( p_version);
}

bool Version :: operator >=( const Version & p_version)
{
	return ! operator <( p_version);
}

//*************************************************************************************************

std::basic_ostream<xchar> & Castor3D::operator <<( std::basic_ostream<xchar> & p_stream, const Version & p_version)
{
	p_stream << p_version.m_iMajor << cuT( ".") << p_version.m_iAnnex << cuT( ".") << p_version.m_iCorrect << cuT( ".") << p_version.m_iPoint;
	return p_stream;
}

//*************************************************************************************************