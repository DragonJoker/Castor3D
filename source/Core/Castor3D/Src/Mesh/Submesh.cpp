#include "Submesh.hpp"

#include "SubmeshUtils.hpp"

#include "Engine.hpp"

#include "Event/Frame/FrameListener.hpp"
#include "Event/Frame/FunctorEvent.hpp"
#include "Mesh/Buffer/Buffer.hpp"
#include "Mesh/Skeleton/BonedVertex.hpp"
#include "Scene/Scene.hpp"
#include "Shader/ShaderProgram.hpp"
#include "Shader/ShaderStorageBuffer.hpp"
#include "Vertex.hpp"

#include <Miscellaneous/BlockTracker.hpp>

using namespace Castor;

//*************************************************************************************************

namespace Castor3D
{
	namespace
	{
		template< typename T, typename U >
		inline void DoCopyVertices( uint32_t p_count, InterleavedVertexT< T > const * p_src, InterleavedVertexT< U > * p_dst )
		{
			for ( uint32_t i{ 0u }; i < p_count; ++i )
			{
				p_dst->m_pos[0] = U( p_src->m_pos[0] );
				p_dst->m_pos[1] = U( p_src->m_pos[1] );
				p_dst->m_pos[2] = U( p_src->m_pos[2] );
				p_dst->m_nml[0] = U( p_src->m_nml[0] );
				p_dst->m_nml[1] = U( p_src->m_nml[1] );
				p_dst->m_nml[2] = U( p_src->m_nml[2] );
				p_dst->m_tan[0] = U( p_src->m_tan[0] );
				p_dst->m_tan[1] = U( p_src->m_tan[1] );
				p_dst->m_tan[2] = U( p_src->m_tan[2] );
				p_dst->m_bin[0] = U( p_src->m_bin[0] );
				p_dst->m_bin[1] = U( p_src->m_bin[1] );
				p_dst->m_bin[2] = U( p_src->m_bin[2] );
				p_dst->m_tex[0] = U( p_src->m_tex[0] );
				p_dst->m_tex[1] = U( p_src->m_tex[1] );
				p_dst->m_tex[2] = U( p_src->m_tex[2] );
				p_dst++;
				p_src++;
			}
		}
	}

	//*************************************************************************************************

	bool BinaryWriter< Submesh >::DoWrite( Submesh const & p_obj )
	{
		bool result = true;

		if ( result )
		{
			VertexBuffer const & buffer = p_obj.GetVertexBuffer();
			size_t size = buffer.GetSize();
			uint32_t stride = buffer.GetDeclaration().stride();
			uint32_t count = uint32_t( size / stride );
			result = DoWriteChunk( count, ChunkType::eSubmeshVertexCount, m_chunk );

			if ( result )
			{
				InterleavedVertex const * srcbuf = reinterpret_cast< InterleavedVertex const * >( buffer.GetData() );
				std::vector< InterleavedVertexT< double > > dstbuf( count );
				DoCopyVertices( count, srcbuf, dstbuf.data() );
				result = DoWriteChunk( dstbuf, ChunkType::eSubmeshVertex, m_chunk );
			}
		}

		if ( result )
		{
			IndexBuffer const & buffer = p_obj.GetIndexBuffer();
			uint32_t count = buffer.GetSize() / 3;
			result = DoWriteChunk( count, ChunkType::eSubmeshFaceCount, m_chunk );

			if ( result )
			{
				FaceIndices const * srcbuf = reinterpret_cast< FaceIndices const * >( buffer.GetData() );
				result = DoWriteChunk( srcbuf, buffer.GetSize() / 3, ChunkType::eSubmeshFaces, m_chunk );
			}
		}

		if ( result && p_obj.HasBonesBuffer() )
		{
			VertexBuffer const & buffer = p_obj.GetBonesBuffer();
			uint32_t stride = buffer.GetDeclaration().stride();
			uint32_t count = buffer.GetSize() / stride;
			result = DoWriteChunk( count, ChunkType::eSubmeshBoneCount, m_chunk );

			if ( result )
			{
				VertexBoneData const * srcbuf = reinterpret_cast< VertexBoneData const * >( buffer.GetData() );
				result = DoWriteChunk( srcbuf, buffer.GetSize() / sizeof( VertexBoneData ), ChunkType::eSubmeshBones, m_chunk );
			}
		}

		return result;
	}

	//*************************************************************************************************

	bool BinaryParser< Submesh >::DoParse( Submesh & p_obj )
	{
		bool result = true;
		String name;
		std::vector< FaceIndices > faces;
		std::vector< VertexBoneData > bones;
		std::vector< InterleavedVertexT< double > > srcbuf;
		uint32_t count{ 0u };
		uint32_t faceCount{ 0u };
		uint32_t boneCount{ 0u };
		BinaryChunk chunk;

		while ( result && DoGetSubChunk( chunk ) )
		{
			switch ( chunk.GetChunkType() )
			{
			case ChunkType::eSubmeshVertexCount:
				result = DoParseChunk( count, chunk );

				if ( result )
				{
					srcbuf.resize( count );
				}

				break;

			case ChunkType::eSubmeshVertex:
				result = DoParseChunk( srcbuf, chunk );

				if ( result && !srcbuf.empty() )
				{
					std::vector< InterleavedVertex > dstbuf( srcbuf.size() );
					DoCopyVertices( uint32_t( srcbuf.size() ), srcbuf.data(), dstbuf.data() );
					p_obj.AddPoints( dstbuf );
				}

				break;

			case ChunkType::eSubmeshBoneCount:
				result = DoParseChunk( count, chunk );

				if ( result )
				{
					boneCount = count;
					bones.resize( count );
				}

				break;

			case ChunkType::eSubmeshBones:
				result = DoParseChunk( bones, chunk );

				if ( result && boneCount > 0 )
				{
					p_obj.AddBoneDatas( bones );
				}

				boneCount = 0u;
				break;

			case ChunkType::eSubmeshFaceCount:
				result = DoParseChunk( count, chunk );

				if ( result )
				{
					faceCount = count;
					faces.resize( count );
				}

				break;

			case ChunkType::eSubmeshFaces:
				result = DoParseChunk( faces, chunk );

				if ( result && faceCount > 0 )
				{
					p_obj.AddFaceGroup( faces );
				}

				faceCount = 0u;
				break;

			default:
				result = false;
				break;
			}
		}

		return result;
	}

	//*************************************************************************************************

	Submesh::Submesh( Scene & p_scene, Mesh & p_mesh, uint32_t p_id )
		: OwnedBy< Scene >( p_scene )
		, m_defaultMaterial( p_scene.GetEngine()->GetMaterialCache().GetDefaultMaterial() )
		, m_id( p_id )
		, m_parentMesh( p_mesh )
		, m_vertexBuffer
		{
			*p_scene.GetEngine(),
			BufferDeclaration
			{
				{
					BufferElementDeclaration( ShaderProgram::Position, uint32_t( ElementUsage::ePosition ), ElementType::eVec3, Vertex::GetOffsetPos() ),
					BufferElementDeclaration( ShaderProgram::Normal, uint32_t( ElementUsage::eNormal ), ElementType::eVec3, Vertex::GetOffsetNml() ),
					BufferElementDeclaration( ShaderProgram::Tangent, uint32_t( ElementUsage::eTangent ), ElementType::eVec3, Vertex::GetOffsetTan() ),
					BufferElementDeclaration( ShaderProgram::Bitangent, uint32_t( ElementUsage::eBitangent ), ElementType::eVec3, Vertex::GetOffsetBin() ),
					BufferElementDeclaration( ShaderProgram::Texture, uint32_t( ElementUsage::eTexCoords ), ElementType::eVec3, Vertex::GetOffsetTex() ),
				}
			}
		}
		, m_indexBuffer{ *p_scene.GetEngine() }
	{
	}

	Submesh::~Submesh()
	{
		Cleanup();
	}

	void Submesh::Initialise()
	{
		if ( !m_generated )
		{
			DoGenerateBuffers();
		}

		if ( !m_initialised )
		{
			m_initialised = m_vertexBuffer.Initialise( BufferAccessType::eDynamic, BufferAccessNature::eDraw );

			if ( m_initialised )
			{
				m_initialised = m_indexBuffer.Initialise( BufferAccessType::eDynamic, BufferAccessNature::eDraw );
			}

			if ( m_initialised && m_animBuffer )
			{
				m_initialised = m_animBuffer->Initialise( BufferAccessType::eDynamic, BufferAccessNature::eDraw );
			}

			if ( m_initialised && m_bonesBuffer )
			{
				m_initialised = m_bonesBuffer->Initialise( BufferAccessType::eDynamic, BufferAccessNature::eDraw );
			}

			if ( m_initialised && m_matrixBuffer )
			{
				m_initialised = m_matrixBuffer->Initialise( BufferAccessType::eDynamic, BufferAccessNature::eDraw );
			}

			if ( m_initialised && m_instancedBonesBuffer )
			{
				m_initialised = m_instancedBonesBuffer->Initialise( BufferAccessType::eDynamic, BufferAccessNature::eDraw );
			}

			m_dirty = !m_initialised;
		}
	}

	void Submesh::Cleanup()
	{
		m_initialised = false;
		DoDestroyBuffers();
		m_faces.clear();
		m_points.clear();
		m_pointsData.clear();
		m_bones.clear();
		m_bonesData.clear();
		m_geometryBuffers.clear();
	}

	void Submesh::ComputeContainers()
	{
		if ( m_pointsData.size() > 0 )
		{
			Point3r min;
			Point3r max;
			Point3r cur;
			Vertex::GetPosition( m_points[0], min );
			Vertex::GetPosition( m_points[0], max );
			uint32_t nbVertex = GetPointsCount();

			for ( uint32_t i = 1; i < nbVertex; i++ )
			{
				Vertex::GetPosition( m_points[i], cur );
				max[0] = std::max( cur[0], max[0] );
				max[1] = std::max( cur[1], max[1] );
				max[2] = std::max( cur[2], max[2] );
				min[0] = std::min( cur[0], min[0] );
				min[1] = std::min( cur[1], min[1] );
				min[2] = std::min( cur[2], min[2] );
			}

			m_box.Load( min, max );
			m_sphere.Load( m_box );
		}
	}

	uint32_t Submesh::GetFaceCount()const
	{
		return std::max( uint32_t( m_faces.size() ), m_indexBuffer.GetSize() / 3 );
	}

	uint32_t Submesh::GetPointsCount()const
	{
		return std::max< uint32_t >( uint32_t( m_points.size() ), m_vertexBuffer.GetSize() / m_vertexBuffer.GetDeclaration().stride() );
	}

	int Submesh::IsInMyPoints( Point3r const & p_vertex, double p_precision )
	{
		int iReturn = -1;
		int iIndex = 0;
		Point3r ptPos;

		for ( VertexPtrArrayConstIt it = m_points.begin(); it != m_points.end() && iReturn == -1; ++it )
		{
			if ( point::length_squared( p_vertex - Vertex::GetPosition( ( *it ), ptPos ) ) < p_precision )
			{
				iReturn = int( iIndex );
			}

			iIndex++;
		}

		return iReturn;
	}

	BufferElementGroupSPtr Submesh::AddPoint( real x, real y, real z )
	{
		BufferElementGroupSPtr result;
		uint32_t stride = 3 * sizeof( real ) * 5;
		m_pointsData.push_back( ByteArray( stride ) );
		uint8_t * data = m_pointsData.back().data();
		result = std::make_shared< BufferElementGroup >( data, uint32_t( m_points.size() ) );
		Vertex::SetPosition( result, x, y, z );
		m_points.push_back( result );
		return result;
	}

	BufferElementGroupSPtr Submesh::AddPoint( Point3r const & p_v )
	{
		return AddPoint( p_v[0], p_v[1], p_v[2] );
	}

	BufferElementGroupSPtr Submesh::AddPoint( real * p_v )
	{
		return AddPoint( p_v[0], p_v[1], p_v[2] );
	}

	void Submesh::AddPoints( InterleavedVertex const * const p_begin, InterleavedVertex const * const p_end )
	{
		uint32_t stride = m_vertexBuffer.GetDeclaration().stride();
		m_pointsData.push_back( ByteArray( std::distance( p_begin, p_end ) * stride ) );
		uint8_t * data = m_pointsData.back().data();

		std::for_each( p_begin, p_end, [this, &data, &stride]( InterleavedVertex const & p_data )
		{
			BufferElementGroupSPtr vertex = std::make_shared< BufferElementGroup >( data, uint32_t( m_points.size() ) );
			Vertex::SetPosition( vertex, p_data.m_pos.data() );
			Vertex::SetNormal( vertex, p_data.m_nml.data() );
			Vertex::SetTangent( vertex, p_data.m_tan.data() );
			Vertex::SetBitangent( vertex, p_data.m_bin.data() );
			Vertex::SetTexCoord( vertex, p_data.m_tex.data() );
			m_points.push_back( vertex );
			data += stride;
		} );
	}

	void Submesh::AddBoneDatas( VertexBoneData const * const p_begin, VertexBoneData const * const p_end )
	{
		uint32_t stride = BonedVertex::Stride;
		m_bonesData.push_back( ByteArray( std::distance( p_begin, p_end ) * stride ) );
		auto data = &( *m_bonesData.rbegin() )[0];

		std::for_each( p_begin, p_end, [this, &data, &stride]( VertexBoneData const & p_data )
		{
			BufferElementGroupSPtr bonesData = std::make_shared< BufferElementGroup >( data, uint32_t( m_bones.size() ) );
			BonedVertex::SetBones( bonesData, p_data );
			m_bones.push_back( bonesData );
			data += stride;
		} );

		AddFlag( m_programFlags, ProgramFlag::eSkinning );
	}

	Face Submesh::AddFace( uint32_t a, uint32_t b, uint32_t c )
	{
		Face result{ a, b, c };

		if ( a < m_points.size() && b < m_points.size() && c < m_points.size() )
		{
			m_faces.push_back( result );
			m_hasNormals = false;
		}
		else
		{
			throw std::range_error( "Submesh::AddFace - One or more index out of bound" );
		}

		return result;
	}

	void Submesh::AddFaceGroup( FaceIndices const * const p_begin, FaceIndices const * const p_end )
	{
		std::for_each( p_begin, p_end, [this]( FaceIndices const & p_data )
		{
			AddFace( p_data.m_index[0], p_data.m_index[1], p_data.m_index[2] );
		} );
	}

	void Submesh::AddQuadFace( uint32_t a, uint32_t b, uint32_t c, uint32_t d, Point3r const & p_minUV, Point3r const & p_maxUV )
	{
		AddFace( a, b, c );
		AddFace( a, c, d );
		Vertex::SetTexCoord( m_points[a], p_minUV[0], p_minUV[1] );
		Vertex::SetTexCoord( m_points[b], p_maxUV[0], p_minUV[1] );
		Vertex::SetTexCoord( m_points[c], p_maxUV[0], p_maxUV[1] );
		Vertex::SetTexCoord( m_points[d], p_minUV[0], p_maxUV[1] );
	}

	void Submesh::ClearFaces()
	{
		m_faces.clear();
		m_indexBuffer.Clear();
	}

	void Submesh::ResetGpuBuffers()
	{
		DoDestroyBuffers();
		DoGenerateBuffers();

		if ( !m_initialised )
		{
			Initialise();
		}

		if ( m_initialised )
		{
			for ( auto & geometryBuffers : m_geometryBuffers )
			{
				DoInitialiseGeometryBuffers( geometryBuffers );
			}
		}
	}

	void Submesh::ResetMatrixBuffers()
	{
		if ( m_matrixBuffer )
		{
			uint32_t count = 0;

			for ( auto it : m_instanceCount )
			{
				count = std::max( count, it.second );
			}

			DoGenerateMatrixBuffer( count );
			m_matrixBuffer->Initialise( BufferAccessType::eStream, BufferAccessNature::eDraw );
		}
	}

	void Submesh::Draw( GeometryBuffers const & p_geometryBuffers )
	{
		ENSURE( m_initialised );
		uint32_t size = m_vertexBuffer.GetSize() / m_vertexBuffer.GetDeclaration().stride();

		if ( !m_indexBuffer.IsEmpty() )
		{
			size = m_indexBuffer.GetSize();
		}

		if ( m_dirty )
		{
			m_vertexBuffer.Upload();

			if ( m_animBuffer )
			{
				m_animBuffer->Upload();
			}

			m_dirty = false;
		}

		p_geometryBuffers.Draw( size, 0 );
	}

	void Submesh::DrawInstanced( GeometryBuffers const & p_geometryBuffers, uint32_t p_count )
	{
		ENSURE( m_initialised );
		uint32_t size = m_vertexBuffer.GetSize() / m_vertexBuffer.GetDeclaration().stride();

		if ( !m_indexBuffer.IsEmpty() )
		{
			size = m_indexBuffer.GetSize();
		}

		if ( m_dirty )
		{
			m_vertexBuffer.Upload();
			m_dirty = false;
		}

		p_geometryBuffers.DrawInstanced( size, 0, p_count );
	}

	void Submesh::ComputeFacesFromPolygonVertex()
	{
		SubmeshUtils::ComputeFacesFromPolygonVertex( *this );
	}

	void Submesh::ComputeNormals( bool p_reverted )
	{
		if ( !m_hasNormals )
		{
			SubmeshUtils::ComputeNormals( *this, p_reverted );
			m_hasNormals = true;
		}
	}

	void Submesh::ComputeNormals( Face const & p_face )
	{
		SubmeshUtils::ComputeNormals( *this, p_face );
	}

	void Submesh::ComputeTangents( Face const & p_face )
	{
		SubmeshUtils::ComputeTangents( *this, p_face );
	}

	void Submesh::ComputeTangentsFromNormals()
	{
		SubmeshUtils::ComputeTangentsFromNormals( *this );
	}

	void Submesh::ComputeTangentsFromBitangents()
	{
		SubmeshUtils::ComputeTangentsFromBitangents( *this );
	}

	void Submesh::ComputeBitangents()
	{
		SubmeshUtils::ComputeBitangents( *this );
	}

	void Submesh::SortByDistance( Point3r const & p_ptCameraPosition )
	{
		ENSURE( m_initialised );

		try
		{
			if ( m_cameraPosition != p_ptCameraPosition )
			{
				if ( m_initialised
					&& !m_indexBuffer.IsEmpty()
					&& !m_vertexBuffer.IsEmpty() )
				{
					IndexBuffer & indices = m_indexBuffer;
					VertexBuffer & vertices = m_vertexBuffer;

					vertices.Bind();
					indices.Bind();
					m_cameraPosition = p_ptCameraPosition;
					uint32_t uiIdxSize = indices.GetSize();
					uint32_t * pIdx = indices.Lock( 0, uiIdxSize, AccessType::eRead | AccessType::eWrite );

					if ( pIdx )
					{
						struct stFACE_DISTANCE
						{
							uint32_t m_index[3];
							double m_distance;
						};
						uint32_t stride = vertices.GetDeclaration().stride();
						uint8_t * pVtx = vertices.GetData();
						DECLARE_VECTOR( stFACE_DISTANCE, Face );
						FaceArray arraySorted;
						arraySorted.reserve( uiIdxSize / 3 );

						if ( pVtx )
						{
							for ( uint32_t * it = pIdx + 0; it < pIdx + uiIdxSize; it += 3 )
							{
								double dDistance = 0.0;
								Coords3r pVtx1( reinterpret_cast< real * >( &pVtx[it[0] * stride] ) );
								dDistance += point::length_squared( pVtx1 - p_ptCameraPosition );
								Coords3r pVtx2( reinterpret_cast< real * >( &pVtx[it[1] * stride] ) );
								dDistance += point::length_squared( pVtx2 - p_ptCameraPosition );
								Coords3r pVtx3( reinterpret_cast< real * >( &pVtx[it[2] * stride] ) );
								dDistance += point::length_squared( pVtx3 - p_ptCameraPosition );
								stFACE_DISTANCE face = { { it[0], it[1], it[2] }, dDistance };
								arraySorted.push_back( face );
							}

							std::sort( arraySorted.begin(), arraySorted.end(), []( stFACE_DISTANCE const & p_left, stFACE_DISTANCE const & p_right )
							{
								return p_left.m_distance < p_right.m_distance;
							} );

							for ( FaceArrayConstIt it = arraySorted.begin(); it != arraySorted.end(); ++it )
							{
								*pIdx++ = it->m_index[0];
								*pIdx++ = it->m_index[1];
								*pIdx++ = it->m_index[2];
							}
						}

						indices.Unlock();
					}

					indices.Unbind();
					vertices.Unbind();
				}
			}
		}
		catch ( Exception const & p_exc )
		{
			Logger::LogError( std::stringstream() << "Submesh::SortFaces - Error: " << p_exc.what() );
		}
	}

	uint32_t Submesh::Ref( MaterialSPtr p_material )
	{
		auto it = m_instanceCount.find( p_material );

		if ( it == m_instanceCount.end() )
		{
			m_instanceCount[p_material] = 0;
			it = m_instanceCount.find( p_material );
		}

		return it->second++;
	}

	uint32_t Submesh::UnRef( MaterialSPtr p_material )
	{
		auto it = m_instanceCount.find( p_material );
		uint32_t result{ 0u };

		if ( it != m_instanceCount.end() )
		{
			result = it->second;

			if ( it->second )
			{
				it->second--;
			}

			if ( !it->second )
			{
				m_instanceCount.erase( it );
			}
		}

		return result;
	}

	uint32_t Submesh::GetRefCount( MaterialSPtr p_material )const
	{
		uint32_t result = 0;
		auto it = m_instanceCount.find( p_material );

		if ( it != m_instanceCount.end() )
		{
			result = it->second;
		}

		return result;
	}

	uint32_t Submesh::GetMaxRefCount()const
	{
		return uint32_t( std::accumulate( m_instanceCount.begin()
			, m_instanceCount.end()
			, 0u
			, []( uint32_t p_value, auto const & p_pair )
		{
			return std::max( p_value, p_pair.second );
		} ) );
	}

	Topology Submesh::GetTopology()const
	{
		Topology result = Topology::eCount;

		for ( auto buffers : m_geometryBuffers )
		{
			result = buffers->GetTopology();
		}

		return result;
	}

	void Submesh::SetTopology( Topology p_value )
	{
		for ( auto buffers : m_geometryBuffers )
		{
			buffers->SetTopology( p_value );
		}
	}

	GeometryBuffersSPtr Submesh::GetGeometryBuffers( ShaderProgram const & p_program )
	{
		GeometryBuffersSPtr geometryBuffers;
		auto it = std::find_if( std::begin( m_geometryBuffers ), std::end( m_geometryBuffers ), [&p_program]( GeometryBuffersSPtr p_buffers )
		{
			return &p_buffers->GetProgram() == &p_program;
		} );

		if ( it == m_geometryBuffers.end() )
		{
			geometryBuffers = GetScene()->GetEngine()->GetRenderSystem()->CreateGeometryBuffers( Topology::eTriangles, p_program );
			m_geometryBuffers.push_back( geometryBuffers );
			DoInitialiseGeometryBuffers( geometryBuffers );
		}
		else
		{
			geometryBuffers = *it;
		}

		return geometryBuffers;
	}

	void Submesh::SetAnimated( bool p_animated )
	{
		if ( p_animated )
		{
			AddFlag( m_programFlags, ProgramFlag::eMorphing );
		}
		else
		{
			RemFlag( m_programFlags, ProgramFlag::eMorphing );
		}
	}

	void Submesh::DoCreateBuffers()
	{
		if ( CheckFlag( GetProgramFlags(), ProgramFlag::eMorphing ) )
		{
			m_animBuffer = std::make_unique< VertexBuffer >( *GetScene()->GetEngine(), BufferDeclaration
			{
				{
					BufferElementDeclaration( ShaderProgram::Position2, uint32_t( ElementUsage::ePosition ), ElementType::eVec3, Vertex::GetOffsetPos() ),
					BufferElementDeclaration( ShaderProgram::Normal2, uint32_t( ElementUsage::eNormal ), ElementType::eVec3, Vertex::GetOffsetNml() ),
					BufferElementDeclaration( ShaderProgram::Tangent2, uint32_t( ElementUsage::eTangent ), ElementType::eVec3, Vertex::GetOffsetTan() ),
					BufferElementDeclaration( ShaderProgram::Bitangent2, uint32_t( ElementUsage::eBitangent ), ElementType::eVec3, Vertex::GetOffsetBin() ),
					BufferElementDeclaration( ShaderProgram::Texture2, uint32_t( ElementUsage::eTexCoords ), ElementType::eVec3, Vertex::GetOffsetTex() ),
				}
			} );
		}
		else if ( GetScene()->GetEngine()->GetRenderSystem()->GetGpuInformations().HasInstancing() )
		{
			uint32_t count = 0;

			for ( auto it : m_instanceCount )
			{
				count = std::max( count, it.second );
			}

			if ( count > 1 )
			{
				AddFlag( m_programFlags, ProgramFlag::eInstantiation );
				m_matrixBuffer = std::make_unique< VertexBuffer >( *GetScene()->GetEngine(), BufferDeclaration
				{
					{
						BufferElementDeclaration{ ShaderProgram::Transform, uint32_t( ElementUsage::eTransform ), ElementType::eMat4, 0, 1 },
						BufferElementDeclaration{ ShaderProgram::Material, uint32_t( ElementUsage::eMatIndex ), ElementType::eInt, 64, 1 },
					}
				} );
			}
			else
			{
				m_matrixBuffer.reset();
			}
		}

		if ( CheckFlag( m_programFlags, ProgramFlag::eSkinning ) && m_parentMesh.GetSkeleton() )
		{
			m_bonesBuffer = std::make_unique< VertexBuffer >( *GetScene()->GetEngine(), BufferDeclaration
			{
				{
					BufferElementDeclaration{ ShaderProgram::BoneIds0, uint32_t( ElementUsage::eBoneIds0 ), ElementType::eIVec4, 0 },
					BufferElementDeclaration{ ShaderProgram::BoneIds1, uint32_t( ElementUsage::eBoneIds1 ), ElementType::eIVec4, 16 },
					BufferElementDeclaration{ ShaderProgram::Weights0, uint32_t( ElementUsage::eBoneWeights0 ), ElementType::eVec4, 32 },
					BufferElementDeclaration{ ShaderProgram::Weights1, uint32_t( ElementUsage::eBoneWeights1 ), ElementType::eVec4, 48 },
				}
			} );
			ENSURE( m_bonesBuffer->GetDeclaration().stride() == BonedVertex::Stride );

			if ( CheckFlag( m_programFlags, ProgramFlag::eInstantiation ) )
			{
				m_instancedBonesBuffer = std::make_unique< ShaderStorageBuffer >( *GetScene()->GetEngine() );
			}
		}
	}

	void Submesh::DoDestroyBuffers()
	{
		m_generated = false;
		m_initialised = false;
		m_dirty = true;
		m_vertexBuffer.Cleanup();

		if ( !m_indexBuffer.IsEmpty() )
		{
			m_indexBuffer.Cleanup();
		}

		if ( m_animBuffer )
		{
			m_animBuffer->Cleanup();
		}

		if ( m_matrixBuffer )
		{
			m_matrixBuffer->Cleanup();
		}

		if ( m_bonesBuffer )
		{
			m_bonesBuffer->Cleanup();
		}

		if ( m_instancedBonesBuffer )
		{
			m_instancedBonesBuffer->Cleanup();
		}

		for ( auto buffers : m_geometryBuffers )
		{
			buffers->Cleanup();
		}
	}

	void Submesh::DoGenerateBuffers()
	{
		DoCreateBuffers();
		DoGenerateVertexBuffer();
		DoGenerateIndexBuffer();
		uint32_t count = 0;

		for ( auto it : m_instanceCount )
		{
			count = std::max( count, it.second );
		}

		if ( count > 1 )
		{
			AddFlag( m_programFlags, ProgramFlag::eInstantiation );
			DoGenerateMatrixBuffer( count );
		}

		if ( !m_bones.empty() )
		{
			DoGenerateBonesBuffer();

			if ( count > 1 )
			{
				DoGenerateInstantiatedBonesBuffer( count );
			}
		}

		if ( CheckFlag( m_programFlags, ProgramFlag::eMorphing ) )
		{
			DoGenerateAnimBuffer();
		}

		m_generated = true;
	}

	void Submesh::DoGenerateVertexBuffer()
	{
		VertexBuffer & vertexBuffer = m_vertexBuffer;
		uint32_t stride = m_vertexBuffer.GetDeclaration().stride();
		uint32_t size = uint32_t( m_points.size() ) * stride;

		if ( size )
		{
			if ( vertexBuffer.GetSize() != size )
			{
				vertexBuffer.Resize( size );
			}

			auto buffer = vertexBuffer.GetData();

			for ( auto it : m_pointsData )
			{
				ENSURE( buffer < vertexBuffer.GetData() + vertexBuffer.GetSize() );
				ENSURE( ( buffer < vertexBuffer.GetData() + vertexBuffer.GetSize()
						&& buffer + it.size() <= vertexBuffer.GetData() + vertexBuffer.GetSize() )
					|| ( buffer == vertexBuffer.GetData() + vertexBuffer.GetSize() ) );
				std::memcpy( buffer, it.data(), it.size() );
				buffer += it.size();
			}

			buffer = vertexBuffer.GetData();

			for ( auto point : m_points )
			{
				ENSURE( buffer < vertexBuffer.GetData() + vertexBuffer.GetSize() );
				ENSURE( ( buffer < vertexBuffer.GetData() + vertexBuffer.GetSize()
						&& buffer + stride <= vertexBuffer.GetData() + vertexBuffer.GetSize() )
					|| ( buffer == vertexBuffer.GetData() + vertexBuffer.GetSize() ) );
				point->LinkCoords( buffer );
				buffer += stride;
			}

			//m_points.clear();
			//m_pointsData.clear();
		}
	}

	void Submesh::DoGenerateAnimBuffer()
	{
		if ( m_animBuffer )
		{
			VertexBuffer & vertexBuffer = m_vertexBuffer;
			VertexBuffer & animBuffer = *m_animBuffer;
			uint32_t size = vertexBuffer.GetSize();

			if ( size && animBuffer.GetSize() != size )
			{
				animBuffer.Resize( size );
			}
		}
	}

	void Submesh::DoGenerateIndexBuffer()
	{
		FaceSPtr pFace;
		IndexBuffer & indexBuffer = m_indexBuffer;
		uint32_t uiSize = uint32_t( m_faces.size() * 3 );

		if ( uiSize )
		{
			if ( indexBuffer.GetSize() != uiSize )
			{
				indexBuffer.Resize( uiSize );
			}

			uint32_t index = 0;

			for ( auto const & face : m_faces )
			{
				indexBuffer[index++] = face[0];
				indexBuffer[index++] = face[1];
				indexBuffer[index++] = face[2];
			}

			//m_faces.clear();
		}
	}

	void Submesh::DoGenerateBonesBuffer()
	{
		if ( m_bonesBuffer )
		{
			VertexBuffer & bonesBuffer = *m_bonesBuffer;
			uint32_t stride = BonedVertex::Stride;
			uint32_t size = uint32_t( m_bones.size() ) * stride;
			auto itbones = m_bones.begin();

			if ( size )
			{
				if ( bonesBuffer.GetSize() != size )
				{
					bonesBuffer.Resize( size );
				}

				auto buffer = bonesBuffer.GetData();

				for ( auto it : m_bonesData )
				{
					std::memcpy( buffer, it.data(), it.size() );
					buffer += it.size();
				}

				buffer = bonesBuffer.GetData();

				for ( auto point : m_bones )
				{
					point->LinkCoords( buffer );
					buffer += stride;
				}

				//m_bones.clear();
				//m_bonesData.clear();
			}
		}
	}

	void Submesh::DoGenerateMatrixBuffer( uint32_t p_count )
	{
		if ( m_matrixBuffer )
		{
			if ( p_count )
			{
				VertexBuffer & matrixBuffer = *m_matrixBuffer;
				uint32_t size = p_count * matrixBuffer.GetDeclaration().stride();

				if ( matrixBuffer.GetSize() != size )
				{
					matrixBuffer.Resize( size );
				}
			}
			else
			{
				m_matrixBuffer.reset();
			}
		}
	}

	void Submesh::DoGenerateInstantiatedBonesBuffer( uint32_t p_count )
	{
		if ( m_instancedBonesBuffer )
		{
			if ( p_count )
			{
				auto & bonesBuffer = *m_instancedBonesBuffer;
				auto stride = uint32_t( sizeof( float ) * 16u * 400u );
				uint32_t size = p_count * stride;

				if ( bonesBuffer.GetSize() != size )
				{
					bonesBuffer.Resize( size );
				}
			}
			else
			{
				m_instancedBonesBuffer.reset();
			}
		}
	}

	void Submesh::DoInitialiseGeometryBuffers( GeometryBuffersSPtr p_geometryBuffers )
	{
		VertexBufferArray buffers;
		buffers.push_back( m_vertexBuffer );

		if ( m_animBuffer )
		{
			buffers.push_back( *m_animBuffer );
		}

		if ( m_bonesBuffer )
		{
			buffers.push_back( *m_bonesBuffer );
		}

		if ( m_matrixBuffer )
		{
			buffers.push_back( *m_matrixBuffer );
		}

		if ( GetScene()->GetEngine()->GetRenderSystem()->GetCurrentContext() )
		{
			p_geometryBuffers->Initialise( buffers, &m_indexBuffer );
		}
		else
		{
			GetScene()->GetListener().PostEvent( MakeFunctorEvent( EventType::ePreRender, [this, p_geometryBuffers, buffers]()
			{
				p_geometryBuffers->Initialise( buffers, &m_indexBuffer );
			} ) );
		}
	}
}
