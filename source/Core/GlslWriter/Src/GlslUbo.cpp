#include "GlslUbo.hpp"

#include "GlslIndentBlock.hpp"
#include "GlslWriter.hpp"

namespace GLSL
{
	Ubo::Ubo( GlslWriter & p_writer
		, Castor::String const & p_name
		, uint32_t p_bind
		, Ubo::Layout p_layout )
		: m_writer( p_writer )
		, m_name( p_name )
		, m_block( nullptr )
		, m_info{ p_layout, p_bind }
	{
		m_stream << std::endl;
		m_stream << m_writer.m_keywords->GetLayout( p_layout, p_bind ) << cuT( "uniform " ) << p_name << std::endl;
		m_block = std::make_unique< IndentBlock >( m_stream );
	}

	void Ubo::End()
	{
		m_block.reset();
		m_stream << cuT( ";" ) << std::endl;

		if ( m_count )
		{
			m_writer.m_stream << m_stream.rdbuf();
		}

		m_writer.RegisterUbo( m_name, m_info );
	}
}
