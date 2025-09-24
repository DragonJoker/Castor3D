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

namespace c3d
{
	class StagedUploadData
		: private DataHolderT< ashes::CommandBufferPtr >
		, public UploadData
	{
		using CommandBufferHolder = DataHolderT< ashes::CommandBufferPtr >;

		struct GpuBufferOffset
		{
			BufferBase * buffer{};
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
			, String debugName
			, ashes::CommandBufferPtr commandBuffer );
		C3D_API ~StagedUploadData()noexcept override;

		C3D_API void begin()override;
		C3D_API SemaphoreUsed end( ashes::Queue const & queue
			, ashes::Fence const * fence
			, Milliseconds timeout )override;
		C3D_API void cleanup()noexcept override;

		uint32_t getFrameIndex()const noexcept
		{
			return m_frameIndex;
		}

	private:
		void doPreprocess( Vector< BufferDataRange > *& pendingBuffers
			, Vector< ImageDataRange > *& pendingImages )override;
		VkDeviceSize doUpload( BufferDataRange & data )override;
		VkDeviceSize doUpload( ImageDataRange & data )override;
		void doPostprocess()override;

		struct StagingBuffer
		{
			explicit StagingBuffer( GpuPackedBaseBufferUPtr buf )
				: buffer{ c3d::move( buf ) }
			{
			}

			GpuPackedBaseBufferUPtr buffer;
			u32 lifetime{};
		};
		using BufferArray = Vector< StagingBuffer >;

		GpuBufferOffset doGetBuffer( BufferArray & pool
			, VkDeviceSize size )const;
		void doPutBuffer( BufferArray & pool
			, GpuBufferOffset const & bufferOffset )const noexcept;

		struct BufferRange
		{
			byte * mapped{};
			VkDeviceSize offset{ ~0ULL };
			VkDeviceSize range{ 0ULL };
		};
		using BuffersRanges = HashMap< BufferBase const *, BufferRange >;

		struct FrameBuffers
		{
			BufferArray pool;
			Map< BufferDataRange const *, GpuBufferOffset > bufferOffsets{};
			Map< ImageDataRange *, GpuBufferOffset > imageOffsets{};
			BuffersRanges buffers{};
			ashes::SemaphorePtr semaphore{};
			bool used{};
			Vector< BufferDataRange > pendingBuffers{};
			Vector< ImageDataRange > pendingImages{};
			VkDeviceSize currentSize{};
			VkDeviceSize buffersCount{};

			explicit FrameBuffers( ashes::SemaphorePtr psemaphore = {}
				, BuffersRanges pbuffers = {}
				, bool pused = true )noexcept
				: buffers{ c3d::move( pbuffers ) }
				, semaphore{ c3d::move( psemaphore ) }
				, used{ pused }
			{
			}

			~FrameBuffers()noexcept
			{
				destroy();
			}

			FrameBuffers( FrameBuffers const & ) = delete;
			FrameBuffers( FrameBuffers && )noexcept = default;
			FrameBuffers & operator=( FrameBuffers const & ) = delete;
			FrameBuffers & operator=( FrameBuffers && rhs )noexcept
			{
				destroy();

				pool = c3d::move( rhs.pool );
				bufferOffsets = c3d::move( rhs.bufferOffsets );
				imageOffsets = c3d::move( rhs.imageOffsets );
				buffers = c3d::move( rhs.buffers );
				semaphore = c3d::move( rhs.semaphore );
				pendingBuffers = c3d::move( rhs.pendingBuffers );
				pendingImages = c3d::move( rhs.pendingImages );
				used = rhs.used;
				currentSize = rhs.currentSize;
				buffersCount = rhs.buffersCount;

				rhs.used = {};
				rhs.currentSize = {};
				rhs.buffersCount = {};

				return *this;
			}

		private:
			void destroy()noexcept
			{
				buffers.clear();
				bufferOffsets.clear();
				imageOffsets.clear();
				pendingBuffers.clear();
				pendingImages.clear();
				semaphore = {};
				used = {};
				currentSize = {};
				buffersCount = {};

				for ( auto & buffer : pool )
					buffer.buffer->getBuffer().destroy();
			}
		};

		HashMap< BufferBase const *, byte * > m_wholeBuffers;
		Array< FrameBuffers, 2u > m_buffers;
		FrameBuffers * m_cpuBuffers{};
		FrameBuffers * m_gpuBuffers{};
		uint32_t m_frameIndex{};
		FramePassTimerUPtr m_timer{};
		RawUniquePtr< crg::FramePassTimerBlock > m_cpuBlock{};
	};

	using InstantStagedUploadData = InstantUploadDataT< StagedUploadData >;
}

#endif
