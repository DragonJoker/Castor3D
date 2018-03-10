/*
This file belongs to RendererLib.
See LICENSE file in root folder.
*/
#pragma once

#include "VkRendererPrerequisites.hpp"

#include <Shader/ShaderModule.hpp>

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
		/**
		*\~copydoc	renderer::ShaderModule::loadShader
		*/
		void loadShader( renderer::UInt32Array const & shader );
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
		void doLoadShader( uint32_t const * const begin
			, uint32_t size );

	private:
		Device const & m_device;
		VkShaderStageFlagBits m_stage;
		VkShaderModule m_shader{ VK_NULL_HANDLE };
	};
}
