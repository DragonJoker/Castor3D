#include "Castor3D/Render/GlobalIllumination/LightPropagationVolumes/LightVolumePassResult.hpp"

CU_ImplementCUSmartPtr( castor3d, LightVolumePassResult )

namespace castor3d
{
	LightVolumePassResult::LightVolumePassResult( Engine & engine
		, RenderDevice const & device
		, castor::String const & prefix
		, uint32_t size )
		: GBufferT< LpvTexture >
		{
			engine,
			prefix + cuT( "LPVResult" ),
			{ nullptr, nullptr, nullptr },
			0u,
			{ size, size, size },
		}
	{
		initialise( device );
	}
}
