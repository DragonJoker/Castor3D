#include "Glyph.hpp"

namespace Castor
{
	Glyph::Glyph( wchar_t p_wcChar, Size const & p_size, Position const & p_position, Size const & p_advance, ByteArray const & p_bitmap )
		: m_size( p_size )
		, m_position( p_position )
		, m_bitmap( p_bitmap )
		, m_advance( p_advance )
		, m_wcCharacter( p_wcChar )
	{
	}

	Glyph::~Glyph()
	{
		m_bitmap.clear();
	}
}
