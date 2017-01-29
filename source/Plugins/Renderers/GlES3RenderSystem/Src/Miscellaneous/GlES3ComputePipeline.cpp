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
		m_program.Bind();

		if ( GetOpenGlES3().HasComputeVariableGroupSize() )
		{
			GetOpenGlES3().DispatchComputeGroupSize(
				p_workgroups[0], p_workgroups[1], p_workgroups[2],
				p_workgroupSize[0], p_workgroupSize[1], p_workgroupSize[2] );
		}
		else
		{
			GetOpenGlES3().DispatchCompute( p_workgroups[0], p_workgroups[1], p_workgroups[2] );
		}

		GetOpenGlES3().MemoryBarrier( GetOpenGlES3().Get( p_barriers ) );
	}
}
