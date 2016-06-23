#include "Mesh/GlVertexBuffer.hpp"

#include "Common/OpenGl.hpp"
#include "Mesh/GlAttribute.hpp"
#include "Render/GlRenderSystem.hpp"
#include "Shader/GlShaderProgram.hpp"

using namespace Castor3D;
using namespace Castor;

namespace GlRender
{
	GlVertexBufferObject::GlVertexBufferObject( GlRenderSystem & p_renderSystem, OpenGl & p_gl, HardwareBufferPtr p_buffer )
		: GlBuffer< uint8_t >( p_renderSystem, p_gl, eGL_BUFFER_TARGET_ARRAY, p_buffer )
	{
	}

	GlVertexBufferObject::~GlVertexBufferObject()
	{
	}

	bool GlVertexBufferObject::Create()
	{
		return GlBuffer< uint8_t >::DoCreate();
	}

	void GlVertexBufferObject::Destroy()
	{
		GlBuffer< uint8_t >::DoDestroy();
	}

	bool GlVertexBufferObject::Initialise( eBUFFER_ACCESS_TYPE p_type, eBUFFER_ACCESS_NATURE p_nature )
	{
		return GlBuffer< uint8_t >::DoInitialise( p_type, p_nature );
	}

	void GlVertexBufferObject::Cleanup()
	{
		GlBuffer< uint8_t >::DoCleanup();
	}

	bool GlVertexBufferObject::Bind()
	{
		HardwareBufferPtr l_pBuffer = GetCpuBuffer();
		bool l_return = false;

		if ( l_pBuffer )
		{
			l_return = DoBind();
		}

		return l_return;
	}

	void GlVertexBufferObject::Unbind()
	{
		HardwareBufferPtr l_pBuffer = GetCpuBuffer();

		if ( l_pBuffer )
		{
			DoUnbind();
		}
	}

	uint8_t * GlVertexBufferObject::Lock( uint32_t p_offset, uint32_t p_count, uint32_t p_flags )
	{
		uint8_t * l_return = nullptr;
		HardwareBufferPtr l_pBuffer = GetCpuBuffer();

		if ( l_pBuffer )
		{
			l_return = GlBuffer< uint8_t >::DoLock( p_offset, p_count, p_flags );
		}

		return l_return;
	}

	void GlVertexBufferObject::Unlock()
	{
		HardwareBufferPtr l_pBuffer = GetCpuBuffer();

		if ( l_pBuffer )
		{
			GlBuffer< uint8_t >::DoUnlock();
		}
	}
}
