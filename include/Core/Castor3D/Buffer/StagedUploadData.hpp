/*
See LICENSE file in root folder
*/
#ifndef ___C3D_StagedUploadData_H___
#define ___C3D_StagedUploadData_H___

#include "Castor3D/Buffer/ObjectBufferOffset.hpp"
#include "Castor3D/Buffer/UploadData.hpp"
#include "Castor3D/Buffer/GpuBufferPool.hpp"

#include <CastorUtils/Design/DataHolder.hpp>

#include <RenderGraph/FramePassTimer.hpp>

#include <ashespp/Command/CommandBuffer.hpp>
#include <ashespp/Sync/Fence.hpp>
#include <ashespp/Sync/Semaphore.hpp>

#include <unordered_set>

namespace castor3d
{
	class StagedUploadData
		: private castor::DataHolderT< ashes::CommandBufferPtr >
		, public UploadData
	{
		using CommandBufferHolder = castor::DataHolderT< ashes::CommandBufferPtr >;

		struct GpuBufferOffset
		{
			ashes::BufferBase * buffer{};
			MemChunk chunk{};

			VkDeviceSize getAskedSize()const
			{
				return chunk.askedSize;
			}

			VkDeviceSize getAllocSize()const
			{
				return chunk.size;
			}

			VkDeviceSize getOffset()const
			{
				return chunk.offset;
			}
		};

	public:
		C3D_API StagedUploadData( RenderDevice const & device
			, std::string debugName
			, ashes::CommandBufferPtr commandBuffer );
		C3D_API ~StagedUploadData()noexcept override;

		uint32_t getFrameIndex()const noexcept
		{
			return m_frameIndex;
		}

	private:
		void doBegin()override;
		void doPreprocess( std::vector< BufferDataRange > *& pendingBuffers
			, std::vector< ImageDataRange > *& pendingImages )override;
		VkDeviceSize doUpload( BufferDataRange & data )override;
		VkDeviceSize doUpload( ImageDataRange & data )override;
		void doPostprocess()override;
		SemaphoreUsed doEnd( ashes::Queue const & queue
			, ashes::Fence const * fence
			, castor::Milliseconds timeout )override;

		struct StagingBuffer
		{
			explicit StagingBuffer( GpuPackedBaseBufferUPtr buf )
				: buffer{ std::move( buf ) }
			{
			}

			GpuPackedBaseBufferUPtr buffer;
			u32 lifetime{};
		};
		using BufferArray = std::vector< StagingBuffer >;

		GpuBufferOffset doGetBuffer( BufferArray & pool
			, VkDeviceSize size );
		void doPutBuffer( BufferArray & pool
			, GpuBufferOffset const & bufferOffset )const noexcept;

		struct BufferRange
		{
			byte * mapped{};
			VkDeviceSize offset{ ~( 0ULL ) };
			VkDeviceSize range{ 0ULL };
		};
		using BuffersRanges = std::unordered_map< ashes::BufferBase const *, BufferRange >;

		struct FrameBuffers
		{
			BufferArray pool;
			std::map< BufferDataRange const *, GpuBufferOffset > bufferOffsets{};
			std::map< ImageDataRange *, GpuBufferOffset > imageOffsets{};
			BuffersRanges buffers{};
			ashes::SemaphorePtr semaphore{};
			bool used{};
			std::vector< BufferDataRange > pendingBuffers{};
			std::vector< ImageDataRange > pendingImages{};
			VkDeviceSize currentSize{};
			VkDeviceSize buffersCount{};

			explicit FrameBuffers( ashes::SemaphorePtr psemaphore = {}
				, BuffersRanges pbuffers = {}
				, bool pused = true )noexcept
				: buffers{ std::move( pbuffers ) }
				, semaphore{ std::move( psemaphore ) }
				, used{ pused }
			{
			}

			~FrameBuffers()noexcept
			{
				for ( auto const & [buffer, bounds] : buffers )
				{
					buffer->unlock();
				}

				buffers.clear();
				semaphore = {};
			}

			FrameBuffers( FrameBuffers const & ) = delete;
			FrameBuffers( FrameBuffers && )noexcept = default;
			FrameBuffers & operator=( FrameBuffers const & ) = delete;
			FrameBuffers & operator=( FrameBuffers && )noexcept = default;
		};

		std::unordered_map< ashes::BufferBase const *, byte * > m_wholeBuffers;
		std::array< FrameBuffers, 2u > m_buffers;
		FrameBuffers * m_cpuBuffers{};
		FrameBuffers * m_gpuBuffers{};
		uint32_t m_frameIndex{};
		FramePassTimerUPtr m_timer{};
		std::unique_ptr< crg::FramePassTimerBlock > m_cpuBlock{};
	};

	using InstantStagedUploadData = InstantUploadDataT< StagedUploadData >;
}

#endif
