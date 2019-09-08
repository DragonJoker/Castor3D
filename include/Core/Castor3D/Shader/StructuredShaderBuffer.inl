#include "Castor3D/Engine.hpp"
#include "Castor3D/Render/RenderSystem.hpp"
#include "Castor3D/Texture/Sampler.hpp"

#include <ashespp/Image/Image.hpp>

namespace castor3d
{
	template< typename ElementTypeTraits >
	StructuredShaderBuffer< ElementTypeTraits >::StructuredShaderBuffer( Engine & engine
		, ShaderBuffer & buffer
		, uint32_t & elementID
		, uint32_t count )
		: m_buffer{ buffer }
		, m_elementID{ elementID }
		, m_traits{ m_buffer.ptr(), count }
		, m_maxElemCount{ count }
	{
	}

	template< typename ElementTypeTraits >
	StructuredShaderBuffer< ElementTypeTraits >::~StructuredShaderBuffer()
	{
	}

	template< typename ElementTypeTraits >
	uint32_t StructuredShaderBuffer< ElementTypeTraits >::add( ElementType & element )
	{
		CU_Require( m_elements.getId() == 0u );
		CU_Require( m_elements.size() < m_maxElemCount );
		m_elements.emplace_back( &element );
		m_elements.setId( m_elementID++ );
		m_connections.emplace_back( element.onChanged.connect( [this]( ElementType const & element )
		{
			m_dirty.emplace_back( &element );
		} ) );
		m_dirty.emplace_back( &element );
		return element.getId();
	}

	template< typename ElementTypeTraits >
	void StructuredShaderBuffer< ElementTypeTraits >::remove( ElementType & element )
	{
		auto id = element.getId();
		CU_Require( id < m_elements.size() );
		CU_Require( &element == m_elements[id] );
		auto it = m_elements.erase( m_elements.begin() + id );

		for ( it; it != m_elements.end(); ++it )
		{
			( *it )->setId( id );
			++id;
		}

		m_connections.erase( m_connections.begin() + id );
		element.setId( 0u );
		m_elementID--;
	}

	template< typename ElementTypeTraits >
	void StructuredShaderBuffer< ElementTypeTraits >::update()
	{
		if ( !m_dirty.empty() )
		{
			std::vector< ElementType const * > dirty;
			std::swap( m_dirty, dirty );
			auto end = std::unique( dirty.begin(), dirty.end() );

			std::for_each( dirty.begin(), end, [this]( ElementType const * element )
			{
				element->accept( *this );
			} );

			m_buffer.update();
		}
	}

	template< typename ElementTypeTraits >
	void StructuredShaderBuffer< ElementTypeTraits >::bind( uint32_t index )const
	{
		m_buffer.bind( index );
	}
}
