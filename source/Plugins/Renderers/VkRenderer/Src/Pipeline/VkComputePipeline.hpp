/**
*\file
*	Texture.h
*\author
*	Sylvain Doremus
*/
#ifndef ___VkRenderer_ComputePipeline_HPP___
#define ___VkRenderer_ComputePipeline_HPP___
#pragma once

#include "VkRendererPrerequisites.hpp"

#include <Pipeline/ComputePipeline.hpp>

namespace vk_renderer
{
	/**
	*\brief
	*	Un pipeline de rendu.
	*/
	class ComputePipeline
		: public renderer::ComputePipeline
	{
	public:
		/**
		*\brief
		*	Constructeur.
		*\param[in] device
		*	Le LogicalDevice parent.
		*\param[in] layout
		*	Le layout du pipeline.
		*\param[in] program
		*	Le programme shader.
		*\param[in] vertexBuffers
		*	Les tampons de sommets utilisés.
		*\param[in] renderPass
		*	La passe de rendu.
		*\param[in] topology
		*	La topologie d'affichage des sommets affichés via ce pipeline.
		*/
		ComputePipeline( Device const & device
			, renderer::PipelineLayout const & layout
			, renderer::ShaderProgram const & program );
		/**
		*\~french
		*\brief
		*	Destructeur.
		*\~english
		*\brief
		*	Destructor.
		*/
		~ComputePipeline();
		/**
		*\~french
		*\brief
		*	Conversion implicite vers VkPipeline.
		*\~english
		*\brief
		*	VkPipeline implicit cast operator.
		*/
		inline operator VkPipeline const &( )const
		{
			return m_pipeline;
		}

	private:
		Device const & m_device;
		PipelineLayout const & m_layout;
		ShaderProgram const & m_shader;
		VkPipeline m_pipeline{ VK_NULL_HANDLE };
	};
}

#endif
