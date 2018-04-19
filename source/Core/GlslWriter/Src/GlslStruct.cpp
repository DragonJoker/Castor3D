#include "GlslStruct.hpp"

#include "GlslIndentBlock.hpp"
#include "GlslWriter.hpp"

namespace glsl
{
	Struct::Struct( GlslWriter & writer, castor::String const & p_name )
		: m_writer( writer )
		, m_name( p_name )
		, m_block( nullptr )
	{
		m_writer << glsl::endl;
		m_writer << cuT( "struct " ) << p_name << glsl::endl;
		m_block = new IndentBlock( m_writer );
	}

	Struct::Struct( GlslWriter & writer, castor::String const & p_name, castor::String const & p_instName )
		: m_writer( writer )
		, m_name( p_name )
		, m_instName( p_instName )
		, m_block( nullptr )
	{
	}

	void Struct::end()
	{
		delete m_block;
		m_block = nullptr;
		m_writer << cuT( ";" ) << glsl::endl;
	}
}
