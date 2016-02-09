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

	void GlMatrixBufferObject::Cleanup()
	{
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

		return l_return;
	}

	void GlMatrixBufferObject::Unbind()
	{
		HardwareBufferPtr l_pBuffer = GetCpuBuffer();

		if ( l_pBuffer && l_pBuffer->IsAssigned() )
		{
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
}
