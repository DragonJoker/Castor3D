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
		bool l_return = true;

		if ( l_return )
		{
			VertexBuffer const & l_buffer = p_obj.GetVertexBuffer();
			size_t l_size = l_buffer.GetSize();
			uint32_t l_stride = l_buffer.GetDeclaration().stride();
			uint32_t l_count = uint32_t( l_size / l_stride );
			l_return = DoWriteChunk( l_count, ChunkType::eSubmeshVertexCount, m_chunk );

			if ( l_return )
			{
				InterleavedVertex const * l_srcbuf = reinterpret_cast< InterleavedVertex const * >( l_buffer.GetData() );
				std::vector< InterleavedVertexT< double > > l_dstbuf( l_count );
				DoCopyVertices( l_count, l_srcbuf, l_dstbuf.data() );
				l_return = DoWriteChunk( l_dstbuf, ChunkType::eSubmeshVertex, m_chunk );
			}
		}

		if ( l_return )
		{
			IndexBuffer const & l_buffer = p_obj.GetIndexBuffer();
			uint32_t l_count = l_buffer.GetSize() / 3;
			l_return = DoWriteChunk( l_count, ChunkType::eSubmeshFaceCount, m_chunk );

			if ( l_return )
			{
				FaceIndices const * l_srcbuf = reinterpret_cast< FaceIndices const * >( l_buffer.GetData() );
				l_return = DoWriteChunk( l_srcbuf, l_buffer.GetSize() / 3, ChunkType::eSubmeshFaces, m_chunk );
			}
		}

		if ( l_return && p_obj.HasBonesBuffer() )
		{
			VertexBuffer const & l_buffer = p_obj.GetBonesBuffer();
			uint32_t l_stride = l_buffer.GetDeclaration().stride();
			uint32_t l_count = l_buffer.GetSize() / l_stride;
			l_return = DoWriteChunk( l_count, ChunkType::eSubmeshBoneCount, m_chunk );

			if ( l_return )
			{
				VertexBoneData const * l_srcbuf = reinterpret_cast< VertexBoneData const * >( l_buffer.GetData() );
				l_return = DoWriteChunk( l_srcbuf, l_buffer.GetSize() / sizeof( VertexBoneData ), ChunkType::eSubmeshBones, m_chunk );
			}
		}

		return l_return;
	}

	//*************************************************************************************************

	bool BinaryParser< Submesh >::DoParse( Submesh & p_obj )
	{
		bool l_return = true;
		String l_name;
		std::vector< FaceIndices > l_faces;
		std::vector< VertexBoneData > l_bones;
		std::vector< InterleavedVertexT< double > > l_srcbuf;
		uint32_t l_count{ 0u };
		uint32_t l_faceCount{ 0u };
		uint32_t l_boneCount{ 0u };
		BinaryChunk l_chunk;

		while ( l_return && DoGetSubChunk( l_chunk ) )
		{
			switch ( l_chunk.GetChunkType() )
			{
			case ChunkType::eSubmeshVertexCount:
				l_return = DoParseChunk( l_count, l_chunk );

				if ( l_return )
				{
					l_srcbuf.resize( l_count );
				}

				break;

			case ChunkType::eSubmeshVertex:
				l_return = DoParseChunk( l_srcbuf, l_chunk );

				if ( l_return && !l_srcbuf.empty() )
				{
					std::vector< InterleavedVertex > l_dstbuf( l_srcbuf.size() );
					DoCopyVertices( uint32_t( l_srcbuf.size() ), l_srcbuf.data(), l_dstbuf.data() );
					p_obj.AddPoints( l_dstbuf );
				}

				break;

			case ChunkType::eSubmeshBoneCount:
				l_return = DoParseChunk( l_count, l_chunk );

				if ( l_return )
				{
					l_boneCount = l_count;
					l_bones.resize( l_count );
				}

				break;

			case ChunkType::eSubmeshBones:
				l_return = DoParseChunk( l_bones, l_chunk );

				if ( l_return && l_boneCount > 0 )
				{
					p_obj.AddBoneDatas( l_bones );
				}

				l_boneCount = 0u;
				break;

			case ChunkType::eSubmeshFaceCount:
				l_return = DoParseChunk( l_count, l_chunk );

				if ( l_return )
				{
					l_faceCount = l_count;
					l_faces.resize( l_count );
				}

				break;

			case ChunkType::eSubmeshFaces:
				l_return = DoParseChunk( l_faces, l_chunk );

				if ( l_return && l_faceCount > 0 )
				{
					p_obj.AddFaceGroup( l_faces );
				}

				l_faceCount = 0u;
				break;

			default:
				l_return = false;
				break;
			}
		}

		return l_return;
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
			Point3r l_min;
			Point3r l_max;
			Point3r l_cur;
			Vertex::GetPosition( m_points[0], l_min );
			Vertex::GetPosition( m_points[0], l_max );
			uint32_t l_nbVertex = GetPointsCount();

			for ( uint32_t i = 1; i < l_nbVertex; i++ )
			{
				Vertex::GetPosition( m_points[i], l_cur );
				l_max[0] = std::max( l_cur[0], l_max[0] );
				l_max[1] = std::max( l_cur[1], l_max[1] );
				l_max[2] = std::max( l_cur[2], l_max[2] );
				l_min[0] = std::min( l_cur[0], l_min[0] );
				l_min[1] = std::min( l_cur[1], l_min[1] );
				l_min[2] = std::min( l_cur[2], l_min[2] );
			}

			m_box.Load( l_min, l_max );
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
		int l_iReturn = -1;
		int l_iIndex = 0;
		Point3r l_ptPos;

		for ( VertexPtrArrayConstIt l_it = m_points.begin(); l_it != m_points.end() && l_iReturn == -1; ++l_it )
		{
			if ( point::length_squared( p_vertex - Vertex::GetPosition( ( *l_it ), l_ptPos ) ) < p_precision )
			{
				l_iReturn = int( l_iIndex );
			}

			l_iIndex++;
		}

		return l_iReturn;
	}

	BufferElementGroupSPtr Submesh::AddPoint( real x, real y, real z )
	{
		BufferElementGroupSPtr l_return;
		uint32_t l_stride = 3 * sizeof( real ) * 5;
		m_pointsData.push_back( ByteArray( l_stride ) );
		uint8_t * l_data = m_pointsData.back().data();
		l_return = std::make_shared< BufferElementGroup >( l_data, uint32_t( m_points.size() ) );
		Vertex::SetPosition( l_return, x, y, z );
		m_points.push_back( l_return );
		return l_return;
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
		uint32_t l_stride = m_vertexBuffer.GetDeclaration().stride();
		m_pointsData.push_back( ByteArray( std::distance( p_begin, p_end ) * l_stride ) );
		uint8_t * l_data = m_pointsData.back().data();

		std::for_each( p_begin, p_end, [this, &l_data, &l_stride]( InterleavedVertex const & p_data )
		{
			BufferElementGroupSPtr l_vertex = std::make_shared< BufferElementGroup >( l_data, uint32_t( m_points.size() ) );
			Vertex::SetPosition( l_vertex, p_data.m_pos.data() );
			Vertex::SetNormal( l_vertex, p_data.m_nml.data() );
			Vertex::SetTangent( l_vertex, p_data.m_tan.data() );
			Vertex::SetBitangent( l_vertex, p_data.m_bin.data() );
			Vertex::SetTexCoord( l_vertex, p_data.m_tex.data() );
			m_points.push_back( l_vertex );
			l_data += l_stride;
		} );
	}

	void Submesh::AddBoneDatas( VertexBoneData const * const p_begin, VertexBoneData const * const p_end )
	{
		uint32_t l_stride = BonedVertex::Stride;
		m_bonesData.push_back( ByteArray( std::distance( p_begin, p_end ) * l_stride ) );
		auto l_data = &( *m_bonesData.rbegin() )[0];

		std::for_each( p_begin, p_end, [this, &l_data, &l_stride]( VertexBoneData const & p_data )
		{
			BufferElementGroupSPtr l_bonesData = std::make_shared< BufferElementGroup >( l_data, uint32_t( m_bones.size() ) );
			BonedVertex::SetBones( l_bonesData, p_data );
			m_bones.push_back( l_bonesData );
			l_data += l_stride;
		} );

		AddFlag( m_programFlags, ProgramFlag::eSkinning );
	}

	Face Submesh::AddFace( uint32_t a, uint32_t b, uint32_t c )
	{
		Face l_return{ a, b, c };

		if ( a < m_points.size() && b < m_points.size() && c < m_points.size() )
		{
			m_faces.push_back( l_return );
			m_hasNormals = false;
		}
		else
		{
			throw std::range_error( "Submesh::AddFace - One or more index out of bound" );
		}

		return l_return;
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
			for ( auto & l_geometryBuffers : m_geometryBuffers )
			{
				DoInitialiseGeometryBuffers( l_geometryBuffers );
			}
		}
	}

	void Submesh::ResetMatrixBuffers()
	{
		if ( m_matrixBuffer )
		{
			uint32_t l_count = 0;

			for ( auto l_it : m_instanceCount )
			{
				l_count = std::max( l_count, l_it.second );
			}

			DoGenerateMatrixBuffer( l_count );
			m_matrixBuffer->Initialise( BufferAccessType::eStream, BufferAccessNature::eDraw );
		}
	}

	void Submesh::Draw( GeometryBuffers const & p_geometryBuffers )
	{
		ENSURE( m_initialised );
		uint32_t l_size = m_vertexBuffer.GetSize() / m_vertexBuffer.GetDeclaration().stride();

		if ( !m_indexBuffer.IsEmpty() )
		{
			l_size = m_indexBuffer.GetSize();
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

		p_geometryBuffers.Draw( l_size, 0 );
	}

	void Submesh::DrawInstanced( GeometryBuffers const & p_geometryBuffers, uint32_t p_count )
	{
		ENSURE( m_initialised );
		uint32_t l_size = m_vertexBuffer.GetSize() / m_vertexBuffer.GetDeclaration().stride();

		if ( !m_indexBuffer.IsEmpty() )
		{
			l_size = m_indexBuffer.GetSize();
		}

		if ( m_dirty )
		{
			m_vertexBuffer.Upload();
			m_dirty = false;
		}

		p_geometryBuffers.DrawInstanced( l_size, 0, p_count );
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
					IndexBuffer & l_indices = m_indexBuffer;
					VertexBuffer & l_vertices = m_vertexBuffer;

					l_vertices.Bind();
					l_indices.Bind();
					m_cameraPosition = p_ptCameraPosition;
					uint32_t l_uiIdxSize = l_indices.GetSize();
					uint32_t * l_pIdx = l_indices.Lock( 0, l_uiIdxSize, AccessType::eRead | AccessType::eWrite );

					if ( l_pIdx )
					{
						struct stFACE_DISTANCE
						{
							uint32_t m_index[3];
							double m_distance;
						};
						uint32_t l_stride = l_vertices.GetDeclaration().stride();
						uint8_t * l_pVtx = l_vertices.GetData();
						DECLARE_VECTOR( stFACE_DISTANCE, Face );
						FaceArray l_arraySorted;
						l_arraySorted.reserve( l_uiIdxSize / 3 );

						if ( l_pVtx )
						{
							for ( uint32_t * l_it = l_pIdx + 0; l_it < l_pIdx + l_uiIdxSize; l_it += 3 )
							{
								double l_dDistance = 0.0;
								Coords3r l_pVtx1( reinterpret_cast< real * >( &l_pVtx[l_it[0] * l_stride] ) );
								l_dDistance += point::length_squared( l_pVtx1 - p_ptCameraPosition );
								Coords3r l_pVtx2( reinterpret_cast< real * >( &l_pVtx[l_it[1] * l_stride] ) );
								l_dDistance += point::length_squared( l_pVtx2 - p_ptCameraPosition );
								Coords3r l_pVtx3( reinterpret_cast< real * >( &l_pVtx[l_it[2] * l_stride] ) );
								l_dDistance += point::length_squared( l_pVtx3 - p_ptCameraPosition );
								stFACE_DISTANCE l_face = { { l_it[0], l_it[1], l_it[2] }, l_dDistance };
								l_arraySorted.push_back( l_face );
							}

							std::sort( l_arraySorted.begin(), l_arraySorted.end(), []( stFACE_DISTANCE const & p_left, stFACE_DISTANCE const & p_right )
							{
								return p_left.m_distance < p_right.m_distance;
							} );

							for ( FaceArrayConstIt l_it = l_arraySorted.begin(); l_it != l_arraySorted.end(); ++l_it )
							{
								*l_pIdx++ = l_it->m_index[0];
								*l_pIdx++ = l_it->m_index[1];
								*l_pIdx++ = l_it->m_index[2];
							}
						}

						l_indices.Unlock();
					}

					l_indices.Unbind();
					l_vertices.Unbind();
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
		auto l_it = m_instanceCount.find( p_material );

		if ( l_it == m_instanceCount.end() )
		{
			m_instanceCount[p_material] = 0;
			l_it = m_instanceCount.find( p_material );
		}

		return l_it->second++;
	}

	uint32_t Submesh::UnRef( MaterialSPtr p_material )
	{
		auto l_it = m_instanceCount.find( p_material );
		uint32_t l_return{ 0u };

		if ( l_it != m_instanceCount.end() )
		{
			l_return = l_it->second;

			if ( l_it->second )
			{
				l_it->second--;
			}

			if ( !l_it->second )
			{
				m_instanceCount.erase( l_it );
			}
		}

		return l_return;
	}

	uint32_t Submesh::GetRefCount( MaterialSPtr p_material )const
	{
		uint32_t l_return = 0;
		auto l_it = m_instanceCount.find( p_material );

		if ( l_it != m_instanceCount.end() )
		{
			l_return = l_it->second;
		}

		return l_return;
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
		Topology l_return = Topology::eCount;

		for ( auto l_buffers : m_geometryBuffers )
		{
			l_return = l_buffers->GetTopology();
		}

		return l_return;
	}

	void Submesh::SetTopology( Topology p_value )
	{
		for ( auto l_buffers : m_geometryBuffers )
		{
			l_buffers->SetTopology( p_value );
		}
	}

	GeometryBuffersSPtr Submesh::GetGeometryBuffers( ShaderProgram const & p_program )
	{
		GeometryBuffersSPtr l_geometryBuffers;
		auto l_it = std::find_if( std::begin( m_geometryBuffers ), std::end( m_geometryBuffers ), [&p_program]( GeometryBuffersSPtr p_buffers )
		{
			return &p_buffers->GetProgram() == &p_program;
		} );

		if ( l_it == m_geometryBuffers.end() )
		{
			l_geometryBuffers = GetScene()->GetEngine()->GetRenderSystem()->CreateGeometryBuffers( Topology::eTriangles, p_program );
			m_geometryBuffers.push_back( l_geometryBuffers );
			DoInitialiseGeometryBuffers( l_geometryBuffers );
		}
		else
		{
			l_geometryBuffers = *l_it;
		}

		return l_geometryBuffers;
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
			uint32_t l_count = 0;

			for ( auto l_it : m_instanceCount )
			{
				l_count = std::max( l_count, l_it.second );
			}

			if ( l_count > 1 )
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

		for ( auto l_buffers : m_geometryBuffers )
		{
			l_buffers->Cleanup();
		}
	}

	void Submesh::DoGenerateBuffers()
	{
		DoCreateBuffers();
		DoGenerateVertexBuffer();
		DoGenerateIndexBuffer();
		uint32_t l_count = 0;

		for ( auto l_it : m_instanceCount )
		{
			l_count = std::max( l_count, l_it.second );
		}

		if ( l_count > 1 )
		{
			AddFlag( m_programFlags, ProgramFlag::eInstantiation );
			DoGenerateMatrixBuffer( l_count );
		}

		if ( !m_bones.empty() )
		{
			DoGenerateBonesBuffer();

			if ( l_count > 1 )
			{
				DoGenerateInstantiatedBonesBuffer( l_count );
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
		VertexBuffer & l_vertexBuffer = m_vertexBuffer;
		uint32_t l_stride = m_vertexBuffer.GetDeclaration().stride();
		uint32_t l_size = uint32_t( m_points.size() ) * l_stride;

		if ( l_size )
		{
			if ( l_vertexBuffer.GetSize() != l_size )
			{
				l_vertexBuffer.Resize( l_size );
			}

			auto l_buffer = l_vertexBuffer.GetData();

			for ( auto l_it : m_pointsData )
			{
				ENSURE( l_buffer < l_vertexBuffer.GetData() + l_vertexBuffer.GetSize() );
				ENSURE( ( l_buffer < l_vertexBuffer.GetData() + l_vertexBuffer.GetSize()
						&& l_buffer + l_it.size() <= l_vertexBuffer.GetData() + l_vertexBuffer.GetSize() )
					|| ( l_buffer == l_vertexBuffer.GetData() + l_vertexBuffer.GetSize() ) );
				std::memcpy( l_buffer, l_it.data(), l_it.size() );
				l_buffer += l_it.size();
			}

			l_buffer = l_vertexBuffer.GetData();

			for ( auto l_point : m_points )
			{
				ENSURE( l_buffer < l_vertexBuffer.GetData() + l_vertexBuffer.GetSize() );
				ENSURE( ( l_buffer < l_vertexBuffer.GetData() + l_vertexBuffer.GetSize()
						&& l_buffer + l_stride <= l_vertexBuffer.GetData() + l_vertexBuffer.GetSize() )
					|| ( l_buffer == l_vertexBuffer.GetData() + l_vertexBuffer.GetSize() ) );
				l_point->LinkCoords( l_buffer );
				l_buffer += l_stride;
			}

			l_buffer = l_vertexBuffer.GetData();

			//m_points.clear();
			//m_pointsData.clear();
		}
	}

	void Submesh::DoGenerateAnimBuffer()
	{
		if ( m_animBuffer )
		{
			VertexBuffer & l_vertexBuffer = m_vertexBuffer;
			VertexBuffer & l_animBuffer = *m_animBuffer;
			uint32_t l_size = l_vertexBuffer.GetSize();

			if ( l_size && l_animBuffer.GetSize() != l_size )
			{
				l_animBuffer.Resize( l_size );
			}
		}
	}

	void Submesh::DoGenerateIndexBuffer()
	{
		FaceSPtr l_pFace;
		IndexBuffer & l_indexBuffer = m_indexBuffer;
		uint32_t l_uiSize = uint32_t( m_faces.size() * 3 );

		if ( l_uiSize )
		{
			if ( l_indexBuffer.GetSize() != l_uiSize )
			{
				l_indexBuffer.Resize( l_uiSize );
			}

			uint32_t l_index = 0;

			for ( auto const & l_face : m_faces )
			{
				l_indexBuffer[l_index++] = l_face[0];
				l_indexBuffer[l_index++] = l_face[1];
				l_indexBuffer[l_index++] = l_face[2];
			}

			//m_faces.clear();
		}
	}

	void Submesh::DoGenerateBonesBuffer()
	{
		if ( m_bonesBuffer )
		{
			VertexBuffer & l_bonesBuffer = *m_bonesBuffer;
			uint32_t l_stride = BonedVertex::Stride;
			uint32_t l_size = uint32_t( m_bones.size() ) * l_stride;
			auto l_itbones = m_bones.begin();

			if ( l_size )
			{
				if ( l_bonesBuffer.GetSize() != l_size )
				{
					l_bonesBuffer.Resize( l_size );
				}

				auto l_buffer = l_bonesBuffer.GetData();

				for ( auto l_it : m_bonesData )
				{
					std::memcpy( l_buffer, l_it.data(), l_it.size() );
					l_buffer += l_it.size();
				}

				l_buffer = l_bonesBuffer.GetData();

				for ( auto l_point : m_bones )
				{
					l_point->LinkCoords( l_buffer );
					l_buffer += l_stride;
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
				VertexBuffer & l_matrixBuffer = *m_matrixBuffer;
				uint32_t l_size = p_count * l_matrixBuffer.GetDeclaration().stride();

				if ( l_matrixBuffer.GetSize() != l_size )
				{
					l_matrixBuffer.Resize( l_size );
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
				auto & l_bonesBuffer = *m_instancedBonesBuffer;
				auto l_stride = uint32_t( sizeof( float ) * 16u * 400u );
				uint32_t l_size = p_count * l_stride;

				if ( l_bonesBuffer.GetSize() != l_size )
				{
					l_bonesBuffer.Resize( l_size );
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
		VertexBufferArray l_buffers;
		l_buffers.push_back( m_vertexBuffer );

		if ( m_animBuffer )
		{
			l_buffers.push_back( *m_animBuffer );
		}

		if ( m_bonesBuffer )
		{
			l_buffers.push_back( *m_bonesBuffer );
		}

		if ( m_matrixBuffer )
		{
			l_buffers.push_back( *m_matrixBuffer );
		}

		if ( GetScene()->GetEngine()->GetRenderSystem()->GetCurrentContext() )
		{
			p_geometryBuffers->Initialise( l_buffers, &m_indexBuffer );
		}
		else
		{
			GetScene()->GetListener().PostEvent( MakeFunctorEvent( EventType::ePreRender, [this, p_geometryBuffers, l_buffers]()
			{
				p_geometryBuffers->Initialise( l_buffers, &m_indexBuffer );
			} ) );
		}
	}
}
