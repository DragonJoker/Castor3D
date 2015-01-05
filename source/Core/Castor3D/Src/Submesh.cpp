#include "Submesh.hpp"
#include "InitialiseEvent.hpp"
#include "BonedVertex.hpp"
#include "Buffer.hpp"
#include "Material.hpp"
#include "Face.hpp"
#include "Pass.hpp"
#include "Pipeline.hpp"
#include "ShaderProgram.hpp"
#include "Engine.hpp"
#include "ShaderManager.hpp"
#include "Skeleton.hpp"

#include <BlockTracker.hpp>

using namespace Castor;

namespace Castor3D
{
	Submesh::TextLoader::TextLoader( File::eENCODING_MODE p_eEncodingMode )
		:	Loader< Submesh, eFILE_TYPE_TEXT, TextFile >( File::eOPEN_MODE_DUMMY, p_eEncodingMode )
	{
	}

	bool Submesh::TextLoader::operator()( Submesh const & p_submesh, TextFile & p_file )
	{
		bool l_bReturn = p_file.WriteText( cuT( "\t\t\tsubmesh\n\t\t\t{\n" ) ) > 0;
		Point3r l_ptPos;
		Point3r l_ptTan;
		Point3r l_ptNml;
		Point3r l_ptTex;

		for ( uint32_t i = 0; i < p_submesh.GetPointsCount() && l_bReturn; i++ )
		{
			Vertex::GetPosition(	p_submesh[i]->const_ptr(), l_ptPos );
			Vertex::GetNormal(	p_submesh[i]->const_ptr(), l_ptNml );
			Vertex::GetTangent(	p_submesh[i]->const_ptr(), l_ptTan );
			Vertex::GetTexCoord(	p_submesh[i]->const_ptr(), l_ptTex );
			StringStream l_streamPos, l_streamNml, l_streamTan, l_streamTex;
			l_streamPos << l_ptPos[0] << cuT( " " ) <<  l_ptPos[1] << cuT( " " ) << l_ptPos[2];
			l_streamNml << l_ptNml[0] << cuT( " " ) <<  l_ptNml[1] << cuT( " " ) << l_ptNml[2];
			l_streamTan << l_ptTan[0] << cuT( " " ) <<  l_ptTan[1] << cuT( " " ) << l_ptTan[2];
			l_streamTex << l_ptTex[0] << cuT( " " ) <<  l_ptTex[1] << cuT( " " ) << l_ptTex[2];
			l_bReturn = p_file.Print( 1024, cuT( "\t\t\t\tvertex %s\n" ), l_streamPos.str().c_str() ) > 0;
			l_bReturn = p_file.Print( 1024, cuT( "\t\t\t\tnormal %s\n" ), l_streamNml.str().c_str() ) > 0;
			l_bReturn = p_file.Print( 1024, cuT( "\t\t\t\ttangent %s\n" ), l_streamTan.str().c_str() ) > 0;
			l_bReturn = p_file.Print( 1024, cuT( "\t\t\t\tuvw %s\n" ), l_streamTex.str().c_str() ) > 0;
		}

		uint32_t l_uiNbFaces = p_submesh.GetFaceCount();

		for ( uint32_t j = 0; j < l_uiNbFaces && l_bReturn; j++ )
		{
			l_bReturn = Face::TextLoader()( *p_submesh.GetFace( j ), p_file );
		}

		if ( l_bReturn )
		{
			l_bReturn = p_file.WriteText( cuT( "\t\t\t}\n" ) ) > 0;
		}

		return l_bReturn;
	}

	//*************************************************************************************************

	Submesh::BinaryParser::BinaryParser( Path const & p_path )
		:	Castor3D::BinaryParser< Submesh >( p_path )
	{
	}

	bool Submesh::BinaryParser::Fill( Submesh const & p_obj, BinaryChunk & p_chunk )const
	{
		bool l_bReturn = true;
		BinaryChunk l_chunk( eCHUNK_TYPE_SUBMESH );
		double l_buffer[12];
		Point3r l_ptPos;
		Point3r l_ptTan;
		Point3r l_ptNml;
		Point3r l_ptTex;

		for ( uint32_t i = 0; i < p_obj.GetPointsCount() && l_bReturn; i++ )
		{
			Vertex::GetPosition(	p_obj[i]->const_ptr(), l_ptPos );
			Vertex::GetNormal(	p_obj[i]->const_ptr(), l_ptNml );
			Vertex::GetTangent(	p_obj[i]->const_ptr(), l_ptTan );
			Vertex::GetTexCoord(	p_obj[i]->const_ptr(), l_ptTex );
			l_buffer[ 0] = double( l_ptPos[0] );
			l_buffer[ 1] = double( l_ptPos[1] );
			l_buffer[ 2] = double( l_ptPos[2] );
			l_buffer[ 3] = double( l_ptNml[0] );
			l_buffer[ 4] = double( l_ptNml[1] );
			l_buffer[ 5] = double( l_ptNml[2] );
			l_buffer[ 6] = double( l_ptTan[0] );
			l_buffer[ 7] = double( l_ptTan[1] );
			l_buffer[ 8] = double( l_ptTan[2] );
			l_buffer[ 9] = double( l_ptTex[0] );
			l_buffer[10] = double( l_ptTex[1] );
			l_buffer[11] = double( l_ptTex[2] );
			l_bReturn = DoFillChunk( l_buffer, eCHUNK_TYPE_SUBMESH_VERTEX, l_chunk );
		}

		uint32_t l_uiNbFaces = p_obj.GetFaceCount();

		for ( uint32_t j = 0; j < l_uiNbFaces && l_bReturn; j++ )
		{
			l_bReturn = Face::BinaryParser( m_path ).Fill( *p_obj.GetFace( j ), l_chunk );
		}

		if ( l_bReturn )
		{
			l_chunk.Finalise();
			p_chunk.AddSubChunk( l_chunk );
		}

		return l_bReturn;
	}

	bool Submesh::BinaryParser::Parse( Submesh & p_obj, BinaryChunk & p_chunk )const
	{
		bool l_bReturn = true;
		String l_name;
		std::vector< real > l_pos;
		std::vector< real > l_tan;
		std::vector< real > l_nml;
		std::vector< real > l_tex;
		std::vector< stFACE_INDICES > l_faces;
		double l_buffer[12];
		Face l_face( 0, 0, 0 );
		l_pos.reserve( p_chunk.GetRemaining() / 5 );
		l_tan.reserve( p_chunk.GetRemaining() / 5 );
		l_nml.reserve( p_chunk.GetRemaining() / 5 );
		l_tex.reserve( p_chunk.GetRemaining() / 5 );
		l_faces.reserve( p_chunk.GetRemaining() / 5 );

		while ( p_chunk.CheckAvailable( 1 ) )
		{
			BinaryChunk l_chunk;
			l_bReturn = p_chunk.GetSubChunk( l_chunk );

			if ( l_bReturn )
			{
				switch ( l_chunk.GetChunkType() )
				{
				case eCHUNK_TYPE_SUBMESH_VERTEX:
					l_bReturn = DoParseChunk( l_buffer, l_chunk );

					if ( l_bReturn )
					{
						l_pos.push_back( real( l_buffer[ 0] ) );
						l_pos.push_back( real( l_buffer[ 1] ) );
						l_pos.push_back( real( l_buffer[ 2] ) );
						l_nml.push_back( real( l_buffer[ 3] ) );
						l_nml.push_back( real( l_buffer[ 4] ) );
						l_nml.push_back( real( l_buffer[ 5] ) );
						l_tan.push_back( real( l_buffer[ 6] ) );
						l_tan.push_back( real( l_buffer[ 7] ) );
						l_tan.push_back( real( l_buffer[ 8] ) );
						l_tex.push_back( real( l_buffer[ 9] ) );
						l_tex.push_back( real( l_buffer[10] ) );
						l_tex.push_back( real( l_buffer[11] ) );
					}

					break;

				case eCHUNK_TYPE_SUBMESH_FACE:
					l_bReturn = Face::BinaryParser( m_path ).Parse( l_face, l_chunk );

					if ( l_bReturn )
					{
						stFACE_INDICES l_indices = { l_face.GetVertexIndex( 0 ), l_face.GetVertexIndex( 1 ), l_face.GetVertexIndex( 2 ) };
						l_faces.push_back( l_indices );
					}

					break;

				default:
					l_bReturn = false;
					break;
				}
			}

			if ( !l_bReturn )
			{
				p_chunk.EndParse();
			}
		}

		if ( l_bReturn && !l_pos.empty() )
		{
			stVERTEX_GROUP l_group = { uint32_t( l_pos.size() / 3 ), l_pos.data(), l_nml.data(), l_tan.data(), NULL, l_tex.data(), NULL };
			p_obj.AddPoints( l_group );
			p_obj.AddFaceGroup( l_faces.data(), uint32_t( l_faces.size() ) );
		}

		return l_bReturn;
	}

	//*************************************************************************************************

	Submesh::Submesh( MeshRPtr p_pMesh, Engine * p_pEngine, uint32_t p_uiId )
		:	Renderable< Submesh, SubmeshRenderer >( p_pEngine )
		,	m_defaultMaterial( p_pEngine->GetMaterialManager().GetDefaultMaterial() )
		,	m_uiID( p_uiId )
		,	m_pParentMesh( p_pMesh )
		,	m_uiProgramFlags( 0 )
	{
	}

	Submesh::~Submesh()
	{
		Cleanup();
	}

	void Submesh::Cleanup()
	{
		m_arrayFaces.clear();
		m_points.clear();
		m_pointsData.clear();
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
		return std::max( uint32_t( m_arrayFaces.size() ), m_pRenderer.lock()->GetGeometryBuffers()->GetIndexBuffer().GetSize() / 3 );
	}

	uint32_t Submesh::GetPointsCount()const
	{
// Mais pourquoi * sizeof( real ) ?	return std::max< uint32_t >( uint32_t( m_points.size() ), uint32_t( sizeof( real ) * m_vertex.lock()->GetSize() / m_pDeclaration->GetStride() ) );
		return std::max< uint32_t >( uint32_t( m_points.size() ), uint32_t( m_pRenderer.lock()->GetGeometryBuffers()->GetVertexBuffer().GetSize() / m_pDeclaration->GetStride() ) );
	}

	int Submesh::IsInMyPoints( Point3r const & p_vertex )
	{
		int l_iReturn = -1;
		int l_iIndex = 0;
		Point3r l_ptPos;

		for ( VertexPtrArrayConstIt l_it = m_points.begin(); l_it != m_points.end() && l_iReturn == -1; ++l_it )
		{
			if ( p_vertex == Vertex::GetPosition( ( *l_it ), l_ptPos ) )
			{
				l_iReturn = int( l_iIndex );
			}

			l_iIndex++;
		}

		return l_iReturn;
	}

	BufferElementGroupSPtr Submesh::AddPoint( real x, real y, real z )
	{
		DoUpdateDeclaration();
		BufferElementGroupSPtr l_pReturn;
		m_pointsData.push_back( ByteArray( m_pDeclaration->GetStride() ) );
		uint8_t * l_pData = &( *m_pointsData.rbegin() )[0];
		l_pReturn = std::make_shared< BufferElementGroup >( l_pData, uint32_t( m_points.size() ) );
		Vertex::SetPosition( l_pReturn, x, y, z );
		m_points.push_back( l_pReturn );
		return l_pReturn;
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
		DoUpdateDeclaration();
		stVERTEX_GROUP l_vertices = p_vertices;
		uint32_t l_uiStride = m_pDeclaration->GetStride();
		uint32_t l_uiSize = l_vertices.m_uiCount * l_uiStride;
		m_pointsData.push_back( ByteArray( l_uiSize ) );
		uint8_t * l_pData = &( *m_pointsData.rbegin() )[0];
		uint32_t l_uiVtxCount = uint32_t( m_points.size() );

		if ( l_vertices.m_pVtx )
		{
			if ( l_vertices.m_pNml && l_vertices.m_pTan && l_vertices.m_pBin && l_vertices.m_pTex )
			{
				for ( uint32_t i = 0; i < l_vertices.m_uiCount; i++ )
				{
					BufferElementGroupSPtr l_pVertex = std::make_shared< BufferElementGroup >( l_pData, uint32_t( m_points.size() ) );
					Vertex::SetPosition( l_pVertex, l_vertices.m_pVtx );
					Vertex::SetNormal( l_pVertex, l_vertices.m_pNml );
					Vertex::SetTangent( l_pVertex, l_vertices.m_pTan );
					Vertex::SetBitangent( l_pVertex, l_vertices.m_pBin );
					Vertex::SetTexCoord( l_pVertex, l_vertices.m_pTex );
					m_points.push_back( l_pVertex );
					l_pData += l_uiStride;
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
					BufferElementGroupSPtr l_pVertex = std::make_shared< BufferElementGroup >( l_pData, uint32_t( m_points.size() ) );
					Vertex::SetPosition( l_pVertex, l_vertices.m_pVtx );
					Vertex::SetNormal( l_pVertex, l_vertices.m_pNml );
					Vertex::SetTexCoord( l_pVertex, l_vertices.m_pTex );
					m_points.push_back( l_pVertex );
					l_pData += l_uiStride;
					l_vertices.m_pVtx += Vertex::GetCountPos();
					l_vertices.m_pNml += Vertex::GetCountNml();
					l_vertices.m_pTex += Vertex::GetCountTex();
				}
			}
			else if ( l_vertices.m_pNml )
			{
				for ( uint32_t i = 0; i < l_vertices.m_uiCount; i++ )
				{
					BufferElementGroupSPtr l_pVertex = std::make_shared< BufferElementGroup >( l_pData, uint32_t( m_points.size() ) );
					Vertex::SetPosition( l_pVertex, l_vertices.m_pVtx );
					Vertex::SetNormal( l_pVertex, l_vertices.m_pNml );
					m_points.push_back( l_pVertex );
					l_pData += l_uiStride;
					l_vertices.m_pVtx += Vertex::GetCountPos();
					l_vertices.m_pNml += Vertex::GetCountNml();
				}
			}
			else if ( l_vertices.m_pTex )
			{
				for ( uint32_t i = 0; i < l_vertices.m_uiCount; i++ )
				{
					BufferElementGroupSPtr l_pVertex = std::make_shared< BufferElementGroup >( l_pData, uint32_t( m_points.size() ) );
					Vertex::SetPosition( l_pVertex, l_vertices.m_pVtx );
					Vertex::SetTexCoord( l_pVertex, l_vertices.m_pTex );
					m_points.push_back( l_pVertex );
					l_pData += l_uiStride;
					l_vertices.m_pVtx += Vertex::GetCountPos();
					l_vertices.m_pTex += Vertex::GetCountTex();
				}
			}
			else
			{
				for ( uint32_t i = 0; i < l_vertices.m_uiCount; i++ )
				{
					BufferElementGroupSPtr l_pVertex = std::make_shared< BufferElementGroup >( l_pData, uint32_t( m_points.size() ) );
					Vertex::SetPosition( l_pVertex, l_vertices.m_pVtx );
					m_points.push_back( l_pVertex );
					l_pData += l_uiStride;
					l_vertices.m_pVtx += Vertex::GetCountPos();
				}
			}

			if ( l_vertices.m_pBones )
			{
//				l_pData = &(*m_pointsData.rbegin())[0];
				for ( VertexPtrArrayIt l_it = m_points.begin() + l_uiVtxCount; l_it != m_points.end(); ++l_it )
				{
					BonedVertex::SetBones( *l_it, l_vertices.m_pBones );
//					l_pData += l_uiStride;
					l_vertices.m_pBones++;
				}

				m_uiProgramFlags |= ePROGRAM_FLAG_SKINNING;
			}
		}
	}

	FaceSPtr Submesh::AddFace( uint32_t a, uint32_t b, uint32_t c )
	{
		FaceSPtr l_pReturn;

		if ( a < m_points.size() && b < m_points.size() && c < m_points.size() )
		{
			l_pReturn = std::make_shared< Face >( a, b, c );
			m_arrayFaces.push_back( l_pReturn );
			m_bHasNormals = false;
		}
		else
		{
			CASTOR_ASSERT( false );
			throw ( std::range_error( "Submesh::AddFace - One or more index out of bound" ) );
		}

		return l_pReturn;
	}

	void Submesh::AddFaceGroup( stFACE_INDICES * p_pFaces, uint32_t p_uiNbFaces )
	{
		for ( uint32_t i = 0; i < p_uiNbFaces; i++ )
		{
			AddFace( p_pFaces[i].m_uiVertexIndex[0], p_pFaces[i].m_uiVertexIndex[1], p_pFaces[i].m_uiVertexIndex[2] );
		}
	}

	void Submesh::AddQuadFace( uint32_t a, uint32_t b, uint32_t c, uint32_t d, Point3r const & p_ptMinUV, Point3r const & p_ptMaxUV )
	{
		FaceSPtr l_face1 = AddFace( a, b, c );
		FaceSPtr l_face2 = AddFace( a, c, d );
		Vertex::SetTexCoord( m_points[a], p_ptMinUV[0], p_ptMinUV[1] );
		Vertex::SetTexCoord( m_points[b], p_ptMaxUV[0], p_ptMinUV[1] );
		Vertex::SetTexCoord( m_points[c], p_ptMaxUV[0], p_ptMaxUV[1] );
		Vertex::SetTexCoord( m_points[d], p_ptMinUV[0], p_ptMaxUV[1] );
	}

	void Submesh::ClearFaces()
	{
		m_arrayFaces.clear();

		if ( m_pRenderer.lock()->GetGeometryBuffers()->HasIndexBuffer() )
		{
			m_pRenderer.lock()->GetGeometryBuffers()->GetIndexBuffer().Clear();
		}
	}

	void Submesh::GenerateBuffers()
	{
		DoGenerateVertexBuffer();
		DoGenerateIndexBuffer();
//		if( m_pEngine->GetRenderSystem()->HasInstancing() )
		{
			DoGenerateMatrixBuffer();
		}
		m_pEngine->PostEvent( std::make_shared< InitialiseEvent< SubmeshRenderer > >( *GetRenderer() ) );
	}

	SubmeshSPtr Submesh::Clone()
	{
		SubmeshSPtr l_clone = std::make_shared< Submesh >( m_pParentMesh, m_pEngine, m_uiID );
		uint32_t l_uiStride = m_pDeclaration->GetStride();

		//On effectue une copie des vertex
		for ( VertexPtrArrayConstIt l_it = m_points.begin(); l_it != m_points.end(); ++l_it )
		{
			BufferElementGroupSPtr l_pVertexSrc = ( *l_it );
			BufferElementGroupSPtr l_pVertexDst = l_clone->AddPoint( 0, 0, 0 );
			memcpy( l_pVertexDst->ptr(), l_pVertexSrc->const_ptr(), l_uiStride );
		}

		for ( uint32_t j = 0; j < GetFaceCount(); j++ )
		{
			FaceSPtr l_pFaceSrc = GetFace( j );
			l_clone->AddFace( l_pFaceSrc->GetVertexIndex( 0 ), l_pFaceSrc->GetVertexIndex( 1 ), l_pFaceSrc->GetVertexIndex( 2 ) );
		}

		return l_clone;
	}

	void Submesh::ResetGpuBuffers()
	{
		if ( !m_pRenderer.expired() )
		{
			SubmeshRendererSPtr l_pRenderer = m_pRenderer.lock();
			l_pRenderer->Cleanup();
			l_pRenderer->CreateBuffers();

			if ( l_pRenderer->IsInitialised() )
			{
				l_pRenderer->Initialise();
			}
		}
	}

	void Submesh::Render()
	{
		/* UNUSED
				if( !m_material.expired() && !m_pRenderer.expired() )
				{
					SubmeshRendererPtr l_pRenderer = m_pRenderer.lock();
					MaterialPtr l_pMaterial = m_material.lock();
					uint32_t l_uiCount = l_pMaterial->GetNbPasses();

					for (uint32_t j = 0; j < l_uiCount; j++)
					{
						PassPtr l_pass = l_pMaterial->GetPass( j);
						l_pass->Render();
						l_pRenderer->Draw( *l_pass );
						l_pass->EndRender();
					}
				}
		*/
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

	void Submesh::ComputeNormals( bool p_bReverted )
	{
		if ( !m_bHasNormals )
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

//			std::vector< FaceAndAnglePtrArray > l_arrayFaces;
//			FaceAndAnglePtr	l_pFaceAndAngle;
//			FaceAndAnglePtr l_faa1;
//			FaceAndAnglePtr l_faa2;
//			FaceAndAnglePtr l_faa3;
//			uint32_t l_uiIndex = 0;
//			Point3r l_vec1m2;
//			Point3r l_vec1m3;
//			Point3r l_vec2m3;
//			Point3r l_vec3m2;
//
//			//creation d'un vecteur de liste (chaque point contient la liste des faces auxquelles il appartient)
//			l_arrayFaces.resize(	m_points.size() );
//
//			//Pour chaque vertex, on stocke la liste des faces auxquelles il appartient
//			for( FacePtrArray::iterator l_it = m_arrayFaces.begin(); l_it != m_arrayFaces.end(); ++l_it )
//			{
//				FaceSPtr l_pFace = (*l_it);
//
//				l_pVtx1 = m_points[l_pFace->GetVertexIndex( 0 )];
//				l_pVtx2 = m_points[l_pFace->GetVertexIndex( 1 )];
//				l_pVtx3 = m_points[l_pFace->GetVertexIndex( 2 )];
//
//				Vertex::GetPosition( l_pVtx1, l_pt1 );
//				Vertex::GetPosition( l_pVtx2, l_pt2 );
//				Vertex::GetPosition( l_pVtx3, l_pt3 );
//
//				Vertex::GetTexCoord( l_pVtx1, l_uv1 );
//				Vertex::GetTexCoord( l_pVtx2, l_uv2 );
//				Vertex::GetTexCoord( l_pVtx3, l_uv3 );
//
//				l_vec1m2 = l_pt1 - l_pt2;
//				l_vec1m3 = l_pt1 - l_pt3;
//				l_vec2m1 = l_pt2 - l_pt1;
//				l_vec2m3 = l_pt2 - l_pt3;
//				l_vec3m1 = l_pt3 - l_pt1;
//				l_vec3m2 = l_pt3 - l_pt2;
//
//				l_tex2m1 = l_uv2 - l_uv1;
//				l_tex3m1 = l_uv3 - l_uv1;
//
//				l_vFaceNormal	= point::get_normalised( l_vec3m1 ^ l_vec2m1 );
//				l_vFaceTangent	= point::get_normalised( (l_vec3m1 * l_tex2m1[1]) - (l_vec2m1 * l_tex3m1[1]) );
//
//				l_faa1 = std::make_shared< FaceAndAngle >();
//				l_faa1->m_rAngle = real( std::abs( acos( point::cos_theta( l_vec3m1, l_vec2m1 ) ) ) );
//				l_faa1->m_ptNormal = l_vFaceNormal * l_faa1->m_rAngle;
//				l_faa1->m_ptTangent = l_vFaceTangent * l_faa1->m_rAngle;
//
//				l_faa2 = std::make_shared< FaceAndAngle >();
//				l_faa2->m_rAngle = real( std::abs( acos( point::cos_theta( l_vec1m2, l_vec3m2 ) ) ) );
//				l_faa2->m_ptNormal = l_vFaceNormal * l_faa2->m_rAngle;
//				l_faa2->m_ptTangent = l_vFaceTangent * l_faa2->m_rAngle;
//
//				l_faa3 = std::make_shared< FaceAndAngle >();
//				l_faa3->m_rAngle = real( std::abs( acos( point::cos_theta( l_vec1m3, l_vec2m3 ) ) ) );
//				l_faa3->m_ptNormal = l_vFaceNormal * l_faa3->m_rAngle;
//				l_faa3->m_ptTangent = l_vFaceTangent * l_faa3->m_rAngle;
//
//				l_arrayFaces[l_pFace->GetVertexIndex( 0 )].push_back( l_faa1 );
//				l_arrayFaces[l_pFace->GetVertexIndex( 1 )].push_back( l_faa2 );
//				l_arrayFaces[l_pFace->GetVertexIndex( 2 )].push_back( l_faa3 );
//			}
//
//			//On effectue la moyennes des normales
//			for( std::vector< FaceAndAnglePtrArray >::iterator l_itFaa = l_arrayFaces.begin(); l_itFaa != l_arrayFaces.end(); ++l_itFaa )
//			{
//				Point3r l_normal, l_tangent;
//
//				for( FaceAndAnglePtrArray::const_iterator l_it = l_itFaa->begin(); l_it != l_itFaa->end(); ++l_it )
//				{
//					l_pFaceAndAngle	 = (*l_it);
//					l_normal		+= l_pFaceAndAngle->m_ptNormal;
//					l_tangent		+= l_pFaceAndAngle->m_ptTangent;
//				}
//
//				point::normalise( l_normal );
//				point::normalise( l_tangent );
//				if( p_bReverted )
//				{
//					Vertex::SetNormal( m_points[l_uiIndex], -l_normal[0], -l_normal[1], -l_normal[2] );
//					Vertex::SetTangent( m_points[l_uiIndex], -l_tangent[0], -l_tangent[1], -l_tangent[2] );
//				}
//				else
//				{
//					Vertex::SetNormal( m_points[l_uiIndex], l_normal );
//					Vertex::SetTangent( m_points[l_uiIndex], l_tangent );
//				}
//
//				l_uiIndex++;
//			}
//
//			clear_container( l_arrayFaces );

			// First we flush normals and tangents
			for ( VertexPtrArray::iterator l_it = m_points.begin(); l_it != m_points.end(); ++l_it )
			{
				l_pVtx1 = *l_it;
				Vertex::SetNormal( l_pVtx1, l_pt1 );
				Vertex::SetTangent( l_pVtx1, l_pt1 );
			}

			Coords3r l_coord;

			// Then we compute normals and tangents
			if ( p_bReverted )
			{
				for ( FacePtrArray::iterator l_it = m_arrayFaces.begin(); l_it != m_arrayFaces.end(); ++l_it )
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
				for ( FacePtrArray::iterator l_it = m_arrayFaces.begin(); l_it != m_arrayFaces.end(); ++l_it )
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

			m_bHasNormals = true;
		}
	}

	void Submesh::ComputeNormals( FaceSPtr p_pFace )
	{
		BufferElementGroupSPtr l_pVtx1, l_pVtx2, l_pVtx3;
		Point3r l_vec2m1;
		Point3r l_vec3m1;
		Point3r l_vFaceNormal;
		Point3r l_pt1;
		Point3r l_pt2;
		Point3r l_pt3;
		l_pVtx1 = m_points[p_pFace->GetVertexIndex( 0 )];
		l_pVtx2 = m_points[p_pFace->GetVertexIndex( 1 )];
		l_pVtx3 = m_points[p_pFace->GetVertexIndex( 2 )];
		Vertex::GetPosition( l_pVtx1, l_pt1 );
		Vertex::GetPosition( l_pVtx2, l_pt2 );
		Vertex::GetPosition( l_pVtx3, l_pt3 );
		l_vec2m1 = l_pt2 - l_pt1;
		l_vec3m1 = l_pt3 - l_pt1;
		l_vFaceNormal = point::get_normalised( l_vec2m1 ^ l_vec3m1 );
		Vertex::SetNormal( l_pVtx1, l_vFaceNormal );
		Vertex::SetNormal( l_pVtx2, l_vFaceNormal );
		Vertex::SetNormal( l_pVtx3, l_vFaceNormal );
		ComputeTangents( p_pFace );
	}

	void Submesh::ComputeTangents( FaceSPtr p_pFace )
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
		l_pVtx1 = m_points[p_pFace->GetVertexIndex( 0 )];
		l_pVtx2 = m_points[p_pFace->GetVertexIndex( 1 )];
		l_pVtx3 = m_points[p_pFace->GetVertexIndex( 2 )];
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
		for ( FacePtrArray::iterator l_it = m_arrayFaces.begin(); l_it != m_arrayFaces.end(); ++l_it )
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
		if ( m_ptCameraPosition != p_ptCameraPosition )
		{
			GeometryBuffersSPtr l_pGeometryBuffers = GetRenderer()->GetGeometryBuffers();
			IndexBuffer & l_indices = l_pGeometryBuffers->GetIndexBuffer();
			VertexBuffer & l_vertices = l_pGeometryBuffers->GetVertexBuffer();

			if ( l_pGeometryBuffers && l_pGeometryBuffers->Bind() )
			{
				m_ptCameraPosition = p_ptCameraPosition;
				uint32_t l_uiIdxSize = l_indices.GetSize();
				uint32_t * l_pIdx = l_indices.Lock( 0, l_uiIdxSize, eLOCK_FLAG_WRITE_ONLY | eLOCK_FLAG_READ_ONLY );

				if ( l_pIdx )
				{
					struct stFACE_DISTANCE
					{
						uint32_t m_index[3];
						double m_distance;
					};
					uint32_t l_uiStride = l_vertices.GetDeclaration().GetStride();
					uint8_t * l_pVtx = l_vertices.data();
					DECLARE_VECTOR( stFACE_DISTANCE, Face );
					FaceArray l_arraySorted;
					l_arraySorted.reserve( l_uiIdxSize / 3 );

					if ( l_pVtx )
					{
						for ( uint32_t * l_it = l_pIdx + 0; l_it < l_pIdx + l_uiIdxSize; l_it += 3 )
						{
							double l_dDistance = 0.0;
							Point3r l_pVtx1( reinterpret_cast< real const * >( &l_pVtx[l_it[0] * l_uiStride] ) );
							l_pVtx1 -= p_ptCameraPosition;
							l_dDistance += point::distance_squared( l_pVtx1 );
							Point3r l_pVtx2( reinterpret_cast< real const * >( &l_pVtx[l_it[1] * l_uiStride] ) );
							l_pVtx2 -= p_ptCameraPosition;
							l_dDistance += point::distance_squared( l_pVtx2 );
							Point3r l_pVtx3( reinterpret_cast< real const * >( &l_pVtx[l_it[2] * l_uiStride] ) );
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
							*l_pIdx = l_it->m_index[0];
							l_pIdx++;
							*l_pIdx = l_it->m_index[1];
							l_pIdx++;
							*l_pIdx = l_it->m_index[2];
							l_pIdx++;
						}
					}

					l_indices.Unlock();
				}

				l_pGeometryBuffers->Unbind();
			}
		}
	}

	void Submesh::Ref( MaterialSPtr p_material )
	{
		std::map< MaterialSPtr, uint32_t >::iterator l_it = m_uiInstanceCount.find( p_material );

		if ( l_it == m_uiInstanceCount.end() )
		{
			m_uiInstanceCount[p_material] = 0;
			l_it = m_uiInstanceCount.find( p_material );
		}

		l_it->second++;
	}

	void Submesh::UnRef( MaterialSPtr p_material )
	{
		std::map< MaterialSPtr, uint32_t >::iterator l_it = m_uiInstanceCount.find( p_material );

		if ( l_it == m_uiInstanceCount.end() )
		{
			if ( l_it->second )
			{
				l_it->second--;
			}

			if ( !l_it->second )
			{
				m_uiInstanceCount.erase( l_it );
			}
		}
	}

	uint32_t Submesh::GetRefCount( MaterialSPtr p_material )const
	{
		uint32_t l_return = 0;
		std::map< MaterialSPtr, uint32_t >::const_iterator l_it = m_uiInstanceCount.find( p_material );

		if ( l_it != m_uiInstanceCount.end() )
		{
			l_return = l_it->second;
		}

		return l_return;
	}

	void Submesh::DoGenerateVertexBuffer()
	{
		if ( m_pRenderer.lock()->GetGeometryBuffers() )
		{
			uint8_t 	*		l_pBuffer;
			VertexBuffer 	&	l_vertexBuffer	= m_pRenderer.lock()->GetGeometryBuffers()->GetVertexBuffer();
			uint32_t			l_uiStride		= m_pDeclaration->GetStride();
			uint32_t			l_uiSize		= uint32_t( m_points.size() ) * l_uiStride;
			VertexPtrArrayIt	l_itPoints		= m_points.begin();

			if ( l_uiSize )
			{
				if ( l_vertexBuffer.GetSize() != l_uiSize )
				{
					l_vertexBuffer.Resize( l_uiSize );
				}

				l_pBuffer = l_vertexBuffer.data();

				for ( BytePtrListIt l_it = m_pointsData.begin(); l_it != m_pointsData.end(); ++l_it )
				{
					std::memcpy( l_pBuffer, &( *l_it )[0], l_it->size() );
					l_pBuffer += l_it->size();
				}

				m_pointsData.clear();
				l_pBuffer = l_vertexBuffer.data();

				for ( VertexPtrArrayIt l_it = m_points.begin(); l_it != m_points.end(); ++l_it )
				{
					( *l_it )->LinkCoords( l_pBuffer );
					l_pBuffer += l_uiStride;
				}

				//m_points.clear();
			}
		}
	}

	void Submesh::DoGenerateIndexBuffer()
	{
		if ( m_pRenderer.lock()->GetGeometryBuffers() && m_pRenderer.lock()->GetGeometryBuffers()->HasIndexBuffer() )
		{
			FaceSPtr		l_pFace;
			uint32_t		l_uiIndex		= 0;
			IndexBuffer &	l_indexBuffer	= m_pRenderer.lock()->GetGeometryBuffers()->GetIndexBuffer();
			uint32_t		l_uiSize		= uint32_t( m_arrayFaces.size() * 3 );

			if ( l_uiSize )
			{
				if ( l_indexBuffer.GetSize() != l_uiSize )
				{
					l_indexBuffer.Resize( l_uiSize );
				}

				for ( FacePtrArray::iterator l_it = m_arrayFaces.begin(); l_it != m_arrayFaces.end(); ++l_it )
				{
					if ( *l_it )
					{
						l_pFace = *l_it;
						l_indexBuffer.SetElement( l_uiIndex++, l_pFace->GetVertexIndex( 0 ) );
						l_indexBuffer.SetElement( l_uiIndex++, l_pFace->GetVertexIndex( 1 ) );
						l_indexBuffer.SetElement( l_uiIndex++, l_pFace->GetVertexIndex( 2 ) );
					}
				}

//				m_arrayFaces.clear();
			}
		}
	}

	void Submesh::DoGenerateMatrixBuffer()
	{
		if ( m_pRenderer.lock()->GetGeometryBuffers()->HasMatrixBuffer() )
		{
			uint32_t l_count = 0;
			l_count = std::accumulate( m_uiInstanceCount.begin(), m_uiInstanceCount.end(), l_count, [&]( uint32_t p_count, std::pair< MaterialSPtr, uint32_t > const & p_pair )
			{
				return p_count + p_pair.second;
			} );

			if ( l_count )
			{
				MatrixBuffer & l_matrixBuffer = m_pRenderer.lock()->GetGeometryBuffers()->GetMatrixBuffer();
				uint32_t l_uiSize = l_count * 16;

				if ( l_matrixBuffer.GetSize() != l_uiSize )
				{
					l_matrixBuffer.Resize( l_uiSize );
				}
			}
		}
	}

	void Submesh::DoUpdateDeclaration()
	{
		if ( !m_pDeclaration )
		{
			std::vector< BufferElementDeclaration >	l_vertexDeclarationElements;
			l_vertexDeclarationElements.push_back( BufferElementDeclaration( 0, eELEMENT_USAGE_POSITION,	eELEMENT_TYPE_3FLOATS ) );
			l_vertexDeclarationElements.push_back( BufferElementDeclaration( 0, eELEMENT_USAGE_NORMAL,		eELEMENT_TYPE_3FLOATS ) );
			l_vertexDeclarationElements.push_back( BufferElementDeclaration( 0, eELEMENT_USAGE_TANGENT,		eELEMENT_TYPE_3FLOATS ) );
			l_vertexDeclarationElements.push_back( BufferElementDeclaration( 0, eELEMENT_USAGE_BITANGENT,	eELEMENT_TYPE_3FLOATS ) );
			l_vertexDeclarationElements.push_back( BufferElementDeclaration( 0, eELEMENT_USAGE_TEXCOORDS0,	eELEMENT_TYPE_3FLOATS ) );

			if ( GetSkeleton() )
			{
				l_vertexDeclarationElements.push_back( BufferElementDeclaration( 0, eELEMENT_USAGE_BONE_IDS,		eELEMENT_TYPE_4INTS ) );
				l_vertexDeclarationElements.push_back( BufferElementDeclaration( 0, eELEMENT_USAGE_BONE_WEIGHTS,	eELEMENT_TYPE_4FLOATS ) );
			}

			m_pDeclaration = std::make_shared< BufferDeclaration >( &l_vertexDeclarationElements[0], uint32_t( l_vertexDeclarationElements.size() ) );

			if ( !m_pRenderer.expired() )
			{
				SubmeshRendererSPtr l_pRenderer = m_pRenderer.lock();
				l_pRenderer->CreateBuffers();
			}
		}
	}
}
