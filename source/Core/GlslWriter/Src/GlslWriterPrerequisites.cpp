#include "GlslWriterPrerequisites.hpp"

#include "GlslWriter.hpp"

namespace GLSL
{
	void WriteLine( GlslWriter & p_writer, Castor::String const & p_line )
	{
		p_writer << p_line << Endl{};
	}

	void RegisterName( GlslWriter & p_writer, Castor::String const & p_name, TypeName p_type )
	{
		p_writer.RegisterName( p_name, p_type );
	}

	void CheckNameExists( GlslWriter & p_writer, Castor::String const & p_name, TypeName p_type )
	{
		p_writer.CheckNameExists( p_name, p_type );
	}
}
