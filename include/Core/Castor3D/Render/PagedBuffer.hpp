/*
See LICENSE file in root folder
*/
#ifndef ___C3D_PagedBuffer_H___
#define ___C3D_PagedBuffer_H___

#include "RenderModule.hpp"

#include <RenderGraph/BufferData.hpp>
#include <RenderGraph/BufferViewData.hpp>

namespace c3d
{
	struct PagedBufferBase
	{
		C3D_API PagedBufferBase()noexcept = default;
		C3D_API PagedBufferBase( PagedBufferBase const & ) = delete;
		C3D_API PagedBufferBase & operator=( PagedBufferBase const & ) = delete;
		C3D_API PagedBufferBase( PagedBufferBase && rhs )noexcept;
		C3D_API PagedBufferBase & operator=( PagedBufferBase && rhs )noexcept;

		C3D_API PagedBufferBase( RenderDevice const & device
			, crg::ResourcesCache & resources
			, String const & name
			, BufferCreateFlags flags
			, DeviceSize size
			, uint32_t maxPages
			, BufferUsageFlags usage
			, MemoryPropertyFlags memory = MemoryPropertyFlags::eDeviceLocal );
		C3D_API virtual ~PagedBufferBase()noexcept;

		C3D_API void create();
		C3D_API void destroy()noexcept;

		operator bool()const noexcept
		{
			return resources != nullptr
				&& device != nullptr
				&& !m_pages.empty();
		}

		uint32_t getAllocatedPageCount()const noexcept
		{
			return bufferId.data->allocatedPages;
		}

		uint32_t getMaxPageCount()const noexcept
		{
			return bufferId.data->maxPages;
		}

		DeviceSize getPageSize()const noexcept
		{
			return crg::getSize( bufferId );
		}

		DeviceSize getAllocatedSize()const noexcept
		{
			return getPageSize() * getAllocatedPageCount();
		}

		DeviceSize getMaxSize()const noexcept
		{
			return getPageSize() * getMaxPageCount();
		}

		crg::Attachment const * getLastAttach()const noexcept
		{
			return m_attach;
		}

		crg::Attachment const * setLastAttach( crg::Attachment const * attach )noexcept
		{
			m_attach = attach;
			return m_attach;
		}

		ashes::BufferBase const & getPageBuffer( u32 pageIndex )const noexcept
		{
			return *m_pages[pageIndex];
		}

		std::string const & getName()const noexcept
		{
			return bufferId.data->name;
		}

		crg::Buffer & getBufferPages()const noexcept
		{
			return *m_bufferPages;
		}

		void countDescriptors( uint32_t & dstBinding )const
		{
			dstBinding += u32( bufferId.data->maxPages );
		}

		template< typename BindingT >
		ashes::WriteDescriptorSet getDescriptorWrite( BindingT dstBinding
			, uint32_t dstArrayElement = 0u )const
		{
			return doGetDescriptorWrite( uint32_t( dstBinding ), dstArrayElement );
		}

		template< typename BindingT >
		void addDescriptorWriteT( ashes::WriteDescriptorSetArray & writes
			, BindingT dstBinding
			, uint32_t dstArrayElement = 0u )const
		{
			writes.emplace_back( getDescriptorWrite( dstBinding, dstArrayElement ) );
		}

		void addDescriptorWrite( ashes::WriteDescriptorSetArray & writes
			, uint32_t & dstBinding
			, uint32_t dstArrayElement = 0u )const
		{
			writes.emplace_back( getDescriptorWrite( dstBinding, dstArrayElement ) );
			dstBinding += u32( bufferId.data->maxPages );
		}

		template< typename BindingT >
		VkDescriptorSetLayoutBinding getLayoutBinding( BindingT index
			, VkShaderStageFlags stages )const
		{
			return VkDescriptorSetLayoutBinding{ uint32_t( index )
				, ( checkFlag( bufferId.data->info.usage, BufferUsageFlags::eUniformBuffer )
					? VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER
					: VK_DESCRIPTOR_TYPE_STORAGE_BUFFER )
				, bufferId.data->maxPages
				, stages
				, nullptr };
		}

		template< typename BindingT >
		void addLayoutBindingT( ashes::VkDescriptorSetLayoutBindingArray & bindings
			, BindingT index
			, VkShaderStageFlags stages )const
		{
			bindings.push_back( getLayoutBinding( index, stages ) );
		}

		void addLayoutBinding( ashes::VkDescriptorSetLayoutBindingArray & bindings
			, uint32_t & index
			, VkShaderStageFlags stages )const
		{
			bindings.push_back( getLayoutBinding( index, stages ) );
			index += u32( bufferId.data->maxPages );
		}

		crg::ResourcesCache * resources{};
		RenderDevice const * device{};
		crg::BufferId bufferId{};
		crg::BufferViewId bufferViewId{};

	protected:
		C3D_API uint8_t * doLockBuffer( uint32_t pageIndex )const;
		C3D_API void doUnlock( uint32_t pageIndex )const;
		C3D_API void doFlush( uint32_t pageIndex )const;

	private:
		C3D_API ashes::WriteDescriptorSet doGetDescriptorWrite( uint32_t dstBinding
			, uint32_t dstArrayElement = 0u )const;

	private:
		crg::AttachmentPtr m_ownAttach;
		crg::Attachment const * m_attach{};
		bool m_ownBuffer{};
		crg::Buffer * m_bufferPages{};
		Vector< ashes::BufferBasePtr > m_pages;
	};

	struct PagedBuffer : PagedBufferBase
	{
		C3D_API PagedBuffer()noexcept = default;
		C3D_API PagedBuffer( PagedBuffer const & ) = delete;
		C3D_API PagedBuffer & operator=( PagedBuffer const & ) = delete;
		C3D_API PagedBuffer( PagedBuffer && rhs )noexcept = default;
		C3D_API PagedBuffer & operator=( PagedBuffer && rhs )noexcept = default;
		C3D_API ~PagedBuffer()noexcept override = default;

		C3D_API PagedBuffer( RenderDevice const & device
			, crg::ResourcesCache & resources
			, String const & name
			, BufferCreateFlags flags
			, DeviceSize size, uint32_t maxPages
			, BufferUsageFlags usage
			, MemoryPropertyFlags memory = MemoryPropertyFlags::eDeviceLocal );

		uint8_t * lock( uint32_t pageIndex )const
		{
			return doLockBuffer( pageIndex );
		}

		void unlock( uint32_t pageIndex )const
		{
			doUnlock( pageIndex );
		}

		void flush( uint32_t pageIndex )const
		{
			doFlush( pageIndex );
		}
	};

	template< typename DataT >
	struct PagedBufferT : PagedBufferBase
	{
		PagedBufferT()noexcept = default;
		PagedBufferT( PagedBufferT const & ) = delete;
		PagedBufferT & operator=( PagedBufferT const & ) = delete;
		PagedBufferT( PagedBufferT && rhs )noexcept = default;
		PagedBufferT & operator=( PagedBufferT && rhs )noexcept = default;
		~PagedBufferT()noexcept override = default;

		PagedBufferT( RenderDevice const & device
			, crg::ResourcesCache & resources
			, String const & name
			, BufferCreateFlags flags
			, DeviceSize count, uint32_t maxPages
			, BufferUsageFlags usage
			, MemoryPropertyFlags memory = MemoryPropertyFlags::eDeviceLocal )
			: PagedBufferBase{ device, resources, name
				, flags, count * sizeof( DataT ), maxPages
				, usage, memory }
		{
		}

		DeviceSize getPageElementCount()const noexcept
		{
			return getPageSize() / sizeof( DataT );
		}

		DeviceSize getAllocatedElementCount()const noexcept
		{
			return getPageElementCount() * getAllocatedPageCount();
		}

		DeviceSize getMaxElementCount()const noexcept
		{
			return getPageElementCount() * getMaxPageCount();
		}

		DataT * lock( uint32_t pageIndex )const
		{
			return reinterpret_cast< DataT * >( doLockBuffer( pageIndex ) );
		}

		void unlock( uint32_t pageIndex )const
		{
			doUnlock( pageIndex );
		}

		void flush( uint32_t pageIndex )const
		{
			doFlush( pageIndex );
		}
	};

	CU_DeclareTemplateSmartPtr( c3d, PagedBuffer );

	template< typename T >
	inline PagedBufferUPtrT< T > makePagedBuffer( RenderDevice const & device
		, crg::ResourcesCache & resources
		, DeviceSize count, uint32_t maxPages
		, BufferUsageFlags usage
		, MemoryPropertyFlags flags
		, String const & name )
	{
		auto result = makeUnique< PagedBufferT< T > >( device, resources
			, name + cuT( "Buf" )
			, BufferCreateFlags::eNone
			, count, maxPages, usage, flags );
		result->create();
		return result;
	}

	inline PagedBufferUPtr makePagedBufferBase( RenderDevice const & device
		, crg::ResourcesCache & resources
		, DeviceSize size, uint32_t maxPages
		, BufferUsageFlags usage
		, MemoryPropertyFlags flags
		, String const & name )
	{
		auto result = makeUnique< PagedBuffer >( device, resources
			, name + cuT( "Buf" )
			, BufferCreateFlags::eNone
			, size, maxPages, usage, flags );
		result->create();
		return result;
	}
}

#endif
