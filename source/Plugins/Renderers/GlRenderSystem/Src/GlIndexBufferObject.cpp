#include "GlIndexBufferObject.hpp"

using namespace Castor3D;
using namespace Castor;

namespace GlRender
{
	GlIndexBufferObject::GlIndexBufferObject( GlRenderSystem & p_renderSystem, OpenGl & p_gl, HardwareBufferPtr p_pBuffer )
		: GlBuffer< uint32_t >( p_renderSystem, p_gl, eGL_BUFFER_TARGET_ELEMENT_ARRAY, p_pBuffer )
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

	bool GlIndexBufferObject::Initialise( eBUFFER_ACCESS_TYPE p_type, eBUFFER_ACCESS_NATURE p_eNature, Castor3D::ShaderProgramBaseSPtr p_pProgram )
	{
		bool l_return = GlBuffer< uint32_t >::DoInitialise( p_type, p_eNature );
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

	uint32_t * GlIndexBufferObject::Lock( uint32_t p_uiOffset, uint32_t p_uiCount, uint32_t p_uiFlags )
	{
		uint32_t * l_return = NULL;
		HardwareBufferPtr l_pBuffer = GetCpuBuffer();

		if ( l_pBuffer && l_pBuffer->IsAssigned() )
		{
			l_return = GlBuffer< uint32_t >::DoLock( p_uiOffset, p_uiCount, p_uiFlags );
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
