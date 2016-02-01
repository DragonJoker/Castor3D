#include "GlMatrixBufferObject.hpp"
#include "GlAttribute.hpp"
#include "GlRenderSystem.hpp"
#include "GlShaderProgram.hpp"
#include "OpenGl.hpp"

using namespace Castor3D;
using namespace Castor;

namespace GlRender
{
	GlMatrixBufferObject::GlMatrixBufferObject( GlRenderSystem & p_renderSystem, OpenGl & p_gl, HardwareBufferPtr p_buffer )
		: GlBuffer< real >( p_renderSystem, p_gl, eGL_BUFFER_TARGET_ARRAY, p_buffer )
		, m_attribute( std::make_shared< GlAttribute< real, 16 > >( p_gl, &p_renderSystem, cuT( "transform" ) ) )
	{
	}

	GlMatrixBufferObject::~GlMatrixBufferObject()
	{
	}

	bool GlMatrixBufferObject::Create()
	{
		return GlBuffer< real >::DoCreate();
	}

	void GlMatrixBufferObject::Destroy()
	{
		GlBuffer< real >::DoDestroy();
	}

	bool GlMatrixBufferObject::Initialise( eBUFFER_ACCESS_TYPE p_type, eBUFFER_ACCESS_NATURE p_nature )
	{
		return GlBuffer< real >::DoInitialise( p_type, p_nature );
	}

	bool GlMatrixBufferObject::AttachTo( Castor3D::ShaderProgramBaseSPtr p_program )
	{
		bool l_return = true;
		GlShaderProgramSPtr l_pNewProgram = std::static_pointer_cast< GlShaderProgram >( p_program );
		GlShaderProgramSPtr l_pOldProgram = m_program.lock();

		if ( l_pOldProgram != l_pNewProgram )
		{
			m_attribute->SetShader( l_pNewProgram );
			m_program = l_pNewProgram;

			if ( l_pNewProgram )
			{
				l_return = DoAttributeInitialise();
			}
		}

		return l_return;
	}

	void GlMatrixBufferObject::Cleanup()
	{
		DoAttributeCleanup();
		GlBuffer< real >::DoCleanup();
	}

	bool GlMatrixBufferObject::Bind( bool p_instantiated )
	{
		HardwareBufferPtr l_pBuffer = GetCpuBuffer();
		bool l_return = l_pBuffer && l_pBuffer->IsAssigned();

		if ( l_return )
		{
			l_return = GlBuffer< real >::DoBind();
		}

		if ( l_return )
		{
			l_return = DoAttributeBind( p_instantiated );
		}

		return l_return;
	}

	void GlMatrixBufferObject::Unbind()
	{
		HardwareBufferPtr l_pBuffer = GetCpuBuffer();

		if ( l_pBuffer && l_pBuffer->IsAssigned() )
		{
			DoAttributeUnbind();
			GlBuffer< real >::DoUnbind();
		}
	}

	real * GlMatrixBufferObject::Lock( uint32_t p_offset, uint32_t p_count, uint32_t p_flags )
	{
		real * l_return = NULL;
		HardwareBufferPtr l_pBuffer = GetCpuBuffer();

		if ( l_pBuffer && l_pBuffer->IsAssigned() )
		{
			l_return = GlBuffer< real >::DoLock( p_offset, p_count, p_flags );
		}

		return l_return;
	}

	void GlMatrixBufferObject::Unlock()
	{
		HardwareBufferPtr l_pBuffer = GetCpuBuffer();

		if ( l_pBuffer && l_pBuffer->IsAssigned() )
		{
			GlBuffer< real >::DoUnlock();
		}
	}

	void GlMatrixBufferObject::DoAttributeCleanup()
	{
		m_attribute->Cleanup();
	}

	bool GlMatrixBufferObject::DoAttributeInitialise()
	{
		bool l_return = m_attribute->Initialise();
		return l_return;
	}

	bool GlMatrixBufferObject::DoAttributeBind( bool p_instanced )
	{
		bool l_return = true;
		uint32_t l_uiLocation = m_attribute->GetLocation();

		if ( l_uiLocation != eGL_INVALID_INDEX )
		{
			for ( int i = 0; i < 4 && l_return; ++i )
			{
				l_return = GetOpenGl().EnableVertexAttribArray( l_uiLocation + i );
				l_return &= GetOpenGl().VertexAttribPointer( l_uiLocation + i, 4, eGL_TYPE_REAL, false, 16 * sizeof( real ), BUFFER_OFFSET( i * 4 * sizeof( real ) ) );
				l_return &= GetOpenGl().VertexAttribDivisor( l_uiLocation + i, p_instanced ? 1 : 0 );
			}
		}

		return l_return;
	}

	void GlMatrixBufferObject::DoAttributeUnbind()
	{
		uint32_t l_uiLocation = m_attribute->GetLocation();

		if ( l_uiLocation != eGL_INVALID_INDEX )
		{
			for ( int i = 0; i < 4; ++i )
			{
				GetOpenGl().DisableVertexAttribArray( l_uiLocation + i );
			}
		}
	}
}
