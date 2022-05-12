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
		, ProgramFlags const & programFlags
		, SubmeshFlags const & submeshFlags
		, MaterialRPtr material
		, TextureFlagsArray const & mask
		, ashes::BufferCRefArray & buffers
		, std::vector< uint64_t > & offsets
		, ashes::PipelineVertexInputStateCreateInfoCRefArray & layouts
		, uint32_t & currentLocation )
	{
		if ( checkFlag( submeshFlags, SubmeshFlag::eSecondaryUV ) )
		{
			auto layoutIt = m_layouts.find( currentLocation );

			if ( layoutIt == m_layouts.end() )
			{
				auto loc = currentLocation;
				layoutIt = m_layouts.emplace( loc
					, smshcompsecuv::doCreateVertexLayout( currentLocation ) ).first;
			}
			else
			{
				currentLocation = layoutIt->second.vertexAttributeDescriptions.back().location + 1u;
			}

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
		return true;
	}

	void SecondaryUVComponent::doCleanup( RenderDevice const & device )
	{
	}

	void SecondaryUVComponent::doUpload()
	{
		auto count = uint32_t( m_data.size() );
		auto & offsets = getOwner()->getBufferOffsets();
		auto & buffer = offsets.getBufferChunk( SubmeshFlag::eSecondaryUV );

		if ( count && buffer.hasData() )
		{
			std::copy( m_data.begin()
				, m_data.end()
				, buffer.getData< castor::Point3f >().begin() );
			buffer.markDirty();
		}
	}
}
