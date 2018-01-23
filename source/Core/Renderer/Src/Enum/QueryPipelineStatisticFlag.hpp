/*
This file belongs to Renderer.
See LICENSE file in root folder.
*/
#ifndef ___Renderer_QueryPipelineStatisticFlag_HPP___
#define ___Renderer_QueryPipelineStatisticFlag_HPP___
#pragma once

namespace renderer
{
	/**
	*\brief
	*	Enumération des types de statistiques de pipeline.
	*/
	enum class QueryPipelineStatisticFlag
		: uint32_t
	{
		eInputAssemblyVertices = 0x00000001,
		eInputAssemblyPrimitives = 0x00000002,
		eVertexShaderInvocations = 0x00000004,
		eGeometryShaderInvocations = 0x00000008,
		eGeometryShaderPrimitives = 0x00000010,
		eClippingInvocations = 0x00000020,
		eClippingPrimitives = 0x00000040,
		eFragmentShaderInvocations = 0x00000080,
		eTessellationControlShaderPatches = 0x00000100,
		eTessellationControlShaderInvocations = 0x00000200,
		eComputeShaderInvocations = 0x00000400,
	};
	Utils_ImplementFlag( QueryPipelineStatisticFlag )
}

#endif
