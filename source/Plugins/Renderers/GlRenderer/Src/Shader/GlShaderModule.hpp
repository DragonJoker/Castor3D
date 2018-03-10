/*
This file belongs to RendererLib.
See LICENSE file in root folder.
*/
#pragma once

#include "GlRendererPrerequisites.hpp"

#include <Shader/ShaderModule.hpp>

namespace gl_renderer
{
	class ShaderModule
		: public renderer::ShaderModule
	{
	public:
		ShaderModule( Device const & device
			, renderer::ShaderStageFlag stage );
		~ShaderModule();
		/**
		*\~copydoc	renderer::ShaderModule::loadShader
		*/
		void loadShader( std::string const & shader )override;
		/**
		*\~copydoc	renderer::ShaderModule::loadShader
		*/
		void loadShader( renderer::ByteArray const & shader )override;

		inline GLuint getShader()const
		{
			return m_shader;
		}

		inline bool isSpirV()const
		{
			return m_isSpirV;
		}

	private:
		Device const & m_device;
		GLuint m_shader;
		bool m_isSpirV;
	};
}
