#include "Miscellaneous/GlComputePipeline.hpp"

#include "Common/OpenGl.hpp"
#include "Render/GlRenderSystem.hpp"

#include <Log/Logger.hpp>
#include <Shader/ShaderProgram.hpp>

using namespace castor3d;
using namespace castor;

#ifdef MemoryBarrier
#	undef MemoryBarrier
#endif

namespace GlRender
{
	GlComputePipeline::GlComputePipeline( OpenGl & p_gl, GlRenderSystem & renderSystem, ShaderProgram & p_program )
		: ComputePipeline{ renderSystem, p_program }
		, Holder{ p_gl }
	{
	}

	GlComputePipeline::~GlComputePipeline()
	{
	}

	void GlComputePipeline::run( Point3ui const & p_workgroups
		, Point3ui const & p_workgroupSize
		, MemoryBarriers const & p_barriers )const
	{
		m_program.bind();

		if ( getOpenGl().hasComputeVariableGroupSize() )
		{
			getOpenGl().DispatchComputeGroupSize(
				p_workgroups[0], p_workgroups[1], p_workgroups[2],
				p_workgroupSize[0], p_workgroupSize[1], p_workgroupSize[2] );
		}
		else
		{
			getOpenGl().DispatchCompute( p_workgroups[0], p_workgroups[1], p_workgroups[2] );
		}

		getOpenGl().MemoryBarrier( getOpenGl().get( p_barriers ) );
	}
}
