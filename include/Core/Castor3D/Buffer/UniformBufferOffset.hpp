/*
See LICENSE file in root folder
*/
#ifndef ___C3D_UniformBufferOffset_HPP___
#define ___C3D_UniformBufferOffset_HPP___

#include "PoolUniformBuffer.hpp"

#include <ashespp/Descriptor/DescriptorSet.hpp>

#include <RenderGraph/FramePass.hpp>

namespace castor3d
{
	template< typename DataT >
	struct UniformBufferOffsetT
	{
	private:
		PoolUniformBuffer * buffer{ nullptr };

	public:
		VkMemoryPropertyFlags flags{ 0u };
		uint32_t offset{ 0u };
		uint32_t range{ 0u };

		void setPool( PoolUniformBuffer & pool )
		{
			buffer = &pool;
		}

		explicit operator bool()const
		{
			return buffer
				&& buffer->hasBuffer();
		}

		VkDeviceSize getByteOffset()const
		{
			auto const & uniformBuffer = buffer->getBuffer();
			auto size = uniformBuffer.getAlignedSize();
			return offset * size;
		}

		VkDeviceSize getByteRange()const
		{
			auto const & uniformBuffer = buffer->getBuffer();
			auto size = uniformBuffer.getAlignedSize();
			return range * size;
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

		void createPassBinding( crg::FramePass & pass
			, castor::MbString const & name
			, uint32_t binding )const
		{
			pass.addUniformBuffer( { getBuffer(), name }
				, binding
				, getByteOffset()
				, getByteRange() );
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

		ashes::WriteDescriptorSet getDescriptorWrite( uint32_t dstBinding
			, uint32_t dstArrayElement = 0u )const
		{
			auto & uniformBuffer = buffer->getBuffer();
			auto size = uniformBuffer.getAlignedSize();
			auto result = ashes::WriteDescriptorSet{ dstBinding
				, dstArrayElement
				, 1u
				, VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER };
			result.bufferInfo.push_back( VkDescriptorBufferInfo{ uniformBuffer.getBuffer()
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
