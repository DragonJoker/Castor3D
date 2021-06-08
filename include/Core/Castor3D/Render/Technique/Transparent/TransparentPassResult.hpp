/*
See LICENSE file in root folder
*/
#ifndef ___C3D_TransparentPassResult_H___
#define ___C3D_TransparentPassResult_H___

#include "TransparentModule.hpp"

#include "Castor3D/Render/GBuffer.hpp"

namespace castor3d
{
	class TransparentPassResult
		: public GBufferT< WbTexture >
	{
	public:
		C3D_API TransparentPassResult( crg::ResourceHandler & handler
			, RenderDevice const & device
			, Texture const & depthTexture
			, Texture const & velocityTexture );
	};
}

#endif
