/*
See LICENSE file in root folder
*/
#include "Castor3D/Miscellaneous/DebugName.hpp"

#include "Castor3D/Miscellaneous/Logger.hpp"
#include "Castor3D/Miscellaneous/makeVkType.hpp"
#include "Castor3D/Render/RenderDevice.hpp"
#include "Castor3D/Render/Passes/CommandsSemaphore.hpp"

#include <CastorUtils/Log/Logger.hpp>

#include <ashespp/Core/Device.hpp>
#include <ashespp/Miscellaneous/DeviceMemory.hpp>

namespace castor3d
{
	ashes::DeviceMemoryPtr setupMemory( ashes::Device const & device
		, VkMemoryRequirements const & requirements
		, VkMemoryPropertyFlags flags
		, std::string const & name )
	{
		uint32_t deduced = device.deduceMemoryType( requirements.memoryTypeBits
			, flags );
		auto memory = device.allocateMemory( name + "Mem"
			, makeVkStruct< VkMemoryAllocateInfo >( requirements.size, deduced ) );
		return memory;
	}

	ashes::DeviceMemoryPtr setupMemory( RenderDevice const & device
		, VkMemoryRequirements const & requirements
		, VkMemoryPropertyFlags flags
		, std::string const & name )
	{
		return setupMemory( *device, requirements, flags, name );
	}
}
