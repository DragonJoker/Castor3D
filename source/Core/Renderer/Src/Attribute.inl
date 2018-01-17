/*
This file belongs to Renderer.
See LICENSE file in root folder.
*/
#include "FormatGetter.hpp"
#include "VertexLayout.hpp"

namespace renderer
{
	template< typename T >
	inline Attribute< T >::Attribute( VertexLayout & layout
		, uint32_t location
		, uint32_t offset )
		: m_attribute{ layout.createAttribute( location
			, details::FormatGetter< T >::value
			, offset ) }
	{
	}
}
