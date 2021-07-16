#include "Castor3D/Material/Pass/PassModule.hpp"

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

	castor::String getName( PassFlag value )
	{
		switch ( value )
		{
		case PassFlag::eNone:
			return cuT( "none" );
		case PassFlag::eAlphaBlending:
			return cuT( "alpha_blending" );
		case PassFlag::eAlphaTest:
			return cuT( "alpha_test" );
		case PassFlag::eImageBasedLighting:
			return cuT( "image_based_lighing" );
		case PassFlag::eSubsurfaceScattering:
			return cuT( "subsurface_scattering" );
		case PassFlag::eDistanceBasedTransmittance:
			return cuT( "distance_based_transmittance" );
		case PassFlag::eParallaxOcclusionMappingOne:
			return cuT( "parallax_occlusion_mapping_one" );
		case PassFlag::eParallaxOcclusionMappingRepeat:
			return cuT( "parallax_occlusion_mapping_repeat" );
		case PassFlag::eReflection:
			return cuT( "reflection" );
		case PassFlag::eRefraction:
			return cuT( "refraction" );
		default:
			CU_Failure( "Unsupported PassFlag" );
			return castor::cuEmptyString;
		}
	}
}
