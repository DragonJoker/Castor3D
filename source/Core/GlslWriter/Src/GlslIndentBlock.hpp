/*
See LICENSE file in root folder
*/
#ifndef ___GLSL_IndentBlock_H___
#define ___GLSL_IndentBlock_H___

#include "GlslWriterPrerequisites.hpp"

namespace glsl
{
	struct IndentBlock
	{
		GlslWriter_API explicit IndentBlock( GlslWriter & p_writter );
		GlslWriter_API explicit IndentBlock( castor::StringStream & p_stream );
		GlslWriter_API ~IndentBlock();

	private:
		castor::StringStream & m_stream;
		int m_indent;
	};
}

#endif
