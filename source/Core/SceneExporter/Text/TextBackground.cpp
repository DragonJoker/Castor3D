#include "TextBackground.hpp"

namespace c3d
{
	TextWriter< SceneBackground >::TextWriter( String const & tabs
		, Path const & folder )
		: TextWriterT< SceneBackground >{ tabs }
		, m_folder{ folder }
	{
	}

	bool TextWriter< SceneBackground >::operator()( SceneBackground const & background
		, StringStream & file )
	{
		auto result = background.write( tabs(), m_folder, file );
		checkError( result, cuT( "" ) );
		return result;
	}
}
