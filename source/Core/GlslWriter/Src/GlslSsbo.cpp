#include "GlslSsbo.hpp"

#include "GlslIndentBlock.hpp"
#include "GlslWriter.hpp"

namespace glsl
{
	Ssbo::Ssbo( GlslWriter & writer
		, castor::String const & name
		, uint32_t bind
		, uint32_t set
		, Ssbo::Layout layout )
		: m_writer{ writer }
		, m_name{ name }
		, m_block{ nullptr }
		, m_info{ layout, bind, set }
	{
		m_stream << std::endl;
		m_stream << m_writer.m_keywords->getSsboLayout( layout, bind, set ) << cuT( "buffer " ) << name << std::endl;
		m_block = std::make_unique< IndentBlock >( m_stream );
	}

	void Ssbo::end()
	{
		m_block.reset();
		m_stream << cuT( ";" );
		m_stream << std::endl;

		if ( m_count )
		{
			m_writer.m_stream << m_stream.rdbuf();
		}

		m_writer.registerSsbo( m_name, m_info );
	}
}
