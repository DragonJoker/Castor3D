#include "Miscellaneous/GlComputePipeline.hpp"

#include "Common/OpenGl.hpp"
#include "Render/GlRenderSystem.hpp"

#include <Log/Logger.hpp>
#include <Shader/ShaderProgram.hpp>

using namespace Castor3D;
using namespace Castor;

#ifdef MemoryBarrier
#	undef MemoryBarrier
#endif

namespace GlRender
{
	GlComputePipeline::GlComputePipeline( OpenGl & p_gl, GlRenderSystem & p_renderSystem, ShaderProgram & p_program )
		: ComputePipeline{ p_renderSystem, p_program }
		, Holder{ p_gl }
	{
	}

	GlComputePipeline::~GlComputePipeline()
	{
	}

	void GlComputePipeline::Run( Point3ui const & p_workgroups
		, Point3ui const & p_workgroupSize
		, MemoryBarriers const & p_barriers )const
	{
		m_program.Bind();

		if ( GetOpenGl().HasComputeVariableGroupSize() )
		{
			GetOpenGl().DispatchComputeGroupSize(
				p_workgroups[0], p_workgroups[1], p_workgroups[2],
				p_workgroupSize[0], p_workgroupSize[1], p_workgroupSize[2] );
		}
		else
		{
			GetOpenGl().DispatchCompute( p_workgroups[0], p_workgroups[1], p_workgroups[2] );
		}

		GetOpenGl().MemoryBarrier( GetOpenGl().Get( p_barriers ) );
	}
}
