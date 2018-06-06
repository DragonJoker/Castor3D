/*
See LICENSE file in root folder
*/
#ifndef ___C3DSMAA_SmaaUbo_H___
#define ___C3DSMAA_SmaaUbo_H___

#include <PostEffect/PostEffectSurface.hpp>

#include <GlslShader.hpp>

namespace smaa
{
	namespace constants
	{
		extern const castor::String RenderTargetMetrics;
		extern const castor::String Predication;
		extern const castor::String Threshold;
		extern const castor::String LocalContrastAdaptationFactor;
		extern const castor::String DepthThreshold;
		extern const castor::String PredicationScale;
		extern const castor::String PredicationThreshold;
		extern const castor::String PredicationStrength;
		extern const castor::String MaxSearchSteps;
		extern const castor::String MaxSearchStepsDiag;
		extern const castor::String SubsampleIndices;
		extern const castor::String AreaTexPixelSize;
		extern const castor::String AreaTexSubtexSize;
		extern const castor::String AreaTexMaxDistance;
		extern const castor::String AreaTexMaxDistanceDiag;
		extern const castor::String CornerRounding;
		extern const castor::String SearchTexSize;
		extern const castor::String SearchTexPackedSize;
		extern const castor::String CornerRoundingNorm;
		extern const castor::String DisableDiagDetection;
		extern const castor::String DisableCornerDetection;
		extern const castor::String Reprojection;
		extern const castor::String ReprojectionWeightScale;
	}
}

#endif
