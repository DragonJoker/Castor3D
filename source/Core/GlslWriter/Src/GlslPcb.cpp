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
		, m_block{ nullptr }
	{
		m_stream.imbue( Expr::getLocale() );
		m_stream << std::endl;

		if ( writer.hasPushConstants() )
		{
			m_stream << cuT( "layout ( push_constant ) " ) << cuT( "uniform " ) << name << std::endl;
			m_block = std::make_unique< IndentBlock >( m_stream );
		}
	}

	void Pcb::end()
	{
		if ( m_writer.hasPushConstants() )
		{
			m_block.reset();
			m_stream << cuT( ";" ) << std::endl;
		}

		if ( m_count )
		{
			m_writer.m_stream << m_stream.rdbuf();
		}
	}
}
