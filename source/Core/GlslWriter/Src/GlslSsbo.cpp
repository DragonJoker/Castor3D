#include "GlslSsbo.hpp"

#include "GlslIndentBlock.hpp"
#include "GlslWriter.hpp"

namespace GLSL
{
	using Castor::operator<<;

	Ssbo::Ssbo( GlslWriter & p_writer, Castor::String const & p_name, Ssbo::Layout p_layout )
		: m_writer( p_writer )
		, m_name( p_name )
		, m_block( nullptr )
		, m_layout( p_layout )
		, m_info{ m_layout, m_writer.m_uniformIndex }
	{
		m_stream << std::endl;
		m_stream << m_writer.m_keywords->GetLayout( m_layout, m_writer.m_uniformIndex ) << cuT( "buffer " ) << p_name << std::endl;
		m_block = std::make_unique< IndentBlock >( m_stream );
		++m_writer.m_uniformIndex;
	}

	void Ssbo::End()
	{
		m_block.reset();
		m_stream << cuT( ";" );
		m_stream << std::endl;

		if ( m_count )
		{
			m_writer.m_stream << m_stream.rdbuf();
		}
	}
}
