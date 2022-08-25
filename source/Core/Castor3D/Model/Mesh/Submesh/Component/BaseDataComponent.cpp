#include "Castor3D/Model/Mesh/Submesh/Component/BaseDataComponent.hpp"

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
	namespace smshbase
	{
		static ashes::PipelineVertexInputStateCreateInfo createVertexLayout( SubmeshFlag submeshData
			, uint32_t & currentBinding
			, uint32_t & currentLocation )
		{
			ashes::VkVertexInputBindingDescriptionArray bindings{ { currentBinding
				, sizeof( castor::Point4f ), VK_VERTEX_INPUT_RATE_VERTEX } };
			ashes::VkVertexInputAttributeDescriptionArray attributes{ 1u, { currentLocation++
				, currentBinding
				, ( submeshData == SubmeshFlag::ePositions
					? VK_FORMAT_R32G32B32A32_SFLOAT
					: VK_FORMAT_R32G32B32_SFLOAT )
				, 0u } };
			++currentBinding;
			return ashes::PipelineVertexInputStateCreateInfo{ 0u, bindings, attributes };
		}

		static castor::Point4fArray convert( castor::Point3fArray const & src )
		{
			castor::Point4fArray result;
			result.reserve( src.size() );

			for ( auto & value : src )
			{
				result.push_back( castor::Point4f{ value->x, value->y, value->z, 1.0f } );
			}

			return result;
		}
	}

	void uploadBaseData( SubmeshFlag submeshData
		, Submesh const & submesh
		, castor::Point3fArray const & data )
	{
		auto count = uint32_t( data.size() );
		auto & offsets = submesh.getSourceBufferOffsets();
		auto & buffer = offsets.getBufferChunk( submeshData );

		if ( count && buffer.hasData() )
		{
			auto up = smshbase::convert( data );
			std::copy( up.begin()
				, up.end()
				, buffer.getData< castor::Point4f >().begin() );
			buffer.markDirty();
		}
	}

	void gatherBaseDataBuffer( SubmeshFlag submeshData
		, PipelineFlags const & flags
		, ashes::PipelineVertexInputStateCreateInfoCRefArray & layouts
		, uint32_t & currentBinding
		, uint32_t & currentLocation
		, std::unordered_map< size_t, ashes::PipelineVertexInputStateCreateInfo > & cache )
	{
		if ( flags.enableVertexInput( submeshData ) )
		{
			auto hash = std::hash< uint32_t >{}( currentBinding );
			hash = castor::hashCombine( hash, currentLocation );
			auto layoutIt = cache.find( hash );

			if ( layoutIt == cache.end() )
			{
				layoutIt = cache.emplace( hash
					, smshbase::createVertexLayout( submeshData
						, currentBinding
						, currentLocation ) ).first;
			}
			else
			{
				currentLocation = layoutIt->second.vertexAttributeDescriptions.back().location + 1u;
				currentBinding = layoutIt->second.vertexAttributeDescriptions.back().binding + 1u;
			}

			layouts.emplace_back( layoutIt->second );
		}
	}
}
