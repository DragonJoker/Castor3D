#include "GlslPcb.hpp"

#include "GlslIndentBlock.hpp"
#include "GlslWriter.hpp"

namespace glsl
{
	Pcb::Pcb( GlslWriter & writer
		, castor::String const & name
		, castor::String const & instance )
		: m_writer{ writer }
		, m_name{ name }
	{
	}

	void Pcb::end()
	{
	}
}
