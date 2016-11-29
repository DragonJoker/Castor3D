#include "ComputePipeline.hpp"

#include "Render/RenderSystem.hpp"
#include "Shader/ShaderProgram.hpp"

using namespace Castor;

namespace Castor3D
{
	ComputePipeline::ComputePipeline( RenderSystem & p_renderSystem, ShaderProgram & p_program )
		: OwnedBy< RenderSystem >{ p_renderSystem }
		, m_program{ p_program }
	{
	}

	ComputePipeline::~ComputePipeline()
	{
	}

	void ComputePipeline::Cleanup()
	{
		m_program.Cleanup();
	}
}
