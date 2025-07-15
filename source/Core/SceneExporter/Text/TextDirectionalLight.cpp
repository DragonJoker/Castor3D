#include "TextDirectionalLight.hpp"

namespace c3d
{
	TextWriter< DirectionalLight >::TextWriter( String const & tabs )
		: TextWriterT< DirectionalLight >{ tabs }
	{
	}

	bool TextWriter< DirectionalLight >::operator()( DirectionalLight const & light
		, StringStream & file )
	{
		return writeNamedSub( file, cuT( "illumination" ), light.getIllumination().lux() );
	}
}
