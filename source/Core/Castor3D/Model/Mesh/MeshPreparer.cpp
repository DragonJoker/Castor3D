#include "Castor3D/Model/Mesh/MeshPreparer.hpp"

#include "Castor3D/DebugDefines.hpp"
#include "Castor3D/Engine.hpp"
#include "Castor3D/Model/Mesh/Mesh.hpp"
#include "Castor3D/Model/Mesh/Submesh/Submesh.hpp"
#include "Castor3D/Model/Mesh/Submesh/SubmeshUtils.hpp"
#include "Castor3D/Model/Mesh/Submesh/Component/BaseDataComponent.hpp"
#include "Castor3D/Model/Mesh/Submesh/Component/MeshletComponent.hpp"
#include "Castor3D/Model/Mesh/Submesh/Component/MorphComponent.hpp"
#include "Castor3D/Model/Mesh/Submesh/Component/SkinComponent.hpp"
#include "Castor3D/Render/RenderDevice.hpp"
#include "Castor3D/Render/RenderSystem.hpp"

#pragma warning( push )
#pragma warning( disable:4365 )
#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wzero-as-null-pointer-constant"
#pragma GCC diagnostic ignored "-Wold-style-cast"
#include <meshoptimizer.h>
#pragma GCC diagnostic pop
#pragma warning( pop )

namespace castor3d
{
	namespace meshopt
	{
		struct Remapped
		{
			FaceArray indices;
			std::map< SubmeshData, castor::Point3fArray > baseBuffers;
			std::vector< SubmeshAnimationBuffer > morphTargets;
			VertexBoneDataArray skin;
		};

		static std::vector< meshopt_Stream > gather( Submesh & submesh
			, TriFaceMapping const & triangles
			, Remapped & remapped )
		{
			std::vector< meshopt_Stream > result;
			remapped.indices = triangles.getFaces();

			for ( uint32_t i = 1u; i < uint32_t( SubmeshData::eSkin ); ++i )
			{
				auto submeshData = SubmeshData( i );

				if ( submesh.hasComponent( getName( submeshData ) ) )
				{
					if ( submeshData == SubmeshData::ePositions
						|| submeshData == SubmeshData::eNormals
						|| submeshData == SubmeshData::eTexcoords0 )
					{
						result.push_back( { submesh.getBaseData( submeshData ).data()
							, sizeof( castor::Point3f )
							, sizeof( castor::Point3f ) } );
					}

					remapped.baseBuffers.emplace( submeshData, submesh.getBaseData( submeshData ) );
				}
			}

			if ( auto skin = submesh.getComponent< SkinComponent >() )
			{
				remapped.skin = skin->getData();
			}

			if ( auto morph = submesh.getComponent< MorphComponent >() )
			{
				for ( auto & buffers : morph->getMorphTargetsBuffers() )
				{
					remapped.morphTargets.push_back( buffers );
				}
			}

			return result;
		}

		static void * getPtr( castor::Point3fArray & data )
		{
			return data.data()->ptr();
		}

		static void const * getConstPtr( castor::Point3fArray & data )
		{
			return data.data()->constPtr();
		}

		static void * getPtr( VertexBoneDataArray & data )
		{
			return data.data();
		}

		static void const * getConstPtr( VertexBoneDataArray & data )
		{
			return data.data();
		}

		template< typename DataT >
		static void applyRemap( size_t originalVertexCount
			, size_t destinationVertexCount
			, std::vector< uint32_t > const & remap
			, std::vector< DataT > & buffer )
		{
			if ( !buffer.empty() )
			{
				std::vector< DataT > result;
				result.resize( destinationVertexCount );
				meshopt_remapVertexBuffer( getPtr( result )
					, getConstPtr( buffer )
					, originalVertexCount
					, sizeof( DataT )
					, remap.data() );
				buffer = std::move( result );
			}
		}

		static void applyRemap( size_t originalVertexCount
			, size_t destinationVertexCount
			, std::vector< uint32_t > const & remap
			, std::vector< Face > & buffer )
		{
			if ( !buffer.empty() )
			{
				std::vector< Face > result;
				result.resize( buffer.size() );
				meshopt_remapIndexBuffer( result.data()->data()
					, buffer.data()->data()
					, buffer.size() * 3u
					, remap.data() );
				buffer = std::move( result );
			}
		}

		static void applyRemap( size_t originalVertexCount
			, size_t destinationVertexCount
			, std::vector< uint32_t > const & remap
			, SubmeshAnimationBuffer & buffers )
		{
			applyRemap( originalVertexCount
				, destinationVertexCount
				, remap
				, buffers.positions );
			applyRemap( originalVertexCount
				, destinationVertexCount
				, remap
				, buffers.normals );
			applyRemap( originalVertexCount
				, destinationVertexCount
				, remap
				, buffers.tangents );
			applyRemap( originalVertexCount
				, destinationVertexCount
				, remap
				, buffers.texcoords0 );
			applyRemap( originalVertexCount
				, destinationVertexCount
				, remap
				, buffers.texcoords1 );
			applyRemap( originalVertexCount
				, destinationVertexCount
				, remap
				, buffers.texcoords2 );
			applyRemap( originalVertexCount
				, destinationVertexCount
				, remap
				, buffers.texcoords3 );
			applyRemap( originalVertexCount
				, destinationVertexCount
				, remap
				, buffers.colours );
		}

		static void applyRemap( size_t originalVertexCount
			, size_t destinationVertexCount
			, std::vector< uint32_t > const & remap
			, Remapped & remapped )
		{
			applyRemap( originalVertexCount
				, destinationVertexCount
				, remap
				, remapped.indices );

			for ( auto & data : remapped.baseBuffers )
			{
				applyRemap( originalVertexCount
					, destinationVertexCount
					, remap
					, data.second );
			}

			if ( !remapped.skin.empty() )
			{
				applyRemap( originalVertexCount
					, destinationVertexCount
					, remap
					, remapped.skin );
			}

			for ( auto & remappedMorph : remapped.morphTargets )
			{
				applyRemap( originalVertexCount
					, destinationVertexCount
					, remap
					, remappedMorph );
			}
		}

		static size_t remap( std::vector< meshopt_Stream > const & streams
			, Remapped & remapped )
		{
			auto indexCount = remapped.indices.size() * 3u;
			auto vertexCount = remapped.baseBuffers.begin()->second.size();
			std::vector< uint32_t > remap( vertexCount );
			auto newVertexCount = meshopt_generateVertexRemapMulti( remap.data()
				, remapped.indices.data()->data()
				, indexCount
				, vertexCount
				, streams.data()
				, streams.size() );
			applyRemap( vertexCount
				, newVertexCount
				, remap
				, remapped );
			return newVertexCount;
		}

		static void optimizeCache( Remapped & remapped
			, size_t vertexCount )
		{
			auto indexCount = remapped.indices.size() * 3u;
			auto previous = remapped.indices;
			meshopt_optimizeVertexCache( remapped.indices.data()->data()
				, previous.data()->data()
				, indexCount
				, vertexCount );
		}

		static void optimizeOverdraw( Remapped & remapped )
		{
			auto it = remapped.baseBuffers.find( SubmeshData::ePositions );
			auto indexCount = remapped.indices.size() * 3u;
			auto previous = remapped.indices;
			meshopt_optimizeOverdraw( remapped.indices.data()->data()
				, previous.data()->data()
				, indexCount
				, it->second.data()->constPtr()
				, it->second.size()
				, sizeof( castor::Point3f )
				, 1.05f );
		}

		static void optimizeFetch( Remapped & remapped )
		{
			auto indexCount = remapped.indices.size() * 3u;
			auto vertexCount = remapped.baseBuffers.begin()->second.size();
			std::vector< uint32_t > remap;
			remap.resize( vertexCount );
			auto newVertexCount = meshopt_optimizeVertexFetchRemap( remap.data()
				, remapped.indices.data()->data()
				, indexCount
				, vertexCount );
			applyRemap( vertexCount
				, newVertexCount
				, remap
				, remapped );
		}

#if C3D_UseMeshShaders

		static std::vector< Meshlet > buildMeshlets( Remapped const & remapped )
		{
			auto indexCount = remapped.indices.size() * 3u;
			auto maxMeshlets = meshopt_buildMeshletsBound( indexCount
				, MaxMeshletVertexCount
				, MaxMeshletTriangleCount );

			auto vertexCount = remapped.baseBuffers.begin()->second.size();
			std::vector< meshopt_Meshlet > meshlets( maxMeshlets );
			std::vector< uint8_t > triangles( maxMeshlets * MaxMeshletTriangleCount * 3 );
			std::vector< uint32_t > vertices( maxMeshlets * MaxMeshletVertexCount );
			auto meshletCount = meshopt_buildMeshletsScan( meshlets.data()
				, vertices.data()
				, triangles.data()
				, remapped.indices.data()->data()
				, indexCount
				, vertexCount
				, MaxMeshletVertexCount
				, MaxMeshletTriangleCount );
			std::vector< Meshlet > result( meshletCount );
			auto itSrc = meshlets.begin();
			auto itDst = result.begin();

			while ( itDst != result.end() )
			{
				std::copy( triangles.begin() + itSrc->triangle_offset
					, triangles.begin() + itSrc->triangle_offset + itSrc->triangle_count * 3u
					, itDst->primitives.begin() );
				std::copy( vertices.begin() + itSrc->vertex_offset
					, vertices.begin() + itSrc->vertex_offset + itSrc->vertex_count
					, itDst->vertices.begin() );
				itDst->triangleCount = itSrc->triangle_count;
				itDst->vertexCount = itSrc->vertex_count;
				++itSrc;
				++itDst;
			}

			return result;
		}

#	if C3D_UseTaskShaders

		static std::vector< MeshletCullData > buildBoundingData( std::vector< Meshlet > const & meshlets
			, Remapped const & remapped )
		{
			std::vector< MeshletCullData > result;
			result.reserve( meshlets.size() );
			auto it = remapped.baseBuffers.find( SubmeshData::ePositions );

			for ( auto & meshlet : meshlets )
			{
				meshopt_Bounds bounds = meshopt_computeMeshletBounds( meshlet.vertices.data()
					, meshlet.primitives.data()
					, meshlet.triangleCount
					, it->second.data()->constPtr()
					, it->second.size()
					, sizeof( castor::Point3f ) );
				result.emplace_back( castor::Point4f{ bounds.center[0], bounds.center[1], bounds.center[2], bounds.radius } );
			}

			return result;
		}

#	endif
#endif
	}

	bool MeshPreparer::prepare( Mesh & mesh
		, Parameters const & parameters )
	{
		return std::all_of( mesh.begin()
			, mesh.end()
			, [&parameters]( SubmeshSPtr const & submesh )
			{
				return prepare( *submesh, parameters );
			} );
	}

	bool MeshPreparer::prepare( Submesh & submesh
		, Parameters const & parameters )
	{
		auto indexMapping = submesh.getIndexMapping();

		if ( !indexMapping || indexMapping->getComponentsCount() != 3u )
		{
			// Don't optimize non triangular meshes.
			return true;
		}

		auto & triangles = static_cast< TriFaceMapping & >( *indexMapping );
		meshopt::Remapped remapped;
		auto streams = meshopt::gather( submesh
			, triangles
			, remapped );
		auto newVertexCount = meshopt::remap( streams
			, remapped );
		meshopt::optimizeCache( remapped
			, newVertexCount );
		meshopt::optimizeOverdraw( remapped );
		meshopt::optimizeFetch( remapped );

#if C3D_UseMeshShaders
		RenderDevice & device = submesh.getOwner()->getOwner()->getRenderSystem()->getRenderDevice();

		if ( device.hasMeshAndTaskShaders() )
		{
			if ( auto meshlet = submesh.createComponent< MeshletComponent >() )
			{
				auto meshlets = meshopt::buildMeshlets( remapped );
#	if C3D_UseTaskShaders
				auto cullData = meshopt::buildBoundingData( meshlets, remapped );
				meshlet->getCullData() = std::move( cullData );
#	endif
				meshlet->getMeshletsData() = std::move( meshlets );
			}
		}
#endif

		triangles.getFaces() = std::move( remapped.indices );

		for ( auto & data : remapped.baseBuffers )
		{
			submesh.setBaseData( data.first, std::move( data.second ) );
		}

		if ( auto skin = submesh.getComponent< SkinComponent >() )
		{
			skin->getData() = std::move( remapped.skin );
		}

		if ( auto morph = submesh.getComponent< MorphComponent >() )
		{
			morph->getMorphTargetsBuffers() = std::move( remapped.morphTargets );
		}

		return true;
	}
}
