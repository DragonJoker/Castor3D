#include "Castor3D/Scene/ParticleSystem/ParticleDeclaration.hpp"

using namespace castor;

namespace castor3d
{
	ParticleDeclaration::ParticleDeclaration()
		: ParticleDeclaration( nullptr, 0 )
	{
	}

	ParticleDeclaration::ParticleDeclaration( ParticleElementDeclaration const * elements, uint32_t count )
		: m_stride( 0 )
	{
		if ( elements && count )
		{
			for ( uint32_t i = 0; i < count; i++ )
			{
				m_elements.push_back( elements[i] );
				m_elements[i].m_offset = m_stride;
				m_stride += getSize( m_elements[i].m_dataType );
			}
		}
	}

	ParticleDeclaration::~ParticleDeclaration()
	{
	}

	bool operator==( ParticleDeclaration const & lhs, ParticleDeclaration const & rhs )
	{
		bool result = lhs.stride() == rhs.stride() && lhs.size() == rhs.size();
		auto itl = lhs.begin();
		auto itr = rhs.begin();

		while ( result && itl != lhs.end() )
		{
			result = *itl++ == *itr++;
			++itl;
			++itr;
		}

		return result;
	}

	bool operator!=( ParticleDeclaration const & lhs, ParticleDeclaration const & rhs )
	{
		return !( lhs == rhs );
	}
}
