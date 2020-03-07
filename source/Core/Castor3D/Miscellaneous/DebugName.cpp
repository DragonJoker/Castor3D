/*
See LICENSE file in root folder
*/
#include "Castor3D/Miscellaneous/DebugName.hpp"

#include "Castor3D/Render/RenderDevice.hpp"

#include <CastorUtils/Log/Logger.hpp>

#include <ashespp/Core/Device.hpp>
#include <ashespp/Miscellaneous/DeviceMemory.hpp>

namespace castor3d
{
	void setDebugObjectName( RenderDevice const & device
		, uint64_t object
		, uint32_t type
		, std::string const & name
		, std::string const & typeName )
	{
		log::trace << "Created " << typeName << " [" << name << "]" << std::endl;
		device.device->setDebugObjectName(
			{
				VkObjectType( type ),
				object,
				name.c_str()
			} );
	}

	ashes::DeviceMemoryPtr setupMemory( RenderDevice const & device
		, VkMemoryRequirements const & requirements
		, VkMemoryPropertyFlags flags
		, std::string const & name )
	{
		uint32_t deduced = device.device->deduceMemoryType( requirements.memoryTypeBits
			, flags );
		auto memory = device.device->allocateMemory( VkMemoryAllocateInfo{ VK_STRUCTURE_TYPE_MEMORY_ALLOCATE_INFO, nullptr, requirements.size, deduced } );
		setDebugObjectName( device, *memory, name + "Mem" );
		return memory;
	}
}
