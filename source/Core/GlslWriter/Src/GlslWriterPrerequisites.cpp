#include "GlslWriterPrerequisites.hpp"

#include "GlslWriter.hpp"

namespace GLSL
{
	void WriteLine( GlslWriter & p_writer, Castor::String const & p_line )
	{
		p_writer << p_line << Endl{};
	}
}
