/*
See LICENSE file in root folder
*/
#ifndef ___C3D_ShadowMapResult_H___
#define ___C3D_ShadowMapResult_H___

#include "ShadowMapModule.hpp"

#include "Castor3D/Render/GBuffer.hpp"

namespace c3d
{
	class ShadowMapResult
		: public GBufferT< SmTexture >
	{
	public:
		C3D_API ShadowMapResult( crg::ResourcesCache & resources
			, RenderDevice const & device
			, String const & prefix
			, ImageCreateFlags createFlags
			, Size const & size
			, uint32_t layerCount );
	};
}

#endif
