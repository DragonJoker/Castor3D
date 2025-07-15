#include "Castor3D/Render/GlobalIllumination/LightPropagationVolumes/LightVolumePassResult.hpp"

#include "Castor3D/Engine.hpp"
#include "Castor3D/Render/RenderDevice.hpp"
#include "Castor3D/Render/RenderSystem.hpp"

CU_ImplementSmartPtr( c3d, LightVolumePassResult )

namespace c3d
{
	LightVolumePassResult::LightVolumePassResult( crg::ResourcesCache & handler
		, RenderDevice const & device
		, String const & prefix
		, uint32_t size )
		: GBufferT< LpvTexture >{ handler
			, device
			, prefix + cuT( "LPVResult" )
			, { nullptr, nullptr, nullptr }
			, ImageCreateFlags::eNone
			, { size, size, size } }
	{
	}
}
