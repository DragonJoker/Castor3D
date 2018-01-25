#include "VkRendererPrerequisites.hpp"

namespace vk_renderer
{
	VkQueryPipelineStatisticFlags convert( renderer::QueryPipelineStatisticFlags const & flags )
	{
		VkQueryPipelineStatisticFlags result{ 0 };

		if ( checkFlag( flags, renderer::QueryPipelineStatisticFlag::eInputAssemblyVertices ) )
		{
			result |= VK_QUERY_PIPELINE_STATISTIC_INPUT_ASSEMBLY_VERTICES_BIT;
		}

		if ( checkFlag( flags, renderer::QueryPipelineStatisticFlag::eInputAssemblyPrimitives ) )
		{
			result |= VK_QUERY_PIPELINE_STATISTIC_INPUT_ASSEMBLY_PRIMITIVES_BIT;
		}

		if ( checkFlag( flags, renderer::QueryPipelineStatisticFlag::eVertexShaderInvocations ) )
		{
			result |= VK_QUERY_PIPELINE_STATISTIC_VERTEX_SHADER_INVOCATIONS_BIT;
		}

		if ( checkFlag( flags, renderer::QueryPipelineStatisticFlag::eGeometryShaderInvocations ) )
		{
			result |= VK_QUERY_PIPELINE_STATISTIC_GEOMETRY_SHADER_INVOCATIONS_BIT;
		}

		if ( checkFlag( flags, renderer::QueryPipelineStatisticFlag::eGeometryShaderPrimitives ) )
		{
			result |= VK_QUERY_PIPELINE_STATISTIC_GEOMETRY_SHADER_PRIMITIVES_BIT;
		}

		if ( checkFlag( flags, renderer::QueryPipelineStatisticFlag::eClippingInvocations ) )
		{
			result |= VK_QUERY_PIPELINE_STATISTIC_CLIPPING_INVOCATIONS_BIT;
		}

		if ( checkFlag( flags, renderer::QueryPipelineStatisticFlag::eClippingPrimitives ) )
		{
			result |= VK_QUERY_PIPELINE_STATISTIC_CLIPPING_PRIMITIVES_BIT;
		}

		if ( checkFlag( flags, renderer::QueryPipelineStatisticFlag::eFragmentShaderInvocations ) )
		{
			result |= VK_QUERY_PIPELINE_STATISTIC_FRAGMENT_SHADER_INVOCATIONS_BIT;
		}

		if ( checkFlag( flags, renderer::QueryPipelineStatisticFlag::eTessellationControlShaderPatches ) )
		{
			result |= VK_QUERY_PIPELINE_STATISTIC_TESSELLATION_CONTROL_SHADER_PATCHES_BIT;
		}

		if ( checkFlag( flags, renderer::QueryPipelineStatisticFlag::eTessellationControlShaderInvocations ) )
		{
			result |= VK_QUERY_PIPELINE_STATISTIC_TESSELLATION_EVALUATION_SHADER_INVOCATIONS_BIT;
		}

		if ( checkFlag( flags, renderer::QueryPipelineStatisticFlag::eComputeShaderInvocations ) )
		{
			result |= VK_QUERY_PIPELINE_STATISTIC_COMPUTE_SHADER_INVOCATIONS_BIT;
		}

		return result;
	}
}
