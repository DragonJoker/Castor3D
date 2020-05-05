/*
See LICENSE file in root folder
*/
#ifndef ___C3D_DebugConfiguration_H___
#define ___C3D_DebugConfiguration_H___

#include "UbosModule.hpp"
#include "Castor3D/Miscellaneous/MiscellaneousModule.hpp"

#include <cstdint>

namespace castor3d
{
	struct DebugConfig
	{
		uint32_t debugIndex{ 0u };
		int32_t debugLightEye{ 0 };
		int32_t debugLightVSPosition{ 0 };
		int32_t debugLightWSPosition{ 0 };
		int32_t debugLightWSNormal{ 0 };
		int32_t debugLightTexCoord{ 0 };
		int32_t debugDeferredDiffuseLighting{ 0 };
		int32_t debugDeferredSpecularLighting{ 0 };
		int32_t debugDeferredOcclusion{ 0 };
		int32_t debugDeferredSSSTransmittance{ 0 };
		int32_t debugDeferredIBL{ 0 };
		int32_t debugDeferredWSNormal{ 0 };
		int32_t debugDeferredVSNormal{ 0 };
		int32_t debugDeferredCSNormal{ 0 };
		int32_t debugDeferredWSPosition{ 0 };
		int32_t debugDeferredVSPosition{ 0 };
		int32_t debugDeferredCSPosition{ 0 };
		int32_t debugDeferredDepth{ 0 };
		int32_t debugDeferredData1{ 0 };
		int32_t debugDeferredData2{ 0 };
		int32_t debugDeferredData3{ 0 };
		int32_t debugDeferredData4{ 0 };
		int32_t debugDeferredData5{ 0 };

		void accept( PipelineVisitor & visitor );
	};
}

#endif
