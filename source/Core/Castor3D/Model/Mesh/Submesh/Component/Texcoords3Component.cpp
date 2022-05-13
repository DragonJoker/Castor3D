#include "Castor3D/Model/Mesh/Submesh/Component/Texcoords3Component.hpp"

#include "Castor3D/Buffer/GpuBuffer.hpp"
#include "Castor3D/Buffer/GpuBufferPool.hpp"
#include "Castor3D/Material/Pass/Pass.hpp"
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
	namespace smshcomptex3
	{
		static ashes::PipelineVertexInputStateCreateInfo createVertexLayout( uint32_t & currentBinding
			, uint32_t & currentLocation )
		{
			ashes::VkVertexInputBindingDescriptionArray bindings{ { currentBinding
				, sizeof( castor::Point3f ), VK_VERTEX_INPUT_RATE_VERTEX } };
			ashes::VkVertexInputAttributeDescriptionArray attributes{ 1u, { currentLocation++
				, currentBinding
				, VK_FORMAT_R32G32B32_SFLOAT
				, 0u } };
			++currentBinding;
			return ashes::PipelineVertexInputStateCreateInfo{ 0u, bindings, attributes };
		}
	}

	castor::String const Texcoords3Component::Name = cuT( "texcoords3" );

	Texcoords3Component::Texcoords3Component( Submesh & submesh )
		: SubmeshComponent{ submesh, Name, Id }
	{
	}

	void Texcoords3Component::gather( ShaderFlags const & shaderFlags
		, ProgramFlags const & programFlags
		, SubmeshFlags const & submeshFlags
		, MaterialRPtr material
		, TextureFlagsArray const & mask
		, ashes::BufferCRefArray & buffers
		, std::vector< uint64_t > & offsets
		, ashes::PipelineVertexInputStateCreateInfoCRefArray & layouts
		, uint32_t & currentBinding
		, uint32_t & currentLocation )
	{
		if ( checkFlag( programFlags, ProgramFlag::eForceTexCoords )
			|| ( checkFlag( submeshFlags, SubmeshFlag::eTexcoords3 ) && !mask.empty() ) )
		{
			auto hash = std::hash< uint32_t >{}( currentBinding );
			hash = castor::hashCombine( hash, currentLocation );
			auto layoutIt = m_layouts.find( hash );

			if ( layoutIt == m_layouts.end() )
			{
				layoutIt = m_layouts.emplace( hash
					, smshcomptex3::createVertexLayout( currentBinding, currentLocation ) ).first;
			}
			else
			{
				currentLocation = layoutIt->second.vertexAttributeDescriptions.back().location + 1u;
				currentBinding = layoutIt->second.vertexAttributeDescriptions.back().binding + 1u;
			}

			layouts.emplace_back( layoutIt->second );
		}
	}

	SubmeshComponentSPtr Texcoords3Component::clone( Submesh & submesh )const
	{
		auto result = std::make_shared< Texcoords3Component >( submesh );
		result->m_data = m_data;
		return std::static_pointer_cast< SubmeshComponent >( result );
	}

	SubmeshFlags Texcoords3Component::getSubmeshFlags( Pass const * pass )const
	{
		if ( !pass || pass->getMaxTexCoordSet() > 2u )
		{
			return SubmeshFlag::eTexcoords3;
		}

		return SubmeshFlag{};
	}

	bool Texcoords3Component::doInitialise( RenderDevice const & device )
	{
		return true;
	}

	void Texcoords3Component::doCleanup( RenderDevice const & device )
	{
		m_data.clear();
	}

	void Texcoords3Component::doUpload()
	{
		auto count = uint32_t( m_data.size() );
		auto & offsets = getOwner()->getBufferOffsets();
		auto & buffer = offsets.getBufferChunk( SubmeshFlag::eTexcoords3 );

		if ( count && buffer.hasData() )
		{
			std::copy( m_data.begin()
				, m_data.end()
				, buffer.getData< castor::Point3f >().begin() );
			buffer.markDirty();
		}
	}
}
