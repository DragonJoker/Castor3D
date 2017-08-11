#include "GlslWriterPrerequisites.hpp"

#include "GlslWriter.hpp"

namespace glsl
{
	void writeLine( GlslWriter & p_writer, castor::String const & p_line )
	{
		p_writer << p_line << Endl{};
	}

	void registerName( GlslWriter & p_writer, castor::String const & p_name, TypeName p_type )
	{
		p_writer.registerName( p_name, p_type );
	}

	void checkNameExists( GlslWriter & p_writer, castor::String const & p_name, TypeName p_type )
	{
		p_writer.checkNameExists( p_name, p_type );
	}
}
