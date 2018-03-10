/**
*\file
*	Sampler.h
*\author
*	Sylvain Doremus
*/
#ifndef ___GlRenderer_Sampler_HPP___
#define ___GlRenderer_Sampler_HPP___
#pragma once

#include "GlRendererPrerequisites.hpp"

#include <Image/Sampler.hpp>

namespace gl_renderer
{
	class Sampler
		: public renderer::Sampler
	{
	public:
		Sampler( renderer::Device const & device
			, renderer::SamplerCreateInfo const & createInfo );
		/**
		*\brief
		*	Destructeur.
		*/
		~Sampler();
		/**
		*\return
		*	L'échantillonneur OpenGL.
		*/
		inline GLuint getSampler()const noexcept
		{
			return m_sampler;
		}

	private:
		//! L'échantillonneur.
		GLuint m_sampler;
	};
}

#endif
