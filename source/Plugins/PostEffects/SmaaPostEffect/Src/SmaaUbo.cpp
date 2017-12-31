#include "SmaaUbo.hpp"

#include <Engine.hpp>

using namespace castor;
using namespace castor3d;

namespace smaa
{
	namespace constants
	{
		const String PixelSize = cuT( "c3d_pixelSize" );
		const String Threshold = cuT( "c3d_threshold" );
		const String PredicationScale = cuT( "c3d_predicationScale" );
		const String PredicationThreshold = cuT( "c3d_predicationThreshold" );
		const String PredicationStrength = cuT( "c3d_predicationStrength" );
		const String MaxSearchSteps = cuT( "c3d_maxSearchSteps" );
		const String MaxSearchStepsDiag = cuT( "c3d_maxSearchStepsDiag" );
		const String SubsampleIndices = cuT( "c3d_subsampleIndices" );
		const String AreaTexPixelSize = cuT( "c3d_areaTexPixelSize" );
		const String AreaTexSubtexSize = cuT( "c3d_areaTexSubtexSize" );
		const String AreaTexMaxDistance = cuT( "c3d_areaTexMaxDistance" );
		const String AreaTexMaxDistanceDiag = cuT( "c3d_areaTexMaxDistanceDiag" );
		const String CornerRounding = cuT( "c3d_cornerRounding" );
	}
}
