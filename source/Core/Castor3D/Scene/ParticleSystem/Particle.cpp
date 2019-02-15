#include "Castor3D/Scene/ParticleSystem/Particle.hpp"

using namespace castor;

namespace castor3d
{
	Particle::Particle( ParticleDeclaration const & description, StrStrMap const & defaultValues )
		: m_description{ description }
	{
		m_data.resize( description.stride() );
		uint32_t index{ 0u };

		for ( auto element : m_description )
		{
			auto it = defaultValues.find( element.m_name );

			if ( it != defaultValues.end() && !it->second.empty() )
			{
				parseValue( it->second, element.m_dataType, *this, index );
			}

			++index;
		}
	}

	Particle::Particle( ParticleDeclaration const & description )
		: m_description{ description }
	{
		m_data.resize( description.stride() );
	}

	Particle::Particle( Particle const & rhs )
		: m_description{ rhs.m_description }
		, m_data{ rhs.m_data }
	{
	}

	Particle::Particle( Particle && rhs )
		: m_description{ rhs.m_description }
		, m_data{ std::move( rhs.m_data ) }
	{
	}

	Particle & Particle::operator=( Particle const & rhs )
	{
		m_data = rhs.m_data;
		return *this;
	}

	Particle & Particle::operator=( Particle && rhs )
	{
		m_data = std::move( rhs.m_data );
		return *this;
	}
}
