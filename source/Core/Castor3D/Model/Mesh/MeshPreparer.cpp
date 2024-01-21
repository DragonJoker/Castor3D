#include "Castor3D/Model/Mesh/MeshPreparer.hpp"

#include "Castor3D/Config.hpp"
#include "Castor3D/Engine.hpp"
#include "Castor3D/Model/Mesh/Mesh.hpp"
#include "Castor3D/Model/Mesh/Submesh/Submesh.hpp"
#include "Castor3D/Model/Mesh/Submesh/SubmeshUtils.hpp"
#include "Castor3D/Model/Mesh/Submesh/Component/BaseDataComponent.hpp"
#include "Castor3D/Model/Mesh/Submesh/Component/MeshletComponent.hpp"
#include "Castor3D/Model/Mesh/Submesh/Component/MorphComponent.hpp"
#include "Castor3D/Model/Mesh/Submesh/Component/PassMasksComponent.hpp"
#include "Castor3D/Model/Mesh/Submesh/Component/SkinComponent.hpp"
#include "Castor3D/Render/RenderDevice.hpp"
#include "Castor3D/Render/RenderSystem.hpp"

#include <CastorUtils/Config/BeginExternHeaderGuard.hpp>
#include <meshoptimizer.h>
#include <CastorUtils/Config/EndExternHeaderGuard.hpp>

namespace castor3d
{
	namespace meshopt
	{
		struct Remapped
		{
			FaceArray indices;
			std::map< SubmeshData, castor::Point3fArray > baseBuffers;
			castor::Point4fArray tangentBuffer;
			std::vector< SubmeshAnimationBuffer > morphTargets;
			VertexBoneDataArray skin;
			std::vector< PassMasks > passMasks;
		};

		static std::vector< meshopt_Stream > gather( Submesh & submesh
			, TriFaceMapping const & triangles
			, Remapped & remapped )
		{
			std::vector< meshopt_Stream > result;
			remapped.indices = triangles.getData().getFaces();

			for ( uint32_t i = 1u; i < uint32_t( SubmeshData::eOtherMin ); ++i )
			{
				auto submeshData = SubmeshData( i );

				if ( submesh.hasComponent( getBaseDataComponentName( submeshData ) ) )
				{
					if ( submeshData == SubmeshData::eTangents )
					{
						remapped.tangentBuffer = submesh.getTangents();
					}
					else
					{
						if ( submeshData == SubmeshData::ePositions
							|| submeshData == SubmeshData::eNormals
							|| submeshData == SubmeshData::eTexcoords0 )
						{
							result.push_back( { submesh.getBaseData( submeshData ).data()
								, sizeof( castor::Point3f )
								, sizeof( castor::Point3f ) } );
						}

						remapped.baseBuffers.try_emplace( submeshData, submesh.getBaseData( submeshData ) );
					}
				}
			}

			if ( auto skin = submesh.getComponent< SkinComponent >() )
			{
				remapped.skin = skin->getData().getData();
			}

			if ( auto passMasks = submesh.getComponent< PassMasksComponent >() )
			{
				remapped.passMasks = passMasks->getData().getData();
			}

			if ( auto morph = submesh.getComponent< MorphComponent >() )
			{
				for ( auto & buffers : morph->getData().getMorphTargetsBuffers() )
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

		static void * getPtr( castor::Point4fArray & data )
		{
			return data.data()->ptr();
		}

		static void const * getConstPtr( castor::Point3fArray & data )
		{
			return data.data()->constPtr();
		}

		static void const * getConstPtr( castor::Point4fArray & data )
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

		static void * getPtr( std::vector< PassMasks > & data )
		{
			return data.data()->data.data();
		}

		static void const * getConstPtr( std::vector< PassMasks > & data )
		{
			return data.data()->data.data();
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

		static void applyRemap( size_t /*originalVertexCount*/
			, size_t /*destinationVertexCount*/
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
				, buffers.bitangents );
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
			applyRemap( originalVertexCount
				, destinationVertexCount
				, remap
				, buffers.passMasks );
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

			for ( auto & [_, data] : remapped.baseBuffers )
			{
				applyRemap( originalVertexCount
					, destinationVertexCount
					, remap
					, data );
			}

			if ( !remapped.tangentBuffer.empty() )
			{
				applyRemap( originalVertexCount
					, destinationVertexCount
					, remap
					, remapped.tangentBuffer );
			}

			if ( !remapped.skin.empty() )
			{
				applyRemap( originalVertexCount
					, destinationVertexCount
					, remap
					, remapped.skin );
			}

			if ( !remapped.passMasks.empty() )
			{
				applyRemap( originalVertexCount
					, destinationVertexCount
					, remap
					, remapped.passMasks );
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
			uint32_t meshletIndex{};

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
				itDst->meshletIndex = meshletIndex++;
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
				result.push_back( { castor::Point4f{ bounds.center[0], bounds.center[1], bounds.center[2], bounds.radius }
					, castor::Point4f{ bounds.cone_axis[0], bounds.cone_axis[1], bounds.cone_axis[2], bounds.cone_cutoff } } );
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
			, [&parameters]( SubmeshUPtr const & submesh )
			{
				return prepare( *submesh, parameters );
			} );
	}

	bool MeshPreparer::prepare( Submesh & submesh
		, Parameters const & /*parameters*/ )
	{
		auto indexMapping = submesh.getIndexMapping();

		if ( !indexMapping || indexMapping->getComponentsCount() != 3u )
		{
			// Don't optimize non triangular meshes.
			return true;
		}

		auto const & triangles = static_cast< TriFaceMapping const & >( *indexMapping );
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
				meshlet->getData().getCullData() = std::move( cullData );
#	endif
				meshlet->getData().getMeshletsData() = std::move( meshlets );
			}
		}
#endif

		triangles.getData().getFaces() = std::move( remapped.indices );

		for ( auto & [comp, data] : remapped.baseBuffers )
		{
			submesh.setBaseData( comp, std::move( data ) );
		}

		if ( auto tangents = submesh.getComponent< TangentsComponent >() )
		{
			tangents->getData().getData() = std::move( remapped.tangentBuffer );
		}

		if ( auto skin = submesh.getComponent< SkinComponent >() )
		{
			skin->getData().getData() = std::move( remapped.skin );
		}

		if ( auto passMasks = submesh.getComponent< PassMasksComponent >() )
		{
			passMasks->getData().getData() = std::move( remapped.passMasks );
		}

		if ( auto morph = submesh.getComponent< MorphComponent >() )
		{
			morph->getData().getMorphTargetsBuffers() = std::move( remapped.morphTargets );
		}

		return true;
	}
}
