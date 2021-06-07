/*
See LICENSE file in root folder
*/
#ifndef ___C3D_LightVolumePassResult_H___
#define ___C3D_LightVolumePassResult_H___

#include "LightPropagationVolumesModule.hpp"

#include "Castor3D/Render/GBuffer.hpp"

namespace castor3d
{
	class LightVolumePassResult
		: public GBufferT< LpvTexture >
	{
	public:
		C3D_API LightVolumePassResult( crg::ResourceHandler & handler
			, RenderDevice const & device
			, castor::String const & prefix
			, uint32_t size );
	};
}

#endif
