/*
See LICENSE file in root folder
*/
#ifndef ___C3D_DirectUploadData_H___
#define ___C3D_DirectUploadData_H___

#include "Castor3D/Buffer/UploadData.hpp"

#include <ashespp/Command/CommandBuffer.hpp>

namespace castor3d
{
	class DirectUploadData
		: private castor::DataHolderT< ashes::CommandBufferPtr >
		, public UploadData
	{
		using CommandBufferHolder = castor::DataHolderT< ashes::CommandBufferPtr >;

	public:
		C3D_API DirectUploadData( RenderDevice const & device
			, castor::String debugName
			, ashes::CommandBuffer const & commandBuffer );
		C3D_API DirectUploadData( RenderDevice const & device
			, castor::String debugName
			, ashes::CommandPool const & commandPool );

	private:
		void doBegin()override;
		VkDeviceSize doUpload( BufferDataRange & data )override;
		VkDeviceSize doUpload( ImageDataRange & data )override;
		SemaphoreUsed doEnd( ashes::Queue const & queue
			, ashes::Fence const * fence
			, castor::Milliseconds timeout )override;

		castor::Vector< ashes::BufferBasePtr > m_buffers;
	};

	using InstantDirectUploadData = InstantUploadDataT< DirectUploadData >;
}

#endif
