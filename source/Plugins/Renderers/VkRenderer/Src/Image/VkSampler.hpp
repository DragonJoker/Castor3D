/**
*\file
*	Sampler.h
*\author
*	Sylvain Doremus
*/
#ifndef ___VkRenderer_Sampler_HPP___
#define ___VkRenderer_Sampler_HPP___
#pragma once

#include "VkRendererPrerequisites.hpp"

#include <Image/Sampler.hpp>

namespace vk_renderer
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
		*\~french
		*\brief
		*	Constructeur.
		*\param[in] device
		*	Le périphérique logique.
		*\param[in] wrapS, wrapT
		*	Les modes de wrap de texture.
		*\param[in] minFilter, magFilter
		*	Les filtres de minification et magnification.
		*\param[in] mipFilter
		*	Le mode de mipmapping.
		*\~english
		*\brief
		*	Constructor.
		*\param[in] device
		*	The logical connection to the GPU.
		*\param[in] minFilter, magFilter
		*\param[in] wrapS, wrapT, wrapR
		*	The  S, T and R address mode.
		*	The minification and magnification filter.
		*\param[in] mipFilter
		*	The mipmapping mode.
		*/
		Sampler( Device const & device
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
		*\~french
		*\brief
		*	Destructeur.
		*\~english
		*\brief
		*	Destructor.
		*/
		~Sampler();
		/**
		*\~french
		*\brief
		*	Opérateur de conversion implicite vers VkSampler.
		*\~english
		*\brief
		*	VkSampler implicit cast operator.
		*/
		inline operator VkSampler const &( )const
		{
			return m_sampler;
		}

	private:
		Device const & m_device;
		VkSampler m_sampler{};
	};
}

#endif
