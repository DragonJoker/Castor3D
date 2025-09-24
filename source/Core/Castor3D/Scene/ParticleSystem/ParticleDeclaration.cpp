#include "Castor3D/Scene/ParticleSystem/ParticleDeclaration.hpp"

namespace c3d
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
				m_stride += uint32_t( getSize( m_elements[i].m_dataType ) );
			}
		}
	}
}
