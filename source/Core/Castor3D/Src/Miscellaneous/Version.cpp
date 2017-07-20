#include "Version.hpp"

using namespace Castor;

namespace Castor3D
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
		return p_a.GetMajor() == p_b.GetMajor()
			   && p_a.GetMinor() == p_b.GetMinor()
			   && p_a.GetBuild() == p_b.GetBuild();
	}

	bool operator!=( Version const & p_a, Version const & p_b )
	{
		return p_a.GetMajor() != p_b.GetMajor()
			   || p_a.GetMinor() != p_b.GetMinor()
			   || p_a.GetBuild() != p_b.GetBuild();
	}

	bool operator<( Version const & p_a, Version const & p_b )
	{
		return	p_a.GetMajor()  < p_b.GetMajor()
				|| ( p_a.GetMajor() == p_b.GetMajor() && p_a.GetMinor()  < p_b.GetMinor() )
				|| ( p_a.GetMajor() == p_b.GetMajor() && p_a.GetMinor() == p_b.GetMinor() && p_a.GetBuild()  < p_b.GetBuild() );
	}

	bool operator>( Version const & p_a, Version const & p_b )
	{
		return	p_a.GetMajor()  > p_b.GetMajor()
				|| ( p_a.GetMajor() == p_b.GetMajor() && p_a.GetMinor()  > p_b.GetMinor() )
				|| ( p_a.GetMajor() == p_b.GetMajor() && p_a.GetMinor() == p_b.GetMinor() && p_a.GetBuild()  < p_b.GetBuild() );
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
		p_stream << p_version.GetMajor() << "." << p_version.GetMinor() << "." << p_version.GetBuild();
		return p_stream;
	}

	std::wostream & operator<<( std::wostream & p_stream, Castor3D::Version const & p_version )
	{
		p_stream << p_version.GetMajor() << L"." << p_version.GetMinor() << L"." << p_version.GetBuild();
		return p_stream;
	}

	Castor::String & operator<<( Castor::String & p_stream, Castor3D::Version const & p_version )
	{
		StringStream stream;
		stream << p_version;
		return p_stream += stream.str();
	}
}
