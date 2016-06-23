﻿#include "Version.hpp"

using namespace Castor;

namespace Castor3D
{
	Version::Version( int p_iMajor, int p_iMinor, int p_iBuild )
		: m_iMajor( p_iMajor )
		, m_iMinor( p_iMinor )
		, m_iBuild( p_iBuild )
	{
	}

	Version::~Version()
	{
	}

	bool operator==( Version const & p_a, Version const & p_b )
	{
		return p_a.m_iMajor == p_b.m_iMajor
			   && p_a.m_iMinor == p_b.m_iMinor
			   && p_a.m_iBuild == p_b.m_iBuild;
	}

	bool operator!=( Version const & p_a, Version const & p_b )
	{
		return p_a.m_iMajor != p_b.m_iMajor
			   || p_a.m_iMinor != p_b.m_iMinor
			   || p_a.m_iBuild != p_b.m_iBuild;
	}

	bool operator<( Version const & p_a, Version const & p_b )
	{
		return	p_a.m_iMajor  < p_b.m_iMajor
				|| ( p_a.m_iMajor == p_b.m_iMajor && p_a.m_iMinor  < p_b.m_iMinor )
				|| ( p_a.m_iMajor == p_b.m_iMajor && p_a.m_iMinor == p_b.m_iMinor && p_a.m_iBuild  < p_b.m_iBuild );
	}

	bool operator>( Version const & p_a, Version const & p_b )
	{
		return	p_a.m_iMajor  > p_b.m_iMajor
				|| ( p_a.m_iMajor == p_b.m_iMajor && p_a.m_iMinor  > p_b.m_iMinor )
				|| ( p_a.m_iMajor == p_b.m_iMajor && p_a.m_iMinor == p_b.m_iMinor && p_a.m_iBuild  < p_b.m_iBuild );
	}

	bool operator<=( Version const & p_a, Version const & p_b )
	{
		return !( p_a > p_b );
	}

	bool operator>=( Version const & p_a, Version const & p_b )
	{
		return !( p_a < p_b );
	}

	std::ostream & operator<<( std::ostream & p_stream, Castor3D::Version const & p_version )
	{
		p_stream << p_version.m_iMajor << "." << p_version.m_iMinor << "." << p_version.m_iBuild;
		return p_stream;
	}

	std::wostream & operator<<( std::wostream & p_stream, Castor3D::Version const & p_version )
	{
		p_stream << p_version.m_iMajor << L"." << p_version.m_iMinor << L"." << p_version.m_iBuild;
		return p_stream;
	}

	Castor::String & operator<<( Castor::String & p_stream, Castor3D::Version const & p_version )
	{
		StringStream l_stream;
		l_stream << p_version;
		return p_stream += l_stream.str();
	}
}
