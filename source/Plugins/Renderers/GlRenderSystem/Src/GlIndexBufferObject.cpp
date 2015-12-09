#include "GlIndexBufferObject.hpp"

using namespace Castor3D;
using namespace Castor;

namespace GlRender
{
	GlIndexBufferObject::GlIndexBufferObject( GlRenderSystem & p_renderSystem, OpenGl & p_gl, HardwareBufferPtr p_buffer )
		: GlBuffer< uint32_t >( p_renderSystem, p_gl, eGL_BUFFER_TARGET_ELEMENT_ARRAY, p_buffer )
	{
	}

	GlIndexBufferObject::~GlIndexBufferObject()
	{
	}

	bool GlIndexBufferObject::Create()
	{
		return GlBuffer< uint32_t >::DoCreate();
	}

	void GlIndexBufferObject::Destroy()
	{
		GlBuffer< uint32_t >::DoDestroy();
	}

	bool GlIndexBufferObject::Initialise( eBUFFER_ACCESS_TYPE p_type, eBUFFER_ACCESS_NATURE p_nature, Castor3D::ShaderProgramBaseSPtr p_program )
	{
		bool l_return = GlBuffer< uint32_t >::DoInitialise( p_type, p_nature );
		//if( l_return )
		//{
		//	m_pBuffer->Clear();
		//}
		return l_return;
	}

	void GlIndexBufferObject::Cleanup()
	{
		GlBuffer< uint32_t >::DoCleanup();
	}

	bool GlIndexBufferObject::Bind()
	{
		HardwareBufferPtr l_pBuffer = GetCpuBuffer();
		bool l_return = l_pBuffer && l_pBuffer->IsAssigned();

		if ( l_return )
		{
			l_return = GlBuffer< uint32_t >::DoBind();
		}

		return l_return;
	}

	void GlIndexBufferObject::Unbind()
	{
		GlBuffer< uint32_t >::DoUnbind();
	}

	uint32_t * GlIndexBufferObject::Lock( uint32_t p_offset, uint32_t p_count, uint32_t p_flags )
	{
		uint32_t * l_return = NULL;
		HardwareBufferPtr l_pBuffer = GetCpuBuffer();

		if ( l_pBuffer && l_pBuffer->IsAssigned() )
		{
			l_return = GlBuffer< uint32_t >::DoLock( p_offset, p_count, p_flags );
		}

		return l_return;
	}

	void GlIndexBufferObject::Unlock()
	{
		HardwareBufferPtr l_pBuffer = GetCpuBuffer();

		if ( l_pBuffer && l_pBuffer->IsAssigned() )
		{
			GlBuffer< uint32_t >::DoUnlock();
		}
	}
}
