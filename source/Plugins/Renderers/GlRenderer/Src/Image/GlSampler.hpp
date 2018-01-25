/**
*\file
*	Sampler.h
*\author
*	Sylvain Doremus
*/
#ifndef ___VkRenderer_Sampler_HPP___
#define ___VkRenderer_Sampler_HPP___
#pragma once

#include "GlRendererPrerequisites.hpp"

#include <Image/Sampler.hpp>

namespace gl_renderer
{
	/**
	*\brief
	*	Un échantillonneur.
	*/
	class Sampler
		: public renderer::Sampler
	{
	public:
		/**
		*\brief
		*	Constructeur.
		*\param[in] device
		*	Le périphérique logique.
		*\param[in] wrapS, wrapT
		*	Les modes de wrap de texture.
		*\param[in] minFilter, magFilter
		*	Les filtres de minification et magnification.
		*/
		Sampler( renderer::Device const & device
			, renderer::WrapMode wrapS
			, renderer::WrapMode wrapT
			, renderer::WrapMode wrapR
			, renderer::Filter minFilter
			, renderer::Filter magFilter
			, renderer::MipmapMode mipFilter
			, float minLod
			, float maxLod
			, float lodBias
			, renderer::BorderColour borderColour
			, float maxAnisotropy
			, renderer::CompareOp compareOp );
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
