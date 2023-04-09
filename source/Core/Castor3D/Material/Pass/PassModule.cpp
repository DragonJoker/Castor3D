#include "Castor3D/Material/Pass/PassModule.hpp"
#include "Castor3D/Render/RenderModule.hpp"

CU_ImplementSmartPtr( castor3d, RenderPassRegisterInfo )

namespace castor3d
{
	castor::String getName( ParallaxOcclusionMode value )
	{
		switch ( value )
		{
		case ParallaxOcclusionMode::eNone:
			return cuT( "none" );
		case ParallaxOcclusionMode::eOne:
			return cuT( "one" );
		case ParallaxOcclusionMode::eRepeat:
			return cuT( "repeat" );
		default:
			CU_Failure( "Unsupported ParallaxOcclusionMode" );
			return castor::cuEmptyString;
		}
	}
	
	castor::String getName( BlendMode value )
	{
		switch ( value )
		{
		case BlendMode::eNoBlend:
			return cuT( "none" );
		case BlendMode::eAdditive:
			return cuT( "additive" );
		case BlendMode::eMultiplicative:
			return cuT( "multiplicative" );
		case BlendMode::eInterpolative:
			return cuT( "interpolative" );
		case BlendMode::eABuffer:
			return cuT( "a_buffer" );
		case BlendMode::eDepthPeeling:
			return cuT( "depth_peeling" );
		default:
			CU_Failure( "Unsupported BlendMode" );
			return castor::cuEmptyString;
		}
	}
}
