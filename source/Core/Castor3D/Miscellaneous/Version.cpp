#include "Castor3D/Miscellaneous/Version.hpp"

#include <ashes/ashes.hpp>

using namespace castor;

namespace castor3d
{
	Version::Version( int major, int minor, int build )
		: m_major{ uint16_t( major ) }
		, m_minor{ uint16_t( minor ) }
		, m_build{ uint16_t( build ) }
	{
	}
	
	Version::Version( uint32_t vk )
		: m_major{ uint16_t( ashes::getMajor( vk ) ) }
		, m_minor{ uint16_t( ashes::getMinor( vk ) ) }
		, m_build{ uint16_t( ashes::getPatch( vk ) ) }
	{
	}

	Version::~Version()
	{
	}

	uint32_t Version::getVkVersion()const
	{
		return ashes::makeVersion( m_major, m_minor, m_build );
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
		return	p_a.getMajor() < p_b.getMajor()
			|| ( p_a.getMajor() == p_b.getMajor() && p_a.getMinor() < p_b.getMinor() )
			|| ( p_a.getMajor() == p_b.getMajor() && p_a.getMinor() == p_b.getMinor() && p_a.getBuild() < p_b.getBuild() );
	}

	bool operator>( Version const & p_a, Version const & p_b )
	{
		return	p_a.getMajor() > p_b.getMajor()
			|| ( p_a.getMajor() == p_b.getMajor() && p_a.getMinor() > p_b.getMinor() )
			|| ( p_a.getMajor() == p_b.getMajor() && p_a.getMinor() == p_b.getMinor() && p_a.getBuild() < p_b.getBuild() );
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
