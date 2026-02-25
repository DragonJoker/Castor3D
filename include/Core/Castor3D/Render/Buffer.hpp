/*
See LICENSE file in root folder
*/
#ifndef ___C3D_Buffer_H___
#define ___C3D_Buffer_H___

#include "RenderModule.hpp"

#include <RenderGraph/BufferData.hpp>
#include <RenderGraph/BufferViewData.hpp>

namespace c3d
{
	struct BufferBase
	{
		C3D_API BufferBase()noexcept = default;
		C3D_API BufferBase( BufferBase const & ) = delete;
		C3D_API BufferBase & operator=( BufferBase const & ) = delete;
		C3D_API BufferBase( BufferBase && rhs )noexcept;
		C3D_API BufferBase & operator=( BufferBase && rhs )noexcept;

		C3D_API BufferBase( RenderDevice const & device
			, crg::ResourcesCache & resources
			, String const & name
			, BufferCreateFlags flags
			, DeviceSize size
			, BufferUsageFlags usage
			, MemoryPropertyFlags memory = MemoryPropertyFlags::eDeviceLocal );
		C3D_API BufferBase( RenderDevice const & device
			, crg::ResourcesCache & resources
			, crg::BufferViewId view );
		C3D_API virtual ~BufferBase()noexcept;

		C3D_API void create();
		C3D_API void destroy()noexcept;
		C3D_API crg::BufferViewId getSubView( DeviceSize offset, DeviceSize size )const;

		operator bool()const noexcept
		{
			return resources != nullptr
				&& device != nullptr
				&& buffer != nullptr;
		}

		DeviceSize getSize()const noexcept
		{
			return crg::getSize( bufferId );
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

		ashes::BufferBase const & getBuffer()const noexcept
		{
			return *buffer;
		}

		std::string const & getName()const noexcept
		{
			return bufferId.data->name;
		}

		template< typename BindingT >
		ashes::WriteDescriptorSet getDescriptorWrite( BindingT dstBinding
			, uint32_t dstArrayElement = 0u )const
		{
			return ashes::WriteDescriptorSet{ uint32_t( dstBinding ), dstArrayElement
				, ( checkFlag( bufferId.data->info.usage, BufferUsageFlags::eUniformBuffer )
					? VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER
					: VK_DESCRIPTOR_TYPE_STORAGE_BUFFER )
				, { VkDescriptorBufferInfo{ *buffer, 0u, getSize() } } };
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
			++dstBinding;
		}

		template< typename BindingT >
		VkDescriptorSetLayoutBinding getLayoutBinding( BindingT index
			, VkShaderStageFlags stages )const
		{
			return VkDescriptorSetLayoutBinding{ uint32_t( index )
				, ( checkFlag( bufferId.data->info.usage, BufferUsageFlags::eUniformBuffer )
					? VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER
					: VK_DESCRIPTOR_TYPE_STORAGE_BUFFER )
				, 1u
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
			++index;
		}

		crg::ResourcesCache * resources{};
		RenderDevice const * device{};
		crg::BufferId bufferId{};
		ashes::BufferBasePtr buffer{};
		crg::BufferViewId bufferViewId{};

	protected:
		C3D_API uint8_t * doLockBuffer()const;
		C3D_API void doUnlock()const;
		C3D_API void doFlush()const;

	private:
		crg::AttachmentPtr m_ownAttach;
		crg::Attachment const * m_attach{};
		bool m_ownBuffer{};
	};

	struct Buffer : BufferBase
	{
		C3D_API Buffer()noexcept = default;
		C3D_API Buffer( Buffer const & ) = delete;
		C3D_API Buffer & operator=( Buffer const & ) = delete;
		C3D_API Buffer( Buffer && rhs )noexcept = default;
		C3D_API Buffer & operator=( Buffer && rhs )noexcept = default;
		C3D_API ~Buffer()noexcept override = default;

		C3D_API Buffer( RenderDevice const & device
			, crg::ResourcesCache & resources
			, String const & name
			, BufferCreateFlags flags
			, DeviceSize size
			, BufferUsageFlags usage
			, MemoryPropertyFlags memory = MemoryPropertyFlags::eDeviceLocal );
		C3D_API Buffer( RenderDevice const & device
			, crg::ResourcesCache & resources
			, crg::BufferViewId view );

		uint8_t * lock()const
		{
			return doLockBuffer();
		}

		void unlock()const
		{
			doUnlock();
		}

		void flush()const
		{
			doFlush();
		}
	};

	template< typename DataT >
	struct BufferT : BufferBase
	{
		BufferT()noexcept = default;
		BufferT( BufferT const & ) = delete;
		BufferT & operator=( BufferT const & ) = delete;
		BufferT( BufferT && rhs )noexcept = default;
		BufferT & operator=( BufferT && rhs )noexcept = default;
		~BufferT()noexcept override = default;

		BufferT( RenderDevice const & device
			, crg::ResourcesCache & resources
			, String const & name
			, BufferCreateFlags flags
			, DeviceSize count
			, BufferUsageFlags usage
			, MemoryPropertyFlags memory = MemoryPropertyFlags::eDeviceLocal )
			: BufferBase{ device, resources, name
				, flags, count * sizeof( DataT )
				, usage, memory }
		{
		}

		DataT * lock()const
		{
			return reinterpret_cast< DataT * >( doLockBuffer() );
		}

		DeviceSize getCount()const noexcept
		{
			return getSize() / sizeof( DataT );
		}

		void unlock()const
		{
			doUnlock();
		}

		void flush()const
		{
			doFlush();
		}
	};

	CU_DeclareTemplateSmartPtr( c3d, Buffer );

	template< typename T >
	inline BufferUPtrT< T > makeBuffer( RenderDevice const & device
		, crg::ResourcesCache & resources
		, DeviceSize count
		, BufferUsageFlags usage
		, MemoryPropertyFlags flags
		, String const & name )
	{
		auto result = makeUnique< BufferT< T > >( device, resources
			, name + cuT( "Buf" )
			, BufferCreateFlags::eNone
			, count, usage, flags );
		result->create();
		return result;
	}

	inline BufferUPtr makeBufferBase( RenderDevice const & device
		, crg::ResourcesCache & resources
		, DeviceSize size
		, BufferUsageFlags usage
		, MemoryPropertyFlags flags
		, String const & name )
	{
		auto result = makeUnique< Buffer >( device, resources
			, name + cuT( "Buf" )
			, BufferCreateFlags::eNone
			, size, usage, flags );
		result->create();
		return result;
	}

	template< typename T >
	inline BufferUPtrT< T > makeVertexBuffer( RenderDevice const & device
		, crg::ResourcesCache & resources
		, DeviceSize count
		, BufferUsageFlags usage
		, MemoryPropertyFlags flags
		, String const & name )
	{
		return makeBuffer< T >( device, resources
			, count, usage | BufferUsageFlags::eVertexBuffer, flags
			, name + cuT( "Vbo" ) );
	}

	inline BufferUPtr makeVertexBufferBase( RenderDevice const & device
		, crg::ResourcesCache & resources
		, DeviceSize size
		, BufferUsageFlags usage
		, MemoryPropertyFlags flags
		, String const & name )
	{
		return makeBufferBase( device, resources
			, size, usage | BufferUsageFlags::eVertexBuffer, flags
			, name + cuT( "Vbo" ) );
	}
}

#endif
