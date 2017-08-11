#include "GlslSsbo.hpp"

#include "GlslIndentBlock.hpp"
#include "GlslWriter.hpp"

namespace glsl
{
	Ssbo::Ssbo( GlslWriter & p_writer
		, castor::String const & p_name
		, uint32_t p_bind
		, Ssbo::Layout p_layout )
		: m_writer( p_writer )
		, m_name( p_name )
		, m_block( nullptr )
		, m_info{ p_layout, p_bind }
	{
		m_stream << std::endl;
		m_stream << m_writer.m_keywords->getLayout( p_layout, p_bind ) << cuT( "buffer " ) << p_name << std::endl;
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
