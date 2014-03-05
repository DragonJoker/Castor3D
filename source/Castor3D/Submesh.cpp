#include "Castor3D/PrecompiledHeader.hpp"

#include "Castor3D/Submesh.hpp"
#include "Castor3D/Vertex.hpp"
#include "Castor3D/Buffer.hpp"
#include "Castor3D/Material.hpp"
#include "Castor3D/Face.hpp"
#include "Castor3D/Pass.hpp"
#include "Castor3D/Pipeline.hpp"
#include "Castor3D/ShaderProgram.hpp"
#include "Castor3D/Engine.hpp"
#include "Castor3D/ShaderManager.hpp"
#include "Castor3D/Skeleton.hpp"

#include <CastorUtils/FunctionTracker.hpp>

using namespace Castor3D;
using namespace Castor;

//*************************************************************************************************

Submesh::TextLoader :: TextLoader( File::eENCODING_MODE p_eEncodingMode )
	:	Loader< Submesh, eFILE_TYPE_TEXT, TextFile >( File::eOPEN_MODE_DUMMY, p_eEncodingMode )
{
}

bool Submesh::TextLoader :: operator ()( Submesh const & p_submesh, TextFile & p_file)
{
	bool l_bReturn = p_file.WriteText( cuT( "\t\tsubmesh\n\t\t{\n")) > 0;
	Point3r l_ptPos;
	Point3r l_ptTan;
	Point3r l_ptNml;
	Point3r l_ptTex;

	if (l_bReturn)
	{
		l_bReturn = p_file.WriteText( cuT( "\t\t\tmaterial ") + p_submesh.GetMaterialName() + cuT( "\n")) > 0;
	}

	for (uint32_t i = 0; i < p_submesh.GetPointsCount() && l_bReturn; i++)
	{
		Vertex::GetPosition(	p_submesh[i]->const_ptr(), l_ptPos );
		Vertex::GetNormal(		p_submesh[i]->const_ptr(), l_ptNml );
		Vertex::GetTangent(		p_submesh[i]->const_ptr(), l_ptTan );
		Vertex::GetTexCoord(	p_submesh[i]->const_ptr(), l_ptTex );
		l_bReturn = p_file.Print( 1024, cuT( "\t\t\tvertex %f %f %f %f %f %f %f %f %f %f %f\n"), l_ptPos[0], l_ptPos[1], l_ptPos[2], l_ptNml[0], l_ptNml[1], l_ptNml[2], l_ptTan[0], l_ptTan[1], l_ptTan[2], l_ptTex[0], l_ptTex[1] ) > 0;
	}

	uint32_t l_uiNbFaces = p_submesh.GetFacesCount();

	for (uint32_t j = 0; j < l_uiNbFaces && l_bReturn; j++)
	{
		l_bReturn = Face::TextLoader()( *p_submesh.GetFace( j ),p_file);
	}

	if (l_bReturn)
	{
		l_bReturn = p_file.WriteText( cuT( "\t\t\t}\n")) > 0;
	}

	return l_bReturn;
}

//*************************************************************************************************

stVERTEX_BONE_DATA :: stVERTEX_BONE_DATA()
{
	for( int i = 0; i < C3D_MAX_BONES_PER_VERTEX; ++i )
	{
		m_ids[i] = 0;
		m_weights[i] = 0;
	}
}

void stVERTEX_BONE_DATA :: AddBoneData( uint32_t p_uiBoneId, real p_rWeight )
{
	bool l_bDone = false;

	for( int i = 0; i < C3D_MAX_BONES_PER_VERTEX && !l_bDone; i++ )
	{
		if( m_weights[i] == 0.0 )
		{
			m_ids[i]     = p_uiBoneId;
			m_weights[i] = p_rWeight;
			l_bDone = true;
		}
	}

	CASTOR_ASSERT( l_bDone );
}

//*************************************************************************************************

SubmeshRenderer :: SubmeshRenderer( RenderSystem * p_pRenderSystem )
	:	Renderer<Submesh, SubmeshRenderer>	( p_pRenderSystem	)
	,	m_eCurDrawType						( eTOPOLOGY( -1 )	)
	,	m_ePrvDrawType						( eTOPOLOGY( -1 )	)
	,	m_bInitialised						( false				)
	,	m_bDirty							( false				)
{
}

SubmeshRenderer :: ~SubmeshRenderer()
{
	Cleanup();
}

void SubmeshRenderer :: Cleanup()
{
	if( m_pGeometryBuffers )
	{
		m_pGeometryBuffers->Cleanup();
	}

	m_bInitialised = false;
	VertexBufferSPtr l_pVtxBuffer = m_vertex.lock();
	IndexBufferSPtr l_pIdxBuffer = m_indices.lock();
	MatrixBufferSPtr l_pMtxBuffer = m_matrices.lock();

	l_pVtxBuffer->Cleanup();
	l_pVtxBuffer->Destroy();

	if( l_pIdxBuffer )
	{
		l_pIdxBuffer->Cleanup();
		l_pIdxBuffer->Destroy();
	}

	if( l_pMtxBuffer )
	{
		l_pMtxBuffer->Cleanup();
		l_pMtxBuffer->Destroy();
	}
}

void SubmeshRenderer :: Initialise()
{
	VertexBufferSPtr l_pVtxBuffer = m_vertex.lock();
	IndexBufferSPtr l_pIdxBuffer = m_indices.lock();
	MatrixBufferSPtr l_pMtxBuffer = m_matrices.lock();

	l_pVtxBuffer->Create();
	l_pVtxBuffer->Initialise(	eBUFFER_ACCESS_TYPE_DYNAMIC,	eBUFFER_ACCESS_NATURE_DRAW );

	if( l_pIdxBuffer )
	{
		l_pIdxBuffer->Create();
		l_pIdxBuffer->Initialise(	eBUFFER_ACCESS_TYPE_STREAM,		eBUFFER_ACCESS_NATURE_DRAW );
	}

	if( l_pMtxBuffer )
	{
		l_pMtxBuffer->Create();
		l_pMtxBuffer->Initialise(	eBUFFER_ACCESS_TYPE_STREAM,		eBUFFER_ACCESS_NATURE_DRAW );
	}

	m_pGeometryBuffers->Initialise();

	if( !m_target->GetMaterial() )
	{
		m_target->SetMaterial( m_pRenderSystem->GetEngine()->GetMaterialManager().GetDefaultMaterial() );
	}

	if( !m_target->GetMaterial()->HasAlphaBlending() )
	{
		m_target->Cleanup();
	}

	uint32_t l_uiProgramFlags = m_target->GetProgramFlags();
	
	if( m_pRenderSystem->GetCurrentContext()->IsDeferredShadingSet() )
	{
		l_uiProgramFlags |= ePROGRAM_FLAG_DEFERRED;
	}

	SkeletonSPtr l_pSkeleton = m_target->GetSkeleton();

	std::for_each( m_target->GetMaterial()->Begin(), m_target->GetMaterial()->End(), [&]( PassSPtr p_pPass )
	{
		if( !p_pPass->HasShader() )
		{
			ShaderProgramBaseSPtr l_pProgram = GetTarget()->GetEngine()->GetShaderManager().GetAutomaticProgram( p_pPass->GetTextureFlags(), l_uiProgramFlags );
			l_pProgram->Initialise();
		}
	} );

	m_bInitialised = true;
}

void SubmeshRenderer :: Draw( eTOPOLOGY p_eMode, Pass const & p_pass )
{
	if( p_eMode != m_eCurDrawType )
	{
		m_eCurDrawType = p_eMode;
	}

	if( DoPrepareBuffers( p_pass ) )
	{
		ShaderProgramBaseSPtr l_pProgram = p_pass.GetShader< ShaderProgramBase >();
		uint32_t l_uiSize = m_pGeometryBuffers->GetVertexBuffer()->GetSize() / m_pGeometryBuffers->GetVertexBuffer()->GetDeclaration().GetStride();

		if( m_pGeometryBuffers->GetIndexBuffer() )
		{
			l_uiSize = m_pGeometryBuffers->GetIndexBuffer()->GetSize();
		}

		if( GetTarget()->GetRefCount() > 1 )
		{
			if( m_pRenderSystem->HasInstancing() )
			{
				m_pGeometryBuffers->DrawInstanced( m_eCurDrawType, l_pProgram, l_uiSize, 0, GetTarget()->GetRefCount() );
			}
			else
			{
				m_pGeometryBuffers->Draw( m_eCurDrawType, l_pProgram, l_uiSize, 0 );
			}
		}
		else
		{
			m_pGeometryBuffers->Draw( m_eCurDrawType, l_pProgram, l_uiSize, 0 );
		}
	}
}

void SubmeshRenderer :: CreateBuffers()
{
	std::vector< BufferElementDeclaration >	l_vertexDeclarationElements;
	l_vertexDeclarationElements.push_back( BufferElementDeclaration( 0, eELEMENT_USAGE_POSITION,	eELEMENT_TYPE_3FLOATS ) );
	l_vertexDeclarationElements.push_back( BufferElementDeclaration( 0, eELEMENT_USAGE_NORMAL,		eELEMENT_TYPE_3FLOATS ) );
	l_vertexDeclarationElements.push_back( BufferElementDeclaration( 0, eELEMENT_USAGE_TANGENT,		eELEMENT_TYPE_3FLOATS ) );
	l_vertexDeclarationElements.push_back( BufferElementDeclaration( 0, eELEMENT_USAGE_BITANGENT,	eELEMENT_TYPE_3FLOATS ) );
	l_vertexDeclarationElements.push_back( BufferElementDeclaration( 0, eELEMENT_USAGE_TEXCOORDS0,	eELEMENT_TYPE_3FLOATS ) );

	if( m_target && m_target->GetSkeleton() )
	{
		l_vertexDeclarationElements.push_back( BufferElementDeclaration( 0, eELEMENT_USAGE_BONE_IDS,		eELEMENT_TYPE_4INTS ) );
		l_vertexDeclarationElements.push_back( BufferElementDeclaration( 0, eELEMENT_USAGE_BONE_WEIGHTS,	eELEMENT_TYPE_4FLOATS ) );
	}

	VertexBufferSPtr l_pVtxBuffer = std::make_shared< VertexBuffer >( m_pRenderSystem, &l_vertexDeclarationElements[0], uint32_t( l_vertexDeclarationElements.size() ) );
	IndexBufferSPtr l_pIdxBuffer = std::make_shared< IndexBuffer >( m_pRenderSystem );
	MatrixBufferSPtr l_pMtxBuffer = std::make_shared< MatrixBuffer >( m_pRenderSystem );

	l_pVtxBuffer->SetShared( l_pVtxBuffer );
	
	if( l_pIdxBuffer )
	{
		l_pIdxBuffer->SetShared( l_pIdxBuffer );
	}

	if( l_pMtxBuffer )
	{
		l_pMtxBuffer->SetShared( l_pMtxBuffer );
	}

	m_pGeometryBuffers = m_pRenderSystem->CreateGeometryBuffers( l_pVtxBuffer, l_pIdxBuffer, l_pMtxBuffer );

	m_vertex = l_pVtxBuffer;
	m_indices = l_pIdxBuffer;
	m_matrices = l_pMtxBuffer;
}

bool SubmeshRenderer :: DoPrepareBuffers( Pass const & p_pass )
{
	ShaderProgramBaseSPtr l_pProgram = p_pass.GetShader< ShaderProgramBase >();
	bool l_bUseShader = l_pProgram && l_pProgram->GetStatus() == ePROGRAM_STATUS_LINKED;

	if( m_eCurDrawType != m_ePrvDrawType || !m_pGeometryBuffers )
	{
		m_ePrvDrawType = m_eCurDrawType;

		VertexBufferSPtr l_pVtxBuffer = m_vertex.lock();
		IndexBufferSPtr l_pIdxBuffer = m_indices.lock();
		MatrixBufferSPtr l_pMtxBuffer = m_matrices.lock();

		if( l_pVtxBuffer )
		{
			l_pVtxBuffer->Cleanup();

			if( l_bUseShader )
			{
				l_pVtxBuffer->Initialise( eBUFFER_ACCESS_TYPE_DYNAMIC, eBUFFER_ACCESS_NATURE_DRAW, l_pProgram );
			}
			else
			{
				l_pVtxBuffer->Initialise( eBUFFER_ACCESS_TYPE_DYNAMIC, eBUFFER_ACCESS_NATURE_DRAW );
			}
		}

		if( l_pIdxBuffer )
		{
			l_pIdxBuffer->Cleanup();
			l_pIdxBuffer->Initialise( eBUFFER_ACCESS_TYPE_STREAM, eBUFFER_ACCESS_NATURE_DRAW );
		}

		if( l_pMtxBuffer )
		{
			l_pMtxBuffer->Cleanup();
			l_pMtxBuffer->Initialise( eBUFFER_ACCESS_TYPE_DYNAMIC, eBUFFER_ACCESS_NATURE_DRAW, l_pProgram );
		}

		if( m_pGeometryBuffers )
		{
			m_pGeometryBuffers->Cleanup();
		}

		m_pGeometryBuffers->Initialise();
	}
	else if( m_bDirty )
	{
		m_pGeometryBuffers->Initialise();
	}

	if( l_bUseShader )
	{
		m_pRenderSystem->GetPipeline()->ApplyMatrices( *l_pProgram );
	}
	else
	{
		l_bUseShader = true;
	}

	return l_bUseShader;
}

//*************************************************************************************************

Submesh :: Submesh( MeshRPtr p_pMesh, Engine * p_pEngine, uint32_t p_uiId )
	:	Renderable< Submesh, SubmeshRenderer >	( p_pEngine												)
	,	m_strMatName							( cuT( "DefaultMaterial" )								)
	,	m_material								( p_pEngine->GetMaterialManager().GetDefaultMaterial()	)
	,	m_uiID									( p_uiId												)
	,	m_pParentMesh							( p_pMesh												)
	,	m_uiInstanceCount						( 0														)
	,	m_uiProgramFlags						( 0														)
{
}

Submesh :: ~Submesh()
{
	Cleanup();
}

void Submesh :: Cleanup()
{
	m_arrayFaces.clear();
	m_points.clear();
	m_pointsData.clear();
}

String Submesh :: GetMaterialName()const
{
	String l_strReturn = cuEmptyString;

	if( ! m_material.expired() )
	{
		l_strReturn = m_strMatName;
	}

	return l_strReturn;
}

void Submesh :: ComputeContainers()
{
	if( m_points.size() > 0 )
	{
		Point3r l_min;
		Point3r l_max;
		Point3r l_cur;

		Vertex::GetPosition( m_points[0], l_min );
		Vertex::GetPosition( m_points[0], l_max );

		uint32_t l_nbVertex = GetPointsCount();

		for( uint32_t i = 1; i < l_nbVertex; i++ )
		{
			Vertex::GetPosition( m_points[i], l_cur );

			if( l_cur[0] > l_max[0] )
			{
				l_max[0] = l_cur[0];
			}
			if( l_cur[0] < l_min[0] )
			{
				l_min[0] = l_cur[0];
			}
			if( l_cur[1] > l_max[1] )
			{
				l_max[1] = l_cur[1];
			}
			if( l_cur[1] < l_min[1] )
			{
				l_min[1] = l_cur[1];
			}
			if( l_cur[2] > l_max[2] )
			{
				l_max[2] = l_cur[2];
			}
			if( l_cur[2] < l_min[2] )
			{
				l_min[2] = l_cur[2];
			}
		}

		m_box.Load( l_min, l_max );
		m_sphere.Load( m_box );
	}
}

uint32_t Submesh :: GetFacesCount()const
{
	return std::max( uint32_t( m_arrayFaces.size() ), m_indices.lock()->GetSize() / 3 );
}

uint32_t Submesh :: GetPointsCount()const
{
	return std::max< uint32_t >( uint32_t( m_points.size() ), uint32_t( sizeof( real ) * m_vertex.lock()->GetSize() / m_pDeclaration->GetStride() ) );
}

int Submesh :: IsInMyPoints( Point3r const & p_vertex )
{
	int l_iReturn = -1;
	int l_iIndex = 0;
	Point3r l_ptPos;

	for( VertexPtrArrayConstIt l_it = m_points.begin(); l_it != m_points.end() && l_iReturn == -1; ++l_it )
	{
		if( p_vertex == Vertex::GetPosition( (*l_it), l_ptPos ) )
		{
			l_iReturn = int( l_iIndex );
		}

		l_iIndex++;
	}

	return l_iReturn;
}

BufferElementGroupSPtr Submesh :: AddPoint( real x, real y, real z )
{
	DoUpdateDeclaration();
	BufferElementGroupSPtr l_pReturn;
	m_pointsData.push_back( ByteArray( m_pDeclaration->GetStride() ) );
	uint8_t * l_pData = &(*m_pointsData.rbegin())[0];
	l_pReturn = std::make_shared< BufferElementGroup >( l_pData, uint32_t( m_points.size() ) );
	Vertex::SetPosition( l_pReturn, x, y, z );
	m_points.push_back( l_pReturn );
	return l_pReturn;
}

BufferElementGroupSPtr Submesh :: AddPoint( Point3r const & p_v )
{
	return AddPoint( p_v[0], p_v[1], p_v[2] );
}

BufferElementGroupSPtr Submesh :: AddPoint( real * p_v )
{
	return AddPoint( p_v[0], p_v[1], p_v[2] );
}

void Submesh :: AddPoints( stVERTEX_GROUP const & p_vertices )
{
	DoUpdateDeclaration();
	stVERTEX_GROUP l_vertices = p_vertices;
	uint32_t l_uiStride = m_pDeclaration->GetStride();
	uint32_t l_uiSize = l_vertices.m_uiCount * l_uiStride;
	m_pointsData.push_back( ByteArray( l_uiSize ) );
	uint8_t * l_pData = &(*m_pointsData.rbegin())[0];
	uint32_t l_uiVtxCount = uint32_t( m_points.size() );

	if( l_vertices.m_pVtx )
	{
		if( l_vertices.m_pNml && l_vertices.m_pTan && l_vertices.m_pBin && l_vertices.m_pTex )
		{
			for( uint32_t i = 0; i < l_vertices.m_uiCount; i++ )
			{
				BufferElementGroupSPtr l_pVertex = std::make_shared< BufferElementGroup >( l_pData, uint32_t( m_points.size() ) );
				Vertex::SetPosition(	l_pVertex, l_vertices.m_pVtx	);
				Vertex::SetNormal(		l_pVertex, l_vertices.m_pNml	);
				Vertex::SetTangent(		l_pVertex, l_vertices.m_pTan	);
				Vertex::SetBitangent(	l_pVertex, l_vertices.m_pBin	);
				Vertex::SetTexCoord(	l_pVertex, l_vertices.m_pTex	);
				m_points.push_back( l_pVertex );
				l_pData += l_uiStride;
				l_vertices.m_pVtx += Vertex::GetCountPos();
				l_vertices.m_pNml += Vertex::GetCountNml();
				l_vertices.m_pTan += Vertex::GetCountTan();
				l_vertices.m_pBin += Vertex::GetCountBin();
				l_vertices.m_pTex += Vertex::GetCountTex();
			}
		}
		else if( l_vertices.m_pNml && l_vertices.m_pTex )
		{
			for( uint32_t i = 0; i < l_vertices.m_uiCount; i++ )
			{
				BufferElementGroupSPtr l_pVertex = std::make_shared< BufferElementGroup >( l_pData, uint32_t( m_points.size() ) );
				Vertex::SetPosition(	l_pVertex, l_vertices.m_pVtx	);
				Vertex::SetNormal(		l_pVertex, l_vertices.m_pNml	);
				Vertex::SetTexCoord(	l_pVertex, l_vertices.m_pTex	);
				m_points.push_back( l_pVertex );
				l_pData += l_uiStride;
				l_vertices.m_pVtx += Vertex::GetCountPos();
				l_vertices.m_pNml += Vertex::GetCountNml();
				l_vertices.m_pTex += Vertex::GetCountTex();
			}
		}
		else if( l_vertices.m_pNml )
		{
			for( uint32_t i = 0; i < l_vertices.m_uiCount; i++ )
			{
				BufferElementGroupSPtr l_pVertex = std::make_shared< BufferElementGroup >( l_pData, uint32_t( m_points.size() ) );
				Vertex::SetPosition(	l_pVertex, l_vertices.m_pVtx	);
				Vertex::SetNormal(		l_pVertex, l_vertices.m_pNml	);
				m_points.push_back( l_pVertex );
				l_pData += l_uiStride;
				l_vertices.m_pVtx += Vertex::GetCountPos();
				l_vertices.m_pNml += Vertex::GetCountNml();
			}
		}
		else if( l_vertices.m_pTex )
		{
			for( uint32_t i = 0; i < l_vertices.m_uiCount; i++ )
			{
				BufferElementGroupSPtr l_pVertex = std::make_shared< BufferElementGroup >( l_pData, uint32_t( m_points.size() ) );
				Vertex::SetPosition(	l_pVertex, l_vertices.m_pVtx	);
				Vertex::SetTexCoord(	l_pVertex, l_vertices.m_pTex	);
				m_points.push_back( l_pVertex );
				l_pData += l_uiStride;
				l_vertices.m_pVtx += Vertex::GetCountPos();
				l_vertices.m_pTex += Vertex::GetCountTex();
			}
		}
		else
		{
			for( uint32_t i = 0; i < l_vertices.m_uiCount; i++ )
			{
				BufferElementGroupSPtr l_pVertex = std::make_shared< BufferElementGroup >( l_pData, uint32_t( m_points.size() ) );
				Vertex::SetPosition(	l_pVertex, l_vertices.m_pVtx	);
				m_points.push_back( l_pVertex );
				l_pData += l_uiStride;
				l_vertices.m_pVtx += Vertex::GetCountPos();
			}
		}

		if( l_vertices.m_pBones )
		{
//			l_pData = &(*m_pointsData.rbegin())[0];

			for( VertexPtrArrayIt l_it = m_points.begin() + l_uiVtxCount; l_it != m_points.end(); ++l_it )
			{
				BonedVertex::SetBones( *l_it, l_vertices.m_pBones );
//				l_pData += l_uiStride;
				l_vertices.m_pBones++;
			}

			m_uiProgramFlags |= ePROGRAM_FLAG_SKINNING;
		}
	}
}

FaceSPtr Submesh :: AddFace( uint32_t a, uint32_t b, uint32_t c )
{
	FaceSPtr l_pReturn;

	if( a < m_points.size() && b < m_points.size() && c < m_points.size() )
	{
		l_pReturn = std::make_shared< Face >( a, b, c );
		m_arrayFaces.push_back( l_pReturn );
		m_bHasNormals = false;
	}
	else
	{
		CASTOR_ASSERT( false );
		throw( std::range_error( "Submesh :: AddFace - One or more index out of bound" ) );
	}

	return l_pReturn;
}

void Submesh :: AddFaceGroup( stFACE_INDICES * p_pFaces, uint32_t p_uiNbFaces )
{
	for( uint32_t i = 0; i < p_uiNbFaces; i++ )
	{
		AddFace( p_pFaces[i].m_uiVertexIndex[0], p_pFaces[i].m_uiVertexIndex[1], p_pFaces[i].m_uiVertexIndex[2] );
	}
}

void Submesh :: AddQuadFace( uint32_t a, uint32_t b, uint32_t c, uint32_t d, Point3r const & p_ptMinUV, Point3r const & p_ptMaxUV)
{
	FaceSPtr l_face1 = AddFace( a, b, c );
	FaceSPtr l_face2 = AddFace( a, c, d );

	Vertex::SetTexCoord( m_points[a], p_ptMinUV[0], p_ptMinUV[1] );
	Vertex::SetTexCoord( m_points[b], p_ptMaxUV[0], p_ptMinUV[1] );
	Vertex::SetTexCoord( m_points[c], p_ptMaxUV[0], p_ptMaxUV[1] );
	Vertex::SetTexCoord( m_points[d], p_ptMinUV[0], p_ptMaxUV[1] );
}

void Submesh :: ClearFaces()
{
	m_arrayFaces.clear();

	if( !m_indices.expired() )
	{
		m_indices.lock()->Clear();
	}
}

void Submesh :: GenerateBuffers()
{
	DoGenerateVertexBuffer();
	DoGenerateIndexBuffer();

//	if( m_pEngine->GetRenderSystem()->HasInstancing() )
	{
		DoGenerateMatrixBuffer();
	}

	m_pEngine->PostEvent( std::make_shared< InitialiseEvent< SubmeshRenderer > >( *GetRenderer() ) );
}

SubmeshSPtr Submesh :: Clone()
{
	SubmeshSPtr l_clone = std::make_shared< Submesh >( m_pParentMesh, m_pEngine, m_uiID );
	uint32_t l_uiStride = m_pDeclaration->GetStride();

	//On effectue une copie des vertex
	for( VertexPtrArrayConstIt l_it = m_points.begin(); l_it != m_points.end(); ++l_it )
	{
		BufferElementGroupSPtr l_pVertexSrc = (*l_it);
		BufferElementGroupSPtr l_pVertexDst = l_clone->AddPoint( 0, 0, 0 );
		memcpy( l_pVertexDst->ptr(), l_pVertexSrc->const_ptr(), l_uiStride );
	}

	for( uint32_t j = 0; j < GetFacesCount(); j++ )
	{
		FaceSPtr l_pFaceSrc = GetFace( j );
		l_clone->AddFace( l_pFaceSrc->GetVertexIndex( 0 ), l_pFaceSrc->GetVertexIndex( 1 ), l_pFaceSrc->GetVertexIndex( 2 ) );
	}

	return l_clone;
}

void Submesh :: ResetGpuBuffers()
{
	if( !m_pRenderer.expired() )
	{
		SubmeshRendererSPtr l_pRenderer = m_pRenderer.lock();
		l_pRenderer->Cleanup();
		l_pRenderer->CreateBuffers();

		if( l_pRenderer->IsInitialised() )
		{
			l_pRenderer->Initialise();
		}

		m_vertex = l_pRenderer->GetVertex();
		m_indices = l_pRenderer->GetIndices();
		m_matrices = l_pRenderer->GetMatrices();
	}
}

void Submesh :: Render()
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

void Submesh :: ComputeFacesFromPolygonVertex()
{
	if( m_points.size() )
	{
		BufferElementGroupSPtr l_v1 = m_points[0];
		BufferElementGroupSPtr l_v2 = m_points[1];
		BufferElementGroupSPtr l_v3 = m_points[2];

		FaceSPtr l_face = AddFace( 0, 1, 2 );
		Vertex::SetTexCoord( l_v1, 0.0, 0.0 );
		Vertex::SetTexCoord( l_v2, 0.0, 0.0 );
		Vertex::SetTexCoord( l_v3, 0.0, 0.0 );

		for( uint32_t i = 2; i < uint32_t( m_points.size() - 1 ); i++)
		{
			l_v2 = m_points[i];
			l_v3 = m_points[i + 1];

			FaceSPtr l_faceA = AddFace( 0, i, i + 1 );

			Vertex::SetTexCoord( l_v2, 0.0, 0.0 );
			Vertex::SetTexCoord( l_v3, 0.0, 0.0 );
		}
	}
}

void Submesh :: ComputeNormals( bool p_bReverted )
{
	if( !m_bHasNormals )
	{
		Point3r l_vec2m1;
		Point3r l_vec3m1;
		Point3r l_tex2m1;
		Point3r l_tex3m1;
		Point3r l_pt1;
		Point3r l_pt2;
		Point3r l_pt3;
		BufferElementGroupSPtr l_pVtx1;
		BufferElementGroupSPtr l_pVtx2;
		BufferElementGroupSPtr l_pVtx3;
		Point3r l_vFaceNormal;
		Point3r l_vFaceTangent;

//		std::vector< FaceAndAnglePtrArray > l_arrayFaces;
//		FaceAndAnglePtr	l_pFaceAndAngle;
//		FaceAndAnglePtr l_faa1;
//		FaceAndAnglePtr l_faa2;
//		FaceAndAnglePtr l_faa3;
//		uint32_t l_uiIndex = 0;
//		Point3r l_vec1m2;
//		Point3r l_vec1m3;
//		Point3r l_vec2m3;
//		Point3r l_vec3m2;
//		Point3r l_uv1;
//		Point3r l_uv2;
//		Point3r l_uv3;
//
//		//creation d'un vecteur de liste (chaque point contient la liste des faces auxquelles il appartient)
//		l_arrayFaces.resize(	m_points.size() );
//
//		//Pour chaque vertex, on stocke la liste des faces auxquelles il appartient
//		for( FacePtrArray::iterator l_it = m_arrayFaces.begin(); l_it != m_arrayFaces.end(); ++l_it )
//		{
//			FaceSPtr l_pFace = (*l_it);
//
//			l_pVtx1 = m_points[l_pFace->GetVertexIndex( 0 )];
//			l_pVtx2 = m_points[l_pFace->GetVertexIndex( 1 )];
//			l_pVtx3 = m_points[l_pFace->GetVertexIndex( 2 )];
//
//			Vertex::GetPosition( l_pVtx1, l_pt1 );
//			Vertex::GetPosition( l_pVtx2, l_pt2 );
//			Vertex::GetPosition( l_pVtx3, l_pt3 );
//
//			Vertex::GetTexCoord( l_pVtx1, l_uv1 );
//			Vertex::GetTexCoord( l_pVtx2, l_uv2 );
//			Vertex::GetTexCoord( l_pVtx3, l_uv3 );
//
//			l_vec1m2 = l_pt1 - l_pt2;
//			l_vec1m3 = l_pt1 - l_pt3;
//			l_vec2m1 = l_pt2 - l_pt1;
//			l_vec2m3 = l_pt2 - l_pt3;
//			l_vec3m1 = l_pt3 - l_pt1;
//			l_vec3m2 = l_pt3 - l_pt2;
//
//			l_tex2m1 = l_uv2 - l_uv1;
//			l_tex3m1 = l_uv3 - l_uv1;
//
//			l_vFaceNormal	= point::get_normalised( l_vec3m1 ^ l_vec2m1 );
//			l_vFaceTangent	= point::get_normalised( (l_vec3m1 * l_tex2m1[1]) - (l_vec2m1 * l_tex3m1[1]) );
//
//			l_faa1 = std::make_shared< FaceAndAngle >();
//			l_faa1->m_rAngle = real( std::abs( acos( point::cos_theta( l_vec3m1, l_vec2m1 ) ) ) );
//			l_faa1->m_ptNormal = l_vFaceNormal * l_faa1->m_rAngle;
//			l_faa1->m_ptTangent = l_vFaceTangent * l_faa1->m_rAngle;
//
//			l_faa2 = std::make_shared< FaceAndAngle >();
//			l_faa2->m_rAngle = real( std::abs( acos( point::cos_theta( l_vec1m2, l_vec3m2 ) ) ) );
//			l_faa2->m_ptNormal = l_vFaceNormal * l_faa2->m_rAngle;
//			l_faa2->m_ptTangent = l_vFaceTangent * l_faa2->m_rAngle;
//
//			l_faa3 = std::make_shared< FaceAndAngle >();
//			l_faa3->m_rAngle = real( std::abs( acos( point::cos_theta( l_vec1m3, l_vec2m3 ) ) ) );
//			l_faa3->m_ptNormal = l_vFaceNormal * l_faa3->m_rAngle;
//			l_faa3->m_ptTangent = l_vFaceTangent * l_faa3->m_rAngle;
//
//			l_arrayFaces[l_pFace->GetVertexIndex( 0 )].push_back( l_faa1 );
//			l_arrayFaces[l_pFace->GetVertexIndex( 1 )].push_back( l_faa2 );
//			l_arrayFaces[l_pFace->GetVertexIndex( 2 )].push_back( l_faa3 );
//		}
//
//		//On effectue la moyennes des normales
//		for( std::vector< FaceAndAnglePtrArray >::iterator l_itFaa = l_arrayFaces.begin(); l_itFaa != l_arrayFaces.end(); ++l_itFaa )
//		{
//			Point3r l_normal, l_tangent;
//
//			for( FaceAndAnglePtrArray::const_iterator l_it = l_itFaa->begin(); l_it != l_itFaa->end(); ++l_it )
//			{
//				l_pFaceAndAngle	 = (*l_it);
//				l_normal		+= l_pFaceAndAngle->m_ptNormal;
//				l_tangent		+= l_pFaceAndAngle->m_ptTangent;
//			}
//
//			point::normalise( l_normal );
//			point::normalise( l_tangent );
//			if( p_bReverted )
//			{
//				Vertex::SetNormal( m_points[l_uiIndex], -l_normal[0], -l_normal[1], -l_normal[2] );
//				Vertex::SetTangent( m_points[l_uiIndex], -l_tangent[0], -l_tangent[1], -l_tangent[2] );
//			}
//			else
//			{
//				Vertex::SetNormal( m_points[l_uiIndex], l_normal );
//				Vertex::SetTangent( m_points[l_uiIndex], l_tangent );
//			}
//
//			l_uiIndex++;
//		}
//
//		clear_container( l_arrayFaces );

		// First we flush normals and tangents
		for( VertexPtrArray::iterator l_it = m_points.begin(); l_it != m_points.end(); ++l_it )
		{
			l_pVtx1 = *l_it;
			Vertex::SetNormal( l_pVtx1, l_pt1 );
			Vertex::SetTangent( l_pVtx1, l_pt1 );
		}

		Coords3r l_coord;
		// Then we compute normals and tangents
		if( p_bReverted )
		{
			for( FacePtrArray::iterator l_it = m_arrayFaces.begin(); l_it != m_arrayFaces.end(); ++l_it )
			{
				FaceSPtr l_pFace = (*l_it);

				l_pVtx1 = m_points[l_pFace->GetVertexIndex( 0 )];
				l_pVtx2 = m_points[l_pFace->GetVertexIndex( 1 )];
				l_pVtx3 = m_points[l_pFace->GetVertexIndex( 2 )];

				l_vec2m1 = l_pt2 - l_pt1;
				l_vec3m1 = l_pt3 - l_pt1;

				l_tex2m1 = l_uv2 - l_uv1;
				l_tex3m1 = l_uv3 - l_uv1;

				l_vFaceNormal	= -(l_vec3m1 ^ l_vec2m1);
				l_vFaceTangent	= -((l_vec3m1 * l_tex2m1[1]) - (l_vec2m1 * l_tex3m1[1]));

				Vertex::GetNormal( l_pVtx1, l_coord ) += l_vFaceNormal );
				Vertex::GetNormal( l_pVtx2, l_coord ) += l_vFaceNormal );
				Vertex::GetNormal( l_pVtx3, l_coord ) += l_vFaceNormal );

				Vertex::GetTangent( l_pVtx1, l_coord ) += l_vFaceTangent );
				Vertex::GetTangent( l_pVtx2, l_coord ) += l_vFaceTangent );
				Vertex::GetTangent( l_pVtx3, l_coord ) += l_vFaceTangent );
			}
		}
		else
		{
			for( FacePtrArray::iterator l_it = m_arrayFaces.begin(); l_it != m_arrayFaces.end(); ++l_it )
			{
				FaceSPtr l_pFace = (*l_it);

				l_pVtx1 = m_points[l_pFace->GetVertexIndex( 0 )];
				l_pVtx2 = m_points[l_pFace->GetVertexIndex( 1 )];
				l_pVtx3 = m_points[l_pFace->GetVertexIndex( 2 )];

				l_vec2m1 = l_pt2 - l_pt1;
				l_vec3m1 = l_pt3 - l_pt1;

				l_tex2m1 = l_uv2 - l_uv1;
				l_tex3m1 = l_uv3 - l_uv1;

				l_vFaceNormal	= l_vec3m1 ^ l_vec2m1;
				l_vFaceTangent	= (l_vec3m1 * l_tex2m1[1]) - (l_vec2m1 * l_tex3m1[1]);

				Vertex::GetNormal( l_pVtx1, l_coord ) += l_vFaceNormal );
				Vertex::GetNormal( l_pVtx2, l_coord ) += l_vFaceNormal );
				Vertex::GetNormal( l_pVtx3, l_coord ) += l_vFaceNormal );

				Vertex::GetTangent( l_pVtx1, l_coord ) += l_vFaceTangent );
				Vertex::GetTangent( l_pVtx2, l_coord ) += l_vFaceTangent );
				Vertex::GetTangent( l_pVtx3, l_coord ) += l_vFaceTangent );
			}
		}

        m_bHasNormals = true;
    }
}
		
void Submesh :: ComputeNormals( FaceSPtr p_pFace )
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

void Submesh :: ComputeTangents( FaceSPtr p_pFace )
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

	l_vFaceTangent = point::get_normalised( (l_vec2m1 * l_tex3m1[1]) - (l_vec3m1 * l_tex2m1[1]) );

	Vertex::SetTangent( l_pVtx1, l_vFaceTangent );
	Vertex::SetTangent( l_pVtx2, l_vFaceTangent );
	Vertex::SetTangent( l_pVtx3, l_vFaceTangent );
}

void Submesh :: ComputeTangentsFromNormals()
{
	Point3rArray l_arrayTangents( m_points.size() );

	// Pour chaque vertex, on stocke la somme des tangentes qui peuvent lui être affectées
	for( FacePtrArray::iterator l_it = m_arrayFaces.begin(); l_it != m_arrayFaces.end(); ++l_it )
	{
		FaceSPtr l_pFace = (*l_it);

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

		if( l_rDirCorrection )
		{
			l_rDirCorrection = 1 / l_rDirCorrection;

			// Calculates the face tangent to the current triangle.
			l_vFaceTangent[0] = l_rDirCorrection * ((l_vec2m1[0] * l_tex3m1[1]) + (l_vec3m1[0] * -l_tex2m1[1]));
			l_vFaceTangent[1] = l_rDirCorrection * ((l_vec2m1[1] * l_tex3m1[1]) + (l_vec3m1[1] * -l_tex2m1[1]));
			l_vFaceTangent[2] = l_rDirCorrection * ((l_vec2m1[2] * l_tex3m1[1]) + (l_vec3m1[2] * -l_tex2m1[1]));
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

void Submesh :: ComputeTangentsFromBitangents()
{
	std::for_each( m_points.begin(), m_points.end(), [&]( BufferElementGroupSPtr p_pVertex )
	{
		Point3r l_normal;
		Point3r l_bitangent;
		Vertex::GetNormal(	p_pVertex, l_normal );
		Vertex::GetBitangent( p_pVertex, l_bitangent );
		Point3r l_tangent = l_normal ^ l_bitangent;
		Vertex::SetTangent(	p_pVertex, l_tangent	);
	} );
}

void Submesh :: ComputeBitangents()
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

void Submesh :: SortFaces( Point3r const & p_ptCameraPosition )
{
	if( m_ptCameraPosition != p_ptCameraPosition )
	{
		IndexBufferSPtr l_pIndices = m_indices.lock();
		VertexBufferSPtr l_pVertices = m_vertex.lock();
		GeometryBuffersSPtr l_pGeometryBuffers = GetRenderer()->GetGeometryBuffers();

		if( l_pGeometryBuffers && l_pGeometryBuffers->Bind() )
		{
			m_ptCameraPosition = p_ptCameraPosition;
			uint32_t l_uiIdxSize = l_pIndices->GetSize();
			uint32_t * l_pIdx = l_pIndices->Lock( 0, l_uiIdxSize, eLOCK_FLAG_WRITE_ONLY | eLOCK_FLAG_READ_ONLY );

			if( l_pIdx )
			{
				struct stFACE_DISTANCE
				{
					uint32_t m_index[3];
					double m_distance;
				};

				uint32_t l_uiStride = l_pVertices->GetDeclaration().GetStride();
				uint8_t * l_pVtx = l_pVertices->data();
				DECLARE_VECTOR( stFACE_DISTANCE, Face );
				FaceArray l_arraySorted;
				l_arraySorted.reserve( l_uiIdxSize / 3 );

				if( l_pVtx )
				{
					for( uint32_t * l_it = l_pIdx + 0; l_it < l_pIdx + l_uiIdxSize; l_it += 3 )
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

					for( FaceArrayConstIt l_it = l_arraySorted.begin(); l_it != l_arraySorted.end(); ++l_it )
					{
						*l_pIdx = l_it->m_index[0];l_pIdx++;
						*l_pIdx = l_it->m_index[1];l_pIdx++;
						*l_pIdx = l_it->m_index[2];l_pIdx++;
					}
				}

				l_pIndices->Unlock();
			}

			l_pGeometryBuffers->Unbind();
		}
	}
}

void Submesh :: Ref()
{
	m_uiInstanceCount++;

	if( m_uiInstanceCount > 1 )
	{
		m_uiProgramFlags |= ePROGRAM_FLAG_INSTANCIATION;
	}
}

void Submesh :: UnRef()
{
	if( m_uiInstanceCount )
	{
		m_uiInstanceCount--;
	}

	if( m_uiInstanceCount <= 1 )
	{
		m_uiProgramFlags &= ~ePROGRAM_FLAG_INSTANCIATION;
	}
}

void Submesh :: DoGenerateVertexBuffer()
{
	if( !m_vertex.expired() )
	{
		uint8_t *				l_pBuffer;
		VertexBufferSPtr		l_pVertexBuffer	= m_vertex.lock();
		uint32_t				l_uiStride		= m_pDeclaration->GetStride();
		uint32_t				l_uiSize		= uint32_t( m_points.size() ) * l_uiStride;

		if( l_uiSize )
		{
			if( l_pVertexBuffer->GetSize() != l_uiSize )
			{
				l_pVertexBuffer->Resize( l_uiSize );
			}

			l_pBuffer = l_pVertexBuffer->data();

			for( BytePtrListIt l_it = m_pointsData.begin(); l_it != m_pointsData.end(); ++l_it )
			{
				std::memcpy( l_pBuffer, &(*l_it)[0], l_it->size() );
				l_pBuffer += l_it->size();
			}

			l_pBuffer = l_pVertexBuffer->data();

			for( VertexPtrArrayIt l_it = m_points.begin(); l_it != m_points.end(); ++l_it )
			{
				(*l_it)->LinkCoords( l_pBuffer );
				l_pBuffer += l_uiStride;
			}

			m_pointsData.clear();
			m_points.clear();
		}
	}
}

void Submesh :: DoGenerateIndexBuffer()
{
	if( !m_vertex.expired() && !m_indices.expired() )
	{
		FaceSPtr		l_pFace;
		uint32_t		l_uiIndex		= 0;
		IndexBufferSPtr	l_pIndexBuffer	= m_indices.lock();
		uint32_t		l_uiSize		= uint32_t( m_arrayFaces.size() * 3 );

		if( l_uiSize )
		{
			if( l_pIndexBuffer->GetSize() != l_uiSize )
			{
				l_pIndexBuffer->Resize( l_uiSize );
			}

			for( FacePtrArray::iterator l_it = m_arrayFaces.begin(); l_it != m_arrayFaces.end(); ++l_it )
			{
				if( *l_it )
				{
					l_pFace = *l_it;
					l_pIndexBuffer->SetElement( l_uiIndex++, l_pFace->GetVertexIndex( 0 ) );
					l_pIndexBuffer->SetElement( l_uiIndex++, l_pFace->GetVertexIndex( 1 ) );
					l_pIndexBuffer->SetElement( l_uiIndex++, l_pFace->GetVertexIndex( 2 ) );
				}
			}

			m_arrayFaces.clear();
		}
	}
}

void Submesh :: DoGenerateMatrixBuffer()
{
	if( m_uiInstanceCount )
	{
		MatrixBufferSPtr l_pMatrixBuffer = m_matrices.lock();
		uint32_t l_uiSize = m_uiInstanceCount * 16;

		if( l_pMatrixBuffer && l_pMatrixBuffer->GetSize() != l_uiSize )
		{
			l_pMatrixBuffer->Resize( l_uiSize );
		}
	}
}

void Submesh :: DoUpdateDeclaration()
{
	if( !m_pDeclaration )
	{
		std::vector< BufferElementDeclaration >	l_vertexDeclarationElements;
		l_vertexDeclarationElements.push_back( BufferElementDeclaration( 0, eELEMENT_USAGE_POSITION,	eELEMENT_TYPE_3FLOATS ) );
		l_vertexDeclarationElements.push_back( BufferElementDeclaration( 0, eELEMENT_USAGE_NORMAL,		eELEMENT_TYPE_3FLOATS ) );
		l_vertexDeclarationElements.push_back( BufferElementDeclaration( 0, eELEMENT_USAGE_TANGENT,		eELEMENT_TYPE_3FLOATS ) );
		l_vertexDeclarationElements.push_back( BufferElementDeclaration( 0, eELEMENT_USAGE_BITANGENT,	eELEMENT_TYPE_3FLOATS ) );
		l_vertexDeclarationElements.push_back( BufferElementDeclaration( 0, eELEMENT_USAGE_TEXCOORDS0,	eELEMENT_TYPE_3FLOATS ) );

		if( GetSkeleton() )
		{
			l_vertexDeclarationElements.push_back( BufferElementDeclaration( 0, eELEMENT_USAGE_BONE_IDS,		eELEMENT_TYPE_4INTS ) );
			l_vertexDeclarationElements.push_back( BufferElementDeclaration( 0, eELEMENT_USAGE_BONE_WEIGHTS,	eELEMENT_TYPE_4FLOATS ) );
		}

		m_pDeclaration = std::make_shared< BufferDeclaration >( &l_vertexDeclarationElements[0], uint32_t( l_vertexDeclarationElements.size() ) );

		if( !m_pRenderer.expired() )
		{
			SubmeshRendererSPtr l_pRenderer = m_pRenderer.lock();
			l_pRenderer->CreateBuffers();
			m_vertex = l_pRenderer->GetVertex();
			m_indices = l_pRenderer->GetIndices();
			m_matrices = l_pRenderer->GetMatrices();
		}
	}
}

//*************************************************************************************************
