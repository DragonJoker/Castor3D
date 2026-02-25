#include "SmaaPostEffect/SmaaUbo.hpp"

namespace smaa
{
	//*********************************************************************************************

	SmaaData::SmaaData( sdw::ShaderWriter & writer
		, ast::expr::ExprPtr expr
		, bool enabled )
		: StructInstanceHelperT{ writer, c3d::move( expr ), enabled }
		, rtMetrics{ getMember< "rtMetrics" >() }
		, threshold{ getMember< "threshold" >() }
		, predicationThreshold{ getMember< "predicationThreshold" >() }
		, predicationScale{ getMember< "predicationScale" >() }
		, predicationStrength{ getMember< "predicationStrength" >() }
		, subsampleIndices{ getMember< "subsampleIndices" >() }
		, searchTexSize{ getMember< "searchTexSize" >() }
		, searchTexPackedSize{ getMember < "searchTexPackedSize" >() }
		, areaTexPixelSize{ getMember < "areaTexPixelSize" >() }
		, localContrastAdaptationFactor{ getMember < "localContrastAdaptationFactor" >() }
		, cornerRounding{ getMember < "cornerRounding" >() }
		, areaTexMaxDistance{ getMember < "areaTexMaxDistance" >() }
		, areaTexMaxDistanceDiag{ getMember < "areaTexMaxDistanceDiag" >() }
		, areaTexSubtexSize{ getMember < "areaTexSubtexSize" >() }
		, reprojectionWeightScale{ getMember < "reprojectionWeightScale" >() }
		, maxSearchSteps{ getMember < "maxSearchSteps" >() }
		, maxSearchStepsDiag{ getMember < "maxSearchStepsDiag" >() }
		, disableCornerDetection{ getMember < "disableCornerDetection" >() }
		, disableDiagonalDetection{ getMember < "disableDiagonalDetection" >() }
		, enableReprojection{ getMember < "enableReprojection" >() }
		, cornerRoundingNorm{ getMember < "cornerRoundingNorm" >() }
		, depthThreshold{ getMember < "depthThreshold" >() }
	{
	}

	//*********************************************************************************************

	c3d::MbString const SmaaUbo::Buffer = "SmaaBuffer";
	c3d::MbString const SmaaUbo::Data = "c3d_smaaData";

	SmaaUbo::SmaaUbo( c3d::RenderDevice const & device )
		: UboT{ device, c3d::MemoryPropertyFlags::eDeviceLocal }
	{
	}

	void SmaaUbo::cpuUpdate( c3d::Size const & renderSize
		, SmaaConfig const & config )
	{
		auto & data = getNCData();
		data.rtMetrics = { 1.0f / float( renderSize.getWidth() )
			, 1.0f / float( renderSize.getHeight() )
			, float( renderSize.getWidth() )
			, float( renderSize.getHeight() ) };
		data.threshold = config.data.threshold;
		data.predicationThreshold = config.data.predicationThreshold;
		data.predicationScale = config.data.predicationScale;
		data.predicationStrength = config.data.predicationStrength;
		data.subsampleIndices = config.subsampleIndices[config.subsampleIndex];
		data.searchTexSize = config.data.searchTexSize;
		data.searchTexPackedSize = config.data.searchTexPackedSize;
		data.areaTexPixelSize = config.data.areaTexPixelSize;
		data.localContrastAdaptationFactor = config.data.localContrastAdaptationFactor;
		data.cornerRounding = config.data.cornerRounding;
		data.areaTexMaxDistance = config.data.areaTexMaxDistance;
		data.areaTexMaxDistanceDiag = config.data.areaTexMaxDistanceDiag;
		data.areaTexSubtexSize = config.data.areaTexSubtexSize;
		data.reprojectionWeightScale = config.data.reprojectionWeightScale;
		data.maxSearchSteps = config.data.maxSearchSteps;
		data.maxSearchStepsDiag = config.data.maxSearchStepsDiag;
		data.disableCornerDetection = ( config.data.disableCornerDetection ? 1 : 0 );
		data.disableDiagonalDetection = ( config.data.disableDiagonalDetection ? 1 : 0 );
		data.enableReprojection = ( config.data.enableReprojection ? 1 : 0 );
		data.cornerRoundingNorm = float( config.data.cornerRounding ) / 100.0f;
		data.depthThreshold = config.data.threshold * 0.1f;
	}

	//*********************************************************************************************
}
