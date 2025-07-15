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

	private:
		void doBegin()override;
		VkDeviceSize doUpload( BufferDataRange & data )override;
		VkDeviceSize doUpload( ImageDataRange & data )override;
		SemaphoreUsed doEnd( ashes::Queue const & queue
			, ashes::Fence const * fence
			, Milliseconds timeout )override;

		Vector< ashes::BufferBasePtr > m_buffers;
	};

	using InstantDirectUploadData = InstantUploadDataT< DirectUploadData >;
}

#endif
