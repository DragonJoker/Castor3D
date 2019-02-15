#include "SmaaUbo.hpp"

#include <Engine.hpp>

using namespace castor;
using namespace castor3d;

namespace smaa
{
	namespace constants
	{
		// General
		const String RenderTargetMetrics = cuT( "SMAA_RT_METRICS" );

		// Edge detection
		const String Predication = cuT( "SMAA_PREDICATION" );
		const String Threshold = cuT( "SMAA_THRESHOLD" );
		const String LocalContrastAdaptationFactor = cuT( "SMAA_LOCAL_CONTRAST_ADAPTATION_FACTOR" );
		const String DepthThreshold = cuT( "SMAA_DEPTH_THRESHOLD" );
		const String PredicationScale = cuT( "SMAA_PREDICATION_SCALE" );
		const String PredicationThreshold = cuT( "SMAA_PREDICATION_THRESHOLD" );
		const String PredicationStrength = cuT( "SMAA_PREDICATION_STRENGTH" );

		// Blending weights calculation
		const String MaxSearchSteps = cuT( "SMAA_MAX_SEARCH_STEPS" );
		const String DisableDiagDetection = cuT( "SMAA_DISABLE_DIAG_DETECTION" );
		const String MaxSearchStepsDiag = cuT( "SMAA_MAX_SEARCH_STEPS_DIAG" );
		const String AreaTexMaxDistanceDiag = cuT( "SMAA_AREATEX_MAX_DISTANCE_DIAG" );
		const String AreaTexPixelSize = cuT( "SMAA_AREATEX_PIXEL_SIZE" );
		const String AreaTexSubtexSize = cuT( "SMAA_AREATEX_SUBTEX_SIZE" );
		const String AreaTexMaxDistance = cuT( "SMAA_AREATEX_MAX_DISTANCE" );
		const String SearchTexSize = cuT( "SMAA_SEARCHTEX_SIZE" );
		const String SearchTexPackedSize = cuT( "SMAA_SEARCHTEX_PACKED_SIZE" );
		const String DisableCornerDetection = cuT( "SMAA_DISABLE_CORNER_DETECTION" );
		const String CornerRoundingNorm = cuT( "SMAA_CORNER_ROUNDING_NORM" );
		const String CornerRounding = cuT( "SMAA_CORNER_ROUNDING" );

		// Reprojection
		const String Reprojection = cuT( "SMAA_REPROJECTION" );
		const String ReprojectionWeightScale = cuT( "SMAA_REPROJECTION_WEIGHT_SCALE" );

		const String SubsampleIndices = cuT( "c3d_subsampleIndices" );
	}
}
