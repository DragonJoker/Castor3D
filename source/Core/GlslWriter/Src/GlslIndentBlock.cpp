#include "GlslIndentBlock.hpp"

#include "GlslWriter.hpp"

#include <Stream/StreamIndentManipulators.hpp>

using namespace Castor;

namespace GLSL
{
	using Castor::operator<<;

	//*****************************************************************************************

	IndentBlock::IndentBlock( GlslWriter & p_writter )
		: m_stream( p_writter.m_stream )
	{
		using namespace Castor;
		m_stream << cuT( "{" );
		m_indent = format::get_indent( m_stream );
		m_stream << format::indent( m_indent + 4 );
		m_stream << std::endl;
	}

	IndentBlock::IndentBlock( Castor::StringStream & p_stream )
		: m_stream( p_stream )
	{
		using namespace Castor;
		m_stream << cuT( "{" );
		m_indent = format::get_indent( m_stream );
		m_stream << format::indent( m_indent + 4 );
		m_stream << std::endl;
	}

	IndentBlock::~IndentBlock()
	{
		using namespace Castor;
		m_stream << format::indent( m_indent );
		m_stream << cuT( "}" );
	}
}
