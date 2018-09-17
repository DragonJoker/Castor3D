/*
See LICENSE file in root folder
*/
#ifndef ___C3DSMAA_SMAA_H___
#define ___C3DSMAA_SMAA_H___

#include <Castor3DPrerequisites.hpp>

#include "SmaaConfig.hpp"

namespace smaa
{
	castor::String getEdgeDetectionVS();
	castor::String getBlendingWeightCalculationVS();
	castor::String getNeighborhoodBlendingVS();
	castor::String getColorEdgeDetectionPS();
	castor::String getLumaEdgeDetectionPS();
	castor::String getDepthEdgeDetectionPS();
	castor::String getBlendingWeightCalculationPS();
	castor::String getNeighborhoodBlendingPS();
	castor::String getResolvePS();
	castor::String getSeparatePS();
	void writeConstants( glsl::GlslWriter & writer
		, SmaaConfig const & config
		, castor::Point4f const & renderTargetMetrics );
}

#endif
