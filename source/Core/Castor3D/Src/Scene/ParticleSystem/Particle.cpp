#include "Particle.hpp"

using namespace Castor;

namespace Castor3D
{
	Particle::Particle( BufferDeclaration const & p_description, StrStrMap const & p_defaultValues )
		: m_description{ p_description }
	{
		m_data.resize( p_description.stride() );
		uint32_t l_index{ 0u };

		for ( auto l_element : m_description )
		{
			auto l_it = p_defaultValues.find( l_element.m_name );

			if ( l_it != p_defaultValues.end() && !l_it->second.empty() )
			{
				ParseValue( l_it->second, l_element.m_dataType, *this, l_index );
			}

			++l_index;
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
