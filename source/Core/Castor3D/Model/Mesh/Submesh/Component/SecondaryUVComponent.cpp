#include "Castor3D/Model/Mesh/Submesh/Component/SecondaryUVComponent.hpp"

#include "Castor3D/Buffer/GpuBuffer.hpp"
#include "Castor3D/Buffer/GpuBufferPool.hpp"
#include "Castor3D/Model/Mesh/Submesh/Submesh.hpp"
#include "Castor3D/Model/Vertex.hpp"
#include "Castor3D/Miscellaneous/makeVkType.hpp"
#include "Castor3D/Render/RenderDevice.hpp"
#include "Castor3D/Render/RenderNodesPass.hpp"
#include "Castor3D/Scene/Scene.hpp"

#include <CastorUtils/Miscellaneous/Hash.hpp>

#include <ashespp/Buffer/VertexBuffer.hpp>

namespace castor3d
{
	namespace smshcompsecuv
	{
		static ashes::PipelineVertexInputStateCreateInfo doCreateVertexLayout( uint32_t & currentLocation )
		{
			ashes::VkVertexInputBindingDescriptionArray bindings{ { SecondaryUVComponent::BindingPoint
				, sizeof( castor::Point3f ), VK_VERTEX_INPUT_RATE_VERTEX } };
			ashes::VkVertexInputAttributeDescriptionArray attributes{ 1u, { currentLocation++
				, SecondaryUVComponent::BindingPoint
				, VK_FORMAT_R32G32B32_SFLOAT
				, 0u } };
			return ashes::PipelineVertexInputStateCreateInfo{ 0u, bindings, attributes };
		}
	}

	castor::String const SecondaryUVComponent::Name = cuT( "secondary_uv" );

	SecondaryUVComponent::SecondaryUVComponent( Submesh & submesh )
		: SubmeshComponent{ submesh, Name, BindingPoint }
	{
	}

	void SecondaryUVComponent::gather( ShaderFlags const & shaderFlags
		, SubmeshFlags const & submeshFlags
		, MaterialRPtr material
		, ashes::BufferCRefArray & buffers
		, std::vector< uint64_t > & offsets
		, ashes::PipelineVertexInputStateCreateInfoCRefArray & layouts
		, TextureFlagsArray const & mask
		, uint32_t & currentLocation )
	{
		if ( checkFlag( submeshFlags, SubmeshFlag::eSecondaryUV ) )
		{
			auto layoutIt = m_layouts.find( currentLocation );

			if ( layoutIt == m_layouts.end() )
			{
				layoutIt = m_layouts.emplace( currentLocation
					, smshcompsecuv::doCreateVertexLayout( currentLocation ) ).first;
			}
			else
			{
				currentLocation = layoutIt->second.vertexAttributeDescriptions.back().location + 1u;
			}

			buffers.emplace_back( m_buffer.getBuffer().getBuffer() );
			offsets.emplace_back( m_buffer.getOffset() );
			layouts.emplace_back( layoutIt->second );
		}
	}

	SubmeshComponentSPtr SecondaryUVComponent::clone( Submesh & submesh )const
	{
		auto result = std::make_shared< SecondaryUVComponent >( submesh );
		result->m_data = m_data;
		return std::static_pointer_cast< SubmeshComponent >( result );
	}

	void SecondaryUVComponent::addTexcoords( std::vector< castor::Point3f > const & uvs )
	{
		m_data.reserve( m_data.size() + uvs.size() );
		m_data.insert( m_data.end(), uvs.begin(), uvs.end() );
	}

	bool SecondaryUVComponent::doInitialise( RenderDevice const & device )
	{
		auto count = getOwner()->getBufferOffsets().getVertexCount< InterleavedVertex >();

		if ( !m_buffer || m_buffer.getCount() != count )
		{
			m_buffer = device.bufferPool->getBuffer< castor::Point3f >( VK_BUFFER_USAGE_VERTEX_BUFFER_BIT
				, count
				, VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT );
		}

		return bool( m_buffer );
	}

	void SecondaryUVComponent::doCleanup( RenderDevice const & device )
	{
		if ( m_buffer )
		{
			device.bufferPool->putBuffer( m_buffer );
			m_buffer = {};
		}
	}

	void SecondaryUVComponent::doUpload()
	{
		if ( getOwner()->hasBufferOffsets() )
		{
			if ( getOwner()->getBufferOffsets().hasVertices() )
			{
				std::copy( m_data.begin()
					, m_data.end()
					, m_buffer.getData().begin() );
				m_buffer.markDirty( VK_ACCESS_VERTEX_ATTRIBUTE_READ_BIT
					, VK_PIPELINE_STAGE_VERTEX_INPUT_BIT );
			}
		}
	}
}
