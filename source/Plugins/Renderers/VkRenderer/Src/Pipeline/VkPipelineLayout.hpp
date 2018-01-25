/*
This file belongs to Renderer.
See LICENSE file in root folder
*/
#pragma once

#include "VkRendererPrerequisites.hpp"

#include <Pipeline/PipelineLayout.hpp>

namespace vk_renderer
{
	/**
	*\brief
	*	Wrapper de VkPipelineLayout.
	*/
	class PipelineLayout
		: public renderer::PipelineLayout
	{
	public:
		/**
		*\brief
		*	Constructeur.
		*\param[in] device
		*	Le LogicalDevice parent.
		*\param[in] layout
		*	Le layout des descripteurs du pipeline.
		*/
		PipelineLayout( Device const & device
			, renderer::DescriptorSetLayout const * layout );
		/**
		*\brief
		*	Destructeur.
		*/
		~PipelineLayout();
		/**
		*\~french
		*\brief
		*	Conversion implicite vers VkPipelineLayout.
		*\~english
		*\brief
		*	VkPipelineLayout implicit cast operator.
		*/
		inline operator VkPipelineLayout const &( )const
		{
			return m_layout;
		}

	private:
		Device const & m_device;
		VkPipelineLayout m_layout{ VK_NULL_HANDLE };
	};
}
