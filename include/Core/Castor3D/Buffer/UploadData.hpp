/*
See LICENSE file in root folder
*/
#ifndef ___C3D_UploadData_H___
#define ___C3D_UploadData_H___

#include "Castor3D/Buffer/BufferModule.hpp"
#include "Castor3D/Render/RenderModule.hpp"

#include <CastorUtils/Graphics/ImageMemoryLayout.hpp>

namespace c3d
{
	class UploadData
	{
	public:
		struct SemaphoreUsed
		{
			ashes::Semaphore const * semaphore;
			bool * used;
			DeviceSize uploadSize;
			DeviceSize buffersCount;
		};

		UploadData( UploadData const & ) = delete;
		UploadData( UploadData && )noexcept = delete;
		UploadData & operator=( UploadData const & ) = delete;
		UploadData & operator=( UploadData && )noexcept = delete;
		C3D_API virtual ~UploadData()noexcept = default;

		C3D_API virtual void begin() = 0;
		C3D_API void pushUpload( void const * srcData
			, DeviceSize srcSize
			, BufferBase const & dstBuffer
			, DeviceSize dstOffset
			, AccessState const & dstAccessState );
		C3D_API void pushUpload( void const * srcData
			, DeviceSize srcSize
			, ashes::BufferBase const & dstBuffer
			, DeviceSize dstOffset
			, AccessState const & dstAccessState );
		C3D_API void pushUpload( void const * srcData
			, DeviceSize srcSize
			, ashes::Image const & dstImage
			, ImageMemoryLayout dstLayout
			, ImageSubresourceRange dstRange
			, ImageLayout dstImageLayout
			, PipelineStageFlags dstPipelineFlags );
		C3D_API void process();
		C3D_API virtual SemaphoreUsed end( ashes::Queue const & queue
			, ashes::Fence const * fence = nullptr
			, Milliseconds timeout = Milliseconds{ ashes::MaxTimeout } ) = 0;
		C3D_API virtual void cleanup()noexcept = 0;

		void pushUpload( void const * srcData
			, DeviceSize srcSize
			, ashes::BufferBase const & dstBuffer
			, DeviceSize dstOffset
			, AccessFlags dstAccessFlags
			, PipelineStageFlags dstPipelineFlags )
		{
			pushUpload( srcData
				, srcSize
				, dstBuffer
				, dstOffset
				, AccessState{ dstAccessFlags, dstPipelineFlags });
		}

		void pushUpload( ByteArray const & srcData
			, ashes::BufferBase const & dstBuffer
			, DeviceSize dstOffset
			, AccessFlags dstAccessFlags
			, PipelineStageFlags dstPipelineFlags )
		{
			pushUpload( srcData.data()
				, srcData.size()
				, dstBuffer
				, dstOffset
				, dstAccessFlags
				, dstPipelineFlags );
		}

		void pushUpload( ByteArray const & srcData
			, ashes::Image const & dstImage
			, ImageMemoryLayout dstLayout
			, ImageSubresourceRange dstRange
			, ImageLayout dstImageLayout
			, PipelineStageFlags dstPipelineFlags )
		{
			pushUpload( srcData.data()
				, srcData.size()
				, dstImage
				, c3d::move( dstLayout )
				, c3d::move( dstRange )
				, dstImageLayout
				, dstPipelineFlags );
		}

		void pushUpload( ByteArrayView const & srcData
			, ashes::BufferBase const & dstBuffer
			, DeviceSize dstOffset
			, AccessFlags dstAccessFlags
			, PipelineStageFlags dstPipelineFlags )
		{
			pushUpload( srcData.data()
				, srcData.size()
				, dstBuffer
				, dstOffset
				, dstAccessFlags
				, dstPipelineFlags );
		}

		void pushUpload( ByteArrayView const & srcData
			, ashes::Image const & dstImage
			, ImageMemoryLayout dstLayout
			, ImageSubresourceRange dstRange
			, ImageLayout dstImageLayout
			, PipelineStageFlags dstPipelineFlags )
		{
			pushUpload( srcData.data()
				, srcData.size()
				, dstImage
				, c3d::move( dstLayout )
				, c3d::move( dstRange )
				, dstImageLayout
				, dstPipelineFlags );
		}

		ashes::CommandBuffer const & getCommandBuffer()const noexcept
		{
			return *m_commandBuffer;
		}

		RenderDevice const & getDevice()const noexcept
		{
			return m_device;
		}

		String const & getName()const noexcept
		{
			return m_debugName;
		}

	protected:
		struct BufferDataRange
		{
			void const * srcData{};
			DeviceSize srcSize{};
			ashes::BufferBase const * dstBuffer{};
			DeviceSize dstOffset{};
			AccessState dstAccessState{};
		};

		struct ImageDataRange
		{
			void const * srcData{};
			DeviceSize srcSize{};
			ashes::Image const * dstImage{};
			ImageMemoryLayout dstLayout{};
			ImageSubresourceRange dstRange{};
			ImageLayout dstImageLayout{};
			PipelineStageFlags dstPipelineFlags{};
		};

		C3D_API UploadData( RenderDevice const & device
			, String debugName
			, ashes::CommandBuffer const * commandBuffer );

		C3D_API void doBegin()const;
		C3D_API void doEnd()const;
		C3D_API void doBeginDebugBlock( MbStringView name, FramePassTimer & timer )const;
		C3D_API void doEndDebugBlock( FramePassTimer & timer )const;
		C3D_API void doCleanup()noexcept;
		C3D_API bool doCopyData( void const * srcData
			, DeviceSize size
			, ashes::BufferBase const & dstBuffer
			, DeviceSize dstOffset )const;
		C3D_API void doUploadBuffer( BufferDataRange const & data
			, ashes::BufferBase const * srcBuffer
			, DeviceSize srcOffset )const;
		C3D_API void doUploadImage( ImageDataRange & data
			, ashes::BufferBase const & srcBuffer
			, DeviceSize srcOffset )const;
		C3D_API void doMemoryBarrier( VkPipelineStageFlags after
			, VkPipelineStageFlags before
			, VkBufferMemoryBarrier const & transitionBarrier )const;

		Vector< BufferDataRange > const & getPendingBuffers()const noexcept
		{
			return m_pendingBuffers;
		}

		Vector< ImageDataRange > const & getPendingImages()const noexcept
		{
			return m_pendingImages;
		}

	private:
		virtual DeviceSize doUpload( BufferDataRange & data ) = 0;
		virtual DeviceSize doUpload( ImageDataRange & data ) = 0;

		virtual void doPreprocess( Vector< BufferDataRange > *& pendingBuffers
			, Vector< ImageDataRange > *& pendingImages )
		{
			pendingBuffers = &m_pendingBuffers;
			pendingImages = &m_pendingImages;
		}

		virtual void doPostprocess()
		{
		}

	private:
		RenderDevice const & m_device;
		String m_debugName;
		ashes::CommandBuffer const * m_commandBuffer;
		Vector< BufferDataRange > m_pendingBuffers;
		Vector< ImageDataRange > m_pendingImages;
	};

	C3D_API OutputStream & operator<<( OutputStream & stream, ImageSubresourceRange const & rhs );
}

#endif
