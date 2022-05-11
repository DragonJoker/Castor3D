#include "Castor3D/Model/Mesh/Submesh/Component/PositionsComponent.hpp"

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
	namespace smshcomppos
	{
		static ashes::PipelineVertexInputStateCreateInfo createVertexLayout( uint32_t & currentLocation )
		{
			ashes::VkVertexInputBindingDescriptionArray bindings{ { PositionsComponent::BindingPoint
				, sizeof( castor::Point3f ), VK_VERTEX_INPUT_RATE_VERTEX } };
			ashes::VkVertexInputAttributeDescriptionArray attributes{ 1u, { currentLocation++
				, PositionsComponent::BindingPoint
				, VK_FORMAT_R32G32B32_SFLOAT
				, 0u } };
			return ashes::PipelineVertexInputStateCreateInfo{ 0u, bindings, attributes };
		}
	}

	castor::String const PositionsComponent::Name = cuT( "positions" );

	PositionsComponent::PositionsComponent( Submesh & submesh )
		: SubmeshComponent{ submesh, Name, BindingPoint }
	{
	}

	void PositionsComponent::gather( ShaderFlags const & shaderFlags
		, SubmeshFlags const & submeshFlags
		, MaterialRPtr material
		, ashes::BufferCRefArray & buffers
		, std::vector< uint64_t > & offsets
		, ashes::PipelineVertexInputStateCreateInfoCRefArray & layouts
		, TextureFlagsArray const & mask
		, uint32_t & currentLocation )
	{
		if ( checkFlag( submeshFlags, SubmeshFlag::ePositions ) )
		{
			auto layoutIt = m_layouts.find( currentLocation );

			if ( layoutIt == m_layouts.end() )
			{
				auto loc = currentLocation;
				layoutIt = m_layouts.emplace( loc
					, smshcomppos::createVertexLayout( currentLocation ) ).first;
			}
			else
			{
				currentLocation = layoutIt->second.vertexAttributeDescriptions.back().location + 1u;
			}

			layouts.emplace_back( layoutIt->second );
		}
	}

	SubmeshComponentSPtr PositionsComponent::clone( Submesh & submesh )const
	{
		auto result = std::make_shared< PositionsComponent >( submesh );
		result->m_data = m_data;
		return std::static_pointer_cast< SubmeshComponent >( result );
	}

	bool PositionsComponent::doInitialise( RenderDevice const & device )
	{
		return true;
	}

	void PositionsComponent::doCleanup( RenderDevice const & device )
	{
		m_data.clear();
	}

	void PositionsComponent::doUpload()
	{
		auto count = uint32_t( m_data.size() );
		auto & offsets = getOwner()->getBufferOffsets();
		auto & buffer = offsets.getBufferChunk( SubmeshFlag::ePositions );

		if ( count && buffer.hasData() )
		{
			std::copy( m_data.begin()
				, m_data.end()
				, buffer.getData< castor::Point3f >().begin() );
			buffer.markDirty();
		}
	}
}
