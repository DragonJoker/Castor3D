/*
See LICENSE file in root folder
*/
#include "Castor3D/Miscellaneous/DebugName.hpp"

#include "Castor3D/Miscellaneous/makeVkType.hpp"
#include "Castor3D/Render/RenderDevice.hpp"

#include <ashespp/Core/Device.hpp>

namespace c3d
{
	ashes::DeviceMemoryPtr setupMemory( ashes::Device const & device
		, VkMemoryRequirements const & requirements
		, MemoryPropertyFlags flags
		, String const & name )
	{
		uint32_t deduced = device.deduceMemoryType( requirements.memoryTypeBits
			, getMemoryPropertyFlags( flags ) );
		auto memory = device.allocateMemory( toUtf8( name + cuT( "Mem" ) )
			, makeVkStruct< VkMemoryAllocateInfo >( requirements.size, deduced ) );
		return memory;
	}

	ashes::DeviceMemoryPtr setupMemory( RenderDevice const & device
		, VkMemoryRequirements const & requirements
		, MemoryPropertyFlags flags
		, String const & name )
	{
		return setupMemory( *device, requirements, flags, name );
	}
}
