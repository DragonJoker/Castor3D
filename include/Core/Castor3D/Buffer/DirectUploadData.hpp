/*
See LICENSE file in root folder
*/
#ifndef ___C3D_DirectUploadData_H___
#define ___C3D_DirectUploadData_H___

#include "Castor3D/Buffer/UploadData.hpp"

#include <ashespp/Command/CommandBuffer.hpp>

namespace c3d
{
	class DirectUploadData
		: private DataHolderT< ashes::CommandBufferPtr >
		, public UploadData
	{
		using CommandBufferHolder = DataHolderT< ashes::CommandBufferPtr >;

	public:
		C3D_API DirectUploadData( RenderDevice const & device
			, String debugName
			, ashes::CommandBuffer const & commandBuffer );
		C3D_API DirectUploadData( RenderDevice const & device
			, String debugName
			, ashes::CommandPool const & commandPool );
		C3D_API ~DirectUploadData()noexcept;

		C3D_API void begin()override;
		C3D_API SemaphoreUsed end( ashes::Queue const & queue
			, ashes::Fence const * fence
			, Milliseconds timeout )override;
		C3D_API void cleanup()noexcept override;

	private:
		VkDeviceSize doUpload( BufferDataRange & data )override;
		VkDeviceSize doUpload( ImageDataRange & data )override;

		Vector< BufferUPtr > m_buffers;
	};

	using InstantDirectUploadData = InstantUploadDataT< DirectUploadData >;
}

#endif
