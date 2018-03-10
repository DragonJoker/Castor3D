/*
This file belongs to Renderer.
See LICENSE file in root folder.
*/
#pragma once

#include "GlRendererPrerequisites.hpp"

namespace gl_renderer
{
	class ShaderProgram
	{
	public:
		ShaderProgram( std::vector< renderer::ShaderStageState > const & stages );
		ShaderProgram( renderer::ShaderStageState const & stage );
		~ShaderProgram();
		void link()const;

		inline GLuint getProgram()const
		{
			return m_program;
		}

	private:
		GLuint m_program;
		renderer::UInt32Array m_shaders;
	};
}
