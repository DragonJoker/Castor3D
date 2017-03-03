#include "Miscellaneous/GlES3ComputePipeline.hpp"

#include "Common/OpenGlES3.hpp"
#include "Render/GlES3RenderSystem.hpp"

#include <Log/Logger.hpp>
#include <Shader/ShaderProgram.hpp>

using namespace Castor3D;
using namespace Castor;

namespace GlES3Render
{
	GlES3ComputePipeline::GlES3ComputePipeline( OpenGlES3 & p_gl, GlES3RenderSystem & p_renderSystem, ShaderProgram & p_program )
		: ComputePipeline{ p_renderSystem, p_program }
		, Holder{ p_gl }
	{
	}

	GlES3ComputePipeline::~GlES3ComputePipeline()
	{
	}

	void GlES3ComputePipeline::Run(
		Point3ui const & p_workgroups,
		Point3ui const & p_workgroupSize,
		FlagCombination< MemoryBarrier > const & p_barriers )const
	{
		FAILURE( "Compute shaders are not supported on OpenGL ES 3.x" );
	}
}
