#include "Dx9RenderSystem/PrecompiledHeader.h"

#include "Dx9RenderSystem/Dx9SubmeshRenderer.h"
#include "Dx9RenderSystem/Dx9RenderSystem.h"
#include "Dx9RenderSystem/Dx9Buffer.h"
#include "Dx9RenderSystem/Dx9ShaderProgram.h"

using namespace Castor3D;

Dx9SubmeshRenderer :: Dx9SubmeshRenderer( SceneManager * p_pSceneManager)
	:	SubmeshRenderer( p_pSceneManager)
{
	_createVbo();
}

Dx9SubmeshRenderer :: ~Dx9SubmeshRenderer()
{
}

void Dx9SubmeshRenderer :: Cleanup()
{
	SubmeshRenderer::Cleanup();
	_createVbo();
}
/**/
void Dx9SubmeshRenderer :: Initialise()
{
	m_vertex->Initialise( eBufferStatic);

	for (int i = 0 ; i < eNbDrawTypes ; i++)
	{
		m_indices[i]->Initialise( eBufferStatic);
	}
}

void Dx9SubmeshRenderer :: _drawBuffers( const Pass & p_pass)
{
	Pipeline::ApplyMatrices();
	m_vertex->Activate();
	m_indices[m_eLastDrawType]->Activate();
	CheckDxError( Dx9RenderSystem::GetDevice()->DrawIndexedPrimitive( D3dEnum::Get( m_eLastDrawType), 0, 0, m_vertex->GetFilled() / Vertex::Size, 0, m_indices[m_eLastDrawType]->GetFilled() / 3), CU_T( "Dx9SubmeshRenderer :: Render - DrawIndexedPrimitive"), false);
	m_indices[m_eLastDrawType]->Deactivate();
	m_vertex->Deactivate();
}
/**/
/*
struct TVertex
{
	float x, y, z;
	DWORD color;

	enum FVF
	{
		FVF_Flags = D3DFVF_XYZ | D3DFVF_DIFFUSE
	};
};

TVertex g_cubeVertices_indexed[] =
{
	{-1.0f, 1.0f,-1.0f,  D3DCOLOR_COLORVALUE( 1.0, 0.0, 0.0, 1.0 ) }, // 0
	{ 1.0f, 1.0f,-1.0f,  D3DCOLOR_COLORVALUE( 0.0, 1.0, 0.0, 1.0 ) }, // 1
	{-1.0f,-1.0f,-1.0f,  D3DCOLOR_COLORVALUE( 0.0, 0.0, 1.0, 1.0 ) }, // 2
	{ 1.0f,-1.0f,-1.0f,  D3DCOLOR_COLORVALUE( 1.0, 1.0, 0.0, 1.0 ) }, // 3
	{-1.0f, 1.0f, 1.0f,  D3DCOLOR_COLORVALUE( 1.0, 0.0, 1.0, 1.0 ) }, // 4
	{-1.0f,-1.0f, 1.0f,  D3DCOLOR_COLORVALUE( 0.0, 1.0, 1.0, 1.0 ) }, // 5
	{ 1.0f, 1.0f, 1.0f,  D3DCOLOR_COLORVALUE( 1.0, 1.0, 1.0, 1.0 ) }, // 6
	{ 1.0f,-1.0f, 1.0f,  D3DCOLOR_COLORVALUE( 1.0, 0.0, 0.0, 1.0 ) }  // 7
};

WORD g_cubeIndices[] =
{
	0, 3, 2,
	0, 1, 3, // Quad 0
	4, 5, 7,
	4, 7, 6, // Quad 1
	0, 4, 6,
	0, 6, 1, // Quad 2
	2, 7, 5,
	2, 3, 7, // Quad 3
	1, 7, 3,
	1, 6, 7, // Quad 4
	0, 2, 5,
	0, 5, 4  // Quad 5
};

LPDIRECT3DVERTEXBUFFER9 g_pVertexBuffer_Indexed = NULL;
LPDIRECT3DINDEXBUFFER9  g_pIndexBuffer = NULL;
LPDIRECT3DVERTEXDECLARATION9 g_pVertexDeclaration;

void Dx9SubmeshRenderer :: Initialise()
{
	D3DVERTEXELEMENT9 l_dx9Declaration[3] =
	{
		{ 0, 0, D3DDECLTYPE_FLOAT3, D3DDECLMETHOD_DEFAULT, D3DDECLUSAGE_POSITION, 0 },
		{ 0, 12, D3DDECLTYPE_D3DCOLOR, D3DDECLMETHOD_DEFAULT, D3DDECLUSAGE_COLOR, 0 },
		D3DDECL_END()
	};

	CheckDxError( Dx9RenderSystem::GetDevice()->CreateVertexDeclaration( l_dx9Declaration, & g_pVertexDeclaration), "Dx9SubmeshRenderer :: Initialise - CreateVertexDeclaration", false);

	CheckDxError( Dx9RenderSystem::GetDevice()->CreateVertexBuffer( 8 * sizeof( TVertex), D3DUSAGE_WRITEONLY, 0, D3DPOOL_DEFAULT, & g_pVertexBuffer_Indexed, NULL), "Dx9SubmeshRenderer :: Initialise - CreateVertexBuffer", false);
	void * pVertices = NULL;

	CheckDxError( g_pVertexBuffer_Indexed->Lock( 0, sizeof( g_cubeVertices_indexed), (void **) & pVertices, 0), "Dx9SubmeshRenderer :: Initialise - g_pVertexBuffer_Indexed Lock", false);
	memcpy( pVertices, g_cubeVertices_indexed, sizeof( g_cubeVertices_indexed));
	CheckDxError( g_pVertexBuffer_Indexed->Unlock(), "Dx9SubmeshRenderer :: Initialise - g_pVertexBuffer_Indexed Unlock", false);

	//
	// Create an index buffer to use with our indexed vertex buffer...
	//

	CheckDxError( Dx9RenderSystem::GetDevice()->CreateIndexBuffer( sizeof( g_cubeIndices), D3DUSAGE_WRITEONLY, D3DFMT_INDEX16, D3DPOOL_DEFAULT, & g_pIndexBuffer, NULL), "Dx9SubmeshRenderer :: Initialise - CreateIndexBuffer", false);
	WORD * pIndices = NULL;

	CheckDxError( g_pIndexBuffer->Lock( 0, sizeof( g_cubeIndices), (void **) & pIndices, 0 ), "Dx9SubmeshRenderer :: Initialise - g_pIndexBuffer Lock", false);
	memcpy( pIndices, g_cubeIndices, sizeof( g_cubeIndices));
	CheckDxError( g_pIndexBuffer->Unlock(), "Dx9SubmeshRenderer :: Initialise - g_pIndexBuffer Unlock", false);
}

void Dx9SubmeshRenderer :: _drawBuffers( const Pass & p_pass)
{
	Pipeline::ApplyMatrices();
	Dx9RenderSystem::GetDevice()->SetStreamSource( 0, g_pVertexBuffer_Indexed, 0, sizeof( TVertex));
	Dx9RenderSystem::GetDevice()->SetIndices( g_pIndexBuffer);
	Dx9RenderSystem::GetDevice()->SetVertexDeclaration( g_pVertexDeclaration);
	Dx9RenderSystem::GetDevice()->DrawIndexedPrimitive( D3DPT_TRIANGLELIST, 0, 0, 8,  0, 12);
}
/**/