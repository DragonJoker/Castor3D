#include "Version.hpp"

using namespace Castor;

namespace Castor3D
{
	Version::Version( int p_iMajor, int p_iMinor, int p_iBuild )
		:	m_iMajor( p_iMajor )
		,	m_iMinor( p_iMinor )
		,	m_iBuild( p_iBuild )
	{
	}

	Version::Version( Version const & p_version )
		:	m_iMajor( p_version.m_iMajor )
		,	m_iMinor( p_version.m_iMinor )
		,	m_iBuild( p_version.m_iBuild )
	{
	}

	Version::Version( Version && p_version )
		:	m_iMajor( std::move( p_version.m_iMajor ) )
		,	m_iMinor( std::move( p_version.m_iMinor ) )
		,	m_iBuild( std::move( p_version.m_iBuild ) )
	{
		p_version.m_iMajor	= 0;
		p_version.m_iMinor	= 0;
		p_version.m_iBuild	= 0;
	}

	Version::~Version()
	{
	}

	Version & Version::operator =( Version const & p_version )
	{
		m_iMajor	= p_version.m_iMajor;
		m_iMinor	= p_version.m_iMinor;
		m_iBuild	= p_version.m_iBuild;
		return * this;
	}

	Version & Version::operator =( Version && p_version )
	{
		if ( this != & p_version )
		{
			m_iMajor	= std::move( p_version.m_iMajor );
			m_iMinor	= std::move( p_version.m_iMinor );
			m_iBuild	= std::move( p_version.m_iBuild );
			p_version.m_iMajor	= 0;
			p_version.m_iMinor	= 0;
			p_version.m_iBuild	= 0;
		}

		return * this;
	}

	bool Version::operator ==( Version const & p_version )
	{
		return	m_iMajor	== p_version.m_iMajor
				&&	m_iMinor	== p_version.m_iMinor
				&&	m_iBuild	== p_version.m_iBuild;
	}

	bool Version::operator !=( Version const & p_version )
	{
		return	m_iMajor	!= p_version.m_iMajor
				||	m_iMinor	!= p_version.m_iMinor
				||	m_iBuild	!= p_version.m_iBuild;
	}

	bool Version::operator < ( Version const & p_version )
	{
		return	m_iMajor  < p_version.m_iMajor
				|| ( m_iMajor == p_version.m_iMajor && m_iMinor  < p_version.m_iMinor )
				|| ( m_iMajor == p_version.m_iMajor && m_iMinor == p_version.m_iMinor && m_iBuild  < p_version.m_iBuild );
	}

	bool Version::operator > ( Version const & p_version )
	{
		return	m_iMajor  > p_version.m_iMajor
				|| ( m_iMajor == p_version.m_iMajor && m_iMinor  > p_version.m_iMinor )
				|| ( m_iMajor == p_version.m_iMajor && m_iMinor == p_version.m_iMinor && m_iBuild  < p_version.m_iBuild );
	}

	bool Version::operator <=( Version const & p_version )
	{
		return ! operator >( p_version );
	}

	bool Version::operator >=( Version const & p_version )
	{
		return ! operator <( p_version );
	}
}

std::ostream & operator <<( std::ostream & p_stream, Castor3D::Version const & p_version )
{
	p_stream << p_version.m_iMajor << "." << p_version.m_iMinor << "." << p_version.m_iBuild;
	return p_stream;
}

std::wostream & operator <<( std::wostream & p_stream, Castor3D::Version const & p_version )
{
	p_stream << p_version.m_iMajor << L"." << p_version.m_iMinor << L"." << p_version.m_iBuild;
	return p_stream;
}

Castor::String & operator <<( Castor::String & p_stream, Castor3D::Version const & p_version )
{
	p_stream + str_utils::to_string( p_version.m_iMajor ) + cuT( "." ) + str_utils::to_string( p_version.m_iMinor ) + cuT( "." ) + str_utils::to_string( p_version.m_iBuild );
	return p_stream;
}
