#include "TextSpotLight.hpp"

namespace c3d
{
	TextWriter< SpotLight >::TextWriter( String const & tabs )
		: TextWriterT< SpotLight >{ tabs }
	{
	}

	bool TextWriter< SpotLight >::operator()( SpotLight const & light
		, StringStream & file )
	{
		return writeNamedSub( file, cuT( "intensity" ), light.getIntensity().candela() )
			&& writeNamedSub( file, cuT( "range" ), light.getRange() )
			&& write( file, cuT( "exponent" ), light.getExponent() )
			&& write( file, cuT( "inner_cut_off" ), light.getInnerCutOff().degrees() )
			&& write( file, cuT( "outer_cut_off" ), light.getOuterCutOff().degrees() );
	}
}
