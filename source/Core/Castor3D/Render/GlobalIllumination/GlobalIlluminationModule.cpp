#include "Castor3D/Render/GlobalIllumination/GlobalIlluminationModule.hpp"

namespace castor3d
{
	castor::String getName( GlobalIlluminationType giType )
	{
		static castor::Array< castor::String, size_t( GlobalIlluminationType::eCount ) > const Values
		{
			{
				cuT( "none" ),
				cuT( "rsm" ),
				cuT( "vct" ),
				cuT( "lpv" ),
				cuT( "lpv_geometry" ),
				cuT( "layered_lpv" ),
				cuT( "layered_lpv_geometry" ),
			}
		};
		return Values[size_t( giType )];
	}
}
