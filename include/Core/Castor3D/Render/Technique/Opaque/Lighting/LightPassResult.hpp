/*
See LICENSE file in root folder
*/
#ifndef ___C3D_LightPassResult_H___
#define ___C3D_LightPassResult_H___

#include "LightingModule.hpp"

#include "Castor3D/Render/GBuffer.hpp"

namespace castor3d
{
	class LightPassResult
		: public GBufferT< LpTexture >
	{
	public:
		C3D_API LightPassResult( crg::ResourceHandler & handler
			, RenderDevice const & device
			, castor::Size const & size );
	};
}

#endif
