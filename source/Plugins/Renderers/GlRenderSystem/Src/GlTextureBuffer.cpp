#include "GlTextureBuffer.hpp"
#include "GlRenderSystem.hpp"
#include "OpenGl.hpp"

using namespace Castor3D;
using namespace Castor;

namespace GlRender
{
	GlTextureBufferObject::GlTextureBufferObject( OpenGl & p_gl, HardwareBufferPtr p_pBuffer )
		:	GlBuffer< uint8_t >( p_gl, eGL_BUFFER_TARGET_TEXTURE, p_pBuffer )
	{
	}

	GlTextureBufferObject::~GlTextureBufferObject()
	{
	}

	bool GlTextureBufferObject::Create()
	{
		return GlBuffer< uint8_t >::DoCreate();
	}

	void GlTextureBufferObject::Destroy()
	{
		return GlBuffer< uint8_t >::DoDestroy();
	}

	bool GlTextureBufferObject::Initialise( Castor3D::eBUFFER_ACCESS_TYPE p_type, eBUFFER_ACCESS_NATURE p_eNature, ShaderProgramBaseSPtr p_pProgram )
	{
		bool l_return = GlBuffer< uint8_t >::DoInitialise( p_type, p_eNature );
		//if( l_return )
		//{
		//	m_pBuffer->Clear();
		//}
		return l_return;
	}

	void GlTextureBufferObject::Cleanup()
	{
		GlBuffer< uint8_t >::DoCleanup();
	}

	bool GlTextureBufferObject::Bind()
	{
		HardwareBufferPtr l_pBuffer = GetCpuBuffer();
		bool l_return = l_pBuffer && l_pBuffer->IsAssigned();

		if ( l_return )
		{
			l_return = GlBuffer< uint8_t >::DoBind();
		}

		return l_return;
	}

	void GlTextureBufferObject::Unbind()
	{
		GlBuffer< uint8_t >::DoUnbind();
	}

	uint8_t * GlTextureBufferObject::Lock( uint32_t p_uiOffset, uint32_t p_uiCount, uint32_t p_uiFlags )
	{
		uint8_t * l_pReturn = NULL;
		HardwareBufferPtr l_pBuffer = GetCpuBuffer();

		if ( l_pBuffer && l_pBuffer->IsAssigned() )
		{
			l_pReturn = GlBuffer< uint8_t >::DoLock( p_uiOffset, p_uiCount, p_uiFlags );
		}

		return l_pReturn;
	}

	void GlTextureBufferObject::Unlock()
	{
		HardwareBufferPtr l_pBuffer = GetCpuBuffer();

		if ( l_pBuffer && l_pBuffer->IsAssigned() )
		{
			GlBuffer< uint8_t >::DoUnlock();
		}
	}
}
