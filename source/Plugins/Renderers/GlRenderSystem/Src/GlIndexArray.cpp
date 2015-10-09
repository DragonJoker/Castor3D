#include "GlIndexArray.hpp"

#include "GlRenderSystem.hpp"

using namespace Castor3D;
using namespace Castor;

namespace GlRender
{
	GlIndexArray::GlIndexArray( GlRenderSystem & p_renderSystem, OpenGl & p_gl, HardwareBufferPtr p_pBuffer )
		: GpuBuffer( p_renderSystem )
		, m_pBuffer( p_pBuffer )
		, m_gl( p_gl )
	{
	}

	GlIndexArray::~GlIndexArray()
	{
	}

	bool GlIndexArray::Create()
	{
		return true;
	}

	void GlIndexArray::Destroy()
	{
	}

	bool GlIndexArray::Initialise( eBUFFER_ACCESS_TYPE CU_PARAM_UNUSED( p_type ), eBUFFER_ACCESS_NATURE CU_PARAM_UNUSED( p_eNature ), Castor3D::ShaderProgramBaseSPtr CU_PARAM_UNUSED( p_pProgram ) )
	{
		return true;
	}

	void GlIndexArray::Cleanup()
	{
	}

	bool GlIndexArray::Bind()
	{
		return true;
	}

	void GlIndexArray::Unbind()
	{
	}
}
