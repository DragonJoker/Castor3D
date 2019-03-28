#include "Castor3D/Shader/ShaderBuffer.hpp"

#include "Castor3D/Engine.hpp"
#include "Castor3D/Render/RenderSystem.hpp"

#include <Ashes/Buffer/Buffer.hpp>
#include <Ashes/Buffer/UniformBuffer.hpp>
#include <Ashes/Core/Device.hpp>
#include <Ashes/Descriptor/DescriptorSet.hpp>
#include <Ashes/Descriptor/DescriptorSetLayoutBinding.hpp>

using namespace castor;

namespace castor3d
{
	//*********************************************************************************************

	namespace
	{
		ashes::BufferBasePtr doCreateBuffer( Engine & engine
			, ashes::DeviceSize size
			, castor::String name )
		{
			ashes::BufferBasePtr result;
			ashes::BufferTarget target = engine.getRenderSystem()->getGpuInformations().hasFeature( GpuFeature::eShaderStorageBuffers )
					? ashes::BufferTarget::eStorageBuffer
					: ashes::BufferTarget::eUniformTexelBuffer;
			auto & device = getCurrentDevice( engine );
			result = device.createBuffer( uint32_t( size )
				, target | ashes::BufferTarget::eTransferDst
				, ashes::MemoryPropertyFlag::eHostVisible );
			device.debugMarkerSetObjectName(
				{
					ashes::DebugReportObjectType::eBuffer,
					result.get(),
					std::move( name )
				} );
			return result;
		}

		ashes::BufferViewPtr doCreateView( Engine & engine
			, ashes::Format tboFormat
			, ashes::BufferBase const & buffer )
		{
			ashes::BufferViewPtr result;

			if ( !engine.getRenderSystem()->getGpuInformations().hasFeature( GpuFeature::eShaderStorageBuffers ) )
			{
				result = getCurrentDevice( engine ).createBufferView( buffer
					, tboFormat
					, 0u
					, uint32_t( buffer.getSize() ) );
			}

			return result;
		}
	}

	//*********************************************************************************************

	ShaderBuffer::ShaderBuffer( Engine & engine
		, uint32_t size
		, castor::String name
		, ashes::Format tboFormat )
		: m_engine{ engine }
		, m_size{ size }
		, m_buffer{ doCreateBuffer( engine, m_size, name ) }
		, m_bufferView{ doCreateView( engine, tboFormat, *m_buffer ) }
		, m_data( size_t( m_size ), uint8_t( 0 ) )
	{
	}

	ShaderBuffer::~ShaderBuffer()
	{
		m_bufferView.reset();
		m_buffer.reset();
	}

	void ShaderBuffer::update()
	{
		doUpdate( 0u, ashes::WholeSize );
	}

	void ShaderBuffer::update( ashes::DeviceSize offset
		, ashes::DeviceSize size )
	{
		doUpdate( 0u
			, std::min( m_size
				, ashes::getAlignedSize( size
					, uint32_t( m_engine.getRenderSystem()->getProperties().limits.nonCoherentAtomSize ) ) ) );
	}

	ashes::DescriptorSetLayoutBinding ShaderBuffer::createLayoutBinding( uint32_t index )const
	{
		if ( m_bufferView )
		{
			return { index, ashes::DescriptorType::eUniformTexelBuffer, ashes::ShaderStageFlag::eFragment };
		}
		else
		{
			return { index, ashes::DescriptorType::eStorageBuffer, ashes::ShaderStageFlag::eFragment };
		}
	}

	void ShaderBuffer::createBinding( ashes::DescriptorSet & descriptorSet
		, ashes::DescriptorSetLayoutBinding const & binding )const
	{
		if ( m_bufferView )
		{
			descriptorSet.createBinding( binding
				, *m_buffer
				, *m_bufferView
				, 0u );
		}
		else
		{
			descriptorSet.createBinding( binding
				, *m_buffer
				, 0u
				, uint32_t( m_size ) );
		}
	}

	void ShaderBuffer::doUpdate( ashes::DeviceSize offset
		, ashes::DeviceSize size )
	{
		CU_Require( ( offset + size <= m_size )
			|| ( offset == 0u && size == ashes::WholeSize ) );

		if ( uint8_t * buffer = m_buffer->lock( offset
			, size
			, ashes::MemoryMapFlag::eWrite ) )
		{
			std::memcpy( buffer, m_data.data(), std::min( size, m_size ) );
			m_buffer->flush( offset, size );
			m_buffer->unlock();
		}
	}
}
