#include "GlslUbo.hpp"

#include "GlslIndentBlock.hpp"
#include "GlslWriter.hpp"

namespace GLSL
{
	Ubo::Ubo( GlslWriter & p_writer
		, Castor::String const & p_name
		, Ubo::Layout p_layout )
		: m_writer( p_writer )
		, m_name( p_name )
		, m_block( nullptr )
		, m_layout( p_layout )
		, m_info{ m_layout, m_writer.m_uniformIndex }
	{
		if ( m_writer.HasConstantsBuffers() )
		{
			m_stream << std::endl;
			m_stream << m_writer.m_keywords->GetLayout( m_layout, m_writer.m_uniformIndex ) << m_writer.m_uniform << p_name << std::endl;
			m_writer.m_uniform.clear();
			m_block = std::make_unique< IndentBlock >( m_stream );
			++m_writer.m_uniformIndex;
		}
	}

	void Ubo::End()
	{
		m_block.reset();
		m_writer.m_uniform = cuT( "uniform " );

		if ( m_writer.HasConstantsBuffers() )
		{
			m_stream << cuT( ";" );
		}

		m_stream << std::endl;

		if ( m_count )
		{
			m_writer.m_stream << m_stream.rdbuf();
		}
	}
}
