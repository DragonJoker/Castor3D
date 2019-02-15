#include "Castor3D/Miscellaneous/Version.hpp"

using namespace castor;

namespace castor3d
{
	Version::Version( int p_iMajor, int p_iMinor, int p_iBuild )
		: m_major( p_iMajor )
		, m_minor( p_iMinor )
		, m_build( p_iBuild )
	{
	}

	Version::~Version()
	{
	}

	bool operator==( Version const & p_a, Version const & p_b )
	{
		return p_a.getMajor() == p_b.getMajor()
			   && p_a.getMinor() == p_b.getMinor()
			   && p_a.getBuild() == p_b.getBuild();
	}

	bool operator!=( Version const & p_a, Version const & p_b )
	{
		return p_a.getMajor() != p_b.getMajor()
			   || p_a.getMinor() != p_b.getMinor()
			   || p_a.getBuild() != p_b.getBuild();
	}

	bool operator<( Version const & p_a, Version const & p_b )
	{
		return	p_a.getMajor()  < p_b.getMajor()
				|| ( p_a.getMajor() == p_b.getMajor() && p_a.getMinor()  < p_b.getMinor() )
				|| ( p_a.getMajor() == p_b.getMajor() && p_a.getMinor() == p_b.getMinor() && p_a.getBuild()  < p_b.getBuild() );
	}

	bool operator>( Version const & p_a, Version const & p_b )
	{
		return	p_a.getMajor()  > p_b.getMajor()
				|| ( p_a.getMajor() == p_b.getMajor() && p_a.getMinor()  > p_b.getMinor() )
				|| ( p_a.getMajor() == p_b.getMajor() && p_a.getMinor() == p_b.getMinor() && p_a.getBuild()  < p_b.getBuild() );
	}

	bool operator<=( Version const & p_a, Version const & p_b )
	{
		return !( p_a > p_b );
	}

	bool operator>=( Version const & p_a, Version const & p_b )
	{
		return !( p_a < p_b );
	}

	std::ostream & operator<<( std::ostream & p_stream, castor3d::Version const & p_version )
	{
		p_stream << p_version.getMajor() << "." << p_version.getMinor() << "." << p_version.getBuild();
		return p_stream;
	}

	std::wostream & operator<<( std::wostream & p_stream, castor3d::Version const & p_version )
	{
		p_stream << p_version.getMajor() << L"." << p_version.getMinor() << L"." << p_version.getBuild();
		return p_stream;
	}

	castor::String & operator<<( castor::String & p_stream, castor3d::Version const & p_version )
	{
		StringStream stream( makeStringStream() );
		stream << p_version;
		return p_stream += stream.str();
	}
}
