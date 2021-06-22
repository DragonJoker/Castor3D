#include "Castor3D/Render/GlobalIllumination/LightPropagationVolumes/LightVolumePassResult.hpp"

#include "Castor3D/Engine.hpp"
#include "Castor3D/Render/RenderDevice.hpp"
#include "Castor3D/Render/RenderSystem.hpp"

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
