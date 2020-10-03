/*
See LICENSE file in root folder
*/
#ifndef ___C3D_LightVolumePassResult_H___
#define ___C3D_LightVolumePassResult_H___

#include "LightVolumeGIModule.hpp"

#include "Castor3D/Render/GBuffer.hpp"

namespace castor3d
{
	class LightVolumePassResult
		: public GBufferT< LpvTexture >
	{
	public:
		C3D_API LightVolumePassResult( Engine & engine
			, RenderDevice const & device
			, castor::String const & prefix
			, uint32_t size );
	};
}

#endif
