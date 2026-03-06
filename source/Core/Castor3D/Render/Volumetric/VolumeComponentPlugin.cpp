#include "Castor3D/Render/Volumetric/VolumeComponentPlugin.hpp"

CU_ImplementSmartPtr( c3d, VolumeComponentPlugin )

namespace c3d
{
	VolumeComponentPlugin::~VolumeComponentPlugin() = default;
	VolumeComponentPlugin::VolumeComponentPlugin( VolumeComponentPlugin && rhs )noexcept = default;
}
