#include "Castor3D/Shader/ShaderBuffer.hpp"

#include "Castor3D/Engine.hpp"
#include "Castor3D/Buffer/GpuBuffer.hpp"
#include "Castor3D/Buffer/UploadData.hpp"
#include "Castor3D/Render/RenderSystem.hpp"

#include <ashespp/Descriptor/DescriptorSet.hpp>

#include <RenderGraph/FramePass.hpp>

CU_ImplementSmartPtr( castor3d, ShaderBuffer )

namespace castor3d
{
	namespace shdbuf
	{
		static VkDeviceSize constexpr HeaderSize = 4u * sizeof( uint32_t );
	}

	//*********************************************************************************************

	ShaderBuffer::ShaderBuffer( Engine & engine
		, RenderDevice const & device
		, VkDeviceSize size
		, castor::String name
		, crg::AccessState wantedState )
		: m_device{ device }
		, m_size{ ashes::getAlignedSize( size + 4 * sizeof( uint32_t )
			, m_device.renderSystem.getValue( GpuMin::eBufferMapSize ) ) }
		, m_wantedState{ std::move( wantedState ) }
		, m_buffer{ makeBufferBase( m_device
			, m_size
			, ( VK_BUFFER_USAGE_STORAGE_BUFFER_BIT | VK_BUFFER_USAGE_TRANSFER_DST_BIT )
			, VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT
			, name ) }
		, m_ownData( size_t( m_size ), byte{} )
		, m_rawData( m_ownData.data() )
		, m_data{ ( m_rawData + shdbuf::HeaderSize ) }
		, m_counts{ castor::makeArrayView( reinterpret_cast< uint32_t * >( m_rawData )
			, reinterpret_cast< uint32_t * >( m_data ) ) }
	{
		CU_Require( m_rawData );
	}

	void ShaderBuffer::upload( UploadData & uploader )const
	{
		uploader.pushUpload( m_rawData
			, m_size
			, *m_buffer
			, 0u
			, m_wantedState.access
			, m_wantedState.pipelineStage );
	}

	VkDescriptorSetLayoutBinding ShaderBuffer::createLayoutBinding( uint32_t index
		, VkShaderStageFlags stages )const
	{
		return makeDescriptorSetLayoutBinding( index
			, VK_DESCRIPTOR_TYPE_STORAGE_BUFFER
			, stages );
	}

	void ShaderBuffer::createPassBinding( crg::FramePass & pass
		, castor::String const & name
		, uint32_t binding )const
	{
		pass.addInputStorageBuffer( { *m_buffer, name }
			, binding
			, 0u
			, uint32_t( m_size ) );
	}

	ashes::WriteDescriptorSet ShaderBuffer::getSingleBinding( uint32_t binding
		, VkDeviceSize offset
		, VkDeviceSize size )const
	{
		auto result = ashes::WriteDescriptorSet{ binding
			, 0u
			, 1u
			, VK_DESCRIPTOR_TYPE_STORAGE_BUFFER };
		result.bufferInfo.push_back( VkDescriptorBufferInfo{ *m_buffer
			, offset + sizeof( uint32_t ) * 4u
			, size } );
		return result;
	}

	ashes::WriteDescriptorSet ShaderBuffer::getBinding( uint32_t binding )const
	{
		auto result = ashes::WriteDescriptorSet{ binding
			, 0u
			, 1u
			, VK_DESCRIPTOR_TYPE_STORAGE_BUFFER };
		result.bufferInfo.push_back( VkDescriptorBufferInfo{ *m_buffer
			, 0u
			, m_size } );
		return result;
	}

	void ShaderBuffer::createBinding( ashes::DescriptorSet & descriptorSet
		, VkDescriptorSetLayoutBinding const & binding )const
	{
		descriptorSet.createBinding( binding
			, *m_buffer
			, 0u
			, uint32_t( m_size ) );
	}
}
