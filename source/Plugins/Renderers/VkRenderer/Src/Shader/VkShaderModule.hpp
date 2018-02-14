/*
This file belongs to RendererLib.
See LICENSE file in root folder.
*/
#pragma once

#include "VkRendererPrerequisites.hpp"

namespace vk_renderer
{
	/**
	*\~french
	*\brief
	*	Classe encapsulant un VkShaderModule.
	*\~english
	*\brief
	*	VkShaderModule wrapper.
	*/
	class ShaderModule
	{
	public:
		ShaderModule( ShaderModule const & ) = delete;
		ShaderModule & operator=( ShaderModule const & ) = delete;
		ShaderModule( ShaderModule && rhs );
		ShaderModule & operator=( ShaderModule && rhs );
		/**
		*\~french
		*\brief
		*	Constructeur.
		*\param[in] device
		*	Le LogicalDevice parent.
		*\param[in] fileData
		*	Les données SPIR-V du shader.
		*\param[in] stage
		*	Le niveau de shader utilisé pour le module.
		*\~english
		*\brief
		*	Constructor.
		*\param[in] device
		*	The logical connection to the GPU.
		*\param[in] fileData
		*	The shader SPIR-V data.
		*\param[in] stage
		*	The shader stage used for the module.
		*/
		ShaderModule( Device const & device
			, renderer::ByteArray const & fileData
			, renderer::ShaderStageFlag stage );
		/**
		*\~french
		*\brief
		*	Constructeur.
		*\param[in] device
		*	Le LogicalDevice parent.
		*\param[in] fileData
		*	Les données SPIR-V du shader.
		*\param[in] stage
		*	Le niveau de shader utilisé pour le module.
		*\~english
		*\brief
		*	Constructor.
		*\param[in] device
		*	The logical connection to the GPU.
		*\param[in] fileData
		*	The shader SPIR-V data.
		*\param[in] stage
		*	The shader stage used for the module.
		*/
		ShaderModule( Device const & device
			, renderer::UInt32Array const & fileData
			, renderer::ShaderStageFlag stage );
		/**
		*\~french
		*\brief
		*	Destructeur.
		*\~english
		*\brief
		*	Destructor.
		*/
		~ShaderModule();
		/**
		*\~french
		*\brief
		*	Le niveau de shader utilisé pour le module.
		*\~english
		*\brief
		*	The stage used for the module.
		*/
		inline VkShaderStageFlagBits getStage()const
		{
			return m_stage;
		}
		/**
		*\~french
		*\brief
		*	Conversion implicite vers VkShaderModule.
		*\~english
		*\brief
		*	VkShaderModule implicit cast operator.
		*/
		inline operator VkShaderModule const &()const
		{
			return m_shader;
		}

	private:
		Device const & m_device;
		VkShaderStageFlagBits m_stage;
		VkShaderModule m_shader{ VK_NULL_HANDLE };
	};
}
