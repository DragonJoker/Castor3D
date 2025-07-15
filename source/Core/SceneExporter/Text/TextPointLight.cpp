#include "TextPointLight.hpp"

namespace c3d
{
	TextWriter< PointLight >::TextWriter( String const & tabs )
		: TextWriterT< PointLight >{ tabs }
	{
	}

	bool TextWriter< PointLight >::operator()( PointLight const & light
		, StringStream & file )
	{
		return writeNamedSub( file, cuT( "intensity" ), light.getIntensity().candela() )
			&& writeNamedSub( file, cuT( "range" ), light.getRange() );
	}
}
