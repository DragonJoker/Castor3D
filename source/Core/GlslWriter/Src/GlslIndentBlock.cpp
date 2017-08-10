#include "GlslIndentBlock.hpp"

#include "GlslWriter.hpp"

#include <Stream/StreamIndentManipulators.hpp>

using namespace castor;

namespace GLSL
{
	using castor::operator<<;

	//*****************************************************************************************

	IndentBlock::IndentBlock( GlslWriter & p_writter )
		: m_stream( p_writter.m_stream )
	{
		using namespace castor;
		m_stream << cuT( "{" );
		m_indent = format::getIndent( m_stream );
		m_stream << format::Indent( m_indent + 4 );
		m_stream << std::endl;
	}

	IndentBlock::IndentBlock( castor::StringStream & p_stream )
		: m_stream( p_stream )
	{
		using namespace castor;
		m_stream << cuT( "{" );
		m_indent = format::getIndent( m_stream );
		m_stream << format::Indent( m_indent + 4 );
		m_stream << std::endl;
	}

	IndentBlock::~IndentBlock()
	{
		using namespace castor;
		m_stream << format::Indent( m_indent );
		m_stream << cuT( "}" );
	}
}
