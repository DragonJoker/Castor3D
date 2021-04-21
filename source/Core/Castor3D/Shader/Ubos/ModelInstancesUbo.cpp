#include "Castor3D/Shader/Ubos/ModelInstancesUbo.hpp"

#include "Castor3D/Engine.hpp"
#include "Castor3D/Buffer/UniformBufferPools.hpp"
#include "Castor3D/Render/RenderSystem.hpp"

#include <ashespp/Buffer/StagingBuffer.hpp>

namespace castor3d
{
	castor::String const ModelInstancesUbo::BufferModelInstances = cuT( "ModelInstances" );
	castor::String const ModelInstancesUbo::Instances = cuT( "c3d_instances" );
	castor::String const ModelInstancesUbo::InstanceCount = cuT( "c3d_instanceCount" );

	ModelInstancesUbo::ModelInstancesUbo( Engine & engine )
		: m_engine{ engine }
	{
		if ( engine.getRenderSystem()->hasCurrentRenderDevice() )
		{
			initialise( engine.getRenderSystem()->getCurrentRenderDevice() );
		}
	}

	ModelInstancesUbo::~ModelInstancesUbo()
	{
	}

	void ModelInstancesUbo::initialise( RenderDevice const & device )
	{
		if ( !m_ubo )
		{
			m_ubo = device.uboPools->getBuffer< Configuration >( VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT );
		}
	}

	void ModelInstancesUbo::cleanup( RenderDevice const & device )
	{
		if ( m_ubo )
		{
			device.uboPools->putBuffer( m_ubo );
		}
	}

	void ModelInstancesUbo::cpuUpdate( UInt32Array const & instances )
	{
		castor3d::cpuUpdate( m_ubo, instances );
	}

	void cpuUpdate( UniformBufferOffsetT< ModelInstancesUboConfiguration > & buffer
		, UInt32Array const & instances )
	{
		CU_Require( buffer );
		auto & configuration = buffer.getData();
		uint32_t index = 0u;

		for ( uint32_t i = 0u; i < instances.size(); ++i )
		{
			auto & instance = configuration.instances[index++];

			for ( uint32_t j = 0u; j < 4u && i < instances.size(); ++i, ++j )
			{
				instance[j] = instances[i];
			}

			--i;
		}

		configuration.instanceCount = uint32_t( instances.size() );
	}
}
