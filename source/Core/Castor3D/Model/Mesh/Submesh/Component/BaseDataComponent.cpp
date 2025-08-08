#include "Castor3D/Model/Mesh/Submesh/Component/BaseDataComponent.hpp"

#include "Castor3D/Buffer/GpuBuffer.hpp"
#include "Castor3D/Buffer/GpuBufferPool.hpp"
#include "Castor3D/Buffer/UploadData.hpp"
#include "Castor3D/Model/Mesh/Submesh/Submesh.hpp"
#include "Castor3D/Model/Vertex.hpp"
#include "Castor3D/Miscellaneous/makeVkType.hpp"
#include "Castor3D/Render/RenderDevice.hpp"
#include "Castor3D/Render/RenderNodesPass.hpp"
#include "Castor3D/Scene/Scene.hpp"

#include <CastorUtils/Miscellaneous/Hash.hpp>

#include <ashespp/Buffer/VertexBuffer.hpp>

namespace c3d
{
	namespace smshbase
	{
		static ashes::PipelineVertexInputStateCreateInfo createVertexLayout( SubmeshData submeshData
			, uint32_t & currentBinding
			, uint32_t & currentLocation )
		{
			ashes::VkVertexInputBindingDescriptionArray bindings{ { currentBinding
				, sizeof( Point4f ), VK_VERTEX_INPUT_RATE_VERTEX } };
			ashes::VkVertexInputAttributeDescriptionArray attributes{ 1u, { currentLocation++
				, currentBinding
				, ( ( submeshData == SubmeshData::ePositions || submeshData == SubmeshData::eTangents )
					? VK_FORMAT_R32G32B32A32_SFLOAT
					: VK_FORMAT_R32G32B32_SFLOAT )
				, 0u } };
			++currentBinding;
			return ashes::PipelineVertexInputStateCreateInfo{ 0u, bindings, attributes };
		}

		static Point4fArray convert( Point3fArray const & src )
		{
			Point4fArray result;
			result.reserve( src.size() );

			for ( auto & value : src )
			{
				result.push_back( Point4f{ value->x, value->y, value->z, 1.0f } );
			}

			return result;
		}
	}

	void uploadBaseData( SubmeshData submeshData
		, Submesh const & submesh
		, Point4fArray const & data
		, Point4fArray &
		, UploadData & uploader )
	{
		auto count = uint32_t( data.size() );
		auto & offsets = submesh.getSourceBufferOffsets();
		auto & buffer = offsets.getBufferChunk( submeshData );

		if ( count && buffer.hasData() )
		{
			uploader.pushUpload( data.data()
				, data.size() * sizeof( Point4f )
				, buffer.getBuffer(), buffer.getOffset()
				, VertexAttributeInputState );
		}
	}

	void uploadBaseData( SubmeshData submeshData
		, Submesh const & submesh
		, Point3fArray const & data
		, Point4fArray & up
		, UploadData & uploader )
	{
		up = smshbase::convert( data );
		uploadBaseData( submeshData
			, submesh
			, up
			, up
			, uploader );
	}

	void gatherBaseDataBuffer( SubmeshData submeshData
		, ObjectBufferOffset const & bufferOffsets
		, PipelineFlags const & flags
		, ashes::BufferCRefArray & buffers
		, Vector< uint64_t > & offsets
		, ashes::PipelineVertexInputStateCreateInfoCRefArray & layouts
		, uint32_t & currentBinding
		, uint32_t & currentLocation
		, HashMap< size_t, ashes::PipelineVertexInputStateCreateInfo > & cache )
	{
		auto & bufferChunk = bufferOffsets.getBufferChunk( submeshData );

		if ( bufferChunk.hasData()
			&& flags.enableVertexInput( submeshData ) )
		{
			auto hash = std::hash< uint32_t >{}( currentBinding );
			hash = hashCombine( hash, currentLocation );
			auto layoutIt = cache.find( hash );

			if ( layoutIt == cache.end() )
			{
				layoutIt = cache.try_emplace( hash
					, smshbase::createVertexLayout( submeshData
						, currentBinding
						, currentLocation ) ).first;
			}
			else
			{
				currentLocation = layoutIt->second.vertexAttributeDescriptions.back().location + 1u;
				currentBinding = layoutIt->second.vertexAttributeDescriptions.back().binding + 1u;
			}

			buffers.emplace_back( bufferChunk.getBuffer().getBuffer() );
			offsets.emplace_back( 0u );
			layouts.emplace_back( layoutIt->second );
		}
	}

	void fillBaseSurfaceType( SubmeshData submeshData
		, sdw::type::IOStruct & type
		, uint32_t & index )
	{
		switch (submeshData)
		{
		case SubmeshData::ePositions:
			type.declMember( "position", ast::type::Kind::eVec4F, ast::type::NotArray, index );
			++index;
			break;
		case SubmeshData::eNormals:
			type.declMember( "normal", ast::type::Kind::eVec3F, ast::type::NotArray, index );
			++index;
			break;
		case SubmeshData::eTangents:
			type.declMember( "tangent", ast::type::Kind::eVec4F, ast::type::NotArray, index );
			++index;
			break;
		case SubmeshData::eBitangents:
			type.declMember( "bitangent", ast::type::Kind::eVec3F, ast::type::NotArray, index );
			++index;
			break;
		case SubmeshData::eTexcoords0:
			type.declMember( "texture0", ast::type::Kind::eVec3F, ast::type::NotArray, index );
			++index;
			break;
		case SubmeshData::eTexcoords1:
			type.declMember( "texture1", ast::type::Kind::eVec3F, ast::type::NotArray, index );
			++index;
			break;
		case SubmeshData::eTexcoords2:
			type.declMember( "texture2", ast::type::Kind::eVec3F, ast::type::NotArray, index );
			++index;
			break;
		case SubmeshData::eTexcoords3:
			type.declMember( "texture3", ast::type::Kind::eVec3F, ast::type::NotArray, index );
			++index;
			break;
		case SubmeshData::eColours:
			type.declMember( "colour", ast::type::Kind::eVec3F, ast::type::NotArray, index );
			++index;
			break;
		case SubmeshData::ePassMasks:
			type.declMember( "passMasks", ast::type::Kind::eVec4U, ast::type::NotArray, index );
			++index;
			break;
		default:
			break;
		}
	}

	void fillBaseSurfaceType( SubmeshData submeshData
		, sdw::type::BaseStruct & type )
	{
		switch (submeshData)
		{
		case SubmeshData::ePositions:
			type.declMember( "position", ast::type::Kind::eVec4F, ast::type::NotArray );
			break;
		case SubmeshData::eNormals:
			type.declMember( "normal", ast::type::Kind::eVec3F, ast::type::NotArray );
			break;
		case SubmeshData::eTangents:
			type.declMember( "tangent", ast::type::Kind::eVec4F, ast::type::NotArray );
			break;
		case SubmeshData::eBitangents:
			type.declMember( "bitangent", ast::type::Kind::eVec3F, ast::type::NotArray );
			break;
		case SubmeshData::eTexcoords0:
			type.declMember( "texture0", ast::type::Kind::eVec3F, ast::type::NotArray );
			break;
		case SubmeshData::eTexcoords1:
			type.declMember( "texture1", ast::type::Kind::eVec3F, ast::type::NotArray );
			break;
		case SubmeshData::eTexcoords2:
			type.declMember( "texture2", ast::type::Kind::eVec3F, ast::type::NotArray );
			break;
		case SubmeshData::eTexcoords3:
			type.declMember( "texture3", ast::type::Kind::eVec3F, ast::type::NotArray );
			break;
		case SubmeshData::eColours:
			type.declMember( "colour", ast::type::Kind::eVec3F, ast::type::NotArray );
			break;
		case SubmeshData::ePassMasks:
			type.declMember( "passMasks", ast::type::Kind::eVec4U, ast::type::NotArray );
			break;
		default:
			break;
		}
	}

	String getBaseDataComponentName( SubmeshData submeshData )
	{
		return cuT( "c3d.submesh." ) + string::lowerCase( getName( submeshData ) );
	}
}
