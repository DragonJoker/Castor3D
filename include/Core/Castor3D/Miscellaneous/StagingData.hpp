/*
See LICENSE file in root folder
*/
#ifndef ___C3D_StagingData_H___
#define ___C3D_StagingData_H___

#include "MiscellaneousModule.hpp"
#include "Castor3D/Render/RenderModule.hpp"

#include <ashespp/Buffer/StagingBuffer.hpp>
#include <ashespp/Command/CommandBuffer.hpp>
#include <ashespp/Sync/Fence.hpp>

namespace castor3d
{
	class StagingData
	{
	public:
		C3D_API StagingData( RenderDevice const & device
			, std::string const & debugName );

		C3D_API void upload( void const * data
			, VkDeviceSize size
			, VkDeviceSize offset
			, ashes::BufferBase const & buffer );
		C3D_API void uploadNoWait( void const * data
			, VkDeviceSize size
			, VkDeviceSize offset
			, ashes::BufferBase const & buffer );

		void upload( castor::ByteArray const & data
			, VkDeviceSize offset
			, ashes::BufferBase const & buffer )
		{
			upload( data.data()
				, data.size()
				, offset
				, buffer );
		}

		void uploadNoWait( castor::ByteArray const & data
			, VkDeviceSize offset
			, ashes::BufferBase const & buffer )
		{
			uploadNoWait( data.data()
				, data.size()
				, offset
				, buffer );
		}

	private:
		RenderDevice const & m_device;
		std::string m_debugName;
		ashes::StagingBufferPtr m_staging;
		ashes::CommandBufferPtr m_command;
		ashes::FencePtr m_fence;
	};
}

#endif
