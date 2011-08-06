#include "GlRenderSystem/PrecompiledHeader.hpp"

#include "GlRenderSystem/GlSubmeshRenderer.hpp"
#include "GlRenderSystem/GlRenderSystem.hpp"
#include "GlRenderSystem/GlBuffer.hpp"
#include "GlRenderSystem/GlContext.hpp"
#include "GlRenderSystem/GlShaderProgram.hpp"
#include "GlRenderSystem/OpenGl.hpp"

using namespace Castor3D;

GlSubmeshRenderer :: GlSubmeshRenderer()
	:	SubmeshRenderer		()
	,	m_eDrawType			( ePRIMITIVE_TYPE( -1))
	,	m_eNormalsMode		( eNORMALS_MODE( -1))
	,	m_pfnDrawBuffers	( nullptr)
{
	if (GlRenderSystem::GetOpenGlMajor() < 3)
	{
		m_pfnDrawBuffers = & GlSubmeshRenderer::_drawBuffersGl2;
	}
	else
	{
		m_pfnDrawBuffers = & GlSubmeshRenderer::_drawBuffersGl3;
	}

	_createVbo();
}

GlSubmeshRenderer :: ~GlSubmeshRenderer()
{
}

void GlSubmeshRenderer :: Cleanup()
{
	CASTOR_TRACK;
	m_vao.Cleanup();
	SubmeshRenderer::Cleanup();
	_createVbo();
}

void GlSubmeshRenderer :: Initialise()
{
	CASTOR_TRACK;
	if (GlRenderSystem::GetSingletonPtr()->UseVertexBufferObjects())
	{
		m_vertex->Initialise( eBUFFER_MODE_STATIC);

		for (int i = 0 ; i < ePRIMITIVE_TYPE_COUNT ; i++)
		{
			m_indices[i]->Initialise( eBUFFER_MODE_STATIC);
		}
	}

	if ( ! m_target->GetMaterial())
	{
		m_target->SetMaterial( Root::GetSingleton()->GetMaterialManager()->GetDefaultMaterial());
	}
}

void GlSubmeshRenderer :: _createVAOs( ShaderProgramPtr p_pProgram)
{
	CASTOR_TRACK;
	if (m_eDrawType != m_eLastDrawType || m_eNormalsMode != m_eLastNormalsMode || m_vao.GetIndex() == GL_INVALID_INDEX)
	{
		m_eDrawType = m_eLastDrawType;
		m_eNormalsMode = m_eLastNormalsMode;

		static_pointer_cast<GlVertexBufferObject>( m_vertex)->CleanupBuffer();
		static_pointer_cast<GlIndexBufferObject>( m_indices[m_eDrawType])->CleanupBuffer();

		static_pointer_cast<GlVertexBufferObject>( m_vertex)->SetShaderProgram( p_pProgram);
		m_indices[m_eDrawType]->Initialise( eBUFFER_MODE_STATIC);

		m_vao.Cleanup();
		m_vao.Initialise();
		m_vao.Activate();
		m_vertex->Activate();
		m_indices[m_eLastDrawType]->Activate();
		m_vao.Deactivate();
	}

	CASTOR_ASSERT( m_vao.GetIndex() != GL_INVALID_INDEX);
}

void GlSubmeshRenderer :: _drawBuffers( const Pass & p_pass)
{
	CASTOR_TRACK;

	(this->*m_pfnDrawBuffers)( p_pass);
}

void GlSubmeshRenderer :: _drawBuffersGl2( const Pass & p_pass)
{
	CASTOR_TRACK;
	GlShaderProgramPtr l_pProgram = p_pass.GetShader<GlShaderProgram>();
	bool l_bUseShader = (l_pProgram) && l_pProgram->IsLinked();

	if (l_bUseShader)
	{
		static_pointer_cast<GlVertexBufferObject>( m_vertex)->SetShaderProgram( l_pProgram);
		Pipeline::ApplyMatrices();
	}

	GLenum l_eMode = GlEnum::Get( m_eLastDrawType);

	m_vertex->Activate();
	m_indices[m_eLastDrawType]->Activate();
	OpenGl::DrawElements( l_eMode, m_indices[m_eLastDrawType]->size(), GL_UNSIGNED_INT, 0);
	m_indices[m_eLastDrawType]->Deactivate();
	m_vertex->Deactivate();
}

void GlSubmeshRenderer :: _drawBuffersGl3( const Pass & p_pass)
{
	CASTOR_TRACK;
	ShaderProgramPtr l_pProgram = p_pass.GetShader<ShaderProgramBase>();
	bool l_bUseShader = l_pProgram && l_pProgram->IsLinked();

	if (l_bUseShader)
	{
		switch (l_pProgram->GetType())
		{
		case ShaderProgramBase::eSHADER_LANGUAGE_GLSL:
			l_bUseShader = OpenGl::IsProgram( p_pass.GetShader<GlShaderProgram>()->GetProgramObject());
			break;
		}
	}

	if (l_bUseShader)
	{
		Pipeline::ApplyMatrices();

		GLenum l_eMode = GlEnum::Get( m_eLastDrawType);
		_createVAOs( l_pProgram);
		m_vao.Activate();
		OpenGl::DrawElements( l_eMode, m_indices[m_eLastDrawType]->size(), GL_UNSIGNED_INT, 0);
		m_vao.Deactivate();
	}
}
