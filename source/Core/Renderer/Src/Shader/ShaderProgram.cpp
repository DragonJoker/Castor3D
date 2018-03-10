/*
This file belongs to RendererLib.
See LICENSE file in root folder.
*/
#include "Shader/ShaderProgram.hpp"

#include "Core/Device.hpp"
#include "Shader/ShaderModule.hpp"

namespace renderer
{
	ShaderProgram::ShaderProgram( Device const & device )
		: m_device{ device }
	{
	}

	ShaderModulePtr ShaderProgram::createModule( std::string const & shader
		, ShaderStageFlag stage )
	{
		auto result = m_device.createShaderModule( stage );
		result->loadShader( shader );
		return result;
	}

	ShaderModulePtr ShaderProgram::createModule( ByteArray const & shader
		, ShaderStageFlag stage )
	{
		auto result = m_device.createShaderModule( stage );
		result->loadShader( shader );
		return result;
	}
}
