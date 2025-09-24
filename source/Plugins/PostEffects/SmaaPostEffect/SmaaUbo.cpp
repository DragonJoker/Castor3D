#include "SmaaPostEffect/SmaaUbo.hpp"

#include <Castor3D/Engine.hpp>
#include <Castor3D/Buffer/UniformBufferPool.hpp>
#include <Castor3D/Render/RenderDevice.hpp>

#include <ShaderWriter/Writer.hpp>

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
		, cornerRoundingNorm{ writer.cast< sdw::Float >( cornerRounding ) / 100.0_f }
		, depthThreshold{ threshold * 0.1_f }
	{
	}

	//*********************************************************************************************

	c3d::MbString const SmaaUbo::Buffer = "SmaaBuffer";
	c3d::MbString const SmaaUbo::Data = "c3d_smaaData";

	SmaaUbo::SmaaUbo( c3d::RenderDevice const & device )
		: m_device{ device }
		, m_ubo{ m_device.uboPool->getBuffer< SmaaUboConfiguration >( c3d::MemoryPropertyFlags::eDeviceLocal ) }
	{
	}

	SmaaUbo::~SmaaUbo()
	{
		m_device.uboPool->putBuffer< SmaaUboConfiguration >( m_ubo );
	}

	void SmaaUbo::cpuUpdate( c3d::Size const & renderSize
		, SmaaConfig const & config )
	{
		auto & data = m_ubo.getData();
		data.rtMetrics = { 1.0f / float( renderSize.getWidth() )
			, 1.0f / float( renderSize.getHeight() )
			, float( renderSize.getWidth() )
			, float( renderSize.getHeight() ) };
		data.predication = { config.data.threshold
			, config.data.predicationThreshold
			, config.data.predicationScale
			, config.data.predicationStrength };
		data.subsampleIndices = config.subsampleIndices[config.subsampleIndex];
		data.searchSizes = { config.data.searchTexSize->x
			, config.data.searchTexSize->y
			, config.data.searchTexPackedSize->x
			, config.data.searchTexPackedSize->y };
		data.areaTexPixelSizeAndLocalContrast = { config.data.areaTexPixelSize->x
			, config.data.areaTexPixelSize->y
			, config.data.localContrastAdaptationFactor
			, config.data.cornerRounding };
		data.areaTexSizesReprojWS = { config.data.areaTexMaxDistance
			, config.data.areaTexMaxDistanceDiag
			, config.data.areaTexSubtexSize
			, config.data.reprojectionWeightScale };
		data.maxsSearchSteps = { config.data.maxSearchSteps
			, config.data.maxSearchStepsDiag
			, 0
			, 0 };
		data.tweaks = { config.data.disableCornerDetection ? 1 : 0
			, config.data.disableDiagonalDetection ? 1 : 0
			, config.data.enableReprojection ? 1 : 0
			, 0 };
	}

	//*********************************************************************************************
}
