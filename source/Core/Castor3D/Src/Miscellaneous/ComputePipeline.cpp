#include "ComputePipeline.hpp"

#include "Render/RenderSystem.hpp"
#include "Shader/ShaderProgram.hpp"

using namespace castor;

namespace castor3d
{
	ComputePipeline::ComputePipeline( RenderSystem & renderSystem, ShaderProgram & p_program )
		: OwnedBy< RenderSystem >{ renderSystem }
		, m_program{ p_program }
	{
	}

	ComputePipeline::~ComputePipeline()
	{
	}

	void ComputePipeline::cleanup()
	{
		m_program.cleanup();
	}
}
