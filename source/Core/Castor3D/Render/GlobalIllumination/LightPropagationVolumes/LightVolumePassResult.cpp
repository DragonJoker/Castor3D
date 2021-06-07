#include "Castor3D/Render/GlobalIllumination/LightPropagationVolumes/LightVolumePassResult.hpp"

#include "Engine.hpp"
#include "Render/RenderDevice.hpp"
#include "Render/RenderSystem.hpp"

CU_ImplementCUSmartPtr( castor3d, LightVolumePassResult )

namespace castor3d
{
	LightVolumePassResult::LightVolumePassResult( crg::ResourceHandler & handler
		, RenderDevice const & device
		, castor::String const & prefix
		, uint32_t size )
		: GBufferT< LpvTexture >{ handler
			, device
			, prefix + cuT( "LPVResult" )
			, { nullptr, nullptr, nullptr }
			, 0u
			, { size, size, size } }
	{
	}
}
