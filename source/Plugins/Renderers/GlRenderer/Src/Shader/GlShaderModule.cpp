/*
This file belongs to RendererLib.
See LICENSE file in root folder.
*/
#include "Shader/GlShaderModule.hpp"

#include "Core/GlDevice.hpp"
#include "Core/GlPhysicalDevice.hpp"

#include <iostream>

namespace gl_renderer
{
	namespace
	{
		std::string doRetrieveCompilerLog( GLuint shaderName )
		{
			std::string log;
			int infologLength = 0;
			int charsWritten = 0;
			glLogCall( gl::GetShaderiv, shaderName, GL_INFO_LOG_LENGTH, &infologLength );

			if ( infologLength > 0 )
			{
				std::vector< char > infoLog( infologLength + 1 );
				glLogCall( gl::GetShaderInfoLog, shaderName, infologLength, &charsWritten, infoLog.data() );
				log = infoLog.data();
			}

			if ( !log.empty() )
			{
				log = log.substr( 0, log.size() - 1 );
			}

			return log;
		}

		bool doCheckCompileErrors( bool compiled
			, GLuint shaderName )
		{
			auto compilerLog = doRetrieveCompilerLog( shaderName );

			if ( !compilerLog.empty() )
			{
				if ( !compiled )
				{
					std::cerr << compilerLog << std::endl;
				}
				else
				{
					std::cout << compilerLog << std::endl;
				}
			}
			else if ( !compiled )
			{
				std::cerr << "Shader compilation failed" << std::endl;
			}

			return compiled;
		}
	}

	ShaderModule::ShaderModule( Device const & device
		, renderer::ShaderStageFlag stage )
		: renderer::ShaderModule{ stage }
		, m_device{ device }
		, m_shader{ gl::CreateShader( convert( stage ) ) }
		, m_isSpirV{ false }
	{
	}

	ShaderModule::~ShaderModule()
	{
		// Shader object is destroyed by the ShaderProgram.
	}

	void ShaderModule::loadShader( std::string const & shader )
	{
		auto length = int( shader.size() );
		auto data = shader.data();
		glLogCall( gl::ShaderSource, m_shader, 1, &data, &length );
		glLogCall( gl::CompileShader, m_shader );
		int compiled = 0;
		glLogCall( gl::GetShaderiv, m_shader, GL_INFO_COMPILE_STATUS, &compiled );

		if ( !doCheckCompileErrors( compiled != 0, m_shader ) )
		{
			throw std::runtime_error{ "Shader compilation failed." };
		}
	}

	void ShaderModule::loadShader( renderer::ByteArray const & fileData )
	{
		if ( !m_device.getPhysicalDevice().isSPIRVSupported() )
		{
			throw std::runtime_error{ "Shader compilation from SPIR-V is not supported." };
		}

		gl::ShaderBinary( 1u, &m_shader, GL_SHADER_BINARY_FORMAT_SPIR_V, fileData.data(), GLsizei( fileData.size() ) );
		m_isSpirV = true;
	}
}
