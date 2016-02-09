#include "Submesh.hpp"

#include "BonedVertex.hpp"
#include "Buffer.hpp"
#include "Engine.hpp"
#include "Face.hpp"
#include "InitialiseEvent.hpp"
#include "Material.hpp"
#include "MaterialManager.hpp"
#include "Pass.hpp"
#include "Pipeline.hpp"
#include "RenderSystem.hpp"
#include "ShaderManager.hpp"
#include "ShaderProgram.hpp"
#include "Skeleton.hpp"
#include "Vertex.hpp"

#include <BlockTracker.hpp>

using namespace Castor;

namespace Castor3D
{
	Submesh::TextLoader::TextLoader( File::eENCODING_MODE p_encodingMode )
		: Loader< Submesh, eFILE_TYPE_TEXT, TextFile >( File::eOPEN_MODE_DUMMY, p_encodingMode )
	{
	}

	bool Submesh::TextLoader::operator()( Submesh const & p_submesh, TextFile & p_file )
	{
		bool l_return = p_file.WriteText( cuT( "\t\t\tsubmesh\n\t\t\t{\n" ) ) > 0;
		Point3r l_pos;
		Point3r l_tan;
		Point3r l_nml;
		Point3r l_tex;

		for ( uint32_t i = 0; i < p_submesh.GetPointsCount() && l_return; i++ )
		{
			Vertex::GetPosition( p_submesh[i]->const_ptr(), l_pos );
			Vertex::GetNormal( p_submesh[i]->const_ptr(), l_nml );
			Vertex::GetTangent( p_submesh[i]->const_ptr(), l_tan );
			Vertex::GetTexCoord( p_submesh[i]->const_ptr(), l_tex );
			StringStream l_streamPos, l_streamNml, l_streamTan, l_streamTex;
			l_streamPos << l_pos[0] << cuT( " " ) << l_pos[1] << cuT( " " ) << l_pos[2];
			l_streamNml << l_nml[0] << cuT( " " ) << l_nml[1] << cuT( " " ) << l_nml[2];
			l_streamTan << l_tan[0] << cuT( " " ) << l_tan[1] << cuT( " " ) << l_tan[2];
			l_streamTex << l_tex[0] << cuT( " " ) << l_tex[1] << cuT( " " ) << l_tex[2];
			l_return = p_file.Print( 1024, cuT( "\t\t\t\tvertex %s\n" ), l_streamPos.str().c_str() ) > 0;
			l_return = p_file.Print( 1024, cuT( "\t\t\t\tnormal %s\n" ), l_streamNml.str().c_str() ) > 0;
			l_return = p_file.Print( 1024, cuT( "\t\t\t\ttangent %s\n" ), l_streamTan.str().c_str() ) > 0;
			l_return = p_file.Print( 1024, cuT( "\t\t\t\tuvw %s\n" ), l_streamTex.str().c_str() ) > 0;
		}

		uint32_t l_uiNbFaces = p_submesh.GetFaceCount();

		for ( uint32_t j = 0; j < l_uiNbFaces && l_return; j++ )
		{
			l_return = Face::TextLoader()( *p_submesh.GetFace( j ), p_file );
		}

		if ( l_return )
		{
			l_return = p_file.WriteText( cuT( "\t\t\t}\n" ) ) > 0;
		}

		return l_return;
	}

	//*************************************************************************************************

	Submesh::BinaryParser::BinaryParser( Path const & p_path )
		: Castor3D::BinaryParser< Submesh >( p_path )
	{
	}

	bool Submesh::BinaryParser::Fill( Submesh const & p_obj, BinaryChunk & p_chunk )const
	{
		bool l_return = true;
		BinaryChunk l_chunk( eCHUNK_TYPE_SUBMESH );
		double l_buffer[15];
		Point3r l_pos;
		Point3r l_tan;
		Point3r l_bit;
		Point3r l_nml;
		Point3r l_tex;

		for ( uint32_t i = 0; i < p_obj.GetPointsCount() && l_return; i++ )
		{
			Vertex::GetPosition( p_obj[i]->const_ptr(), l_pos );
			Vertex::GetNormal( p_obj[i]->const_ptr(), l_tan );
			Vertex::GetTangent( p_obj[i]->const_ptr(), l_bit );
			Vertex::GetBitangent( p_obj[i]->const_ptr(), l_nml );
			Vertex::GetTexCoord( p_obj[i]->const_ptr(), l_tex );
			l_buffer[ 0] = double( l_pos[0] );
			l_buffer[ 1] = double( l_pos[1] );
			l_buffer[ 2] = double( l_pos[2] );
			l_buffer[ 3] = double( l_tan[0] );
			l_buffer[ 4] = double( l_tan[1] );
			l_buffer[ 5] = double( l_tan[2] );
			l_buffer[ 6] = double( l_bit[0] );
			l_buffer[ 7] = double( l_bit[1] );
			l_buffer[ 8] = double( l_bit[2] );
			l_buffer[ 9] = double( l_nml[0] );
			l_buffer[10] = double( l_nml[1] );
			l_buffer[11] = double( l_nml[2] );
			l_buffer[12] = double( l_tex[0] );
			l_buffer[13] = double( l_tex[1] );
			l_buffer[14] = double( l_tex[2] );
			l_return = DoFillChunk( l_buffer, eCHUNK_TYPE_SUBMESH_VERTEX, l_chunk );
		}

		uint32_t l_uiNbFaces = p_obj.GetFaceCount();

		for ( uint32_t j = 0; j < l_uiNbFaces && l_return; j++ )
		{
			l_return = Face::BinaryParser( m_path ).Fill( *p_obj.GetFace( j ), l_chunk );
		}

		if ( l_return )
		{
			l_chunk.Finalise();
			p_chunk.AddSubChunk( l_chunk );
		}

		return l_return;
	}

	bool Submesh::BinaryParser::Parse( Submesh & p_obj, BinaryChunk & p_chunk )const
	{
		bool l_return = true;
		String l_name;
		std::vector< real > l_pos;
		std::vector< real > l_tan;
		std::vector< real > l_bit;
		std::vector< real > l_nml;
		std::vector< real > l_tex;
		std::vector< stFACE_INDICES > l_faces;
		double l_buffer[12];
		Face l_face( 0, 0, 0 );
		l_pos.reserve( p_chunk.GetRemaining() / 6 );
		l_tan.reserve( p_chunk.GetRemaining() / 6 );
		l_bit.reserve( p_chunk.GetRemaining() / 6 );
		l_nml.reserve( p_chunk.GetRemaining() / 6 );
		l_tex.reserve( p_chunk.GetRemaining() / 6 );
		l_faces.reserve( p_chunk.GetRemaining() / 6 );

		while ( p_chunk.CheckAvailable( 1 ) )
		{
			BinaryChunk l_chunk;
			l_return = p_chunk.GetSubChunk( l_chunk );

			if ( l_return )
			{
				switch ( l_chunk.GetChunkType() )
				{
				case eCHUNK_TYPE_SUBMESH_VERTEX:
					l_return = DoParseChunk( l_buffer, l_chunk );

					if ( l_return )
					{
						l_pos.push_back( real( l_buffer[ 0] ) );
						l_pos.push_back( real( l_buffer[ 1] ) );
						l_pos.push_back( real( l_buffer[ 2] ) );
						l_tan.push_back( real( l_buffer[ 2] ) );
						l_tan.push_back( real( l_buffer[ 3] ) );
						l_tan.push_back( real( l_buffer[ 4] ) );
						l_bit.push_back( real( l_buffer[ 5] ) );
						l_bit.push_back( real( l_buffer[ 6] ) );
						l_bit.push_back( real( l_buffer[ 7] ) );
						l_nml.push_back( real( l_buffer[ 8] ) );
						l_nml.push_back( real( l_buffer[ 9] ) );
						l_nml.push_back( real( l_buffer[10] ) );
						l_tex.push_back( real( l_buffer[11] ) );
						l_tex.push_back( real( l_buffer[12] ) );
						l_tex.push_back( real( l_buffer[13] ) );
					}

					break;

				case eCHUNK_TYPE_SUBMESH_FACE:
					l_return = Face::BinaryParser( m_path ).Parse( l_face, l_chunk );

					if ( l_return )
					{
						l_faces.push_back( { l_face.GetVertexIndex( 0 ), l_face.GetVertexIndex( 1 ), l_face.GetVertexIndex( 2 ) } );
					}

					break;

				default:
					l_return = false;
					break;
				}
			}

			if ( !l_return )
			{
				p_chunk.EndParse();
			}
		}

		if ( l_return && !l_pos.empty() )
		{
			stVERTEX_GROUP l_group = { uint32_t( l_pos.size() / 3 ), l_pos.data(), l_nml.data(), l_tan.data(), NULL, l_tex.data(), NULL };
			p_obj.AddPoints( l_group );
			p_obj.AddFaceGroup( l_faces.data(), uint32_t( l_faces.size() ) );
		}

		return l_return;
	}

	//*************************************************************************************************

	Submesh::Submesh( Engine & p_engine, MeshRPtr p_mesh, uint32_t p_uiId )
		: OwnedBy< Engine >( p_engine )
		, m_defaultMaterial( p_engine.GetMaterialManager().GetDefaultMaterial() )
		, m_id( p_uiId )
		, m_parentMesh( p_mesh )
		, m_programFlags( 0 )
		, m_initialised( false )
		, m_dirty( true )
		, m_layout( {
						BufferElementDeclaration( ShaderProgram::Position, eELEMENT_TYPE_3FLOATS, Vertex::GetOffsetPos() ),
						BufferElementDeclaration( ShaderProgram::Normal, eELEMENT_TYPE_3FLOATS, Vertex::GetOffsetNml() ),
						BufferElementDeclaration( ShaderProgram::Tangent, eELEMENT_TYPE_3FLOATS, Vertex::GetOffsetTan() ),
						BufferElementDeclaration( ShaderProgram::Bitangent, eELEMENT_TYPE_3FLOATS, Vertex::GetOffsetBin() ),
						BufferElementDeclaration( ShaderProgram::Texture, eELEMENT_TYPE_3FLOATS, Vertex::GetOffsetTex() ),
					} )
	{
	}

	Submesh::~Submesh()
	{
		Cleanup();
	}

	void Submesh::Initialise()
	{
		if ( !m_vertexBuffer )
		{
			GenerateBuffers();
		}

		if ( !m_initialised )
		{
			if ( m_vertexBuffer )
			{
				m_initialised = m_vertexBuffer->Create();
				m_initialised &= m_vertexBuffer->Initialise( eBUFFER_ACCESS_TYPE_DYNAMIC, eBUFFER_ACCESS_NATURE_DRAW);
			}

			if ( m_initialised && m_indexBuffer )
			{
				m_initialised = m_indexBuffer->Create();
				m_initialised &= m_indexBuffer->Initialise( eBUFFER_ACCESS_TYPE_STREAM, eBUFFER_ACCESS_NATURE_DRAW );
			}

			if ( m_initialised && m_bonesBuffer )
			{
				m_initialised = m_bonesBuffer->Create();
				m_initialised &= m_bonesBuffer->Initialise( eBUFFER_ACCESS_TYPE_STREAM, eBUFFER_ACCESS_NATURE_DRAW );
			}

			if ( m_initialised && m_matrixBuffer )
			{
				m_initialised = m_matrixBuffer->Create();
				m_initialised &= m_matrixBuffer->Initialise( eBUFFER_ACCESS_TYPE_STREAM, eBUFFER_ACCESS_NATURE_DRAW );
			}
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

				if ( l_cur[0] > l_max[0] )
				{
					l_max[0] = l_cur[0];
				}

				if ( l_cur[0] < l_min[0] )
				{
					l_min[0] = l_cur[0];
				}

				if ( l_cur[1] > l_max[1] )
				{
					l_max[1] = l_cur[1];
				}

				if ( l_cur[1] < l_min[1] )
				{
					l_min[1] = l_cur[1];
				}

				if ( l_cur[2] > l_max[2] )
				{
					l_max[2] = l_cur[2];
				}

				if ( l_cur[2] < l_min[2] )
				{
					l_min[2] = l_cur[2];
				}
			}

			m_box.Load( l_min, l_max );
			m_sphere.Load( m_box );
		}
	}

	uint32_t Submesh::GetFaceCount()const
	{
		return std::max( uint32_t( m_faces.size() ), m_indexBuffer ? m_indexBuffer->GetSize() / 3 : 0u );
	}

	uint32_t Submesh::GetPointsCount()const
	{
		return std::max< uint32_t >( uint32_t( m_points.size() ), m_vertexBuffer ? m_vertexBuffer->GetSize() / m_layout.GetStride() : 0u );
	}

	int Submesh::IsInMyPoints( Point3r const & p_vertex, double p_precision )
	{
		int l_iReturn = -1;
		int l_iIndex = 0;
		Point3r l_ptPos;

		for ( VertexPtrArrayConstIt l_it = m_points.begin(); l_it != m_points.end() && l_iReturn == -1; ++l_it )
		{
			if ( point::distance_squared( p_vertex - Vertex::GetPosition( ( *l_it ), l_ptPos ) ) < p_precision )
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
		uint8_t * l_pData = &( *m_pointsData.rbegin() )[0];
		l_return = std::make_shared< BufferElementGroup >( l_pData, uint32_t( m_points.size() ) );
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

	void Submesh::AddPoints( stVERTEX_GROUP const & p_vertices )
	{
		stVERTEX_GROUP l_vertices = p_vertices;
		uint32_t l_stride = m_layout.GetStride();
		m_pointsData.push_back( ByteArray( l_vertices.m_uiCount * l_stride ) );
		uint8_t * l_pData = &( *m_pointsData.rbegin() )[0];
		uint32_t l_uiVtxCount = uint32_t( m_points.size() );

		if ( l_vertices.m_pVtx )
		{
			if ( l_vertices.m_pNml && l_vertices.m_pTan && l_vertices.m_pBin && l_vertices.m_pTex )
			{
				for ( uint32_t i = 0; i < l_vertices.m_uiCount; i++ )
				{
					BufferElementGroupSPtr l_vertex = std::make_shared< BufferElementGroup >( l_pData, uint32_t( m_points.size() ) );
					Vertex::SetPosition( l_vertex, l_vertices.m_pVtx );
					Vertex::SetNormal( l_vertex, l_vertices.m_pNml );
					Vertex::SetTangent( l_vertex, l_vertices.m_pTan );
					Vertex::SetBitangent( l_vertex, l_vertices.m_pBin );
					Vertex::SetTexCoord( l_vertex, l_vertices.m_pTex );
					m_points.push_back( l_vertex );
					l_pData += l_stride;
					l_vertices.m_pVtx += Vertex::GetCountPos();
					l_vertices.m_pNml += Vertex::GetCountNml();
					l_vertices.m_pTan += Vertex::GetCountTan();
					l_vertices.m_pBin += Vertex::GetCountBin();
					l_vertices.m_pTex += Vertex::GetCountTex();
				}
			}
			else if ( l_vertices.m_pNml && l_vertices.m_pTex )
			{
				for ( uint32_t i = 0; i < l_vertices.m_uiCount; i++ )
				{
					BufferElementGroupSPtr l_vertex = std::make_shared< BufferElementGroup >( l_pData, uint32_t( m_points.size() ) );
					Vertex::SetPosition( l_vertex, l_vertices.m_pVtx );
					Vertex::SetNormal( l_vertex, l_vertices.m_pNml );
					Vertex::SetTexCoord( l_vertex, l_vertices.m_pTex );
					m_points.push_back( l_vertex );
					l_pData += l_stride;
					l_vertices.m_pVtx += Vertex::GetCountPos();
					l_vertices.m_pNml += Vertex::GetCountNml();
					l_vertices.m_pTex += Vertex::GetCountTex();
				}
			}
			else if ( l_vertices.m_pNml )
			{
				for ( uint32_t i = 0; i < l_vertices.m_uiCount; i++ )
				{
					BufferElementGroupSPtr l_vertex = std::make_shared< BufferElementGroup >( l_pData, uint32_t( m_points.size() ) );
					Vertex::SetPosition( l_vertex, l_vertices.m_pVtx );
					Vertex::SetNormal( l_vertex, l_vertices.m_pNml );
					m_points.push_back( l_vertex );
					l_pData += l_stride;
					l_vertices.m_pVtx += Vertex::GetCountPos();
					l_vertices.m_pNml += Vertex::GetCountNml();
				}
			}
			else if ( l_vertices.m_pTex )
			{
				for ( uint32_t i = 0; i < l_vertices.m_uiCount; i++ )
				{
					BufferElementGroupSPtr l_vertex = std::make_shared< BufferElementGroup >( l_pData, uint32_t( m_points.size() ) );
					Vertex::SetPosition( l_vertex, l_vertices.m_pVtx );
					Vertex::SetTexCoord( l_vertex, l_vertices.m_pTex );
					m_points.push_back( l_vertex );
					l_pData += l_stride;
					l_vertices.m_pVtx += Vertex::GetCountPos();
					l_vertices.m_pTex += Vertex::GetCountTex();
				}
			}
			else
			{
				for ( uint32_t i = 0; i < l_vertices.m_uiCount; i++ )
				{
					BufferElementGroupSPtr l_vertex = std::make_shared< BufferElementGroup >( l_pData, uint32_t( m_points.size() ) );
					Vertex::SetPosition( l_vertex, l_vertices.m_pVtx );
					m_points.push_back( l_vertex );
					l_pData += l_stride;
					l_vertices.m_pVtx += Vertex::GetCountPos();
				}
			}

			if ( l_vertices.m_pBones )
			{
				l_stride = BonedVertex::Stride;
				m_bonesData.push_back( ByteArray( l_vertices.m_uiCount * l_stride ) );
				l_pData = &( *m_bonesData.rbegin() )[0];

				for ( uint32_t i = 0; i < l_vertices.m_uiCount; i++ )
				{
					BufferElementGroupSPtr l_bonesData = std::make_shared< BufferElementGroup >( l_pData, uint32_t( m_bones.size() ) );
					BonedVertex::SetBones( l_bonesData, l_vertices.m_pBones );
					m_bones.push_back( l_bonesData );
					l_vertices.m_pBones++;
					l_pData += l_stride;
				}

				m_programFlags |= ePROGRAM_FLAG_SKINNING;
			}
		}
	}

	FaceSPtr Submesh::AddFace( uint32_t a, uint32_t b, uint32_t c )
	{
		FaceSPtr l_return;

		if ( a < m_points.size() && b < m_points.size() && c < m_points.size() )
		{
			l_return = std::make_shared< Face >( a, b, c );
			m_faces.push_back( l_return );
			m_hasNormals = false;
		}
		else
		{
			throw ( std::range_error( "Submesh::AddFace - One or more index out of bound" ) );
		}

		return l_return;
	}

	void Submesh::AddFaceGroup( stFACE_INDICES * p_faces, uint32_t p_count )
	{
		for ( uint32_t i = 0; i < p_count; i++ )
		{
			AddFace( p_faces[i].m_uiVertexIndex[0], p_faces[i].m_uiVertexIndex[1], p_faces[i].m_uiVertexIndex[2] );
		}
	}

	void Submesh::AddQuadFace( uint32_t a, uint32_t b, uint32_t c, uint32_t d, Point3r const & p_minUV, Point3r const & p_maxUV )
	{
		FaceSPtr l_face1 = AddFace( a, b, c );
		FaceSPtr l_face2 = AddFace( a, c, d );
		Vertex::SetTexCoord( m_points[a], p_minUV[0], p_minUV[1] );
		Vertex::SetTexCoord( m_points[b], p_maxUV[0], p_minUV[1] );
		Vertex::SetTexCoord( m_points[c], p_maxUV[0], p_maxUV[1] );
		Vertex::SetTexCoord( m_points[d], p_minUV[0], p_maxUV[1] );
	}

	void Submesh::ClearFaces()
	{
		m_faces.clear();

		if ( m_indexBuffer )
		{
			m_indexBuffer->Clear();
		}
	}

	void Submesh::GenerateBuffers()
	{
		DoCreateBuffers();
		DoGenerateVertexBuffer();
		DoGenerateIndexBuffer();
		uint32_t l_count = 0;
		l_count = std::accumulate( m_instanceCount.begin(), m_instanceCount.end(), l_count, [&]( uint32_t p_count, std::pair< MaterialSPtr, uint32_t > const & p_pair )
		{
			return p_count + p_pair.second;
		} );

		if ( l_count > 1 )
		{
			DoGenerateMatrixBuffer( l_count );
		}

		if ( !m_bones.empty() )
		{
			DoGenerateBonesBuffer();
		}
	}

	SubmeshSPtr Submesh::Clone()
	{
		SubmeshSPtr l_clone = std::make_shared< Submesh >( *GetEngine(), m_parentMesh, m_id );
		uint32_t l_stride = m_layout.GetStride();

		//On effectue une copie des vertex
		for ( VertexPtrArrayConstIt l_it = m_points.begin(); l_it != m_points.end(); ++l_it )
		{
			BufferElementGroupSPtr l_pVertexSrc = ( *l_it );
			BufferElementGroupSPtr l_pVertexDst = l_clone->AddPoint( 0, 0, 0 );
			memcpy( l_pVertexDst->ptr(), l_pVertexSrc->const_ptr(), l_stride );
		}

		// TODO copier les bones

		for ( uint32_t j = 0; j < GetFaceCount(); j++ )
		{
			FaceSPtr l_pFaceSrc = GetFace( j );
			l_clone->AddFace( l_pFaceSrc->GetVertexIndex( 0 ), l_pFaceSrc->GetVertexIndex( 1 ), l_pFaceSrc->GetVertexIndex( 2 ) );
		}

		return l_clone;
	}

	void Submesh::ResetGpuBuffers()
	{
		DoDestroyBuffers();
		DoCreateBuffers();
	}

	void Submesh::Draw( Pass const & p_pass )
	{
		GeometryBuffers & l_geometryBuffers = DoPrepareGeometryBuffers( p_pass );
		ShaderProgramSPtr l_program = p_pass.GetShader();
		uint32_t l_uiSize = m_vertexBuffer->GetSize() / m_layout.GetStride();

		auto l_matrixBuffer = p_pass.GetMatrixBuffer();

		if ( l_program && l_matrixBuffer )
		{
			GetEngine()->GetRenderSystem()->GetPipeline().ApplyMatrices( *l_matrixBuffer, 0xFFFFFFFFFFFFFFFF );
		}

		if ( m_indexBuffer )
		{
			l_uiSize = m_indexBuffer->GetSize();
		}

		uint32_t l_count = GetRefCount( p_pass.GetParent() );

		if ( l_count > 1 )
		{
			if ( GetEngine()->GetRenderSystem()->HasInstancing() )
			{
				l_geometryBuffers.DrawInstanced( *l_program, l_uiSize, 0, l_count );
			}
			else
			{
				l_geometryBuffers.Draw( *l_program, l_uiSize, 0 );
			}
		}
		else
		{
			l_geometryBuffers.Draw( *l_program, l_uiSize, 0 );
		}
	}

	void Submesh::ComputeFacesFromPolygonVertex()
	{
		if ( m_points.size() )
		{
			BufferElementGroupSPtr l_v1 = m_points[0];
			BufferElementGroupSPtr l_v2 = m_points[1];
			BufferElementGroupSPtr l_v3 = m_points[2];
			FaceSPtr l_face = AddFace( 0, 1, 2 );
			Vertex::SetTexCoord( l_v1, 0.0, 0.0 );
			Vertex::SetTexCoord( l_v2, 0.0, 0.0 );
			Vertex::SetTexCoord( l_v3, 0.0, 0.0 );

			for ( uint32_t i = 2; i < uint32_t( m_points.size() - 1 ); i++ )
			{
				l_v2 = m_points[i];
				l_v3 = m_points[i + 1];
				FaceSPtr l_faceA = AddFace( 0, i, i + 1 );
				Vertex::SetTexCoord( l_v2, 0.0, 0.0 );
				Vertex::SetTexCoord( l_v3, 0.0, 0.0 );
			}
		}
	}

	void Submesh::ComputeNormals( bool p_reverted )
	{
		if ( !m_hasNormals )
		{
			Point3r l_vec2m1;
			Point3r l_vec3m1;
			Point3r l_tex2m1;
			Point3r l_tex3m1;
			Point3r l_pt1;
			Point3r l_pt2;
			Point3r l_pt3;
			Point3r l_uv1;
			Point3r l_uv2;
			Point3r l_uv3;
			BufferElementGroupSPtr l_pVtx1;
			BufferElementGroupSPtr l_pVtx2;
			BufferElementGroupSPtr l_pVtx3;
			Point3r l_vFaceNormal;
			Point3r l_vFaceTangent;

			// First we flush normals and tangents
			for ( VertexPtrArray::iterator l_it = m_points.begin(); l_it != m_points.end(); ++l_it )
			{
				l_pVtx1 = *l_it;
				Vertex::SetNormal( l_pVtx1, l_pt1 );
				Vertex::SetTangent( l_pVtx1, l_pt1 );
			}

			Coords3r l_coord;

			// Then we compute normals and tangents
			if ( p_reverted )
			{
				for ( FacePtrArray::iterator l_it = m_faces.begin(); l_it != m_faces.end(); ++l_it )
				{
					FaceSPtr l_pFace = ( *l_it );
					l_pVtx1 = m_points[l_pFace->GetVertexIndex( 0 )];
					l_pVtx2 = m_points[l_pFace->GetVertexIndex( 1 )];
					l_pVtx3 = m_points[l_pFace->GetVertexIndex( 2 )];
					Vertex::GetPosition( l_pVtx1, l_pt1 );
					Vertex::GetPosition( l_pVtx2, l_pt2 );
					Vertex::GetPosition( l_pVtx3, l_pt3 );
					Vertex::GetTexCoord( l_pVtx1, l_uv1 );
					Vertex::GetTexCoord( l_pVtx2, l_uv2 );
					Vertex::GetTexCoord( l_pVtx3, l_uv3 );
					l_vec2m1 = l_pt2 - l_pt1;
					l_vec3m1 = l_pt3 - l_pt1;
					l_tex2m1 = l_uv2 - l_uv1;
					l_tex3m1 = l_uv3 - l_uv1;
					l_vFaceNormal	= -( l_vec3m1 ^ l_vec2m1 );
					l_vFaceTangent	= ( l_vec2m1 * l_tex3m1[1] ) - ( l_vec3m1 * l_tex2m1[1] );
					Vertex::GetNormal( l_pVtx1, l_coord ) += l_vFaceNormal;
					Vertex::GetNormal( l_pVtx2, l_coord ) += l_vFaceNormal;
					Vertex::GetNormal( l_pVtx3, l_coord ) += l_vFaceNormal;
					Vertex::GetTangent( l_pVtx1, l_coord ) += l_vFaceTangent;
					Vertex::GetTangent( l_pVtx2, l_coord ) += l_vFaceTangent;
					Vertex::GetTangent( l_pVtx3, l_coord ) += l_vFaceTangent;
				}
			}
			else
			{
				for ( FacePtrArray::iterator l_it = m_faces.begin(); l_it != m_faces.end(); ++l_it )
				{
					FaceSPtr l_pFace = ( *l_it );
					l_pVtx1 = m_points[l_pFace->GetVertexIndex( 0 )];
					l_pVtx2 = m_points[l_pFace->GetVertexIndex( 1 )];
					l_pVtx3 = m_points[l_pFace->GetVertexIndex( 2 )];
					Vertex::GetPosition( l_pVtx1, l_pt1 );
					Vertex::GetPosition( l_pVtx2, l_pt2 );
					Vertex::GetPosition( l_pVtx3, l_pt3 );
					Vertex::GetTexCoord( l_pVtx1, l_uv1 );
					Vertex::GetTexCoord( l_pVtx2, l_uv2 );
					Vertex::GetTexCoord( l_pVtx3, l_uv3 );
					l_vec2m1 = l_pt2 - l_pt1;
					l_vec3m1 = l_pt3 - l_pt1;
					l_tex2m1 = l_uv2 - l_uv1;
					l_tex3m1 = l_uv3 - l_uv1;
					l_vFaceNormal	= l_vec3m1 ^ l_vec2m1;
					l_vFaceTangent	= ( l_vec3m1 * l_tex2m1[1] ) - ( l_vec2m1 * l_tex3m1[1] );
					Vertex::GetNormal( l_pVtx1, l_coord ) += l_vFaceNormal;
					Vertex::GetNormal( l_pVtx2, l_coord ) += l_vFaceNormal;
					Vertex::GetNormal( l_pVtx3, l_coord ) += l_vFaceNormal;
					Vertex::GetTangent( l_pVtx1, l_coord ) += l_vFaceTangent;
					Vertex::GetTangent( l_pVtx2, l_coord ) += l_vFaceTangent;
					Vertex::GetTangent( l_pVtx3, l_coord ) += l_vFaceTangent;
				}
			}

			// Eventually we normalize the normals and tangents
			for ( VertexPtrArray::iterator l_it = m_points.begin(); l_it != m_points.end(); ++l_it )
			{
				l_pVtx1 = *l_it;
				Coords3r l_value;
				Vertex::GetNormal( l_pVtx1, l_value );
				point::normalise( l_value );
				Vertex::GetTangent( l_pVtx1, l_value );
				point::normalise( l_value );
			}

			m_hasNormals = true;
		}
	}

	void Submesh::ComputeNormals( FaceSPtr p_face )
	{
		BufferElementGroupSPtr l_pVtx1, l_pVtx2, l_pVtx3;
		Point3r l_vec2m1;
		Point3r l_vec3m1;
		Point3r l_vFaceNormal;
		Point3r l_pt1;
		Point3r l_pt2;
		Point3r l_pt3;
		l_pVtx1 = m_points[p_face->GetVertexIndex( 0 )];
		l_pVtx2 = m_points[p_face->GetVertexIndex( 1 )];
		l_pVtx3 = m_points[p_face->GetVertexIndex( 2 )];
		Vertex::GetPosition( l_pVtx1, l_pt1 );
		Vertex::GetPosition( l_pVtx2, l_pt2 );
		Vertex::GetPosition( l_pVtx3, l_pt3 );
		l_vec2m1 = l_pt2 - l_pt1;
		l_vec3m1 = l_pt3 - l_pt1;
		l_vFaceNormal = point::get_normalised( l_vec2m1 ^ l_vec3m1 );
		Vertex::SetNormal( l_pVtx1, l_vFaceNormal );
		Vertex::SetNormal( l_pVtx2, l_vFaceNormal );
		Vertex::SetNormal( l_pVtx3, l_vFaceNormal );
		ComputeTangents( p_face );
	}

	void Submesh::ComputeTangents( FaceSPtr p_face )
	{
		BufferElementGroupSPtr l_pVtx1, l_pVtx2, l_pVtx3;
		Point3r l_vec2m1;
		Point3r l_vec3m1;
		Point3r l_vFaceTangent;
		Point3r l_tex2m1;
		Point3r l_tex3m1;
		Point3r l_uv1;
		Point3r l_uv2;
		Point3r l_uv3;
		Point3r l_pt1;
		Point3r l_pt2;
		Point3r l_pt3;
		l_pVtx1 = m_points[p_face->GetVertexIndex( 0 )];
		l_pVtx2 = m_points[p_face->GetVertexIndex( 1 )];
		l_pVtx3 = m_points[p_face->GetVertexIndex( 2 )];
		Vertex::GetPosition( l_pVtx1, l_pt1 );
		Vertex::GetPosition( l_pVtx2, l_pt2 );
		Vertex::GetPosition( l_pVtx3, l_pt3 );
		Vertex::GetTexCoord( l_pVtx1, l_uv1 );
		Vertex::GetTexCoord( l_pVtx2, l_uv2 );
		Vertex::GetTexCoord( l_pVtx3, l_uv3 );
		l_vec2m1 = l_pt2 - l_pt1;
		l_vec3m1 = l_pt3 - l_pt1;
		l_tex2m1 = l_uv2 - l_uv1;
		l_tex3m1 = l_uv3 - l_uv1;
		l_vFaceTangent = point::get_normalised( ( l_vec2m1 * l_tex3m1[1] ) - ( l_vec3m1 * l_tex2m1[1] ) );
		Vertex::SetTangent( l_pVtx1, l_vFaceTangent );
		Vertex::SetTangent( l_pVtx2, l_vFaceTangent );
		Vertex::SetTangent( l_pVtx3, l_vFaceTangent );
	}

	void Submesh::ComputeTangentsFromNormals()
	{
		Point3rArray l_arrayTangents( m_points.size() );

		// Pour chaque vertex, on stocke la somme des tangentes qui peuvent lui être affectées
		for ( FacePtrArray::iterator l_it = m_faces.begin(); l_it != m_faces.end(); ++l_it )
		{
			FaceSPtr l_pFace = ( *l_it );
			BufferElementGroupSPtr	l_pVtx1 = m_points[l_pFace->GetVertexIndex( 0 )];
			BufferElementGroupSPtr	l_pVtx2 = m_points[l_pFace->GetVertexIndex( 1 )];
			BufferElementGroupSPtr	l_pVtx3 = m_points[l_pFace->GetVertexIndex( 2 )];
			Point3r l_pt1;
			Point3r l_pt2;
			Point3r l_pt3;
			Point3r l_uv1;
			Point3r l_uv2;
			Point3r l_uv3;
			Vertex::GetPosition( l_pVtx1, l_pt1 );
			Vertex::GetPosition( l_pVtx2, l_pt2 );
			Vertex::GetPosition( l_pVtx3, l_pt3 );
			Vertex::GetTexCoord( l_pVtx1, l_uv1 );
			Vertex::GetTexCoord( l_pVtx2, l_uv2 );
			Vertex::GetTexCoord( l_pVtx3, l_uv3 );
			Point3r l_vec2m1 = l_pt2 - l_pt1;
			Point3r l_vec3m1 = l_pt3 - l_pt1;
			Point3r l_vec3m2 = l_pt3 - l_pt2;
			Point3r l_tex2m1 = l_uv2 - l_uv1;
			Point3r l_tex3m1 = l_uv3 - l_uv1;
			// Calculates the triangle's area.
			real l_rDirCorrection = l_tex2m1[0] * l_tex3m1[1] - l_tex2m1[1] * l_tex3m1[0];
			Point3r l_vFaceTangent;

			if ( l_rDirCorrection )
			{
				l_rDirCorrection = 1 / l_rDirCorrection;
				// Calculates the face tangent to the current triangle.
				l_vFaceTangent[0] = l_rDirCorrection * ( ( l_vec2m1[0] * l_tex3m1[1] ) + ( l_vec3m1[0] * -l_tex2m1[1] ) );
				l_vFaceTangent[1] = l_rDirCorrection * ( ( l_vec2m1[1] * l_tex3m1[1] ) + ( l_vec3m1[1] * -l_tex2m1[1] ) );
				l_vFaceTangent[2] = l_rDirCorrection * ( ( l_vec2m1[2] * l_tex3m1[1] ) + ( l_vec3m1[2] * -l_tex2m1[1] ) );
			}

			l_arrayTangents[l_pFace->GetVertexIndex( 0 )] += l_vFaceTangent;
			l_arrayTangents[l_pFace->GetVertexIndex( 1 )] += l_vFaceTangent;
			l_arrayTangents[l_pFace->GetVertexIndex( 2 )] += l_vFaceTangent;
		}

		uint32_t i = 0;
		//On effectue la moyennes des tangentes
		std::for_each( l_arrayTangents.begin(), l_arrayTangents.end(), [&]( Point3r & p_tangent )
		{
			Point3r l_normal;
			Vertex::GetNormal( m_points[i], l_normal );
			Point3r l_tangent 	= point::get_normalised( p_tangent );
			l_tangent -= l_normal * point::dot( l_tangent, l_normal );
			Point3r l_bitangent = l_normal ^ l_tangent;
			Vertex::SetTangent(	m_points[i], l_tangent );
			Vertex::SetBitangent(	m_points[i], l_bitangent );
			i++;
		} );
	}

	void Submesh::ComputeTangentsFromBitangents()
	{
		std::for_each( m_points.begin(), m_points.end(), [&]( BufferElementGroupSPtr p_pVertex )
		{
			Point3r l_normal;
			Point3r l_bitangent;
			Vertex::GetNormal(	p_pVertex, l_normal );
			Vertex::GetBitangent( p_pVertex, l_bitangent );
			Point3r l_tangent = l_normal ^ l_bitangent;
			Vertex::SetTangent(	p_pVertex, l_tangent );
		} );
	}

	void Submesh::ComputeBitangents()
	{
		std::for_each( m_points.begin(), m_points.end(), [&]( BufferElementGroupSPtr p_pVertex )
		{
			Point3r l_normal;
			Point3r l_tangent;
			Vertex::GetNormal( p_pVertex, l_normal );
			Vertex::GetTangent( p_pVertex, l_tangent );
			Point3r l_bitangent = l_tangent ^ l_normal;
			Vertex::SetBitangent( p_pVertex, l_bitangent );
		} );
	}

	void Submesh::SortFaces( Point3r const & p_ptCameraPosition )
	{
		try
		{
			if ( m_cameraPosition != p_ptCameraPosition )
			{
				if ( m_initialised && m_indexBuffer && m_vertexBuffer  )
				{
					IndexBuffer & l_indices = *m_indexBuffer;
					VertexBuffer & l_vertices = *m_vertexBuffer;

					if ( l_vertices.Bind() )
					{
						if ( l_indices.Bind() )
						{
							m_cameraPosition = p_ptCameraPosition;
							uint32_t l_uiIdxSize = l_indices.GetSize();
							uint32_t * l_pIdx = l_indices.Lock( 0, l_uiIdxSize, eACCESS_TYPE_WRITE | eACCESS_TYPE_READ );

							if ( l_pIdx )
							{
								struct stFACE_DISTANCE
								{
									uint32_t m_index[3];
									double m_distance;
								};
								uint32_t l_stride = l_vertices.GetDeclaration().GetStride();
								uint8_t * l_pVtx = l_vertices.data();
								DECLARE_VECTOR( stFACE_DISTANCE, Face );
								FaceArray l_arraySorted;
								l_arraySorted.reserve( l_uiIdxSize / 3 );

								if ( l_pVtx )
								{
									for ( uint32_t * l_it = l_pIdx + 0; l_it < l_pIdx + l_uiIdxSize; l_it += 3 )
									{
										double l_dDistance = 0.0;
										Point3r l_pVtx1( reinterpret_cast< real const * >( &l_pVtx[l_it[0] * l_stride] ) );
										l_pVtx1 -= p_ptCameraPosition;
										l_dDistance += point::distance_squared( l_pVtx1 );
										Point3r l_pVtx2( reinterpret_cast< real const * >( &l_pVtx[l_it[1] * l_stride] ) );
										l_pVtx2 -= p_ptCameraPosition;
										l_dDistance += point::distance_squared( l_pVtx2 );
										Point3r l_pVtx3( reinterpret_cast< real const * >( &l_pVtx[l_it[2] * l_stride] ) );
										l_pVtx3 -= p_ptCameraPosition;
										l_dDistance += point::distance_squared( l_pVtx3 );
										stFACE_DISTANCE l_face = { {l_it[0], l_it[1], l_it[2]}, l_dDistance };
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
						}

						l_vertices.Unbind();
					}
				}
			}
		}
		catch ( Exception const & p_exc )
		{
			Logger::LogError( std::stringstream() << "Submesh::SortFaces - Error: " << p_exc.what() );
		}
	}

	void Submesh::Ref( MaterialSPtr p_material )
	{
		std::map< MaterialSPtr, uint32_t >::iterator l_it = m_instanceCount.find( p_material );

		if ( l_it == m_instanceCount.end() )
		{
			m_instanceCount[p_material] = 0;
			l_it = m_instanceCount.find( p_material );
		}

		l_it->second++;
	}

	void Submesh::UnRef( MaterialSPtr p_material )
	{
		std::map< MaterialSPtr, uint32_t >::iterator l_it = m_instanceCount.find( p_material );

		if ( l_it == m_instanceCount.end() )
		{
			if ( l_it->second )
			{
				l_it->second--;
			}

			if ( !l_it->second )
			{
				m_instanceCount.erase( l_it );
			}
		}
	}

	uint32_t Submesh::GetRefCount( MaterialSPtr p_material )const
	{
		uint32_t l_return = 0;
		std::map< MaterialSPtr, uint32_t >::const_iterator l_it = m_instanceCount.find( p_material );

		if ( l_it != m_instanceCount.end() )
		{
			l_return = l_it->second;
		}

		return l_return;
	}

	eTOPOLOGY Submesh::GetTopology()const
	{
		eTOPOLOGY l_return = eTOPOLOGY_COUNT;

		for ( auto l_buffers : m_geometryBuffers )
		{
			l_return = l_buffers->GetTopology();
		}

		return l_return;
	}

	void Submesh::SetTopology( eTOPOLOGY p_value )
	{
		for ( auto l_buffers : m_geometryBuffers )
		{
			l_buffers->SetTopology( p_value );
		}
	}

	void Submesh::DoCreateBuffers()
	{
		m_vertexBuffer = std::make_unique< VertexBuffer >( *GetEngine(), m_layout );
		m_indexBuffer = std::make_unique< IndexBuffer >( *GetEngine() );

		if ( ( GetProgramFlags() & ePROGRAM_FLAG_SKINNING ) == ePROGRAM_FLAG_SKINNING )
		{
			m_bonesBuffer = std::make_unique< VertexBuffer >( *GetEngine(), BufferDeclaration
			{ {
				BufferElementDeclaration{ ShaderProgram::BoneIds0, eELEMENT_TYPE_3INTS, 0 },
				BufferElementDeclaration{ ShaderProgram::BoneIds1, eELEMENT_TYPE_3INTS, 0 },
				BufferElementDeclaration{ ShaderProgram::Weights0, eELEMENT_TYPE_3FLOATS, 0 },
				BufferElementDeclaration{ ShaderProgram::Weights1, eELEMENT_TYPE_3FLOATS, 0 },
			} } );
		}
		else if ( GetEngine()->GetRenderSystem()->HasInstancing()
				  && ( GetProgramFlags() & ePROGRAM_FLAG_INSTANCIATION ) == ePROGRAM_FLAG_INSTANCIATION )
		{
			uint32_t l_count = 0;
			l_count = std::accumulate( m_instanceCount.begin(), m_instanceCount.end(), l_count, [&]( uint32_t p_count, std::pair< MaterialSPtr, uint32_t > const & p_pair )
			{
				return p_count + p_pair.second;
			} );

			if ( l_count > 1 )
			{
				m_matrixBuffer = std::make_unique< MatrixBuffer >( *GetEngine() );
			}
		}
	}

	void Submesh::DoDestroyBuffers()
	{
		m_initialised = false;

		if ( m_vertexBuffer )
		{
			m_vertexBuffer->Cleanup();
			m_vertexBuffer->Destroy();
		}

		if ( m_indexBuffer )
		{
			m_indexBuffer->Cleanup();
			m_indexBuffer->Destroy();
		}

		if ( m_matrixBuffer )
		{
			m_matrixBuffer->Cleanup();
			m_matrixBuffer->Destroy();
		}

		if ( m_bonesBuffer )
		{
			m_bonesBuffer->Cleanup();
			m_bonesBuffer->Destroy();
		}

		m_geometryBuffers.clear();
	}

	void Submesh::DoGenerateVertexBuffer()
	{
		if ( m_vertexBuffer )
		{
			uint8_t * l_buffer = nullptr;
			VertexBuffer & l_vertexBuffer = *m_vertexBuffer;
			uint32_t l_stride = m_layout.GetStride();
			uint32_t l_size = uint32_t( m_points.size() ) * l_stride;
			VertexPtrArrayIt l_itPoints = m_points.begin();

			if ( l_size )
			{
				if ( l_vertexBuffer.GetSize() != l_size )
				{
					l_vertexBuffer.Resize( l_size );
				}

				l_buffer = l_vertexBuffer.data();

				for ( auto l_it : m_pointsData )
				{
					std::memcpy( l_buffer, l_it.data(), l_it.size() );
					l_buffer += l_it.size();
				}

				l_buffer = l_vertexBuffer.data();

				for ( auto l_point : m_points )
				{
					l_point->LinkCoords( l_buffer );
					l_buffer += l_stride;
				}

				m_points.clear();
				m_pointsData.clear();
			}
		}
	}

	void Submesh::DoGenerateIndexBuffer()
	{
		if ( m_indexBuffer )
		{
			FaceSPtr l_pFace;
			uint32_t l_index = 0;
			IndexBuffer & l_indexBuffer = *m_indexBuffer;
			uint32_t l_uiSize = uint32_t( m_faces.size() * 3 );

			if ( l_uiSize )
			{
				if ( l_indexBuffer.GetSize() != l_uiSize )
				{
					l_indexBuffer.Resize( l_uiSize );
				}

				for ( FacePtrArray::iterator l_it = m_faces.begin(); l_it != m_faces.end(); ++l_it )
				{
					if ( *l_it )
					{
						l_pFace = *l_it;
						l_indexBuffer.SetElement( l_index++, l_pFace->GetVertexIndex( 0 ) );
						l_indexBuffer.SetElement( l_index++, l_pFace->GetVertexIndex( 1 ) );
						l_indexBuffer.SetElement( l_index++, l_pFace->GetVertexIndex( 2 ) );
					}
				}

				m_faces.clear();
			}
		}
	}

	void Submesh::DoGenerateBonesBuffer()
	{
		if ( m_bonesBuffer )
		{
			uint8_t * l_buffer = nullptr;
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

				l_buffer = l_bonesBuffer.data();

				for ( auto l_it : m_bonesData )
				{
					std::memcpy( l_buffer, l_it.data(), l_it.size() );
					l_buffer += l_it.size();
				}

				l_buffer = l_bonesBuffer.data();

				for ( auto l_point : m_bones )
				{
					l_point->LinkCoords( l_buffer );
					l_buffer += l_stride;
				}

				m_bones.clear();
				m_bonesData.clear();
			}
		}
	}

	void Submesh::DoGenerateMatrixBuffer( uint32_t p_count )
	{
		if ( m_matrixBuffer )
		{
			MatrixBuffer & l_matrixBuffer = *m_matrixBuffer;
			uint32_t l_uiSize = p_count * 16;

			if ( l_matrixBuffer.GetSize() != l_uiSize )
			{
				l_matrixBuffer.Resize( l_uiSize );
			}
		}
	}

	GeometryBuffers & Submesh::DoPrepareGeometryBuffers( Pass const & p_pass )
	{
		ShaderProgramSPtr l_program = p_pass.GetShader();

		if ( !l_program || l_program->GetStatus() != ePROGRAM_STATUS_LINKED )
		{
			CASTOR_EXCEPTION( "Can't retrieve a program input layout from a non compiled shader." );
		}

		GeometryBuffersSPtr l_buffers;
		auto const & l_layout = l_program->GetLayout();
		auto l_it = std::find_if( std::begin( m_geometryBuffers ), std::end( m_geometryBuffers ), [&l_layout]( GeometryBuffersSPtr p_buffers )
		{
			return p_buffers->GetLayout() == l_layout;
		} );

		if ( l_it == m_geometryBuffers.end() )
		{
			l_program->GetLayout().SetStride( m_layout.GetStride() );
			l_buffers = GetEngine()->GetRenderSystem()->CreateGeometryBuffers( eTOPOLOGY_TRIANGLES, l_program->GetLayout(), m_vertexBuffer.get(), m_indexBuffer.get(), m_bonesBuffer.get(), m_matrixBuffer.get() );
			m_geometryBuffers.push_back( l_buffers );
		}
		else
		{
			l_buffers = *l_it;
		}

		return *l_buffers;
	}
}
