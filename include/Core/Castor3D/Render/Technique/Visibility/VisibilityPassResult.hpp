/*
See LICENSE file in root folder
*/
#ifndef ___C3D_VisibilityPassResult_H___
#define ___C3D_VisibilityPassResult_H___

#include "VisibilityModule.hpp"

#include "Castor3D/Material/Texture/TextureModule.hpp"

#include "Castor3D/Render/GBuffer.hpp"

namespace castor3d
{
	class VisibilityPassResult
		: public GBufferT< VbTexture >
	{
	public:
		C3D_API VisibilityPassResult( crg::ResourceHandler & handler
			, RenderDevice const & device
			, TexturePtr depthTexture );
	};
}

#endif
