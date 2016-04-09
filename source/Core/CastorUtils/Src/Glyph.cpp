#include "Glyph.hpp"

namespace Castor
{
	Glyph::Glyph( char32_t p_char, Size const & p_size, Position const & p_position, Size const & p_advance, ByteArray const & p_bitmap )
		: m_size( p_size )
		, m_position( p_position )
		, m_bitmap( p_bitmap )
		, m_advance( p_advance )
		, m_character( p_char )
	{
	}

	Glyph::~Glyph()
	{
		m_bitmap.clear();
	}
}
