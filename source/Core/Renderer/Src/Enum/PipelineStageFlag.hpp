/*
This file belongs to Renderer.
See LICENSE file in root folder.
*/
#ifndef ___Renderer_PipelineStageFlag_HPP___
#define ___Renderer_PipelineStageFlag_HPP___
#pragma once

namespace renderer
{
	/**
	*\brief
	*	Enumération des utilisation de tampon de commandes.
	*/
	enum class PipelineStageFlag
		: uint32_t
	{
		eTopOfPipe = 0x00000001,
		eDrawIndirect = 0x00000002,
		eVertexInput = 0x00000004,
		eVertexShader = 0x00000008,
		eTessellationControlShader = 0x00000010,
		eTessellationEvaluationShader = 0x00000020,
		eGeometryShader = 0x00000040,
		eFragmentShader = 0x00000080,
		eEarlyFragmentTests = 0x00000100,
		eLateFragmentTests = 0x00000200,
		eColourAttachmentOutput = 0x00000400,
		eComputeShader = 0x00000800,
		eTransfer = 0x00001000,
		eBottomOfPipe = 0x00002000,
		eHost = 0x00004000,
		eAllGraphics = 0x00008000,
		eAllCommands = 0x00010000,
	};
	Utils_ImplementFlag( PipelineStageFlag )
}

#endif
