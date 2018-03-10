/*
This file belongs to Renderer.
See LICENSE file in root folder.
*/
#include "Shader/GlShaderProgram.hpp"

#include "Core/GlDevice.hpp"
#include "Shader/GlShaderModule.hpp"

#include <Pipeline/ShaderStageState.hpp>

#include <iostream>

namespace gl_renderer
{
	namespace
	{
		std::string doRetrieveLinkerLog( GLuint programName )
		{
			std::string log;
			int infologLength = 0;
			int charsWritten = 0;
			glLogCall( gl::GetProgramiv, programName, GL_INFO_LOG_LENGTH, &infologLength );

			if ( infologLength > 0 )
			{
				std::vector< char > infoLog( infologLength + 1 );
				glLogCall( gl::GetProgramInfoLog, programName, infologLength, &charsWritten, infoLog.data() );
				log = infoLog.data();
			}

			if ( !log.empty() )
			{
				log = log.substr( 0, log.size() - 1 );
			}

			return log;
		}

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

		void doInitialiseState( renderer::ShaderStageState const & stage )
		{
			auto & module = static_cast< ShaderModule const & >( *stage.module );
			auto shader = module.getShader();

			if ( module.isSpirV() )
			{
				if ( stage.specialisationInfo )
				{
					auto & specialisationInfo = *stage.specialisationInfo;
					auto count = GLuint( std::distance( specialisationInfo.begin(), specialisationInfo.end() ) );
					std::vector< GLuint > indices;
					indices.reserve( count );
					std::vector< GLuint > values;
					values.reserve( count );
					auto src = reinterpret_cast< GLuint const * >( specialisationInfo.getData() );
					auto dst = values.data();

					for ( auto & constant : specialisationInfo )
					{
						indices.push_back( constant.constantID );
						values.push_back( *src );
						++src;
					}

					glLogCall( gl::SpecializeShader
						, shader
						, stage.entryPoint.c_str()
						, count
						, indices.data()
						, values.data() );
				}
				else
				{
					glLogCall( gl::SpecializeShader
						, shader
						, stage.entryPoint.c_str()
						, 0u
						, nullptr
						, nullptr );
				}

				int compiled = 0;
				glLogCall( gl::GetShaderiv, shader, GL_INFO_COMPILE_STATUS, &compiled );

				if ( !doCheckCompileErrors( compiled != 0, shader ) )
				{
					throw std::runtime_error{ "Shader compilation failed." };
				}
			}
		}
	}

	ShaderProgram::ShaderProgram( std::vector< renderer::ShaderStageState > const & stages )
		: m_program{ gl::CreateProgram() }
	{
		for ( auto & stage : stages )
		{
			auto & module = static_cast< ShaderModule const & >( *stage.module );
			m_shaders.push_back( module.getShader() );
			doInitialiseState( stage );
			glLogCall( gl::AttachShader, m_program, m_shaders.back() );
		}
	}

	ShaderProgram::ShaderProgram( renderer::ShaderStageState const & stage )
		: m_program{ gl::CreateProgram() }
	{
		auto & module = static_cast< ShaderModule const & >( *stage.module );
		m_shaders.push_back( module.getShader() );
		doInitialiseState( stage );
		glLogCall( gl::AttachShader, m_program, m_shaders.back() );
	}

	ShaderProgram::~ShaderProgram()
	{
		for ( auto shaderName : m_shaders )
		{
			glLogCall( gl::DeleteShader, shaderName );
		}

		glLogCall( gl::DeleteProgram, m_program );
	}

	void ShaderProgram::link()const
	{
		int attached = 0;
		glLogCall( gl::GetProgramiv, m_program, GL_INFO_ATTACHED_SHADERS, &attached );
		glLogCall( gl::LinkProgram, m_program );
		int linked = 0;
		glLogCall( gl::GetProgramiv, m_program, GL_INFO_LINK_STATUS, &linked );
		auto linkerLog = doRetrieveLinkerLog( m_program );

		if ( linked
			&& attached == int( m_shaders.size() )
			&& linkerLog.find( "ERROR" ) == std::string::npos )
		{
			if ( !linkerLog.empty() )
			{
				std::cout << "ShaderProgram::link - " << linkerLog << std::endl;
			}

			int validated = 0;
			glLogCall( gl::GetProgramiv, m_program, GL_INFO_VALIDATE_STATUS, &validated );

			if ( !validated )
			{
				std::cerr << "ShaderProgram::link - Not validated" << std::endl;
			}
		}
		else
		{
			if ( !linkerLog.empty() )
			{
				std::cerr << "ShaderProgram::link - " << linkerLog << std::endl;
			}

			if ( attached != int( m_shaders.size() ) )
			{
				std::cerr << "ShaderProgram::link - The linked shaders count doesn't match the active shaders count." << std::endl;
			}
		}
	}
}
