/*
See LICENSE file in root folder
*/
#ifndef ___C3D_UniformBufferOffset_HPP___
#define ___C3D_UniformBufferOffset_HPP___

#include "PoolUniformBufferBase.hpp"

#include <ashespp/Descriptor/DescriptorSet.hpp>
#include <ashespp/Buffer/UniformBuffer.hpp>

namespace castor3d
{
	template< typename DataT >
	struct UniformBufferOffsetT
	{
	private:
		PoolUniformBufferBase * buffer{ nullptr };

	public:
		VkMemoryPropertyFlags flags{ 0u };
		uint32_t offset{ 0u };
		uint32_t range{ 0u };

		void setPool( PoolUniformBufferBase & pool )
		{
			buffer = &pool;
		}

		explicit operator bool()const
		{
			return buffer
				&& buffer->hasBuffer();
		}

		DataT const & getData()const
		{
			return buffer->getData< DataT >( offset * buffer->getAlignedSize() );
		}

		DataT & getData()
		{
			return buffer->getData< DataT >( offset * buffer->getAlignedSize() );
		}

		PoolUniformBufferBase const & getPool()const
		{
			return *buffer;
		}

		PoolUniformBufferBase & getPool()
		{
			return *buffer;
		}

		ashes::UniformBuffer const & getBuffer()const
		{
			return buffer->getBuffer();
		}

		ashes::UniformBuffer & getBuffer()
		{
			return buffer->getBuffer();
		}

		uint32_t getAlignedSize()const
		{
			return buffer->getAlignedSize( sizeof( DataT ) );
		}

		void createSizedBinding( ashes::DescriptorSet & descriptorSet
			, VkDescriptorSetLayoutBinding const & layoutBinding )const
		{
			auto & uniformBuffer = buffer->getBuffer();
			auto size = uniformBuffer.getAlignedSize();
			descriptorSet.createBinding( layoutBinding
				, uniformBuffer.getBuffer()
				, uint32_t( offset * size )
				, uint32_t( range * size ) );
		}
	};
}

#endif
