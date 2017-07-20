#include "Particle.hpp"

using namespace Castor;

namespace Castor3D
{
	Particle::Particle( BufferDeclaration const & p_description, StrStrMap const & p_defaultValues )
		: m_description{ p_description }
	{
		m_data.resize( p_description.stride() );
		uint32_t index{ 0u };

		for ( auto element : m_description )
		{
			auto it = p_defaultValues.find( element.m_name );

			if ( it != p_defaultValues.end() && !it->second.empty() )
			{
				ParseValue( it->second, element.m_dataType, *this, index );
			}

			++index;
		}
	}

	Particle::Particle( BufferDeclaration const & p_description )
		: m_description{ p_description }
	{
		m_data.resize( p_description.stride() );
	}

	Particle::Particle( Particle const & p_rhs )
		: m_description{ p_rhs.m_description }
		, m_data{ p_rhs.m_data }
	{
	}

	Particle::Particle( Particle && p_rhs )
		: m_description{ p_rhs.m_description }
		, m_data{ std::move( p_rhs.m_data ) }
	{
	}

	Particle & Particle::operator=( Particle const & p_rhs )
	{
		m_data = p_rhs.m_data;
		return *this;
	}

	Particle & Particle::operator=( Particle && p_rhs )
	{
		m_data = std::move( p_rhs.m_data );
		return *this;
	}
}
