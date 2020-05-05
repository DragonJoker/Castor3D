/*
See LICENSE file in root folder
*/
#ifndef ___C3D_DebugName_H___
#define ___C3D_DebugName_H___

#include "MiscellaneousModule.hpp"
#include "Castor3D/Render/RenderModule.hpp"
#include "Castor3D/Render/Passes/PassesModule.hpp"

namespace castor3d
{
	template< typename ResT >
	inline ashes::DeviceMemoryPtr setupMemory( ashes::Device const & device
		, ResT & resource
		, VkMemoryPropertyFlags flags
		, std::string const & name )
	{
		return setupMemory( device
			, resource.getMemoryRequirements()
			, flags
			, name );
	}

	template< typename ResT >
	inline ashes::DeviceMemoryPtr setupMemory( RenderDevice const & device
		, ResT & resource
		, VkMemoryPropertyFlags flags
		, std::string const & name )
	{
		return setupMemory( *device, resource, flags, name );
	}
}

#endif
