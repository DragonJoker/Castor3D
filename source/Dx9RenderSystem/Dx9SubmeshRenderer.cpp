#include "Dx9RenderSystem/PrecompiledHeader.hpp"

#include "Dx9RenderSystem/Dx9SubmeshRenderer.hpp"
#include "Dx9RenderSystem/Dx9RenderSystem.hpp"
#include "Dx9RenderSystem/Dx9Buffer.hpp"
#include "Dx9RenderSystem/Dx9ShaderProgram.hpp"

using namespace Castor3D;

Dx9SubmeshRenderer :: Dx9SubmeshRenderer()
	:	SubmeshRenderer()
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
	m_vertex->Initialise( eBUFFER_MODE_STATIC);

	for (int i = 0 ; i < ePRIMITIVE_TYPE_COUNT ; i++)
	{
		m_indices[i]->Initialise( eBUFFER_MODE_STATIC);
	}
}

void Dx9SubmeshRenderer :: _drawBuffers( const Pass & p_pass)
{
	Pipeline::ApplyMatrices();
	m_vertex->Activate();
	m_indices[m_eLastDrawType]->Activate();
	CheckDxError( Dx9RenderSystem::GetDevice()->DrawIndexedPrimitive( D3dEnum::Get( m_eLastDrawType), 0, 0, m_vertex->size() / Vertex::Size, 0, m_indices[m_eLastDrawType]->size() / 3), cuT( "Dx9SubmeshRenderer :: Render - DrawIndexedPrimitive"), false);
	m_indices[m_eLastDrawType]->Deactivate();
	m_vertex->Deactivate();
}