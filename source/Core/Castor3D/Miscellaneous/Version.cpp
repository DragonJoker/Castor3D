#include "Castor3D/Miscellaneous/Version.hpp"

#include <ashes/ashes.hpp>

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

	uint32_t Version::getVkVersion()const
	{
		return ashes::makeVersion( m_major, m_minor, m_build );
	}

	bool operator==( Version const & lhs, Version const & rhs )
	{
		return lhs.getMajor() == rhs.getMajor()
			&& lhs.getMinor() == rhs.getMinor()
			&& lhs.getBuild() == rhs.getBuild();
	}

	bool operator!=( Version const & lhs, Version const & rhs )
	{
		return lhs.getMajor() != rhs.getMajor()
			|| lhs.getMinor() != rhs.getMinor()
			|| lhs.getBuild() != rhs.getBuild();
	}

	bool operator<( Version const & lhs, Version const & rhs )
	{
		return	lhs.getMajor() < rhs.getMajor()
			|| ( lhs.getMajor() == rhs.getMajor() && lhs.getMinor() < rhs.getMinor() )
			|| ( lhs.getMajor() == rhs.getMajor() && lhs.getMinor() == rhs.getMinor() && lhs.getBuild() < rhs.getBuild() );
	}

	bool operator>( Version const & lhs, Version const & rhs )
	{
		return	lhs.getMajor() > rhs.getMajor()
			|| ( lhs.getMajor() == rhs.getMajor() && lhs.getMinor() > rhs.getMinor() )
			|| ( lhs.getMajor() == rhs.getMajor() && lhs.getMinor() == rhs.getMinor() && lhs.getBuild() < rhs.getBuild() );
	}

	bool operator<=( Version const & lhs, Version const & rhs )
	{
		return !( lhs > rhs );
	}

	bool operator>=( Version const & lhs, Version const & rhs )
	{
		return !( lhs < rhs );
	}
}
