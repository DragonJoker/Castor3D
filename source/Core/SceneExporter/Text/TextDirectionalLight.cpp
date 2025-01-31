#include "TextDirectionalLight.hpp"

namespace castor
{
	using namespace castor3d;

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
