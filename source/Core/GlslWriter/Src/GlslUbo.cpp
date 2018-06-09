#include "GlslUbo.hpp"

#include "GlslIndentBlock.hpp"
#include "GlslWriter.hpp"

namespace glsl
{
	Ubo::Ubo( GlslWriter & writer
		, castor::String const & name
		, uint32_t bind
		, uint32_t set
		, Ubo::Layout layout )
		: m_writer{ writer }
		, m_name{ name }
		, m_block{ nullptr }
		, m_info{ layout, bind, set }
	{
		m_stream.imbue( Expr::getLocale() );
		m_stream << std::endl;
		m_stream << m_writer.m_keywords->getUboLayout( layout, bind, set ) << cuT( "uniform " ) << name << std::endl;
		m_block = std::make_unique< IndentBlock >( m_stream );
	}

	void Ubo::end()
	{
		m_block.reset();
		m_stream << cuT( ";" ) << std::endl;

		if ( m_count )
		{
			m_writer.m_stream << m_stream.rdbuf();
		}

		m_writer.registerUbo( m_name, m_info );
	}
}
