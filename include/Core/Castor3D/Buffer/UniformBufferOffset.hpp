/*
See LICENSE file in root folder
*/
#ifndef ___C3D_UniformBufferOffset_HPP___
#define ___C3D_UniformBufferOffset_HPP___

#include "GpuBuffer.hpp"
#include "PoolUniformBuffer.hpp"

#include <ashespp/Descriptor/DescriptorSet.hpp>

#include <RenderGraph/FramePass.hpp>

namespace c3d
{
	template< typename DataT >
	struct UniformBufferOffsetT
	{
	private:
		PoolUniformBuffer * buffer{ nullptr };

	public:
		MemoryPropertyFlags flags{ 0u };
		uint32_t offset{ 0u };
		uint32_t range{ 0u };
		crg::AttachmentPtr attach{};

		UniformBufferOffsetT()noexcept = default;
		UniformBufferOffsetT( UniformBufferOffsetT const & ) = delete;
		UniformBufferOffsetT & operator=( UniformBufferOffsetT const & ) = delete;

		UniformBufferOffsetT( UniformBufferOffsetT && rhs )noexcept
			: buffer{ rhs.buffer }
			, flags{ rhs.flags }
			, offset{ rhs.offset }
			, range{ rhs.range }
			, attach{ std::move( rhs.attach ) }
		{
			rhs.buffer = {};
			rhs.flags = {};
			rhs.offset = {};
			rhs.range = {};
		}

		UniformBufferOffsetT & operator=( UniformBufferOffsetT && rhs )noexcept
		{
			buffer = rhs.buffer;
			flags = rhs.flags;
			offset = rhs.offset;
			range = rhs.range;
			attach = std::move( rhs.attach );

			rhs.buffer = {};
			rhs.flags = {};
			rhs.offset = {};
			rhs.range = {};

			return *this;
		}

		~UniformBufferOffsetT()noexcept
		{
			CU_Require( buffer == nullptr );
		}

		void setPool( PoolUniformBuffer & pool )
		{
			buffer = &pool;
		}

		void unsetPool()
		{
			buffer = nullptr;
		}

		explicit operator bool()const
		{
			return buffer
				&& buffer->hasBuffer();
		}

		DeviceSize getByteOffset()const
		{
			return offset * buffer->getAlignedSize();
		}

		DeviceSize getByteRange()const
		{
			return range * buffer->getAlignedSize();
		}

		DataT const & getData()const
		{
			return buffer->getData< DataT >( offset * buffer->getAlignedSize() );
		}

		DataT & getData()
		{
			return buffer->getData< DataT >( offset * buffer->getAlignedSize() );
		}

		PoolUniformBuffer const & getPool()const
		{
			return *buffer;
		}

		PoolUniformBuffer & getPool()
		{
			return *buffer;
		}

		Buffer const & getBuffer()const
		{
			return buffer->getBuffer();
		}

		Buffer & getBuffer()
		{
			return buffer->getBuffer();
		}

		uint32_t getDataAlignedSize()const
		{
			return buffer->getAlignedSize( sizeof( DataT ) );
		}

		void createPassBinding( crg::FramePass & pass
			, uint32_t binding )const
		{
			pass.addInputUniform( *attach, binding );
		}

		void createSizedBinding( ashes::DescriptorSet & descriptorSet
			, VkDescriptorSetLayoutBinding const & layoutBinding )const
		{
			auto & uniformBuffer = buffer->getBuffer();
			auto size = buffer->getAlignedSize();
			descriptorSet.createBinding( layoutBinding
				, *uniformBuffer.buffer
				, uint32_t( offset * size )
				, uint32_t( range * size ) );
		}

		ashes::WriteDescriptorSet getDescriptorWrite( uint32_t dstBinding
			, uint32_t dstArrayElement = 0u )const
		{
			auto & uniformBuffer = buffer->getBuffer();
			auto size = buffer->getAlignedSize();
			auto result = ashes::WriteDescriptorSet{ dstBinding
				, dstArrayElement
				, 1u
				, VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER };
			result.bufferInfo.push_back( VkDescriptorBufferInfo{ *uniformBuffer.buffer
				, size * offset
				, size * range } );
			return result;
		}
	};
	/**
	*\~english
	*\brief
	*	Creates a descriptor write for uniform buffer range.
	*\param[in] buffer
	*	The uniform buffer range.
	*\param[in] dstBinding
	*	The binding inside the descriptor set.
	*\param[in] dstArrayElement
	*	The array element index.
	*\~french
	*\brief
	*	Crée un descriptor write pour un intervalle d'uniform buffer.
	*\param[in] buffer
	*	L'intervalle d'uniform buffer.
	*\param[in] dstBinding
	*	Le binding dans le descriptor set.
	*\param[in] dstArrayElement
	*	L'indice dans le tableau d'éléments.
	*/
	template< typename DataT >
	ashes::WriteDescriptorSet makeDescriptorWrite( UniformBufferOffsetT< DataT > const & buffer
		, uint32_t dstBinding
		, uint32_t dstArrayElement = 0u )
	{
		return buffer.getDescriptorWrite( dstBinding
			, dstArrayElement );
	}
}

#endif
