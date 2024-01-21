/*
See LICENSE file in root folder
*/
#ifndef ___C3D_UploadData_H___
#define ___C3D_UploadData_H___

#include "Castor3D/Buffer/BufferModule.hpp"
#include "Castor3D/Render/RenderModule.hpp"

#include <CastorUtils/Graphics/ImageLayout.hpp>

namespace castor3d
{
	class UploadData
	{
	public:
		struct SemaphoreUsed
		{
			ashes::Semaphore const * semaphore;
			bool * used;
			VkDeviceSize uploadSize;
			VkDeviceSize buffersCount;
		};

		UploadData( UploadData const & ) = delete;
		UploadData( UploadData && )noexcept = delete;
		UploadData & operator=( UploadData const & ) = delete;
		UploadData & operator=( UploadData && )noexcept = delete;
		C3D_API virtual ~UploadData()noexcept = default;

		C3D_API void begin();
		C3D_API void pushUpload( void const * srcData
			, VkDeviceSize srcSize
			, ashes::BufferBase const & dstBuffer
			, VkDeviceSize dstOffset
			, VkAccessFlags dstAccessFlags
			, VkPipelineStageFlags dstPipelineFlags );
		C3D_API void pushUpload( void const * srcData
			, VkDeviceSize srcSize
			, ashes::Image const & dstImage
			, castor::ImageLayout dstLayout
			, VkImageSubresourceRange dstRange
			, VkImageLayout dstImageLayout
			, VkPipelineStageFlags dstPipelineFlags );
		C3D_API void process();
		C3D_API SemaphoreUsed end( ashes::Queue const & queue
			, ashes::Fence const * fence = nullptr
			, castor::Milliseconds timeout = castor::Milliseconds{ ashes::MaxTimeout } );

		void pushUpload( castor::ByteArray const & srcData
			, ashes::BufferBase const & dstBuffer
			, VkDeviceSize dstOffset
			, VkAccessFlags dstAccessFlags
			, VkPipelineStageFlags dstPipelineFlags )
		{
			pushUpload( srcData.data()
				, srcData.size()
				, dstBuffer
				, dstOffset
				, dstAccessFlags
				, dstPipelineFlags );
		}

		void pushUpload( castor::ByteArray const & srcData
			, ashes::Image const & dstImage
			, castor::ImageLayout dstLayout
			, VkImageSubresourceRange dstRange
			, VkImageLayout dstImageLayout
			, VkPipelineStageFlags dstPipelineFlags )
		{
			pushUpload( srcData.data()
				, srcData.size()
				, dstImage
				, std::move( dstLayout )
				, dstRange
				, dstImageLayout
				, dstPipelineFlags );
		}

		void pushUpload( castor::ByteArrayView const & srcData
			, ashes::BufferBase const & dstBuffer
			, VkDeviceSize dstOffset
			, VkAccessFlags dstAccessFlags
			, VkPipelineStageFlags dstPipelineFlags )
		{
			pushUpload( srcData.data()
				, srcData.size()
				, dstBuffer
				, dstOffset
				, dstAccessFlags
				, dstPipelineFlags );
		}

		void pushUpload( castor::ByteArrayView const & srcData
			, ashes::Image const & dstImage
			, castor::ImageLayout dstLayout
			, VkImageSubresourceRange dstRange
			, VkImageLayout dstImageLayout
			, VkPipelineStageFlags dstPipelineFlags )
		{
			pushUpload( srcData.data()
				, srcData.size()
				, dstImage
				, std::move( dstLayout )
				, dstRange
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

		castor::String const & getName()const noexcept
		{
			return m_debugName;
		}

	protected:
		struct BufferDataRange
		{
			void const * srcData{};
			VkDeviceSize srcSize{};
			ashes::BufferBase const * dstBuffer{};
			VkDeviceSize dstOffset{};
			VkAccessFlags dstAccessFlags{};
			VkPipelineStageFlags dstPipelineFlags{};
		};

		struct ImageDataRange
		{
			void const * srcData{};
			VkDeviceSize srcSize{};
			ashes::Image const * dstImage{};
			castor::ImageLayout dstLayout{};
			VkImageSubresourceRange dstRange{};
			VkImageLayout dstImageLayout{};
			VkPipelineStageFlags dstPipelineFlags{};
		};

		C3D_API UploadData( RenderDevice const & device
			, std::string debugName
			, ashes::CommandBuffer const * commandBuffer );

		C3D_API bool doCopyData( void const * srcData
			, VkDeviceSize size
			, ashes::BufferBase const & dstBuffer
			, VkDeviceSize dstOffset )const;
		C3D_API void doUploadBuffer( BufferDataRange const & data
			, ashes::BufferBase const * srcBuffer
			, VkDeviceSize srcOffset )const;
		C3D_API void doUploadImage( ImageDataRange & data
			, ashes::BufferBase const & srcBuffer
			, VkDeviceSize srcOffset )const;

		RenderDevice const & m_device;
		std::string m_debugName;
		ashes::CommandBuffer const * m_commandBuffer;
		std::vector< BufferDataRange > m_pendingBuffers;
		std::vector< ImageDataRange > m_pendingImages;

	private:
		virtual VkDeviceSize doUpload( BufferDataRange & data ) = 0;
		virtual VkDeviceSize doUpload( ImageDataRange & data ) = 0;
		virtual void doBegin()
		{
		}

		virtual SemaphoreUsed doEnd( ashes::Queue const & queue
			, ashes::Fence const * fence
			, castor::Milliseconds timeout )
		{
			return {};
		}

		virtual void doPreprocess( std::vector< BufferDataRange > *& pendingBuffers
			, std::vector< ImageDataRange > *& pendingImages )
		{
			pendingBuffers = &m_pendingBuffers;
			pendingImages = &m_pendingImages;
		}

		virtual void doPostprocess()
		{
		}
	};

	C3D_API std::ostream & operator<<( std::ostream & stream, VkImageSubresourceRange const & rhs );
}

#endif
