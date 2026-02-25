#include "Castor3D/Shader/Ubos/RsmConfigUbo.hpp"

#include "Castor3D/Scene/Scene.hpp"

namespace c3d
{
	//*********************************************************************************************

	RsmConfigUbo::RsmConfigUbo( RenderDevice const & device )
		: UboT{ device, MemoryPropertyFlags::eNone }
	{
	}

	void RsmConfigUbo::cpuUpdate( RsmConfig const & rsmConfig
		, uint32_t index )
	{
		auto & rsmData = getNCData();
		rsmData.intensity = *rsmConfig.intensity;
		rsmData.maxRadius = *rsmConfig.maxRadius;
		rsmData.sampleCount = rsmConfig.sampleCount.value().value();
		rsmData.index = int32_t( index );
	}

	//*********************************************************************************************
}
