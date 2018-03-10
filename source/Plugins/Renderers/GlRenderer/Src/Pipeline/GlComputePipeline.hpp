/**
*\file
*	Texture.h
*\author
*	Sylvain Doremus
*/
#ifndef ___GlRenderer_ComputePipeline_HPP___
#define ___GlRenderer_ComputePipeline_HPP___
#pragma once

#include "Shader/GlShaderProgram.hpp"

#include <Buffer/PushConstantsBuffer.hpp>
#include <Pipeline/ComputePipeline.hpp>

namespace gl_renderer
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
		*/
		ComputePipeline( Device const & device
			, renderer::PipelineLayout const & layout
			, renderer::ComputePipelineCreateInfo && createInfo );
		/**
		*\return
		*	Le PipelineLayout.
		*/
		inline renderer::PipelineLayout const & getLayout()const
		{
			return m_layout;
		}
		/**
		*\return
		*	Le ShaderProgram.
		*/
		inline GLuint getProgram()const
		{
			return m_program.getProgram();
		}
		/**
		*\return
		*	Le tampon de push constants correspondant aux constantes de spécialisation.
		*/
		inline std::vector< renderer::PushConstantsBufferBase > const & getConstantsPcbs()const
		{
			return m_constantsPcbs;
		}

	private:
		Device const & m_device;
		renderer::PipelineLayout const & m_layout;
		ShaderProgram m_program;
		std::vector< renderer::PushConstantsBufferBase > m_constantsPcbs;
	};
}

#endif
