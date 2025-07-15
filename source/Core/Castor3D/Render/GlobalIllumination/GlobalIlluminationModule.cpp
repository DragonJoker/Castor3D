#include "Castor3D/Render/GlobalIllumination/GlobalIlluminationModule.hpp"

namespace c3d
{
	String getName( GlobalIlluminationType giType )
	{
		static Array< String, size_t( GlobalIlluminationType::eCount ) > const Values
		{
			{
				cuT( "none" ),
				cuT( "rsm" ),
				cuT( "lpv" ),
				cuT( "lpv_geometry" ),
				cuT( "layered_lpv" ),
				cuT( "layered_lpv_geometry" ),
			}
		};
		return Values[size_t( giType )];
	}
}
