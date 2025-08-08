#include "Castor3D/Shader/ShaderBuffer.hpp"

#include "Castor3D/Engine.hpp"
#include "Castor3D/Buffer/GpuBuffer.hpp"
#include "Castor3D/Buffer/UploadData.hpp"
#include "Castor3D/Render/RenderSystem.hpp"

#include <ashespp/Descriptor/DescriptorSet.hpp>

#include <RenderGraph/FramePass.hpp>

CU_ImplementSmartPtr( c3d, ShaderBuffer )

namespace c3d
{
	namespace shdbuf
	{
		static VkDeviceSize constexpr HeaderSize = 4u * sizeof( uint32_t );
	}

	//*********************************************************************************************

	ShaderBuffer::ShaderBuffer( RenderDevice const & device
		, crg::ResourcesCache & resources
		, VkDeviceSize size
		, String const & name
		, AccessState wantedState )
		: m_device{ device }
		, m_size{ ashes::getAlignedSize( size + shdbuf::HeaderSize
			, m_device.renderSystem.getValue( GpuMin::eBufferMapSize ) ) }
		, m_wantedState{ c3d::move( wantedState ) }
		, m_buffer{ makeBufferBase( m_device, resources
			, m_size
			, BufferUsageFlags::eStorageBuffer | BufferUsageFlags::eTransferDst
			, MemoryPropertyFlags::eDeviceLocal
			, name ) }
		, m_ownData( size_t( m_size ), byte{} )
		, m_rawData( m_ownData.data() )
		, m_data{ ( m_rawData + shdbuf::HeaderSize ) }
		, m_counts{ makeArrayView( reinterpret_cast< uint32_t * >( m_rawData )
			, reinterpret_cast< uint32_t * >( m_data ) ) }
	{
		CU_Require( m_rawData );
		m_buffer->create();
	}

	ShaderBuffer::~ShaderBuffer()noexcept
	{
		m_buffer->destroy();
	}

	void ShaderBuffer::upload( UploadData & uploader )const
	{
		uploader.pushUpload( m_rawData
			, m_size
			, *m_buffer->buffer
			, 0u
			, m_wantedState.access
			, m_wantedState.pipelineStage );
	}

	void ShaderBuffer::upload( UploadData & uploader
		, VkDeviceSize offset
		, VkDeviceSize size )const
	{
		uploader.pushUpload( m_rawData
			, size
			, *m_buffer->buffer
			, offset
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
		, uint32_t binding )const
	{
		pass.addInputStorage( *m_buffer->getLastAttach(), binding );
	}

	ashes::WriteDescriptorSet ShaderBuffer::getSingleBinding( uint32_t binding
		, VkDeviceSize offset
		, VkDeviceSize size )const
	{
		auto result = ashes::WriteDescriptorSet{ binding
			, 0u
			, 1u
			, VK_DESCRIPTOR_TYPE_STORAGE_BUFFER };
		result.bufferInfo.push_back( VkDescriptorBufferInfo{ *m_buffer->buffer
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
		result.bufferInfo.push_back( VkDescriptorBufferInfo{ *m_buffer->buffer
			, 0u
			, m_size } );
		return result;
	}

	void ShaderBuffer::createBinding( ashes::DescriptorSet & descriptorSet
		, VkDescriptorSetLayoutBinding const & binding )const
	{
		descriptorSet.createBinding( binding
			, *m_buffer->buffer
			, 0u
			, uint32_t( m_size ) );
	}
}
