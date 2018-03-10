/**
*\file
*	Texture.h
*\author
*	Sylvain Doremus
*/
#ifndef ___GlRenderer_Validator_HPP___
#define ___GlRenderer_Validator_HPP___
#pragma once

#include "GlRendererPrerequisites.hpp"

namespace gl_renderer
{
	void validatePipeline( PipelineLayout const & layout
		, GLuint program
		, renderer::VertexInputState const & vertexInputState
		, renderer::RenderPass const & renderPass );
}

#endif
