/*
This file belongs to VkLib.
See LICENSE file in root folder.
*/
#include "Shader/VkShaderModule.hpp"

#include "Core/VkDevice.hpp"

namespace vk_renderer
{
	ShaderModule::ShaderModule( ShaderModule && rhs )
		: m_device{ rhs.m_device }
		, m_shader{ rhs.m_shader }
		, m_stage{ rhs.m_stage }
	{
		rhs.m_shader = VK_NULL_HANDLE;
	}

	ShaderModule & ShaderModule::operator=( ShaderModule && rhs )
	{
		if ( &rhs != this )
		{
			m_shader = rhs.m_shader;
			m_stage = rhs.m_stage;
			rhs.m_shader = VK_NULL_HANDLE;
		}

		return *this;
	}

	ShaderModule::ShaderModule( Device const & device
		, renderer::UInt32Array const & fileData
		, renderer::ShaderStageFlag stage )
		: m_device{ device }
		, m_stage{ convert( stage ) }
	{
		VkShaderModuleCreateInfo createInfo
		{
			VK_STRUCTURE_TYPE_SHADER_MODULE_CREATE_INFO,
			nullptr,
			0,                                                          // flags
			fileData.size() * sizeof( uint32_t ),                       // codeSize
			fileData.data()                                             // pCode
		};
		DEBUG_DUMP( createInfo );
		auto res = vk::CreateShaderModule( m_device
			, &createInfo
			, nullptr
			, &m_shader );

		if ( !checkError( res ) )
		{
			throw std::runtime_error{ "Shader module creation failed: " + getLastError() };
		}
	}

	ShaderModule::ShaderModule( Device const & device
		, renderer::ByteArray const & fileData
		, renderer::ShaderStageFlag stage )
		: m_device{ device }
		, m_stage{ convert( stage ) }
	{
		VkShaderModuleCreateInfo createInfo
		{
			VK_STRUCTURE_TYPE_SHADER_MODULE_CREATE_INFO,
			nullptr,
			0,                                                          // flags
			fileData.size(),                                            // codeSize
			reinterpret_cast< uint32_t const * >( fileData.data() )     // pCode
		};
		DEBUG_DUMP( createInfo );
		auto res = vk::CreateShaderModule( m_device
			, &createInfo
			, nullptr
			, &m_shader );

		if ( !checkError( res ) )
		{
			throw std::runtime_error{ "Shader module creation failed: " + getLastError() };
		}
	}

	ShaderModule::~ShaderModule()
	{
		if ( m_shader != VK_NULL_HANDLE )
		{
			vk::DestroyShaderModule( m_device, m_shader, nullptr );
		}
	}
}
