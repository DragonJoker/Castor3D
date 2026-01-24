#include "Castor3D/Material/Pass/PassModule.hpp"
#include "Castor3D/Material/Pass/Component/PassComponent.hpp"

namespace c3d
{
	String getName( ParallaxOcclusionMode value )
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
			return cuEmptyString;
		}
	}
	
	String getName( BlendMode value )
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
			return cuEmptyString;
		}
	}

	PassRPtr getComponentPass( PassComponent const & component )
	{
		return component.getOwner();
	}

	String const & getPassComponentType( PassComponent const & component )
	{
		return component.getType();
	}
}
