#include "Castor3D/Render/GlobalIllumination/LightPropagationVolumes/LightVolumePassResult.hpp"

#include "Render/RenderDevice.hpp"
#include "Render/RenderSystem.hpp"

CU_ImplementCUSmartPtr( castor3d, LightVolumePassResult )

namespace castor3d
{
	LightVolumePassResult::LightVolumePassResult( crg::FrameGraph & graph
		, RenderDevice const & device
		, castor::String const & prefix
		, uint32_t size )
		: GBufferT< LpvTexture >{ graph
			, device
			, prefix + cuT( "LPVResult" )
			, { nullptr, nullptr, nullptr }
			, 0u
			, { size, size, size } }
	{
	}
}
