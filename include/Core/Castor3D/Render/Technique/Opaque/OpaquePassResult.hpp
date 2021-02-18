/*
See LICENSE file in root folder
*/
#ifndef ___C3D_OpaquePassResult_H___
#define ___C3D_OpaquePassResult_H___

#include "OpaqueModule.hpp"

#include "Castor3D/Material/Texture/TextureModule.hpp"

#include "Castor3D/Render/GBuffer.hpp"

namespace castor3d
{
	class OpaquePassResult
		: public GBufferT< DsTexture >
	{
	public:
		C3D_API OpaquePassResult( Engine & engine
			, RenderDevice const & device
			, TextureUnit const & depthTexture
			, TextureUnit const & velocityTexture );
	};
}

#endif
