/*
See LICENSE file in root folder
*/
#ifndef ___C3D_ShadowMapResult_H___
#define ___C3D_ShadowMapResult_H___

#include "ShadowMapModule.hpp"

#include "Castor3D/Render/GBuffer.hpp"

namespace castor3d
{
	class ShadowMapResult
		: public GBufferT< SmTexture >
	{
	public:
		C3D_API ShadowMapResult( Engine & engine
			, castor::String const & prefix
			, VkImageCreateFlags createFlags
			, castor::Size const & size
			, uint32_t layerCount );

	private:
		TextureUnitArray m_owned;
	};
}

#endif
